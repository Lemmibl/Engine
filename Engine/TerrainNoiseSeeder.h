#pragma once
#include <math.h>
#include "TerrainStructs.h"
#include "NoiseClass.h"
#include <stdlib.h> //srand
#include <time.h>
#include <vector>
#include <map>
#include <functional>
#include "MarchingCubeChunk.h"

class TerrainNoiseSeeder
{
private:
	struct IndexingValue
	{
		UINT x, y, z, sizeX, sizeY, sizeZ;

		IndexingValue(UINT x, UINT y, UINT z, UINT sizeX, UINT sizeY, UINT sizeZ)
		: x(x), y(y), z(z), sizeX(sizeX), sizeY(sizeY), sizeZ(sizeZ)
		{
		}
	};

	typedef std::function<float(IndexingValue&, const XMFLOAT3&, NoiseClass*)> NoiseFunction;

public:
	TerrainNoiseSeeder();
	~TerrainNoiseSeeder();

	void Initialize(int sizeX, int sizeY, int sizeZ, NoiseClass* simplexNoise, TerrainTypes::Type terrainType);
	void LSystem();
	void LSystemTree();
	void MCHeightMap();

	//void SetCurrentVoxelField(std::vector<MarchingCubeVoxel>* val){ marchingCubeVertices = val; }

	void Noise3D(unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ, std::vector<MarchingCubeVoxel>* marchingCubeVertices);
	bool GetHighestPositionOfCoordinate(int x, int z, MarchingCubeChunk* chunk, std::vector<MarchingCubeVoxel>* marchingCubeVertices, float* outHeightValue);

	void SetTerrainType(TerrainTypes::Type terrainMode){ this->terrainMode = terrainMode; }

	inline float RandomFloat()
	{
		float scale=RAND_MAX+1.0f;
		float base=rand()/scale;
		float fine=rand()/scale;
		return base+fine/scale;
	}

	int GetTerrainType() { return terrainMode;	}
	
	//float GetHighestPositionOfCoordinate(vector<MarchingCubeVoxel>* marchingCubeVertices, int x, int z);

private:
	int worldSize;
	int worldSizeMargin;
	int worldArraySize;
	TerrainTypes::Type terrainMode;
	float densityToBeInside;
	float XFactor, YFactor, ZFactor;
	int sizeX, sizeY, sizeZ;

	std::map<TerrainTypes::Type, NoiseFunction> functionMap;

	//std::vector<MarchingCubeVoxel>* marchingCubeVertices;
	NoiseClass* noise;
};