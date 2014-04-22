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
	float3 YPosDepthAndRand : TEXCOORD1;
};

//// Input: It uses texture coords as the random number seed.
//// Output: Random number: [0,1), that is between 0.0 and 0.999999... inclusive.
//// Author: Michael Pohoreski
//// Copyright: Copyleft 2012 :-)
//float random( float2 p )
//{
//  // We need irrationals for pseudo randomness.
//  // Most (all?) known transcendental numbers will (generally) work.
//  const float2 r = float2(
//	23.1406926327792690f,  // e^pi (Gelfond's constant)
//	 2.6651441426902251f); // 2^sqrt(2) (Gelfond–Schneider constant)
//  return frac( cos( fmod( 123456789.0f, 1e-7 + 256.0f * dot(p,r) ) ) );  
//}

VS_OUTPUT GrassVS(VertexShaderInput input)
{
	VS_OUTPUT output;

	float4 pos = float4(input.Position.xyz, 1.0f);

	//Don't transform. We transform in geometry shader.
	output.Position	= pos;
	output.Normal = normalize(mul(float4(input.Normal.xyz, 1.0f), World));
	
	//This NEEDS to be 0.0f, because of how I calculate the mix between this and UpNormal in the geometry shader
	output.Normal.w = 0.0f;

	//World Y pos. Will be used for determining texture lookup table index later.
	output.YPosDepthAndRand.x = mul(pos, World).y;

	//You can only do this if you are using a perspective projection matrix! Beware!
	output.YPosDepthAndRand.y = mul(pos, WorldViewProjection).w;

	//Creates a random value with world positions as seeds. Means it'll never change. TODO: Move to cpu/mesh creation? Probably needed.
	output.YPosDepthAndRand.z = input.Position.w;//random(float2(input.Position.z*3.5f, input.Position.x*7.0f));

	return output;
}