#ifndef MCUBES_TYPES_H
#define MCUBES_TYPES_H

#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>

struct MarchingCubeVertex
{
	float posX;
	float posY;
	float posZ;

	float normalX;
	float normalY;
	float normalZ;

	float density;

	bool inside;
};

struct Metapoint
{
	float posX;
	float posY;
	float posZ;

	float power;
};

struct MarchingCubeVectors
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	UINT32 TextureAndMaterialIDs;
	/*float lerpValue;*/
};

#endif