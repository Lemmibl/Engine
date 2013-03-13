cbuffer VertexMatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 LightViewProjection;
};

struct VertexShaderInput
{
		float4 Position : POSITION0;
		float2 tex : TEXCOORD0;
};

struct VertexShaderOutput
{
		float4 Position : SV_POSITION;
		float2 Tex : TEXCOORD0;
		float4 LightViewPosition : TEXCOORD1;
		float4 ViewPosition : TEXCOORD2;
};

//TODO Make a LightDirection + CameraPosition cbuffer that gets sent to vertexbuffer.

VertexShaderOutput ComposeVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position =	mul(input.Position, World);
		output.ViewPosition =		mul(output.Position, View);
		output.Position =		mul(output.ViewPosition, Projection);

		output.LightViewPosition = mul(input.Position, World);
		output.LightViewPosition = mul(output.LightViewPosition, LightViewProjection);
		
		output.Tex = input.tex;

		return output;
}