cbuffer AnimationDataBuffer
{
	float4x4 World					: packoffset(c0.x);
	float4x4 WorldView				: packoffset(c4.x);
	float4x4 WorldViewProjection	: packoffset(c8.x);
	float DeltaTime					: packoffset(c12.x);
	float positionSamplingOffset	: packoffset(c12.y);
	float heightScaling				: packoffset(c12.z);
	float timeScaling				: packoffset(c12.w);
	float farClip					: packoffset(c13.x);
	float3 windDirection			: packoffset(c13.y);
	float waterHeight				: packoffset(c14.x);
}

static const float4 UpNormal = normalize(float4(0.0f, 1.0f, 0.0f, 0.0f));

struct VSInput
{
	float3 Position : POSITION;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float4 TexCoord	: TEXCOORD0;
};

Texture2D noiseTexture;
SamplerState linearSampler;

VSOutput WaterAnimationVertexShader(VSInput input)
{
	VSOutput output;

	float scaledDeltaTime = (DeltaTime * timeScaling);
	float4 pos = float4(input.Position.xyz, 1.0f);

	//Calculate where we'll be sampling from, based on original vertex position, then offset by size offset, wind direction and delta time
	output.TexCoord.xy = ((input.Position.xz*positionSamplingOffset)+(normalize(windDirection.xz) * scaledDeltaTime));

	//Read and scale the height that we sample from the noise texture
	output.TexCoord.z = heightScaling * noiseTexture.SampleLevel(linearSampler, output.TexCoord.xy, 0);

	//Save linear depth
	output.TexCoord.w = mul(pos, WorldView).z / farClip;

	//Finally, take our vertex position, offset it upwards based on the height value we loaded earlier, and transform it with WVP matrix.
	output.Position = mul((pos + (UpNormal * output.TexCoord.z)), WorldViewProjection);

	return output;
}