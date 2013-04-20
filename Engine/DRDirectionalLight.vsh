cbuffer VertexMatrixBuffer
{
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
		float4 Position : POSITION;
		float2 tex : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 ScreenPosition : TEXCOORD1;
};

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position =		mul(input.Position, WorldViewProjection);

		output.ScreenPosition = output.Position;
		output.TexCoord = input.tex;

		return output;
}