cbuffer MatrixVertexBuffer
{
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
};

struct VertexShaderOutput
{
	float4 Position : SV_Position;
	float4 ScreenPosition : TEXCOORD0;
};

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.Position = mul(input.Position, WorldViewProjection);

	output.ScreenPosition = output.Position;

	return output;
}
