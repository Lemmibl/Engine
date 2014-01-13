/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	float4x4 WorldViewProjection	: packoffset(c0.x);
	float cameraYPos				: packoffset(c4.x);
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float4 Position : POSITION;
};

struct PixelInputType
{
	float4 Position : SV_POSITION;
	float2 DomePositionYAndWorldY : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SkysphereVertexShader(VertexInputType input)
{
	PixelInputType output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.Position.w = 1.0f;

	output.DomePositionYAndWorldY.x = input.Position.y;
	output.DomePositionYAndWorldY.y = cameraYPos;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(input.Position, WorldViewProjection);

	return output;
}