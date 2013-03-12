cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

struct VertexShaderInput
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float4 ViewPosition : TEXCOORD1;
	float3x3 TangentToWorld : TEXCOORD2;
};

VertexShaderOutput GBufferVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	float4 worldPosition = mul(float4(input.Position, 1.0f), World);
	output.ViewPosition = mul(worldPosition, View);
	output.Position = mul(output.ViewPosition, Projection);

	output.TexCoord = input.TexCoord;
	output.Normal = input.Normal;

	// calculate tangent space to world space matrix using the world space tangent,
	// binormal, and normal as basis vectors
	output.TangentToWorld[0] = mul(input.Tangent, World);
	output.TangentToWorld[1] = mul(input.Binormal, World);
	output.TangentToWorld[2] = mul(input.Normal, World);

	return output;
}