cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldView;
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
	uint4 IDValues : COLOR0;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float4 WorldPosition : TEXCOORD0;
	float3 ViewPosition : TEXCOORD1;
	uint4 IDValues : TEXCOORD2;
};

VertexShaderOutput MCubesGBufferVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.WorldPosition = mul(input.Position, World);
	output.WorldNormal = mul(input.Normal, World);

	output.ViewPosition = mul(input.Position, WorldView).xyz;
	
	output.Position = mul(input.Position, WorldViewProjection);

	output.IDValues = input.IDValues;

	return output;
}