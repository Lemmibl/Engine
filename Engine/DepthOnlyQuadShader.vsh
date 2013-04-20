cbuffer MatrixBufferType
{
	float4x4 WorldViewProjection;
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

PixelInputType DepthOnlyVertexShader(VertexInputType input)
{
	PixelInputType output;

	output.TexCoord.w = (int)input.InstancePosition.w; //Save texture ID in TexCoord.w to save a channel
	
	// Change the position vector to be 4 units for proper matrix calculations.
	input.Position.w = 1.0f;

	// Update the position of the vertices based on the data for this particular instance.
	input.Position.xyz += input.InstancePosition.xyz;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.Position = mul(input.Position, WorldViewProjection);

	output.TexCoord.xy = input.TexCoord;
	output.TexCoord.z = (output.Position.z / output.Position.w); //Store depth in texcoord. I pack everything into a float4 to save space.
	
	return output;
}