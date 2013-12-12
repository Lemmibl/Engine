cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldViewProjection;
};


struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 Normal : TEXCOORD0;
	float3 YPosDepthAndRand : TEXCOORD1;
};

struct PS_INPUT
{
	float4 Position : SV_POSITION;
	float4 Normal :	TEXCOORD0;
	float4 TexCoord : TEXCOORD1;
	float Opacity : OPACITY;
};

static const float vegetationScale = 2.0f;
static const float4 UpNormal = normalize(float4(0.0f, 1.0f, 0.0f, 1.0f));
static const float vegetationFalloff = 250.0f;

//http://www.braynzarsoft.net/index.php?p=D3D11BILLBOARDS
//http://upvoid.com/devblog/2013/02/prototype-grass/

void MakeFin(VS_OUTPUT v1, VS_OUTPUT v2, inout TriangleStream<PS_INPUT> TriStream)
{ 
	float viewDepth = v1.YPosDepthAndRand.y;

	if(viewDepth < vegetationFalloff && v2.YPosDepthAndRand.z <= 0.65f)
	{
		PS_INPUT output[4];
		int textureID = 1.0f;

		//Taking [0, 1] rand values and changing them to [-1, 1]
		float rand1 = 1.2f * (v1.YPosDepthAndRand.z - 0.5f);
		float rand2 = 1.2f * (v2.YPosDepthAndRand.z - 0.5f);

		//Randomizing our positions
		float4 pos1 = v1.Position - float4(rand1, 0.0f, rand2, 0.0f);
		float4 pos2 = v2.Position + float4(rand2, 0.0f, rand1, 0.0f);

		//float4 dirVector = normalize(pos1 - pos2);

		//pos2 = pos1 + (1.5f * dirVector);
		//pos2.y = v2.Position.y;

		//pos1.xz -= v1.YPosDepthAndRand.z;
		//pos2.xz += v2.YPosDepthAndRand.z;

		float opacity = (1.0f - viewDepth/vegetationFalloff);
		float height = (vegetationScale * opacity);
		float4 randomizedNormal = float4(rand1*0.8f, height, rand2*0.8f, 0.0f);

		float4 normal1 = mul(v1.Normal, World);
		float4 normal2 = mul(v2.Normal, World);

		output[0].Position = mul(pos1, WorldViewProjection); //(v1.Position + (0.0f + float4(v1.Normal.xyz, 1.0f))*length);//
		output[0].Normal = normalize(normal1);
		output[0].TexCoord = float4(0.0f, 1.0f, textureID, viewDepth);
		output[0].Opacity = opacity;

		output[1].Position = mul(pos2, WorldViewProjection); //(v2.Position + (0.0f * float4(v2.Normal.xyz, 1.0f))*length);//
		output[1].Normal = normalize(normal2);
		output[1].TexCoord = float4(1.0f, 1.0f, textureID, viewDepth);
		output[1].Opacity = opacity;

		output[2].Position = mul((pos1 + randomizedNormal), WorldViewProjection); //(v1.Position + (1.0f * float4(v1.Normal.xyz, 1.0f))*length);//
		output[2].Normal = normalize(normal1+randomizedNormal);
		output[2].TexCoord = float4(0.0f, 0.0f, textureID, viewDepth);
		output[2].Opacity = opacity;

		output[3].Position = mul((pos2 + randomizedNormal), WorldViewProjection); //(v2.Position + (1.0f * float4(v2.Normal.xyz, 1.0f))*length);//
		output[3].Normal = normalize(normal2 + randomizedNormal);
		output[3].TexCoord = float4(1.0f, 0.0f, textureID, viewDepth);
		output[3].Opacity = opacity;

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
	//http://upvoid.com/devblog/2013/02/prototype-grass/

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