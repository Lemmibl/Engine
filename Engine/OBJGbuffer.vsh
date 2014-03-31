cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

struct VertexShaderInput
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal	: NORMAL;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 Normal	: NORMAL;
	float3 TexCoord : TEXCOORD0; //Viewdepth in .z
};

VertexShaderOutput OBJGbufferVertex(VertexShaderInput input)
{
	VertexShaderOutput output;

	//Calculate view position so that we can extract view depth
	float4 viewPosition = mul(mul(float4(input.Position, 1.0f), World), View);

	//Transform viewposition by projection to turn it into the "final space"
	output.Position = mul(viewPosition, Projection);

	//Pass along texture coordinates
	output.TexCoord.xy = input.TexCoord;

	//Pass along surface normal, transformed by world matrix
	output.Normal = normalize(mul(input.Normal, (float3x3)World));

	//Save view depth in texcoord z channel
	output.TexCoord.z = viewPosition.z;

	return output;
}

/*
cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
};

struct VertexShaderInput
{
	float3 Position : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal	: NORMAL;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 Normal	: NORMAL;
	float3 TexCoord : TEXCOORD0; //Viewdepth in .z
};

VertexShaderOutput OBJGbufferVertex(VertexShaderInput input)
{
	VertexShaderOutput output;

	//Calculate view position so that we can extract view depth
	float4 viewPosition = mul(mul(float4(input.Position, 1.0f), World), View);

	//Transform viewposition by projection to turn it into the "final space"
	output.Position = mul(viewPosition, Projection);

	//Pass along texture coordinates
	output.TexCoord.xy = input.TexCoord;

	//Pass along surface normal, transformed by world matrix
	output.Normal = normalize(mul(input.Normal, (float3x3)World));

	//Save view depth in texcoord z channel
	output.TexCoord.z = viewPosition.z;

	return output;
}
*/