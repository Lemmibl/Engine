cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldView;
	float4x4 WorldViewProjection;
}

struct VSInput
{
	float3 Position : POSITION;
};

struct VSOutput
{
	float4 Position : SV_POSITION;
	float Depth : TEXCOORD0;
};

VSOutput WaterShaderVS(VSInput input)
{
	VSOutput output;

	float4 pos = float4(input.Position.xyz, 1.0f);

	output.Position = pos;//mul(pos, WorldViewProjection);
	output.Depth = mul(pos, WorldView).z;

	return output;
}