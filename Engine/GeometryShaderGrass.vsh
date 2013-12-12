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

// Input: It uses texture coords as the random number seed.
// Output: Random number: [0,1), that is between 0.0 and 0.999999... inclusive.
// Author: Michael Pohoreski
// Copyright: Copyleft 2012 :-)
float random( float2 p )
{
  // We need irrationals for pseudo randomness.
  // Most (all?) known transcendental numbers will (generally) work.
  const float2 r = float2(
	23.1406926327792690f,  // e^pi (Gelfond's constant)
	 2.6651441426902251f); // 2^sqrt(2) (Gelfond–Schneider constant)
  return frac( cos( fmod( 123456789.0f, 1e-7 + 256.0f * dot(p,r) ) ) );  
}

VS_OUTPUT GrassVS(VertexShaderInput input)
{
	VS_OUTPUT output;

	output.Position	= input.Position;//mul(input.Position, WorldViewProjection);
	output.Normal = float4(input.Normal.xyz, 1.0f);

	output.YPosDepthAndRand.x	= mul(input.Position, World).y;

	//You can only do this if you are using a perspective projection matrix! Beware!
	output.YPosDepthAndRand.y = mul(input.Position, WorldView).z;

	output.YPosDepthAndRand.z = random(float2(input.Position.z, input.Position.x));

	return output;
}