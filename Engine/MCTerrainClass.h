#pragma once
#include <math.h>
#include "customStructs.h"
#include "SimplexNoise.h"
#include <stdlib.h> //srand
#include <time.h>
#include <vector>

using std::vector;

class MCTerrainClass
{
private:
	int sizeX;
	int sizeY;
	int sizeZ;
	int worldSize;
	int worldSizeMargin;
	int x,y,z, idx;
	MarchingCubeVertex *marchingCubeVertices;
	SimplexNoise *noise;
	int worldArraySize;
	int *marchingCubesFluxArray;
	
	vector<vector<vector<float>>> densityArray3D;
public:
	MCTerrainClass(void);

	void Initialize
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

	MarchingCubeVertex* &getMarchingCubeVertices()
	{
		return marchingCubeVertices;
	}

	vector<vector<vector<float>>> const &getDensityArray3D() const
{
    return densityArray3D;
}

	float GetHighestPositionOfCoordinate(int x, int z);


	//float* getDensityArray(){;}

private:
	void CreateMCVerts();



};