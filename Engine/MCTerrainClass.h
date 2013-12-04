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
	//Terrain modes
	static const unsigned int terrainTypesSeaBottom = 1;
	static const unsigned int terrainTypePlains = 2;
	static const unsigned int terrainTypeHills = 3;
	static const unsigned int terrainTypeTerraces = 4;
	static const unsigned int terrainTypeDramaticHills = 5;
	static const unsigned int terrainTypeFlyingIslands = 6;
	static const unsigned int terrainTypeAlien = 7;
	static const unsigned int terrainTypeFancy = 8;
	static const unsigned int terrainTypeCave = 9;

public:
	MCTerrainClass();
	~MCTerrainClass();


	void Initialize(int sizeX, int sizeY, int sizeZ, SimplexNoise* simplexNoise);
	void LSystem();
	void LSystemTree();
	void MCHeightMap();

	void Noise3D(vector<MarchingCubeVoxel>* marchingCubeVertices, unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ);

	float GetDensityRangeUpper()
	{
		return densityRangeUpper;
	}

	float GetDensityRangeLower()
	{
		return densityRangeLower;
	}
	
	void SetTerrainType(int terrainMode)
	{
		this->terrainMode = terrainMode;
	}

	void PulverizeWorldToggle()
	{
		if (pulverize)
		pulverize = false;
		else
		pulverize = true;
	}

	bool GetPulverizeWorld()
	{
		return pulverize;
	}

	void SetPulverizeWorld(bool pulverize)
	{
		this->pulverize = pulverize;
	}

	int getTerrainType()
	{
		return terrainMode;
	}

	float GetHighestPositionOfCoordinate(vector<MarchingCubeVoxel>* marchingCubeVertices, int x, int z);

private:
	void CreateMCVerts(vector<MarchingCubeVoxel>* marchingCubeVertices);


private:
	int sizeX, sizeY, sizeZ;
	int worldSize;
	int worldSizeMargin;
	int x,y,z, idx;
	int worldArraySize;
	int terrainMode;
	float densityToBeInside;
	float maxDensity, minDensity, densityRangeUpper,densityRangeLower;
	float XFactor;
	float YFactor;
	float ZFactor;
	bool pulverize;

	vector<MarchingCubeVoxel>* marchingCubeVertices;
	SimplexNoise *noise;
};