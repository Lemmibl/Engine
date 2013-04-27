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
		float4 ScreenPosition : TEXCOORD0;
		float2 TexCoord : TEXCOORD1;
};

//TODO Make a LightDirection + CameraPosition cbuffer that gets sent to vertexbuffer.

VertexShaderOutput ComposeVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position = mul(input.Position, WorldViewProjection);
		output.ScreenPosition = output.Position;

		output.TexCoord = input.TexCoord;

		return output;
}