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
static const float vegetationFalloff = 250.0f;
static const float forceScale = 0.9f;
static const float waveLength = 0.04f; //0.008f; //
static const float traversalSpeed = 0.1f; //0.05f; //

//http://www.braynzarsoft.net/index.php?p=D3D11BILLBOARDS
//http://upvoid.com/devblog/2013/02/prototype-grass/

//TODO: Sample level........ yeah..
float LoadWindPowerValue(float2 coords)
{
	return windTexture.SampleLevel(linearSampler, coords, 0);
}

//Function for making grass quads
void MakeQuad(VS_OUTPUT v1, VS_OUTPUT v2, VS_OUTPUT v3, inout TriangleStream<PS_INPUT> TriStream)
{ 
	//Sum up random values to decide if we want to draw grass here.
	float randSum = v2.YPosDepthAndRand.z + v1.YPosDepthAndRand.z;

	//Skip if random sum is too low. It's a crude way to make empty patches appear, as opposed to vegetation EVERYWHERE. TODO: Density maps..?
	if(randSum <= 1.15f)
	{
		//Allocate four output values
		PS_INPUT output[4];
		float viewDepth = v1.YPosDepthAndRand.y;

		//In the future, this ID will be loaded from a lookup table
		int textureID = round(v1.YPosDepthAndRand.z * 13);

		//Taking [0, 1] rand values and changing them to a span that can go negative ([-1, 1]).
		float2 rand = (2.0f * float2(v1.YPosDepthAndRand.z, v2.YPosDepthAndRand.z) - 0.5f);

		//Randomizing the ground positions
		float4 pos1 = v2.Position;													// + float4(rand.x, 0.0f, rand.y, 0.0f);
		float4 pos2 = v1.Position + ((v2.YPosDepthAndRand.z * 2.0f) * (normalize(v3.Position - v2.Position)));	//+ float4(rand.x, 0.0f, rand.y, 0.0f);

		float opacity = (1.0f - (viewDepth / vegetationFalloff));
		float height = (vegetationScale * opacity);

		//This is the normal that we use to offset the upper vertices of the quad, as to angle it slightly. 
		//Height is of course used to control how high the quad will be.
		float4 randomizedNormal = float4(rand.x*0.2f, height, rand.y*0.2f, 0.0f);

		//If the grass is close enough to the camera, add wind. Just pointless to add a bunch of wind to grass that is too far away to be seen properly.
		if(viewDepth < (vegetationFalloff/3))
		{
			//TODO: Replace this with an external windDirection in a constant buffer
			float4 WindDirection = normalize(float4( -0.7f+(rand.x*0.3f), 0.0f, -0.4f+(rand.y*0.3f), 0.0f));

			//Add wind direction to our already randomized normal. This value will be 
			randomizedNormal += (WindDirection * (forceScale * LoadWindPowerValue( (v1.Position.xz*waveLength) + (traversalSpeed*DeltaTime) )));
		}

		//Define the four vertices, corners
		output[0].Position = mul(pos1, WorldViewProjection);
		output[0].Normal = normalize(v1.Normal);
		output[0].TexCoord = float4(0.0f, 1.0f, textureID, viewDepth);
		output[0].Opacity = opacity;

		output[1].Position = mul(pos2, WorldViewProjection);
		output[1].Normal = normalize(v2.Normal);
		output[1].TexCoord = float4(1.0f, 1.0f, textureID, viewDepth);
		output[1].Opacity = opacity;

		output[2].Position = mul((pos1 + randomizedNormal), WorldViewProjection);
		output[2].Normal = normalize(v1.Normal+randomizedNormal);
		output[2].TexCoord = float4(0.0f, 0.0f, textureID, viewDepth);
		output[2].Opacity = opacity;

		output[3].Position = mul((pos2 + randomizedNormal), WorldViewProjection);
		output[3].Normal = normalize(v2.Normal + randomizedNormal);
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

[maxvertexcount(20)] 
void GrassGS(triangle VS_OUTPUT Input[3], inout TriangleStream<PS_INPUT> TriStream)
{ 
	//So we do a dot between the three normals of the triangle and an Up vector that just points straight up.
	//Which means that if the surface is angled too far in any direction, we don't create grass there.
	float dotResult = dot(normalize(Input[0].Normal+Input[1].Normal+Input[2].Normal), UpNormal);

	//So if the dot result is satisfactory, and the world YPos is above water level, and the view space depth value is above vegetation falloff point...... we make quads.
	if(dotResult >= 0.92f && Input[0].YPosDepthAndRand.x > 5.5f && Input[0].YPosDepthAndRand.y < vegetationFalloff)
	{
		//Make up to three quads.
		MakeQuad(Input[1], Input[2], Input[0], TriStream);
		MakeQuad(Input[2], Input[0], Input[1], TriStream);
		MakeQuad(Input[0], Input[1], Input[2], TriStream);
	}
}