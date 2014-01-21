#pragma once
#include <math.h>
#include "customStructs.h"
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

	void SetCurrentVoxelField(std::vector<MarchingCubeVoxel>* val){ marchingCubeVertices = val; }

	void Noise3D(unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ);
	float GetHighestPositionOfCoordinate(int x, int z, const MarchingCubeChunk* chunk);

	void SetTerrainType(TerrainTypes::Type terrainMode)
	{
		this->terrainMode = terrainMode;
	}

	inline void CreateNormal(std::vector<MarchingCubeVoxel>& verts, IndexingValue& index, unsigned int idx)
	{
		verts[idx].normal.x = (verts[idx - 1].density -	verts[idx+1].density)					* XFactor;
		verts[idx].normal.y = (verts[idx - index.sizeY].density - verts[idx + index.sizeY].density)			* YFactor;
		verts[idx].normal.z = (verts[idx - (index.sizeY * index.sizeZ)].density - verts[idx + (index.sizeY * index.sizeZ)].density)	* ZFactor;

		//Normalize results.
		float vectorLength = (verts[idx].normal.x*verts[idx].normal.x) + (verts[idx].normal.y*verts[idx].normal.y) + (verts[idx].normal.z*verts[idx].normal.z);

		verts[idx].normal.x = verts[idx].normal.x/vectorLength;
		verts[idx].normal.y = verts[idx].normal.y/vectorLength;
		verts[idx].normal.z = verts[idx].normal.z/vectorLength;
	}

	int GetTerrainType()
	{
		return terrainMode;
	}
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

	std::vector<MarchingCubeVoxel>* marchingCubeVertices;
	NoiseClass* noise;
};