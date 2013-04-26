cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float4 WorldPosition : TEXCOORD0;
	float Depth : TEXCOORD1;
};

VertexShaderOutput MCubesGBufferVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.WorldPosition = mul(input.Position, World);
	output.Position = mul(input.Position, WorldViewProjection);

	output.Depth = output.Position.z/output.Position.w;

	output.WorldNormal = mul(input.Normal, World);

	return output;
}