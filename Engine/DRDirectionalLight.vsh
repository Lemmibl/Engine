cbuffer VertexMatrixBuffer
{
	float4x4 WorldViewProjection;
	float4x4 WorldView;
	float4x4 World;
	float4x4 InvertedViewProjection;
	float4 CameraPosition;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float2 tex : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float4 ScreenPosition : TEXCOORD0;
	float2 TexCoord : TEXCOORD1;
};

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position = mul(input.Position, WorldViewProjection);
		output.ScreenPosition = output.Position;
		output.TexCoord = input.tex;

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