Texture2D windTexture;
SamplerState linearSampler;

cbuffer MatrixBuffer
{
	float4x4 World : packoffset(c0);
	float4x4 WorldViewProjection : packoffset(c4);
	float DeltaTime : packoffset(c8.x);
	float3 WindDirection : packoffset(c8.y);
};

cbuffer VariableBuffer
{
	float vegetationScale			: packoffset(c0.x);
	float vegetationFalloff			: packoffset(c0.y);
	float forceScale				: packoffset(c0.z);
	float waveLength				: packoffset(c0.w);
	float traversalSpeed			: packoffset(c1.x);
	float farClip					: packoffset(c1.y);
	float minimumHeight				: packoffset(c1.z);
	float maximumHeight				: packoffset(c1.w);
	float positionalRandomDeviance	: packoffset(c2.x);
	float angularRandomDeviance		: packoffset(c2.y);
	float acceptableSlopeAngle		: packoffset(c2.z);
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
	float4 Normal	: TEXCOORD0;
	float4 TexCoord : TEXCOORD1;
	float Opacity : OPACITY;
};

//static const float vegetationScale = 2.2f;
static const float4 UpNormal = normalize(float4(0.0f, 1.0f, 0.0f, 0.0f));

//TODO: Sample level........ yeah..
float LoadWindForce(float2 coords)
{
	return windTexture.SampleLevel(linearSampler, coords, 0);
}

static const float densitySamplingSpacing = 0.05f;

//Function for making grass quads
void MakeQuad(VS_OUTPUT v1, VS_OUTPUT v2, float distanceHeightScaling, inout TriangleStream<PS_INPUT> TriStream)
{ 
	//Sum up random values to decide if we want to draw grass here.
	float randSum = v2.YPosDepthAndRand.z + v1.YPosDepthAndRand.z;
	
	float density = (randSum + windTexture.SampleLevel(linearSampler, v2.Position.xz*densitySamplingSpacing, 0));

	//Skip if random sum is too low. It's a crude way to make empty patches appear, as opposed to vegetation EVERYWHERE. TODO: Density maps..?
	if(density >= 1.8f)
	{
		//Allocate four output values
		PS_INPUT output[4];

		//In the future, this ID will be loaded from a lookup table
		int textureID = round(v1.YPosDepthAndRand.z * 30);

		if(textureID >= 20)
		{
			textureID = 20;
		}

		//int textureID = round(v1.YPosDepthAndRand.z * 21);

		//Taking [0, 1] rand values and changing them to a span that can go negative ([-1, 1]).
		float2 rand = (2.0f * float2(v1.YPosDepthAndRand.z, v2.YPosDepthAndRand.z) - 0.5f);

		//Scale how much of the grass should be drawn. This is done to smoothly fade in distant grass.
		float opacity = (1.0f - (v1.YPosDepthAndRand.y / distanceHeightScaling));

		float4 pos1, pos2;

		float3 randomDeviance = float3(rand.x*positionalRandomDeviance, 0.0f, rand.y*positionalRandomDeviance);

		//Create a random position for the quad
		pos1.xyz = v1.Position.xyz + randomDeviance; //Add a tiny offset to make placement of grass seem more random.

		//Second position will be a certain distance away from pos1, scaled by height. This is to always make the quads look uniform as opposed to sometimes being squished/stretched.
		pos2.xyz = pos1.xyz + (((v2.Position.xyz+randomDeviance) - v1.Position.xyz)); //height * normalize
		pos1.w = pos2.w = 1.0f;

		//Distance between the two bases. Needed so that I can scale the height properly. The height of the quad is clamped by the width.
		float vertexDistance = length(pos2 - pos1);

		//Scale height of the quad with opacity times vegetation scale plus a small random value.
		//Clamp it between 1.0f and distance between our vertices
		float height = clamp( (vegetationScale+(rand.x*1.5f)), 1.0f, vertexDistance);

		//This is the normal that we use to offset the upper vertices of the quad, as to angle it slightly. 
		//Height is of course used to control how high the quad will be.
		float4 randomizedNormal = float4(rand.x*angularRandomDeviance, height, rand.y*angularRandomDeviance, 0.0f);

		//If the grass is close enough to the camera, add wind. Just pointless to add a bunch of wind to grass that is too far away to be seen properly.
		if(v1.YPosDepthAndRand.y < (vegetationFalloff*0.4f))
		{
			//Add wind direction to our already randomized normal. This value will be used to offset the two upper vertices of the quad.
			randomizedNormal.xyz += (WindDirection * (forceScale * LoadWindForce((v1.Position.xz*waveLength) + (traversalSpeed*DeltaTime))));

			randomizedNormal.w = 0.0f;
		}

		//Define the four vertices, corners
		output[0].Position = mul(pos1, WorldViewProjection);
		output[0].Normal = normalize(v1.Normal);
		output[0].TexCoord = float4(0.0f, 1.0f, textureID, v1.YPosDepthAndRand.y/farClip);
		output[0].Opacity = opacity;

		output[1].Position = mul(pos2, WorldViewProjection);
		output[1].Normal = normalize(v2.Normal);
		output[1].TexCoord = float4(1.0f, 1.0f, textureID, v2.YPosDepthAndRand.y/farClip);
		output[1].Opacity = opacity;

		output[2].Position = mul((pos1 + randomizedNormal), WorldViewProjection);
		output[2].Normal = normalize(v1.Normal+randomizedNormal);
		output[2].TexCoord = float4(0.0f, 0.0f, textureID, v1.YPosDepthAndRand.y/farClip);
		output[2].Opacity = opacity;

		output[3].Position = mul((pos2 + randomizedNormal), WorldViewProjection);
		output[3].Normal = normalize(v2.Normal + randomizedNormal);
		output[3].TexCoord = float4(1.0f, 0.0f, textureID, v2.YPosDepthAndRand.y/farClip);
		output[3].Opacity = opacity;

		//We're form a quad out of two triangles, meaning four vertices.
		TriStream.Append(output[0]);
		TriStream.Append(output[1]);
		TriStream.Append(output[2]);
		TriStream.Append(output[3]);

		TriStream.RestartStrip();
	}
};

