#pragma once
#include <math.h>
#include "customStructs.h"
#include "SimplexNoise.h"
#include <stdlib.h> //srand
#include <time.h>

class MCTerrainClass
{
private:
	int sizeX;
	int sizeY;
	int sizeZ;
	int worldSize;
	int worldSizeMargin;
	int x,y,z, idx;
	MarchingCubeVertex *marchingCubeVertices2;
	SimplexNoise *noise;
	int worldArraySize;
	int *marchingCubesFluxArray;
	//float[][][] marchingCubesFluxArray;
public:
	MCTerrainClass(void);

	MCTerrainClass
		(
		int sizeX,
		int sizeY,
		int sizeZ,
		MarchingCubeVertex *marchingCubeVertices
		);

	~MCTerrainClass(void);


	void LSystem();


	void LSystemTree();

	void MCHeightMap();

	void Noise3D();

private:
	void CreateMCVerts();

	

};