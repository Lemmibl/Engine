cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float4 WorldPosition : TEXCOORD0;
	float2 Depth : TEXCOORD1;
	float3 WorldNormal : TEXCOORD2;
};

VertexShaderOutput MCubesGBufferVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.WorldPosition = mul(input.Position, World);
	float4 viewPosition = mul(output.WorldPosition, View);
	output.Position = mul(viewPosition, Projection);

	output.Normal = input.Normal;
	output.Depth = output.Position.zw;

	output.WorldNormal = mul(input.Normal, World);

	return output;
}