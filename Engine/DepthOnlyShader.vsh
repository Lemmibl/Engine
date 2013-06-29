cbuffer MatrixBuffer
{
	float4x4 WorldViewProjection;
	float4x4 WorldView;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 ViewPosition : TEXCOORD0;
};

VertexShaderOutput DepthOnlyVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.Position = mul(input.Position, WorldViewProjection);

	output.ViewPosition = mul(input.Position, WorldView).xyz;

	return output;
}