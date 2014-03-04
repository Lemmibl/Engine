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
	float3 Normal : NORMAL;
	float2 TexCoord : TEXCOORD0;
	float ViewDepth : TEXCOORD1;
};

VertexShaderOutput OBJGbufferVertex(VertexShaderInput input)
{
	VertexShaderOutput output;

	//Calculate view position so that we can extract view depth
	float4 viewPosition =	mul(float4(input.Position, 1.0f), mul(World, View));

	//Calculate world view projection
	output.Position = mul(viewPosition, Projection);

	//Pass along texture coordinates
	output.TexCoord		= input.TexCoord;

	//Pass along surface normal
	output.Normal		= normalize(input.Normal);

	//Get view depth
	output.ViewDepth	= viewPosition.z;

	return output;
}