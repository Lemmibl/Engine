#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include "MCTerrainClass.h"
#include <vector>

class Utility
{
public:
	Utility();
	Utility(const Utility&);
	~Utility();

	float RandomFloat();
	float VectorDistance(XMFLOAT3 vector1, XMFLOAT3 vector2);
};