struct VSQuadOut 
{
    float4 Position : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

// outputs a full screen triangle with screen-space coordinates
// input: three empty vertices
VSQuadOut VSQuad( uint vertexID : SV_VertexID )
{
    VSQuadOut result;
    result.TexCoord = float2((vertexID << 1) & 2, vertexID & 2);
    result.Position = float4(result.TexCoord * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);

    return result;
}