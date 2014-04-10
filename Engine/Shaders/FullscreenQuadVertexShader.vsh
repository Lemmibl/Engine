//I don't take credit for this code, I found it here:
//http://www.altdevblogaday.com/2011/08/08/interesting-vertex-shader-trick/
struct VS_Output
{  
    float4 Position : SV_POSITION;       
    float2 TexCoord : TEXCOORD0;
};
 
VS_Output VS_Quad(uint id : SV_VertexID)
{
    VS_Output output;
    output.TexCoord = float2((id << 1) & 2, id & 2);
    output.Position = float4(output.TexCoord * float2(2,-2) + float2(-1,1), 0, 1);

    return output;
}