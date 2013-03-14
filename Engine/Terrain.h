#pragma once
#include <math.h>
#include "customStructs.h"


class Terrain
{
private:
	int sizeX;
	int sizeY;
	int sizeZ;
	int x,y,z, idx;
	MarchingCubeVertex *marchingCubeVertices2;
public:
	Terrain(void);

	Terrain
		(
		int sizeX,
		int sizeY,
		int sizeZ,
		MarchingCubeVertex *marchingCubeVertices
		);

	~Terrain(void);

	void LSystemTree();

	void Initialize();
};