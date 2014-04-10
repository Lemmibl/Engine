
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

cbuffer PixelMatrixBufferType
{
	float4x4 View;
	float4x4 InvertedProjection;
	float4 FogColor;
};

cbuffer VariableBuffer
{
	int toggleSSAO		: packoffset(c0.x);
	float sampleRadius	: packoffset(c0.y);	//Controls sampling radius. 0.1f to 1.0f are pretty ok values.
	float intensity		: packoffset(c0.z); 	//AO intensity. The higher this value is, the darker the occluded parts will be. 1.0f to 10.0f values is pretty ok values.
	float scale			: packoffset(c0.w);		//Scales distance between occluders and occludee. Still a little unsure as to what values would be good to use.
	float bias			: packoffset(c1.x);			//Cutoff value. The higher this value is, the harsher we are with cutting off low AO values. 0.01f to 0.4f values are pretty ok.
	float fogStart		: packoffset(c1.y);
	float fogEnd		: packoffset(c1.z);
	float farClip		: packoffset(c1.w);
	float waterLevel	: packoffset(c2.x);
	float cameraHeight	: packoffset(c2.y);
	float2 randomSize	: packoffset(c2.z);
	float2 screenSize	: packoffset(c3.x);
	float2 PADDING		: packoffset(c3.z);
};

//Decoding of GBuffer Normals
float3 DecodeNormal(float2 enc)
{
	float2 fenc = (enc*4.0f)-2.0f;
	float f = dot(fenc,fenc);
	float g = sqrt(1.0f-(f*0.25f));
	
	return float3(fenc*g, 1.0f - (f*0.5f));
}

static const float2 vecArray[4] = { float2(1,0), float2(-1,0), float2(0,1), float2(0,-1) };

float3 DepthToPosition(float4 ScreenPosition, float depth)
{
	float3 viewPosition = mul(ScreenPosition, InvertedProjection).xyz;
	return float3(viewPosition.xy * (farClip / viewPosition.z), farClip) * depth;
}

float3 ReconstructViewPositionFromDepth(float3 viewRay, float depth)
{
	return float3(viewRay.xy * (farClip / viewRay.z), farClip) * depth;
}

float DoAmbientOcclusion(float2 TexCoord, float2 Offset, float4 ScreenPosition, float3 Position, float3 Normal)
{
	//Sample depth from our new offset position
	float offsetDepth = shaderTextures[2].Sample(samplers[0], (TexCoord + Offset));
	
	//Reconstruct an offset position with the help of our offset screenpos and offset depth
	float3 offsetPosition = DepthToPosition(ScreenPosition, offsetDepth); //float3(TexCoord + Offset, offsetDepth);//

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
		float fogFactor = saturate(((depth*farClip) - fogStart) / (fogEnd - fogStart));
		float4 fogColor = FogColor;

		if(cameraHeight < waterLevel)
		{
			//Everything is darker because we're below the water
			baseColor.xyz -= 0.2f;

			//Our vision range is shorter because underwater
			fogFactor += 0.95f;

			//Make the fog a little darker too.
			fogColor -= 0.3f;
		}
		else if(depth == 0.0f) //Skip as early as possible if this pixel isn't supposed to be lit. (Skysphere for example.)
		{
			return float4(baseColor, 1.0f);
		}

		if(fogFactor >= 1.0f)
		{
			return fogColor; //Cut early. Only return fog color.
		}

		float3 position = ReconstructViewPositionFromDepth(input.ViewPosition.xyz, depth);
		float3 normal = normalize(mul(DecodeNormal(shaderTextures[3].Sample(samplers[0], input.TexCoord).xy * 2.0f - 1.0f), (float3x3)View)); //Viewspace normals

		float ao = 1.0f;		
		
		if(toggleSSAO >= 1)
		{
			float2 rand = normalize(randomTexture.Sample(samplers[0], (screenSize*input.TexCoord / randomSize))  * 2.0f - 1.0f);
			float rad = sampleRadius * (position.z/farClip);
			int iterations = lerp(4, 1, depth); //primitive SSAO LOD; scales with how close to the pixel we are

			//SSAO Calculation
			[unroll]
			for (int i = 0; i < iterations; i++)
			{
				float2 coord1 = reflect(vecArray[i], rand)*rad;
				float2 coord2 = float2(	coord1.x*0.707 - coord1.y*0.707,
										coord1.x*0.707 + coord1.y*0.707);
			
				ao += DoAmbientOcclusion(input.TexCoord, coord1*0.25,	input.ScreenPosition, position, normal);
				ao += DoAmbientOcclusion(input.TexCoord, coord2*0.5,	input.ScreenPosition, position, normal);
				ao += DoAmbientOcclusion(input.TexCoord, coord1*0.75,	input.ScreenPosition, position, normal);
				ao += DoAmbientOcclusion(input.TexCoord, coord2,		input.ScreenPosition, position, normal);
			}

			ao /= (float)(iterations*4.0f);

			//Add AO to final calculation ...
			ao = max((1.0f - ao), 0.0f); //max((1.0f - ao), 0.4f); //We don't want the ssao to make things completely black (0.0f), so we cap it at a minimum of 0.4f.
		}



		//specular intensity is stored in the color map's alpha channel.
		float4 light = shaderTextures[1].Sample(samplers[0], input.TexCoord);
		float3 diffuseLight = light.rgb;
		float specularLight = light.a;
		
		if(toggleSSAO == 2)
		{
			return float4(ao, ao, ao, 1.0f);//lerp((float4((ao * baseColor) * (diffuseLight + specularLight), 1.0f)),  FogColor, fogFactor); // float4(ao, ao, ao, 1.0f);//
		}
		else
		{
			//Default case
			return ao * lerp((float4((baseColor.xyz) * (diffuseLight + specularLight), 1.0f)),  fogColor, fogFactor);
		}
}