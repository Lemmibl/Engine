Texture2D noiseTexture;
SamplerState linearSampler;

//TODO: Recieve world matrix for normals.
cbuffer TimeBuffer
{
	float4x4 WorldViewProjection;
	float DeltaTime;
}

struct VSOutput
{
	float4 Position : SV_POSITION;
	float Depth : TEXCOORD0;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float4 TexCoord	: TEXCOORD0;
};

//1 - 0.04745
static const float positionSamplingOffset = 0.2f;
static const float2 samplingDirection = float2(0.5f, 0.0f);
static const float heightScaling = 0.1f;
static const float timeScaling = 0.45f;
static const float4 UpNormal = normalize(float4(0.0f, 1.0f, 0.0f, 0.0f));

//http://msdn.microsoft.com/en-us/library/windows/desktop/bb205122(v=vs.85).aspx
[maxvertexcount(3)] 
void WaterShaderGS(triangle VSOutput input[3], inout TriangleStream<PSInput> TriStream)
{ 
	PSInput output[3];
	float scaledDeltaTime = (DeltaTime * timeScaling);

	float viewDepth = 1.0f - (input[0].Depth/400.0f);

	output[0].TexCoord.xy = (input[0].Position.xz*positionSamplingOffset)+(samplingDirection * scaledDeltaTime);
	output[1].TexCoord.xy = (input[1].Position.xz*positionSamplingOffset)+(samplingDirection * scaledDeltaTime);
	output[2].TexCoord.xy = (input[2].Position.xz*positionSamplingOffset)+(samplingDirection * scaledDeltaTime);

	output[0].TexCoord.z = noiseTexture.SampleLevel(linearSampler, output[0].TexCoord.xy, 0) * heightScaling;
	output[1].TexCoord.z = noiseTexture.SampleLevel(linearSampler, output[1].TexCoord.xy, 0) * heightScaling;
	output[2].TexCoord.z = noiseTexture.SampleLevel(linearSampler, output[2].TexCoord.xy, 0) * heightScaling;

	float4 pos1 = (input[0].Position);
	float4 pos2 = (input[1].Position);
	float4 pos3 = (input[2].Position);

	if(viewDepth >= 0.5f)
	{
		pos1 += (UpNormal * output[0].TexCoord.z);
		pos2 += (UpNormal * output[1].TexCoord.z);
		pos3 += (UpNormal * output[2].TexCoord.z);
	}


	output[0].Position = mul(pos1, WorldViewProjection);
	output[1].Position = mul(pos2, WorldViewProjection);
	output[2].Position = mul(pos3, WorldViewProjection);

	output[0].TexCoord.w = input[0].Depth / 400.0f; // We store depth in .w channel.
	output[1].TexCoord.w = input[1].Depth / 400.0f; // We store depth in .w channel.
	output[2].TexCoord.w = input[2].Depth / 400.0f; // We store depth in .w channel.

	//Add all vertices to out stream
	TriStream.Append(output[0]);
	TriStream.Append(output[1]);
	TriStream.Append(output[2]);

	//Restart triangle strip for next run
	TriStream.RestartStrip();
}