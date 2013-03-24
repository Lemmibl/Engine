/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix WorldMatrix;
    matrix ViewMatrix;
    matrix ProjectionMatrix;
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
    float4 DomePosition : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType SkysphereVertexShader(VertexInputType input)
{
    PixelInputType output;
    

    // Change the position vector to be 4 units for proper matrix calculations.
    input.Position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.Position = mul(input.Position, WorldMatrix);
    output.Position = mul(output.Position, ViewMatrix);
    output.Position = mul(output.Position, ProjectionMatrix);
    
    // Send the unmodified position through to the pixel shader.
    output.DomePosition = input.Position;

    return output;
}