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
	float largeWavePositionSamplingOffset : packoffset(c14.y);
	float largeWaveHeightOffset		: packoffset(c14.z);
	float largeWaveTimeSamplingOffset : packoffset(c14.w);
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

	//Large wave coordinates
	float2 bigWaveTexCoord = ((input.Position.xz*positionSamplingOffset)+(normalize(windDirection.xz) * (scaledDeltaTime*4.0f))) * 0.07f;

	//Read and scale the height that we sample from the noise texture to create small ripples
	output.TexCoord.z = heightScaling * noiseTexture.SampleLevel(linearSampler, output.TexCoord.xy, 0);

	//Then once more with bigger coordinates and a larger height scaling for actual big waves
	output.TexCoord.z += 1.3f * noiseTexture.SampleLevel(linearSampler, bigWaveTexCoord, 0);

	//Finally, take our vertex position, offset it upwards based on the height value we loaded earlier, and transform it with WVP matrix.
	output.Position = mul((pos + (UpNormal * output.TexCoord.z)), WorldViewProjection);

	//Save linear depth
	output.TexCoord.w = output.Position.w / farClip; //mul(pos, WorldView).z / farClip;

	return output;
}