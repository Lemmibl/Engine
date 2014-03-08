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
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float4 WorldPosition : TEXCOORD1;
	float ViewDepth : TEXCOORD2;
};

VertexShaderOutput GBufferVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	float4 pos = float4(input.Position, 1.0f);

	output.WorldPosition = mul(pos, World);

	float4 viewPosition = mul(output.WorldPosition, View);

	output.Position = mul(viewPosition, Projection);

	output.TexCoord = input.TexCoord;
	output.WorldNormal = mul(input.Normal, (float3x3)World);
	output.ViewDepth = viewPosition.z;

	return output;
}