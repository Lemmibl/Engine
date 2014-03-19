#pragma once
#include <xnamath.h>

struct ShaderMaterialStruct
{
	float Kambience;
	float Kdiffuse;
	float Kspecular;
	float smoothness;
	float shouldBeShadowed; //1 to allow shadowing, 0 to disable shadowing
};

struct OBJMaterialStruct
{
	//Just an identifier to help in assigning the right material to the right mesh when loading obj files.
	WCHAR name[MAX_PATH];

	XMFLOAT3 Ambient;
	XMFLOAT3 Diffuse;
	XMFLOAT3 Specular;

	float smoothness, alpha;
	bool transparent;

	//Used for cross referencing and linking the gbuffer stage and post processing stage together 
	// by saving the material ID in a render target during gbuffer stage then reading it during postprocess
	unsigned short materialID;
};

//Used for my failed procedurally generated texture prototype
struct MaterialColorSpectrumUINT8
{
	UINT8 RedMin, RedMax;
	UINT8 GreenMin, GreenMax;
	UINT8 BlueMin, BlueMax;
};

//We save the pixels in R8G8B8A8. This means that each pixel can have values 1-255.
struct PixelData
{
	UINT8 x, y, z, w;
};

//IDs for different things. Potential TODO: Read these in from an XML or smth?
enum MaterialID
{
	MATERIAL_GRASS,
	MATERIAL_ROCK,
	MATERIAL_SNOW,
	MATERIAL_DIRT,
	MATERIAL_GRASSQUADS,
	MATERIAL_UNDERGROUND
};

enum TextureID
{
	TEXTURE_DIRT,
	TEXTURE_GRASS,
	TEXTURE_ROCK,
	TEXTURE_SAND,
	TEXTURE_SNOW,
	TEXTURE_TILEDSTONE
};