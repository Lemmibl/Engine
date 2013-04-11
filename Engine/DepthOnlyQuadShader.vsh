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
	float2 TexCoord : TEXCOORD0;
	int TextureID : TEXCOORD1;
};

PixelInputType DepthOnlyVertexShader(VertexInputType input)
{
    PixelInputType output;

	output.TextureID = (int)input.InstancePosition.w;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.Position.w = 1.0f;

    // Update the position of the vertices based on the data for this particular instance.
    input.Position.xyz += input.InstancePosition.xyz;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.Position = mul(input.Position, worldMatrix);
    output.Position = mul(output.Position, viewMatrix);
    output.Position = mul(output.Position, projectionMatrix);

	output.TexCoord = input.TexCoord;
    
	return output;
}