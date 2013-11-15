struct VertexShaderOutput
{
		float4 Position : SV_POSITION;
		float4 ScreenPosition : TEXCOORD0;
		float4 ViewPosition : TEXCOORD1;
		float2 TexCoord : TEXCOORD2;
};

SamplerState samplers[2]; //0 == linear sampler, 1 == point
Texture2D randomTexture : register(ps_5_0, t0);
Texture2D shaderTextures[4] : register(ps_5_0, t1); // 0 = color, 1 = light, 2 = depth, 3 = normal

static const float sampleRadius = 0.6f; //Controls sampling radius.
static const float intensity = 6.0f; //AO intensity. The higher this value is, the darker the occluded parts will be.
static const float scale = 0.8f; //Scales distance between occluders and occludee.
static const float bias = 0.5f; //Cutoff value. The higher this value is, the harsher we are with cutting off low AO values.
static const float CameraFarClip = 500.0f;
static const float FogEnd = 300.0f;
static const float FogStart = 100.0f;
static const float2 screenSize = float2(1024.0f, 768.0f);
static const float2 randomSize = float2(64.0f, 64.0f);

cbuffer PixelMatrixBufferType
{
	float4x4 View;
	float4x4 InvertedProjection;
	float4 FogColor;
	int toggleSSAO;
}

//Decoding of GBuffer Normals
float3 DecodeNormal(float2 enc)
{
	float2 fenc = (enc*4.0f)-2.0f;
	float f = dot(fenc,fenc);
	float g = sqrt(1.0f-(f*0.25f));
	
	//float3 n;
	//n.xy = fenc*g;
	//n.z = 1.0f-(f*0.5f);

	return float3(fenc*g, 1.0f - (f*0.5f));
}

float3 DepthToPosition(float4 ScreenPosition, float depth)
{
	float3 viewPosition = mul(ScreenPosition, InvertedProjection).xyz;
	return float3(viewPosition.xy * (CameraFarClip / viewPosition.z), CameraFarClip) * depth;
}

float3 ReconstructViewPositionFromDepth(float3 viewPosition, float depth)
{
	// Sample the depth and scale the view ray to reconstruct view space position
	//float3 viewRay = float3(viewPosition.xy * (150.0f / viewPosition.z), 150.0f);
	//float normalizedDepth = shaderTextures[1].Sample(linearSampler, texCoord);

	return float3(viewPosition.xy * (CameraFarClip / viewPosition.z), CameraFarClip) * depth;
}

float DoAmbientOcclusion(float2 TexCoord, float2 Offset, float4 ScreenPosition, float3 Position, float3 Normal)
{
	//Sample depth from our new offset position
	float offsetDepth = shaderTextures[2].Sample(samplers[0], (TexCoord + Offset));
	

	//Reconstruct an offset position with the help of our offset screenpos and offset depth
	float3 offsetPosition = float3(TexCoord + Offset, offsetDepth);//DepthToPosition(ScreenPosition, offsetDepth);

	//Calculate difference between offset position and position of pixel
	float3 diff = offsetPosition - Position;
	float3 v = normalize(diff);

	//Get sampling distance by scaling the difference vector
	float d = length(diff)*scale;

	//calculate occlusion
	return max(0.0f, dot(Normal, v)-bias) * (1.0/(1.0+d))*intensity;
}

