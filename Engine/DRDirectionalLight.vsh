cbuffer VertexMatrixBuffer
{
	float4x4 WorldViewProjection;
	float4x4 WorldView;
	float3 CameraPosition;
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
	float3 ViewPosition : TEXCOORD1;
};

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position =	mul(input.Position, WorldViewProjection);
		output.TexCoord = input.tex;
		output.ViewPosition = mul(input.Position, WorldView).xyz;

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