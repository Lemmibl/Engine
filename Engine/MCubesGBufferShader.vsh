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
	uint4 IDValues : COLOR0;
};

struct VertexShaderOutput
{
	float4 Position : SV_POSITION;
	float3 WorldNormal : NORMAL;
	float4 WorldPosition : TEXCOORD0;
	float4 ViewPosition : TEXCOORD1;
	uint4 IDValues : TEXCOORD2;
};

VertexShaderOutput MCubesGBufferVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	input.Position.w = 1.0f;

	output.Position			= mul(input.Position, WorldViewProjection);
	output.ViewPosition		= mul(input.Position, WorldView);
	output.WorldPosition	= mul(input.Position, World);
	output.WorldNormal		= mul(input.Normal, World);

	output.IDValues = input.IDValues;

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