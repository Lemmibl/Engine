cbuffer MatrixVertexBuffer
{
	float4x4 WorldViewProjection;
	float4x4 WorldView;
	float4x4 World;
	float3 CameraPosition;
	float padding;
};

struct VertexShaderInput
{
	float4 Position : POSITION;
};

struct VertexShaderOutput
{
	float4 Position : SV_Position;
	float4 ScreenPosition : TEXCOORD0;
	float4 ViewPosition : TEXCOORD1;
	float3 ViewRay : TEXCOORD2;
};

VertexShaderOutput LightVertexShader(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.Position = mul(input.Position, WorldViewProjection);
	output.ScreenPosition = output.Position;
	output.ViewPosition = mul(input.Position, WorldView);
	
	float3 worldPosition = mul(input.Position, World);
	output.ViewRay = (worldPosition.xyz - CameraPosition.xyz);

	return output;
}

/*
################## VIEW SPACE ########################

// Light vertex shader
#if PointLight || SpotLight
// Calculate the view space vertex position
output.PositionVS = mul(input.PositionOS, WorldViewMatrix);
#elif DirectionalLight
// Calculate the view space vertex position (you can also just directly map the vertex to a frustum corner to avoid the transform)
output.PositionVS = mul(input.PositionOS, InvProjMatrix);
#endif

// Light Pixel shader
#if PointLight || SpotLight
// Extrapolate the view space position to the  far clip plane
float3 viewRay = float3(input.PositionVS.xy * (FarClipDistance / input.PositionVS.z), FarClipDistance);
#elif DirectionalLight
// For a directional light, the vertices were already on the far clip plane so we don't need to extrapolate
float3 viewRay = input.PositionVS.xyz;
#endif

// Sample the depth and scale the view ray to reconstruct view space position
float normalizedDepth = DepthTexture.Sample(PointSampler, texCoord).x;
float3 positionVS = viewRay * normalizedDepth;

######################################################
*/


/*
################## WORLD SPACE ########################

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

#######################################################
*/