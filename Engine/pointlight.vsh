cbuffer MatrixVertexBuffer
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
	float4 Position : SV_Position;
	float4 ScreenPosition : TEXCOORD0;
	float4 WorldPosition : TEXCOORD1;
	float4 ViewPosition : TEXCOORD2;
};

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.WorldPosition = mul(input.Position, World);
	output.ViewPosition = mul(output.WorldPosition, View);
	output.Position = mul(output.ViewPosition, Projection);

	output.ScreenPosition = output.Position;

	return output;
}
