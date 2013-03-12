cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float4 ViewPosition : TEXCOORD0;
};

VertexShaderOutput DepthOnlyVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	float4 worldPosition = mul(input.Position, World);
	output.ViewPosition = mul(worldPosition, View);
	output.Position = mul(output.ViewPosition, Projection);

	return output;
}