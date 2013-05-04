cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	uint IDValues : BLENDINDICES;
	float LerpValues : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float4 WorldPosition : TEXCOORD0;
	float Depth : TEXCOORD1;
	float LerpValues : TEXCOORD2;
	uint IDValues : BLENDINDICES;
};

VertexShaderOutput MCubesGBufferVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.WorldPosition = mul(input.Position, World);
	output.Position = mul(input.Position, WorldViewProjection);

	output.Depth = output.Position.z/output.Position.w;

	output.WorldNormal = mul(input.Normal, World);
	output.LerpValues = input.LerpValues;
	output.IDValues = input.IDValues;

	return output;
}