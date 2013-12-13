cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldViewProjection;
	float DeltaTime;
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
static const float vegetationFalloff = 200.0f;

//http://www.braynzarsoft.net/index.php?p=D3D11BILLBOARDS
//http://upvoid.com/devblog/2013/02/prototype-grass/

//Function for making grass quads
void MakeQuad(VS_OUTPUT v1, VS_OUTPUT v2, inout TriangleStream<PS_INPUT> TriStream)
{ 
	float viewDepth = v1.YPosDepthAndRand.y;

	//Skip if distance is too far, or if one of the random values is within a certain range. This was added to make sure that there are some spots that are barren.
	if(viewDepth < vegetationFalloff && v2.YPosDepthAndRand.z <= 0.50f)
	{
		//Allocate four output values
		PS_INPUT output[4];

		//In the future, this ID will be loaded from a lookup table
		int textureID = 1.0f;

		//Taking [0, 1] rand values and changing them to a span that can go negative.
		float rand1 = 1.2f * (v1.YPosDepthAndRand.z - 0.5f);
		float rand2 = 1.2f * (v2.YPosDepthAndRand.z - 0.5f);

		//Randomizing our positions
		float4 pos1 = v1.Position - float4(rand1, 0.0f, rand2, 0.0f);
		float4 pos2 = v2.Position + float4(rand2, 0.0f, rand1, 0.0f);

		float opacity = (1.0f - viewDepth/vegetationFalloff);
		float height = (vegetationScale * opacity);

		//Just a temporary wind direction
		float4 WindDirection = normalize(float4(0.2f, 0.0f, 2.0f, 0.0f));

		//Add wind direction to our already randomized normal. This value will be 
		float4 randomizedNormal = float4(rand1*0.8f, height, rand2*0.8f, 0.0f) + (WindDirection*sin(0.5f*v1.Position.z + 2.0f*DeltaTime));

		float4 normal1 = mul(v1.Normal, World);
		float4 normal2 = mul(v2.Normal, World);


		//Define the four vertices, corners
		output[0].Position = mul(pos1, WorldViewProjection);
		output[0].Normal = normalize(normal1);
		output[0].TexCoord = float4(0.0f, 1.0f, textureID, viewDepth);
		output[0].Opacity = opacity;

		output[1].Position = mul(pos2, WorldViewProjection);
		output[1].Normal = normalize(normal2);
		output[1].TexCoord = float4(1.0f, 1.0f, textureID, viewDepth);
		output[1].Opacity = opacity;

		output[2].Position = mul((pos1 + randomizedNormal), WorldViewProjection);
		output[2].Normal = normalize(normal1+randomizedNormal);
		output[2].TexCoord = float4(0.0f, 0.0f, textureID, viewDepth);
		output[2].Opacity = opacity;

		output[3].Position = mul((pos2 + randomizedNormal), WorldViewProjection);
		output[3].Normal = normalize(normal2 + randomizedNormal);
		output[3].TexCoord = float4(1.0f, 0.0f, textureID, viewDepth);
		output[3].Opacity = opacity;

		//We're form a quad out of two triangles, meaning four vertices.
		TriStream.Append(output[0]);
		TriStream.Append(output[1]);
		TriStream.Append(output[2]);
		TriStream.Append(output[3]);

		TriStream.RestartStrip();
	}
};

[maxvertexcount(12)] void GrassGS(triangle VS_OUTPUT Input[3], inout TriangleStream<PS_INPUT> TriStream)
{ 
	//http://upvoid.com/devblog/2013/02/prototype-grass/
	float dotResult = dot(Input[0].Normal, UpNormal);

	//if the surface is or is pretty close to being perpendicular to the Up vector, we make grass.
	if(dotResult > 0.8f || dotResult < -0.8f)
	{
		MakeQuad(Input[1], Input[2], TriStream);
		MakeQuad(Input[2], Input[0], TriStream);
		MakeQuad(Input[0], Input[1], TriStream); 
	}
}