[maxvertexcount(24)] 
void GrassGS(triangle VS_OUTPUT Input[3], inout TriangleStream<PS_INPUT> TriStream)
{ 
	//So we do a dot between the three normals of the triangle and an Up vector that just points straight up.
	//Which means that if the surface is angled too far in any direction, we don't create grass there.
	float dotResult = dot(normalize(Input[0].Normal+Input[1].Normal+Input[2].Normal), UpNormal);

	float viewDepth = Input[0].YPosDepthAndRand.y;

	//TODO: higher values
	float lowestLODDistance = vegetationFalloff;
	float mediumLODDistance = vegetationFalloff*0.5f;
	float highestLODDistance = vegetationFalloff*0.15f;

	//So if the dot result is satisfactory, and the world YPos is above water level, and the view space depth value is above vegetation falloff point...... we make quads.
	if(dotResult >= acceptableSlopeAngle && Input[0].YPosDepthAndRand.x > minimumHeight && viewDepth <= lowestLODDistance)
	{
		if(viewDepth <= highestLODDistance)
		{
			//Make six quads.
			MakeQuad(Input[1], Input[2], lowestLODDistance, TriStream);
			MakeQuad(Input[2], Input[0], lowestLODDistance, TriStream);
			MakeQuad(Input[0], Input[1], mediumLODDistance, TriStream);
			MakeQuad(Input[0], Input[2], mediumLODDistance, TriStream);
			MakeQuad(Input[1], Input[0], highestLODDistance, TriStream);
			MakeQuad(Input[2], Input[1], highestLODDistance, TriStream);
		}			
		else if(viewDepth <= mediumLODDistance)
		{
			//Make four quads
			MakeQuad(Input[0], Input[1], mediumLODDistance, TriStream);
			MakeQuad(Input[0], Input[2], mediumLODDistance, TriStream);
			MakeQuad(Input[1], Input[2], lowestLODDistance, TriStream);
			MakeQuad(Input[2], Input[0], lowestLODDistance, TriStream);
		}
		else
		{
			//Make two quads
			MakeQuad(Input[1], Input[2], lowestLODDistance, TriStream);
			MakeQuad(Input[2], Input[0], lowestLODDistance, TriStream);
		}
	}
}