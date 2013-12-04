cbuffer MatrixBuffer
{
	float4x4 World;
	float4x4 WorldView;
	float4x4 WorldViewProjection;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
	float3 Normal : NORMAL;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float4 WorldNormal : NORMAL;
	float4 WorldPosition : TEXCOORD0;
	float ViewDepth : TEXCOORD1;
};

VertexShaderOutput MCGBufferTerrainVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.Position	= mul(input.Position, WorldViewProjection);
	
	float4 normal = float4(input.Normal, 1.0f);

	//This is the proper way to do it
	//output.ViewDepth		= mul(input.Position, WorldView).z;

	//You can only do this if you are using a perspective projection matrix! Beware!
	output.ViewDepth = output.Position.w;

	output.WorldPosition	= mul(input.Position, World);
	output.WorldNormal		= normalize(mul(normal, World));

	return output;
}

/*
// G-Buffer vertex shader
// Calculate view space position of the vertex and pass it to the pixel shader
output.PositionVS = mul(input.PositionOS, WorldViewMatrix).xyz;

// G-Buffer pixel shader
// Calculate the length of the view space position to get the distance from camera->surface
output.Distance.x = length(input.PositionVS);

// Light vertex shader
#if PointLight || SpotLight
// Calculate the world space position for a light volume
float3 positionWS = mul(input.PositionOS, WorldMatrix);
#elif DirectionalLight
// Calculate the world space position for a full-screen quad (assume input vertex coordinates are in [-1,1] post-projection space)
float3 positionWS = mul(input.PositionOS, InvViewProjMatrix);
#endif
// Calculate the view ray
output.ViewRay = positionWS - CameraPositionWS;

// Light Pixel shader
// Normalize the view ray, and apply the original distance to reconstruct position
float3 viewRay = normalize(input.ViewRay);
float viewDistance = DistanceTexture.Sample(PointSampler, texCoord);
float3 positionWS = CameraPositionWS + viewRay * viewDistance;
*/