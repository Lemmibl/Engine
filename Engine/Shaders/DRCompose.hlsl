
struct VertexShaderOutput
{
		float4 Position : SV_POSITION;
		float4 ScreenPosition : TEXCOORD0;
		float4 ViewPosition : TEXCOORD1;
		float2 TexCoord : TEXCOORD2;
};

SamplerState linearSampler;
Texture2D randomTexture : register(ps_5_0, t0);
Texture2D shaderTextures[4] : register(ps_5_0, t1); // 0 = color, 1 = light, 2 = depth, 3 = SSAO

cbuffer PixelMatrixBufferType
{
	float4x4 View;
	float4x4 InvertedProjection;
	float4 FogColor;
};

cbuffer VariableBuffer
{
	int toggleSSAO		: packoffset(c0.x);
	//float sampleRadius	: packoffset(c0.y);	//Controls sampling radius. 0.1f to 1.0f are pretty ok values.
	//float intensity		: packoffset(c0.z); 	//AO intensity. The higher this value is, the darker the occluded parts will be. 1.0f to 10.0f values is pretty ok values.
	//float scale			: packoffset(c0.w);		//Scales distance between occluders and occludee. Still a little unsure as to what values would be good to use.
	//float bias			: packoffset(c1.x);			//Cutoff value. The higher this value is, the harsher we are with cutting off low AO values. 0.01f to 0.4f values are pretty ok.
	float fogStart		: packoffset(c0.y);
	float fogEnd		: packoffset(c0.z);
	float farClip		: packoffset(c0.w);
	float waterLevel	: packoffset(c1.x);
	float cameraHeight	: packoffset(c1.y);
	float2 randomSize	: packoffset(c1.z);
	float2 screenSize	: packoffset(c2.x);
	float2 PADDING		: packoffset(c2.z);
};

//Decoding of GBuffer Normals
half3 DecodeNormal(half2 enc)
{
	half2 fenc = enc*4-2;
	half f = dot(fenc,fenc);
	half g = sqrt(1-f/4);
	return half3(fenc*g, 1-f/2);
}

float4 ComposePixelShader(VertexShaderOutput input) : SV_Target0
{
		
		float depth = shaderTextures[2].Sample(linearSampler, input.TexCoord);
		float fogFactor = saturate(((depth*farClip) - fogStart) / (fogEnd - fogStart));

		//These are all variables that need to be predeclared because they might be relevant in case we become underwater
		float3 baseColor = shaderTextures[0].Sample(linearSampler, input.TexCoord).rgb;
		float4 fogColor = FogColor;
		float lightIntensity = 1.0f;

		if((cameraHeight-1) <= waterLevel)
		{
			//Scaling factor based on how far down into the water we are
			float waterDepth = clamp(1.0f - (0.04f * abs(waterLevel - cameraHeight)), 0.2f, 1.0f);

			//Light intensity diminishes the further down we go
			lightIntensity *= waterDepth;
			
			//Our vision range is shorter because underwater. abs because if we go down far enough it'll just flip around and go negative, and in that case we just want the fog to cover everything.. so...
			fogFactor = abs(1.0f - (waterDepth*0.3f));

			//Everything is darker because we're below the water
			baseColor.xyz -= (lightIntensity * float3(0.15f, 0.1f, 0.0f));

			//Make the fog a little darker too.
			fogColor.xyz -= (fogFactor * float3(0.3f, 0.3f, 0.0f));
		}
		else if(depth == 0.0f) 
		{	//Skip as early as possible if this pixel isn't supposed to be lit or affected by anything. (Skysphere for example.) 
			//Exception being if we're underwater, then don't treat skysphere specially, because we're viewing everything through water.
			return float4(baseColor, 1.0f);
		}

		//specular intensity is stored in the color map's alpha channel.
		float4 light = shaderTextures[1].Sample(linearSampler, input.TexCoord) * lightIntensity;
		float3 diffuseLight = light.rgb;
		float specularLight = light.a;
		float ao = shaderTextures[3].Sample(linearSampler, input.TexCoord);
		
		if(toggleSSAO == 1)
		{
			return float4(ao, ao, ao, 1.0f);//lerp((float4((ao * baseColor) * (diffuseLight + specularLight), 1.0f)),  FogColor, fogFactor); // float4(ao, ao, ao, 1.0f);//
		}
		else
		{
			//Default case
			return lerp(ao * (float4((baseColor.xyz) * (diffuseLight + specularLight), 1.0f)),  fogColor, fogFactor);
		}
}