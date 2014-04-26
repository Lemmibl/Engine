cbuffer VertexMatrixBuffer
{
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
		float4 Position : POSITION0;
		float2 TexCoord : TEXCOORD0;
};

struct VertexShaderOutput
{
		float4 Position : SV_POSITION;
		float2 TexCoord : TEXCOORD0;
};

VertexShaderOutput ComposeVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position = mul(input.Position, WorldViewProjection);
		output.TexCoord = input.TexCoord;

		return output;
}
