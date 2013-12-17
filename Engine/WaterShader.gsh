
Texture2D noiseTexture;
SamplerState linearSampler;

//TODO: Recieve world matrix for normals.
cbuffer TimeBuffer
{
	float4x4 WorldViewProjection;
	float4x4 World;
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
	float4 Normal	: NORMAL;
	float WaterHeight : TEXCOORD0;
};

//1 - 0.04745
static const float positionSamplingOffset = 0.02f;
static const float2 samplingDirection = float2(1.0f, 0.0f);


//http://msdn.microsoft.com/en-us/library/windows/desktop/bb205122(v=vs.85).aspx
[maxvertexcount(3)] 
void WaterShaderGS(triangle VSOutput input[3], inout TriangleStream<PSInput> TriStream)
{ 
	PSInput output[3];

	float4 UpNormal = normalize(float4(0.0f, 1.0f, 0.0f, 0.0f));
	float scaledDeltaTime = (0.04f*DeltaTime);

	//float2 samplingPositions[3];
	//samplingPositions[0] = input[0].Position.xz;
	//samplingPositions[1] = input[1].Position.xz;
	//samplingPositions[2] = input[2].Position.xz;

	output[0].WaterHeight = noiseTexture.SampleLevel(linearSampler, ((input[0].Position.xz*positionSamplingOffset)+(samplingDirection * scaledDeltaTime)), 0) * 0.3f;
	output[1].WaterHeight = noiseTexture.SampleLevel(linearSampler, ((input[1].Position.xz*positionSamplingOffset)+(samplingDirection * scaledDeltaTime)), 0) * 0.3f;
	output[2].WaterHeight = noiseTexture.SampleLevel(linearSampler, ((input[2].Position.xz*positionSamplingOffset)+(samplingDirection * scaledDeltaTime)), 0) * 0.3f;

	output[0].Position = mul(input[0].Position + (UpNormal * output[0].WaterHeight), WorldViewProjection);
	output[1].Position = mul(input[1].Position + (UpNormal * output[1].WaterHeight), WorldViewProjection);
	output[2].Position = mul(input[2].Position + (UpNormal * output[2].WaterHeight), WorldViewProjection);


	//Calculate face normal
	float4 faceEdgeA = (output[1].Position - output[0].Position);
	float4 faceEdgeB = (output[2].Position - output[0].Position);
	float4 faceNormal;

	faceNormal = normalize(mul(cross(faceEdgeA, faceEdgeB), World));

	output[0].Normal = faceNormal;
	output[1].Normal = faceNormal;
	output[2].Normal = faceNormal;

	output[0].Normal.w = input[0].Depth; // We store depth in .w channel.
	output[1].Normal.w = input[1].Depth; // We store depth in .w channel.
	output[2].Normal.w = input[2].Depth; // We store depth in .w channel.

	//Add all vertices to out stream
	TriStream.Append(output[0]);
	TriStream.Append(output[1]);
	TriStream.Append(output[2]);
	
	//Restart triangle strip for next run
	TriStream.RestartStrip();
}