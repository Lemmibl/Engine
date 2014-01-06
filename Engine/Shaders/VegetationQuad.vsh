cbuffer MatrixBufferType
{
	float4x4 WorldViewProjection;
	float4x4 WorldView;
	float4x4 World;
}

struct VertexInputType
{
	float4 Position : POSITION;
	float3 Normal :NORMAL;
	float2 TexCoord : TEXCOORD0;
	float4 InstancePosition : TEXCOORD1;
	float InstanceRandomValue : TEXCOORD2;
};

struct PixelInputType
{
	float4 Position : SV_POSITION;
	float3 Normal :	NORMAL;
	float4 TexCoord : TEXCOORD0;
};

static const float3 UpNormal = float3(0.0f, 1.0f, 0.0f);

//rotates a vector by a certain amount around an arbitrary axis.sdd
//http://en.wikipedia.org/wiki/Rodrigues'_rotation_formula
float3 RotateAroundAxis(in float3 vec, in float angle, in float3 axis)
{
	//http://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
	return (vec * cos(angle)) + (cross(axis,vec)*sin(angle)) + (axis * dot(axis,vec)*(1-cos(angle)));
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VegetationQuadVertexShader(VertexInputType input)
{
	PixelInputType output;

	float4 pos;
	
	// Change the position vector to be 4 units for proper matrix calculations.
	pos.w = 1.0f;

	// Update the position of the vertices based on the data for this particular instance.
	pos.xyz = input.InstancePosition.xyz + RotateAroundAxis(input.Position.xyz, input.InstanceRandomValue, UpNormal);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(pos, WorldViewProjection);

	output.Normal = input.Normal;//mul(input.Normal, World); //Currently it's probably not necessary to multiply it by world, but it has to be done if we introduce rotations for all instances in the future

	// Store the texture coordinates for the pixel shader.
	//We've hidden texture ID in instanceposition alpha channel to save a register slot
	output.TexCoord.xyz = float3(input.TexCoord, input.InstancePosition.w);
	
	// Store depth data in texcoord .W channel for further processing in pixelshader
	output.TexCoord.w = mul(pos, WorldView).z;

	return output;
}