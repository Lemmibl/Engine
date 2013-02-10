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
};

//TODO Make a LightDirection + CameraPosition cbuffer that gets sent to vertexbuffer.

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position = mul(input.Position, World);
		output.Position = mul(output.Position, View);
		output.Position = mul(output.Position, Projection);

		output.TexCoord = input.tex;

		return output;
}