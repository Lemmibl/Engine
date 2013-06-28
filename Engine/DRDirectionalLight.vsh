cbuffer VertexMatrixBuffer
{
	float4x4 WorldViewProjection;
	float4x4 World;
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
	float2 TexCoord : TEXCOORD0;
	float3 ViewRay : TEXCOORD1;
};

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position =		mul(input.Position, WorldViewProjection);

		float4 WorldPosition = mul(input.Position, World);

		//TODO: Get worldView matrix in here and multiply to produce viewPos...
		output.ViewRay = (WorldPosition - CameraPosition).xyz;
		output.TexCoord = input.tex;

		return output;
}