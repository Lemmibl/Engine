#pragma once
#include <math.h>
#include "customStructs.h"
#include "NoiseClass.h"
#include <stdlib.h> //srand
#include <time.h>
#include <vector>

using std::vector;

class MCTerrainClass
{
public:
	enum TerrainTypes
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

public:
	MCTerrainClass();
	~MCTerrainClass();


	void Initialize(int sizeX, int sizeY, int sizeZ, NoiseClass* simplexNoise, TerrainTypes terrainType);
	void LSystem();
	void LSystemTree();
	void MCHeightMap();

	void SetCurrentVoxelField(vector<MarchingCubeVoxel>* val){ marchingCubeVertices = val; }

	void Noise3D(unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ);

	float GetDensityRangeUpper()
	{
		return densityRangeUpper;
	}

	float GetDensityRangeLower()
	{
		return densityRangeLower;
	}
	
	void SetTerrainType(TerrainTypes terrainMode)
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

	//float GetHighestPositionOfCoordinate(vector<MarchingCubeVoxel>* marchingCubeVertices, int x, int z);
	float GetHighestPositionOfCoordinate(int x, int z);

private:
	void CreateMCVerts();


private:
	int sizeX, sizeY, sizeZ;
	int worldSize;
	int worldSizeMargin;
	unsigned int x,y,z, idx;
	int worldArraySize;
	TerrainTypes terrainMode;
	float densityToBeInside;
	float maxDensity, minDensity, densityRangeUpper,densityRangeLower;
	float XFactor;
	float YFactor;
	float ZFactor;
	bool pulverize;

	vector<MarchingCubeVoxel>* marchingCubeVertices;
	NoiseClass *noise;
};