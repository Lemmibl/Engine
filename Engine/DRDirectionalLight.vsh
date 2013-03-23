cbuffer VertexMatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
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
		float4 ViewPosition : TEXCOORD1;
		float4 ScreenPosition : TEXCOORD2;
		float4 WorldPosition : TEXCOORD3;
};

//TODO Make a LightDirection + CameraPosition cbuffer that gets sent to vertexbuffer.

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.WorldPosition =		mul(input.Position, World);
		output.ViewPosition =		mul(output.WorldPosition, View);
		output.Position =			mul(output.ViewPosition, Projection);

		output.ScreenPosition = output.Position;

		output.TexCoord = input.tex;

		return output;
}