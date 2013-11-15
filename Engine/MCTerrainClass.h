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

	int sizeX;
	int sizeY;
	int sizeZ;
	int worldSize;
	int worldSizeMargin;
	int x,y,z, idx;
	int worldArraySize;
	int *marchingCubesFluxArray;
	int terrainMode;
	float densityToBeInside;
	float maxDensity, minDensity, densityRangeUpper,densityRangeLower;
	float XFactor;
	float YFactor;
	float ZFactor;

	bool pulverize;


	MarchingCubeVertex *marchingCubeVertices;
	SimplexNoise *noise;

	vector<vector<vector<float>>> densityArray3D;

public:
	MCTerrainClass(void);

	void Initialize(int sizeX, int sizeY, int sizeZ, MarchingCubeVertex *marchingCubeVertices, SimplexNoise* simplexNoise);

	~MCTerrainClass(void);


	void LSystem();


	void LSystemTree();

	void MCHeightMap();

	void Noise3D();

	MarchingCubeVertex* GetMarchingCubeVertices()
	{
		return marchingCubeVertices;
	}

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

	vector<vector<vector<float>>> const &getDensityArray3D() const
	{
		return densityArray3D;
	}



	float GetHighestPositionOfCoordinate(int x, int z);

	float GetHighestPositionOfCoordinateBruteforce(float x, float z);

private:
	void CreateMCVerts();



};