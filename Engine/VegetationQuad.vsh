cbuffer MatrixBufferType
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
}

struct VertexInputType
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 InstancePosition : TEXCOORD1;
};

struct PixelInputType
{
	float4 Position : SV_POSITION;
	float4 TexCoord : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VegetationQuadVertexShader(VertexInputType input)
{
	PixelInputType output;
	
	output.TexCoord.w = (int)input.InstancePosition.w+0.3f; //We've hidden texture ID in instanceposition alpha channel to save a register slot

	// Change the position vector to be 4 units for proper matrix calculations.
	input.Position.w = 1.0f;

	// Update the position of the vertices based on the data for this particular instance.
	input.Position.xyz += input.InstancePosition.xyz;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(input.Position, worldMatrix);
	output.Position = mul(output.Position, viewMatrix);
	output.Position = mul(output.Position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.TexCoord.xy = input.TexCoord;
	
	// Store data in texcoord .zw channel for further processing in pixelshader
	output.TexCoord.z = output.Position.z / output.Position.w;


	return output;
}