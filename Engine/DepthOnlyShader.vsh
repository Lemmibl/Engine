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
	float Depth : TEXCOORD0;
};

VertexShaderOutput DepthOnlyVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.Position = mul(input.Position, World);
	output.Position = mul(output.Position, View);
	output.Position = mul(output.Position, Projection);

	output.Depth = output.Position.z / output.Position.w;

	return output;
}