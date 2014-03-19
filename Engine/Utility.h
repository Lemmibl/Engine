#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include "TerrainNoiseSeeder.h"
#include <vector>
#include <stdint.h>
#include "StructsAndEnums.h"

class Utility
{
public:
	Utility();
	~Utility();
	
	float RandomFloat();
	float VectorDistance(XMFLOAT3 vector1, XMFLOAT3 vector2);

	//Returns new value as a float
	float ConvertFromOneRangeToAnother(float OldValue, float OldMin, float OldMax, float NewMin, float NewMax);
	
	//Takes world Y position and returns the float and int value
	//void GenerateMaterialAndTextureData(const float WorldPositionY, const XMFLOAT3* const normal, float &lerpValue, unsigned int &materialAndTextureIDs);

	uint32_t PackIDsIntoInt32(uint8_t data[4]);
};