float4 ComposePixelShader(VertexShaderOutput input) : SV_Target0
{
		float depth = shaderTextures[2].Sample(samplers[0], input.TexCoord);
		float3 baseColor = shaderTextures[0].Sample(samplers[0], input.TexCoord).rgb;

		if(depth == 0.0f) //Skip as early as possible if this pixel isn't supposed to be lit. (Skysphere for example.)
		{
			return float4(baseColor, 1.0f);
		}

		float fogFactor = saturate(((depth*CameraFarClip) - FogStart) / (FogEnd - FogStart));
		
		if(fogFactor >= 1.0f)
		{
			return FogColor; //Cut early. Only return fog color.
		}

		//FOG:
		//http://www.iquilezles.org/www/articles/ssao/ssao.htm
		//http://jcoluna.wordpress.com/2012/11/06/xna-adding-fog-in-a-lpp-or-deferred-pipeline/
		//http://cloneofduty.com/2012/07/30/deferred-rendering-5-foggy-haze/

		//SSAO:
		//http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
		
		const float2 vec[4] = {	float2(1, 0),float2(-1 ,0),
								float2(0, 1),float2(0, -1) };

		float3 position = ReconstructViewPositionFromDepth(input.ViewPosition.xyz, depth);
		float3 normal = normalize( mul(DecodeNormal(shaderTextures[3].Sample(samplers[0], input.TexCoord).xy), View)*2.0f - 1.0f); //Viewspace normals
		
		//float3 normal = normalize(DecodeNormal(shaderTextures[3].Sample(samplers[0], input.TexCoord).xy)); //Worldspace normals

		float2 rand = normalize(randomTexture.Sample(samplers[0], (screenSize*input.TexCoord / randomSize)) * 2.0f - 1.0f);//

		float ao = 1.0f;
		float rad = sampleRadius / position.z;

		int iterations = lerp(8, 2, depth); //primitive SSAO LOD; scales with how close to the pixel we are

		//SSAO Calculation
		[unroll]
		for (int i = 0; i < iterations; i++)
		{
			float2 coord1 = reflect(vec[i], rand)*rad;
			float2 coord2 = float2(	coord1.x*0.707 - coord1.y*0.707,
									coord1.x*0.707 + coord1.y*0.707);
		
			ao += DoAmbientOcclusion(input.TexCoord, coord1*0.25,	input.ScreenPosition, position, normal);
			ao += DoAmbientOcclusion(input.TexCoord, coord2*0.5,	input.ScreenPosition, position, normal);
			ao += DoAmbientOcclusion(input.TexCoord, coord1*0.75,	input.ScreenPosition, position, normal);
			ao += DoAmbientOcclusion(input.TexCoord, coord2,		input.ScreenPosition, position, normal);
		}

		ao /= (float)iterations*4.0f;

		ao = max((1.0f - ao), 0.4f); //We don't want the ssao to make things completely black (0.0f), so we cap it at a minimum of 0.4f.

		//Add AO to final calculation ...

		//specular intensity is stored in the color map's alpha channel.
		float4 light = shaderTextures[1].Sample(samplers[0], input.TexCoord);
		float3 diffuseLight = light.rgb;
		float specularLight = light.a;
		
		//Not going to apply specular here, it'll be applied in the final composition stage.
		if(toggleSSAO == 0)
		{
			return ao * lerp((float4((baseColor) * (diffuseLight + specularLight), 1.0f)),  FogColor, fogFactor);
		}
		else if(toggleSSAO == 1)
		{
			return lerp(float4((baseColor) * (specularLight + diffuseLight), 1.0f),  FogColor, fogFactor);
		}
		else
		{
			return float4(ao, ao, ao, 1.0f);//lerp((float4((ao * baseColor) * (diffuseLight + specularLight), 1.0f)),  FogColor, fogFactor); // float4(ao, ao, ao, 1.0f);//
		}	
}
/*

uniform sampler2D rnm;
uniform sampler2D normalMap;
varying vec2 uv;
const float totStrength = 1.38;
const float strength = 0.07;
const float offset = 18.0;
const float falloff = 0.000002;
const float rad = 0.006;
#define SAMPLES 10 // 10 is good
const float invSamples = 1.0f/SAMPLES;
void main(void)
{
// these are the random vectors inside a unit sphere
vec3 pSphere[10] = vec3[](vec3(-0.010735935, 0.01647018, 0.0062425877),vec3(-0.06533369, 0.3647007, -0.13746321),vec3(-0.6539235, -0.016726388, -0.53000957),vec3(0.40958285, 0.0052428036, -0.5591124),vec3(-0.1465366, 0.09899267, 0.15571679),vec3(-0.44122112, -0.5458797, 0.04912532),vec3(0.03755566, -0.10961345, -0.33040273),vec3(0.019100213, 0.29652783, 0.066237666),vec3(0.8765323, 0.011236004, 0.28265962),vec3(0.29264435, -0.40794238, 0.15964167));
 
   // grab a normal for reflecting the sample rays later on
   vec3 fres = normalize((texture2D(rnm,uv*offset).xyz*2.0) - vec3(1.0));
 
   vec4 currentPixelSample = texture2D(normalMap,uv);
 
   float currentPixelDepth = currentPixelSample.a;
 
   // current fragment coords in screen space
   vec3 ep = vec3(uv.xy,currentPixelDepth);
  // get the normal of current fragment
   vec3 norm = currentPixelSample.xyz;
 
   float bl = 0.0;
   // adjust for the depth ( not shure if this is good..)
   float radD = rad/currentPixelDepth;
 
   //vec3 ray, se, occNorm;
   float occluderDepth, depthDifference;
   vec4 occluderFragment;
   vec3 ray;
   for(int i=0; i&lt;SAMPLES;++i)
   {
	  // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
	  ray = radD*reflect(pSphere[i],fres);
 
	  // get the depth of the occluder fragment
	  occluderFragment = texture2D(normalMap,ep.xy + sign(dot(ray,norm) )*ray.xy);
	// if depthDifference is negative = occluder is behind current fragment
	  depthDifference = currentPixelDepth-occluderFragment.a;
 
	  // calculate the difference between the normals as a weight
	  // the falloff equation, starts at falloff and is kind of 1/x^2 falling
	  bl += step(falloff,depthDifference)*(1.0-dot(occluderFragment.xyz,norm))*(1.0-smoothstep(falloff,strength,depthDifference));
   }
 
   // output the result
   gl_FragColor.r = 1.0f+bl*invSamples;
}
*/