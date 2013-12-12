cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldView;
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Normal : TEXCOORD0;
	float2 WorldYAndViewDepth : TEXCOORD1;
};

VS_OUTPUT GrassVS(VertexShaderInput input)
{
	VS_OUTPUT output;

	output.Position	= input.Position;//mul(input.Position, WorldViewProjection);
	output.Normal = float4(input.Normal, 1.0f);

	output.WorldYAndViewDepth.x	= mul(input.Position, World).y;

	//You can only do this if you are using a perspective projection matrix! Beware!
	output.WorldYAndViewDepth.y = mul(input.Position, WorldView).z;

	return output;
}