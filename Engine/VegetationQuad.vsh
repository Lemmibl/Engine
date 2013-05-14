cbuffer MatrixBufferType
{
	float4x4 worldViewProjection;
}

struct VertexInputType
{
	float4 Position : POSITION;
	float3 Normal :NORMAL;
	float2 TexCoord : TEXCOORD0;
	float4 InstancePosition : TEXCOORD1;
};

struct PixelInputType
{
	float4 Position : SV_POSITION;
	float3 Normal :NORMAL;
	float4 TexCoord : TEXCOORD0;
	float4 WorldPosition : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VegetationQuadVertexShader(VertexInputType input)
{
	PixelInputType output;
	
	// Change the position vector to be 4 units for proper matrix calculations.
	input.Position.w = 1.0f;

	// Update the position of the vertices based on the data for this particular instance.
	input.Position.xyz += input.InstancePosition.xyz;
	output.WorldPosition = float4(input.InstancePosition.xyz, 1.0f);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(input.Position, worldViewProjection);

	output.Normal = input.Normal;

	// Store the texture coordinates for the pixel shader.
	//We've hidden texture ID in instanceposition alpha channel to save a register slot
	output.TexCoord.xyz = float3(input.TexCoord, input.InstancePosition.w);
	
	// Store data in texcoord .zw channel for further processing in pixelshader
	output.TexCoord.w = output.Position.z / output.Position.w;


	return output;
}