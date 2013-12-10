//http://developer.download.nvidia.com/presentations/2006/siggraph/dx10A Nvidia Corporation 2004

cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldViewProjection;
};


struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL;
	float2 WorldYAndViewDepth : TEXCOORD0;
};

struct PixelShaderInput
{
	float4 Position : SV_POSITION;
	float3 Normal :	NORMAL;
	float4 TexCoord : TEXCOORD0;
};

static const float3 UpNormal = float3(0.0f, 1.0f, 0.0f);
static const float length = 1.0f;

void MakeFin( VertexShaderOutput v1, VertexShaderOutput v2, VertexShaderOutput vAdj, inout TriangleStream<PixelShaderInput> TriStream )
{ 
	float3 N2 = normalize(cross( v1.Position - v2.Position, vAdj.Position - v2.Position ));
	//float3 eyeVec = normalize( Eye - v1.Position );

	//if this is a silhouette edge, extrude it into 2 triangles
	if( dot(UpNormal, N2) >= 0 )
	{
		GS_OUTPUT_FINS Out;
		for(int v=0; v<2; v++)
		{
			Out.Position = mul(v1.Position + v * float4(v1.Normal, 0)*length, WorldViewProj );
			Out.Normal = mul( v1.Normal, World );
			Out.TexCoord.xy = float2(0, 1-v);
			Out.TexCoord.z = 1.0f; //Texture ID
			Out.TexCoord.w = WorldYAndViewDepth.y; //View depth
		
			//append new vertices to the stream to create the fin
			TriStream.Append(Out);
		}
		
		for(int v=0; v<2; v++)
		{
			Out.Position = mul(v2.Position + v * float4(v2.Normal, 0)*length, WorldViewProj );
			Out.Normal = mul( v2.Normal, World);
			Out.TexCoord.xy = float2(1, 1-v);
			Out.TexCoord.z = 1.0f; //Texture ID
			Out.TexCoord.w = WorldYAndViewDepth.y; //View depth

			TriStream.Append(Out);
		}
	
		TriStream.RestartStrip();
	}
}	

//GS shader for the fins
[maxvertexcount(12)]
void GeometryGrassGeometryShader( triangleadj VertexShaderOutput input[6], inout TriangleStream<PixelShaderInput> TriStream)
{ 
	//compute the triangle’s normal
	float3 N1 = normalize(cross( input[0].Position - input[2].Position, input[4].Position - input[2].Position ));
	//float3 eyeVec = normalize( Eye – input[0].Position);
	
	//if the central triangle is front facing, check the other triangles
	if(dot(N1, UpNormal) >= 0.0f )
	{
		MakeFin(input[2],input[0],input[1], TriStream);
		MakeFin(input[4],input[2],input[3], TriStream);
		MakeFin(input[0],input[4],input[5], TriStream); 
	}
}
