////////////////////////////////////////////////////////////////////////////////
// Filename: mcubes.vsh
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType MCubeVertexShader(VertexInputType input)
{
    PixelInputType output;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the input normal for the pixel shader to use.
    output.normal = float4(input.normal, 1.0f);
    
	/*
	//This'll be of use later

	// calculate tangent space to world space matrix using the world space tangent,
	// binormal, and normal as basis vectors
	output.TangentToWorld[0] = mul(input.Tangent, World);
	output.TangentToWorld[1] = mul(input.Binormal, World);
	output.TangentToWorld[2] = mul(input.Normal, World);
	*/

    return output;
}