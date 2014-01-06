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
	float1 Depth : TEXCOORD0;
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


VSOutput WaterShaderVS(VSInput input)
{
	VSOutput output;

	float4 pos = float4(input.Position.xyz, 1.0f);

	output.Position = pos;//mul(pos, WorldViewProjection);
	output.Depth = mul(pos, WorldView).z; //AndRand.x
	//output.DepthAndRand.y = random(float2(input.Position.z*43.5f, input.Position.x*11.0f));

	return output;
}