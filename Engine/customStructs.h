#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>

struct MarchingCubeVoxel
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
	XMFLOAT4 position;
	XMFLOAT3 normal;
};

//http://stackoverflow.com/questions/514194/using-enum-inside-types-compiler-warning-c4482-c
namespace TerrainTypes
{
	typedef int Type;
	enum
	{
		SeaBottom = 1,
		Plains = 2,
		Hills = 3,
		Terraces = 4,
		DramaticHills = 5,
		FlyingIslands = 6,
		Alien = 7,
		Fancy = 8,
		Cave = 9
	};
};