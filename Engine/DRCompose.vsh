cbuffer VertexMatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
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
		float4 ScreenPosition : TEXCOORD1;
		float4 WorldPosition : TEXCOORD2;
};

//TODO Make a LightDirection + CameraPosition cbuffer that gets sent to vertexbuffer.

VertexShaderOutput ComposeVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;
		output.WorldPosition = mul(input.Position, World);
		output.Position = mul(output.WorldPosition, View);
		output.Position = mul(output.Position, Projection);

		output.ScreenPosition = output.Position;
		
		output.Tex = input.tex;

		return output;
}