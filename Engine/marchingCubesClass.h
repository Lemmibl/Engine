#pragma once;
#include "metaballsClass.h"
#include "TreeClass.h"
#include "MCTerrainClass.h"
#include "types.h"
#include <windows.h>
#include "Utility.h"
#include "MarchingCubeChunk.h"

class MarchingCubesClass
{
public:
	MarchingCubesClass(unsigned int sizeX,  unsigned int sizeY, unsigned int sizeZ);
	~MarchingCubesClass();

	//Converts a voxel field to a mesh
	void CalculateMesh(ID3D11Device* device, MarchingCubeChunk* chunk);

private:
	//Extract a cube from the .. voxel field...
	void ExtractCube(MarchingCubeVoxel** cube, vector<MarchingCubeVoxel>* vertices, unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);

	//Calculate the lookup value we'll be using to index into the fields
	void CalculateLookupValue(unsigned int* lookup, unsigned int index, MarchingCubeVoxel** cube);

	//Triangulate the cube that was extracted before, with the help of our lookup value
	void ProcessCube(unsigned int lookupValue, MarchingCubeVoxel* verts, MarchingCubeVoxel** cube, vector<unsigned int>* indices, vector<MarchingCubeVectors>* vertices, unsigned int& indexCounter, unsigned int& vertexCounter, unsigned int sizeX,  unsigned int sizeY, unsigned int sizeZ);

	//Create vertex and index buffers from the data that we've created
	void CreateMesh(ID3D11Device* device, IndexedMesh* mesh, vector<unsigned int>* indices, vector<MarchingCubeVectors>* vertices, unsigned int indexCount, unsigned int vertexCount);

	//Creates a flat mesh from a min and max pos.
	void CreateWaterMesh(ID3D11Device* device, MarchingCubeChunk* chunk, IndexedMesh* waterMesh, XMFLOAT2 minPos, XMFLOAT2 maxPos);

	// Returns a point that is interpolated with ten other points for both normals and possition
	inline MarchingCubeVoxel Interpolate(MarchingCubeVoxel v1, MarchingCubeVoxel v2)
	{
		MarchingCubeVoxel v;
		float diff;

		diff = (this->metaballsIsoValue - v1.density) / (v2.density - v1.density);

		//Interpolate density as well.
		//We don't actually use density after this point, so there's no need to calculate it, but just in case we expand on this in the future ...
		v.density = v1.density + (v2.density - v1.density) * diff;

		//Find where on the line our point is positioned
		v.position.x = v1.position.x + (v2.position.x - v1.position.x) * diff;
		v.position.y = v1.position.y + (v2.position.y - v1.position.y) * diff;
		v.position.z = v1.position.z + (v2.position.z - v1.position.z) * diff;

		//Calculate the average normal for each point
		v.normal.x = v1.normal.x + (v2.normal.x - v1.normal.x) * diff;
		v.normal.y = v1.normal.y + (v2.normal.y - v1.normal.y) * diff;
		v.normal.z = v1.normal.z + (v2.normal.z - v1.normal.z) * diff;

		//Normalize the normal
		float vectorLength = sqrt((v.normal.x*v.normal.x) + (v.normal.y*v.normal.y) + (v.normal.z*v.normal.z));

		v.normal.x /= vectorLength;
		v.normal.y /= vectorLength;
		v.normal.z /= vectorLength;

		return v;
	}

private:
	unsigned int indexX, indexY, indexZ;
	unsigned int sizeX, sizeY, sizeZ;
	int index;
	float metaballsIsoValue;

	// Tables for edge cases and triangle lookup 
	const static int edgeTable[256];
	const static int triTable[256][16];

	XMFLOAT2 minPos, maxPos;
	bool createWater;
};