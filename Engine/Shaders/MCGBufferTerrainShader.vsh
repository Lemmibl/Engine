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

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float4 WorldPosition : TEXCOORD0;
	float4 ViewNormalAndDepth : TEXCOORD1; //Depth in .w
};

VertexShaderOutput MCGBufferTerrainVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	float4 pos = float4(input.Position.xyz, 1.0f);

	output.Position	= mul(pos, WorldViewProjection);

	//This is the proper way to do it
	//output.ViewDepth		= mul(input.Position, WorldView).z;

	//You can only do this if you are using a perspective projection matrix! Beware!
	output.ViewNormalAndDepth.w = output.Position.w;

	output.WorldPosition	= mul(pos, World);
	output.WorldNormal		= normalize(mul(input.Normal, (float3x3)World));
	output.ViewNormalAndDepth.xyz		= normalize(mul(input.Normal, (float3x3)WorldView));

	return output;
}