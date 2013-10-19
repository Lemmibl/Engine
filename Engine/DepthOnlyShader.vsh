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
	float4 ViewPosition : TEXCOORD0;
};

VertexShaderOutput DepthOnlyVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	float4 pos = input.Position;
	pos.w = 1.0f;

	output.Position = mul(pos, WorldViewProjection);
	output.ViewPosition = mul(pos, WorldView);

	return output;
}