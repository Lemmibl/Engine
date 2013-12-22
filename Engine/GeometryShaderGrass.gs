Texture2D windTexture;
SamplerState linearSampler;

cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldViewProjection;
	float DeltaTime;
};
//TODO: float4(windDirection.xyz, deltaTime)

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

static const float vegetationScale = 2.2f;
static const float4 UpNormal = normalize(float4(0.0f, 1.0f, 0.0f, 1.0f));

//TODO: Add all of these to the constant buffer to make them tweakable in normal code 
//Just a temporary wind direction
static const float4 WindDirection = normalize(float4(0.8f, 0.0f, 0.2f, 0.0f));
static const float vegetationFalloff = 150.0f;
static const float forceScale = 0.7f;
static const float waveLength = 0.08f; //0.008f; //
static const float traversalSpeed = 0.15f; //0.05f; //

//http://www.braynzarsoft.net/index.php?p=D3D11BILLBOARDS
//http://upvoid.com/devblog/2013/02/prototype-grass/

//TODO: Sample level........ yeah..
float LoadWindPowerValue(float2 coords)
{
	return windTexture.SampleLevel(linearSampler, coords, 0);
}

//Function for making grass quads
void MakeQuad(VS_OUTPUT v1, VS_OUTPUT v2, inout TriangleStream<PS_INPUT> TriStream)
{ 
	//Sum up random values to decide if we want to draw grass here.
	float randSum = v2.YPosDepthAndRand.z + v1.YPosDepthAndRand.z;

	//Skip if distance is too far, or if one of the random values is within a certain range. This was added to make sure that there are some spots that are barren.
	if(randSum <= 1.15f)
	{
		//Allocate four output values
		PS_INPUT output[4];
		float viewDepth = v1.YPosDepthAndRand.y;

		//In the future, this ID will be loaded from a lookup table
		int textureID = round(v1.YPosDepthAndRand.z * 13);

		//Taking [0, 1] rand values and changing them to a span that can go negative.
		float rand1 = (2.0f * (v1.YPosDepthAndRand.z - 0.5f)) * 0.8f;
		float rand2 = (2.0f * (v2.YPosDepthAndRand.z - 0.5f)) * 0.8f;

		//Randomizing our positions
		float4 pos1 = v1.Position + float4(rand1, 0.0f, rand2, 0.0f);
		float4 pos2 = v2.Position + float4(rand2, 0.0f, rand1, 0.0f);

		float opacity = (1.0f - (viewDepth/vegetationFalloff));
		float height = (vegetationScale * opacity);

		//Add wind direction to our already randomized normal. This value will be 
		float4 randomizedNormal = float4(rand1*0.2f, height, rand2*0.2f, 0.0f);

		//If the grass is close enough to the camera, add wind. Just pointless to add a bunch of wind to grass that is too far away to be seen.
		if(viewDepth < (vegetationFalloff/3))
		{
			randomizedNormal += (WindDirection * (forceScale * LoadWindPowerValue( (v1.Position.xz*waveLength) + (traversalSpeed*DeltaTime) )));
		}


		float4 normal1 = v1.Normal;//mul(, World);
		float4 normal2 = v2.Normal;//mul(, World);


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

[maxvertexcount(12)] 
void GrassGS(triangle VS_OUTPUT Input[3], inout TriangleStream<PS_INPUT> TriStream)
{ 
	float dotResult = dot(Input[1].Normal, UpNormal);

	//if the surface is or is pretty close to being perpendicular to the Up vector, we make grass.
	if(dotResult >= 0.9f && Input[0].YPosDepthAndRand.x > 4.5f && Input[0].YPosDepthAndRand.y < vegetationFalloff)
	{
		MakeQuad(Input[1], Input[2], TriStream);
		MakeQuad(Input[2], Input[0], TriStream);
		MakeQuad(Input[0], Input[1], TriStream); 
	}
}