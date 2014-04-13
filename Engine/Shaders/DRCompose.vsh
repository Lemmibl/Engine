cbuffer VertexMatrixBuffer
{
	float4x4 WorldViewProjection;
	float4x4 WorldView;
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
		float4 ViewPosition : TEXCOORD1;
		float2 TexCoord : TEXCOORD2;
};

VertexShaderOutput ComposeVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position = mul(input.Position, WorldViewProjection);
		output.ScreenPosition = output.Position;
		output.ViewPosition = mul(input.Position, WorldView);

		output.TexCoord = input.TexCoord;

		return output;
}

/*

// Light vertex shader

#if PointLight || SpotLight
// Calculate the view space vertex position
output.PositionVS = mul(input.PositionOS, WorldViewMatrix);

#elif DirectionalLight
// Calculate the view space vertex position (you can also just directly map the vertex to a frustum corner to avoid the transform)
output.PositionVS = mul(input.PositionOS, InvProjMatrix);
#endif


*/