#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include "MCTerrainClass.h"
#include <vector>
#include <stdint.h>
#include "StructsAndEnums.h"

class Utility
{
public:
	Utility();
	Utility(const Utility&);
	~Utility();

	float RandomFloat();
	float VectorDistance(XMFLOAT3 vector1, XMFLOAT3 vector2);
	
	//Takes world Y position and returns the float and int value
	void GenerateMaterialAndTextureData(const float WorldPositionY, const XMFLOAT3* const normal, float &lerpValue, unsigned int &materialAndTextureIDs);
	uint32_t PackIDsIntoInt32(uint8_t data[4]);
};