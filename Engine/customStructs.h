#ifndef MCUBES_TYPES_H
#define MCUBES_TYPES_H

#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>

struct MarchingCubeVertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	float density;
	bool inside;
};

struct Metapoint
{
	XMFLOAT3 position;
	float power;
};

struct MarchingCubeVectors
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
};

#endif