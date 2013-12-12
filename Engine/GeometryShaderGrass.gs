cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldViewProjection;
};


struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Normal : TEXCOORD0;
	float2 WorldYAndViewDepth : TEXCOORD1;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float4 Normal :	TEXCOORD0;
	float4 TexCoord : TEXCOORD1;
};

static const float4 UpNormal = normalize(float4(0.0f, 1.0f, 0.0f, 1.0f));
static const float4 length = float4(0.0f, 3.5f, 0.0f, 0.0f);

//http://www.braynzarsoft.net/index.php?p=D3D11BILLBOARDS

void MakeFin(VS_OUTPUT v1, VS_OUTPUT v2, inout TriangleStream<PS_INPUT> TriStream)
{ 
	float viewDepth = v1.WorldYAndViewDepth.y;

	/*
								1,1		 1,0
								v1       v3
								+--------+
								|		 | 
	VertexShaderOutput 1	<<	|		 |		>>	VertexShaderOutput 2
	 is X == 0					+--------+			  is X == 1
								v0		v2
								0,1		0,0
	
	*/

	if((viewDepth < 100.0f))
	{
		PS_INPUT output[4];
		int textureID = 1.0f;

		output[0].Position = mul(v1.Position, WorldViewProjection); //(v1.Position + (0.0f + float4(v1.Normal.xyz, 1.0f))*length);//
		output[0].Normal = normalize(mul(v1.Normal, World));
		output[0].TexCoord = float4(0.0f, 1.0f, textureID, viewDepth);

		output[1].Position = mul(v2.Position, WorldViewProjection); //(v2.Position + (0.0f * float4(v2.Normal.xyz, 1.0f))*length);//
		output[1].Normal = normalize(mul(v2.Normal, World));
		output[1].TexCoord = float4(1.0f, 1.0f, textureID, viewDepth);

		output[2].Position = mul((v1.Position + (UpNormal*length)), WorldViewProjection); //(v1.Position + (1.0f * float4(v1.Normal.xyz, 1.0f))*length);//
		output[2].Normal = normalize(mul(v1.Normal, World));
		output[2].TexCoord = float4(0.0f, 0.0f, textureID, viewDepth);

		output[3].Position = mul((v2.Position + (UpNormal*length)), WorldViewProjection); //(v2.Position + (1.0f * float4(v2.Normal.xyz, 1.0f))*length);//
		output[3].Normal = normalize(mul(v2.Normal, World));
		output[3].TexCoord = float4(1.0f, 0.0f, textureID, viewDepth);

		//We're forming a quad out of two triangles, meaning four vertices.
		TriStream.Append(output[0]);
		TriStream.Append(output[1]);
		TriStream.Append(output[2]);
		TriStream.Append(output[3]);

		TriStream.RestartStrip();
	}
};

[maxvertexcount(18)] void GrassGS(triangle VS_OUTPUT Input[3], inout TriangleStream<PS_INPUT> TriStream)
{ 
	//compute the triangle’s normal
	float4 averageNormal = normalize(Input[0].Normal);//normalize(cross( Input[0].Position - Input[2].Position, Input[0].Position - Input[1].Position)); //
	
	float dotResult = dot(averageNormal, UpNormal);

	//if the central triangle is front facing, check the other triangles
	if(dotResult > 0.8f || dotResult < -0.8f)
	{
		MakeFin(Input[1], Input[2], TriStream);
		MakeFin(Input[2], Input[0], TriStream);
		MakeFin(Input[0], Input[1], TriStream); 
	}
}