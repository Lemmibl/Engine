cbuffer VertexShaderBuffer
{
	float4x4 WorldViewProjection;
	float thFOV;
	float aspectRatio;
	float2 PADDING;
};

struct VertexShaderInput
{
		float4 Position : POSITION;
		float2 TexCoord : TEXCOORD;
};

struct VertexShaderOutput
{
		float4 Position : SV_POSITION;
		float2 TexCoord : TEXCOORD0;
		float3 ViewRay	: TEXCOORD1;
		float4 ScreenPosition : TEXCOORD2;
};

/*
You can compute the view ray from the normalized device coordinates of the fragment in question and the field of view angle and aspect ratio of the camera, like this:

float thfov = tan(fov / 2.0); // can do this on the CPU
viewray = vec3(
ndc.x * thfov * aspect,
ndc.y * thfov,
1.0
);

You can do this either in the vertex shader (and interpolate the view ray), or directly in the fragment shader (compute ndc as texcoords * 2.0 - 1.0).
*/

VertexShaderOutput SSAOVertexShader(VertexShaderInput input)
{
		VertexShaderOutput output;

		output.Position = mul(input.Position, WorldViewProjection);
		output.TexCoord = input.TexCoord;
		
		//Possibly do this in pixel shader or use texcoords..?
		output.ViewRay = float3(output.Position.x * thFOV * aspectRatio, output.Position.y * thFOV, 1.0f);

		output.ScreenPosition = output.Position;

		return output;
}