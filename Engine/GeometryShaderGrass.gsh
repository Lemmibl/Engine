//http://developer.download.nvidia.com/presentations/2006/siggraph/dx10A Nvidia Corporation 2004

void MakeFin( VS_OUTPUT v1, VS_OUTPUT v2, VS_OUTPUT vAdj, inoutTriangleStream <GS_OUTPUT_FINS> TriStream )
{ 
	float3 N2 = normalize(cross( v1.Position - v2.Position, vAdj.Position - v2.Position ));
	float3 eyeVec = normalize( Eye - v1.Position );

	//if this is a silhouette edge, extrude it into 2 triangles
	if( dot (eyeVec,N2) < 0 )
	{
		GS_OUTPUT_FINS Out;
		for(int v=0; v<2; v++)
		{
			Out.Position = mul(v1.Position + v * float4(v1.Normal,0)*length, WorldViewProj );
			Out.Normal = mul( v1.Normal, World );
			//Out.TextureMesh = v1.Texture;
			Out.TexCoord = float2(0, 1-v);
			//Out.Opacity = opacity;
		
			//append new vertices to the stream to create the fin
			TriStream.Append(Out);
		}
		
		for(int v=0; v<2; v++)
		{
			Out.Position = mul(v2.Position + v* float4(v2.Normal,0)*length, WorldViewProj );
			Out.Normal = mul( v2.Normal, World );
			//Out.TextureMesh = v2.Texture;
			Out.TexCoord = float2(1, 1-v);
			//Out.Opacity = opacity;
	
			TriStream.Append(Out);
		}
	
		TriStream.RestartStrip();
	}
}	

//GS shader for the fins
[maxvertexcount(12)]
void GS( triangleadj VS_OUTPUT input[6], inout TriangleStream<GS_OUTPUT_FINS>TriStream)
{ 
	//compute the triangle’s normal
	float3 N1 = normalize(cross( input[0].Position - input[2].Position, input[4].Position -input[2].Position ));
	float3 eyeVec = normalize( Eye – input[0].Position);
	
	//if the central triangle is front facing, check the other triangles
	if( dot(N1, eyeVec) > 0.0f )
	{
		MakeFin(input[2],input[0],input[1], TriStream);
		MakeFin(input[4],input[2],input[3], TriStream);
		MakeFin(input[0],input[4],input[5], TriStream); 
	}
}
