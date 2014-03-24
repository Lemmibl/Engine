/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	float4x4 WorldViewProjection	: packoffset(c0.x);
	float4x4 World					: packoffset(c4.x);
	float cameraYPos				: packoffset(c8.x);
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
	float3 Position : POSITION;
	//float3 TexCoord : TEXCOORD0;
	//float3 Normal	: NORMAL;

};

struct PixelInputType
{
	float4 Position : SV_POSITION;
	float2 DomePositionYAndWorldY : TEXCOORD0; //face ID in .z channel
	float3 TexCoord : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SkysphereVertexShader(VertexInputType input)
{
	PixelInputType output;

	float4 pos = float4(input.Position, 1.0f);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(pos, WorldViewProjection);

	//We use the model's ownspace coordinates as texture coordinates.
	output.TexCoord = input.Position;//input.TexCoord;

	//Save ownspace y coordinate to scale coloring
	output.DomePositionYAndWorldY.x = pos.y;

	//Camera height in worldspace. Used to scale the fog.
	output.DomePositionYAndWorldY.y = cameraYPos;

	return output;
}