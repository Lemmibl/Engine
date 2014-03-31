#pragma once
#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include "metaballsClass.h"
#include "TreeClass.h"
#include "TerrainNoiseSeeder.h"
#include "types.h"
#include <windows.h>
#include "Utility.h"
#include "MarchingCubeChunk.h"
#include "BulletCollision\CollisionShapes\btTriangleMesh.h"
#include "SettingsDependent.h"

class MarchingCubesClass : public SettingsDependent
{
private:
	struct Triangle
	{
		 btVector3 points[3];
	};

public:
	MarchingCubesClass(unsigned int sizeX,  unsigned int sizeY, unsigned int sizeZ);
	~MarchingCubesClass();

	//Converts a voxel field to a mesh
	void CalculateMesh(ID3D11Device* device, std::shared_ptr<MarchingCubeChunk> chunk, std::vector<MarchingCubeVoxel>* voxels);

	void OnSettingsReload(Config* cfg);

private:
	//Extract a cube from the .. voxel field...
	void ExtractCube(MarchingCubeVoxel** cube, std::vector<MarchingCubeVoxel>* vertices, unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ, unsigned int index);

	//Calculate the lookup value we'll be using to index into the fields
	void CalculateLookupValue(unsigned int* lookup, MarchingCubeVoxel** cube);

	//Triangulate the cube that was extracted before, with the help of our lookup value
	void ProcessCube(unsigned int lookupValue, unsigned int& vertexCounter, MarchingCubeVoxel* verts, MarchingCubeVoxel** cube, std::shared_ptr<MarchingCubeChunk> chunk, 
		unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ, unsigned int indexX, unsigned int indexY, unsigned int indexZ);

	//Create vertex and index buffers from the data that we've created
	void CreateMesh(ID3D11Device* device, IndexedMesh* mesh, std::vector<unsigned int>* indices, std::vector<MarchingCubeVectors>* vertices, unsigned int indexCount, unsigned int vertexCount);

	//Creates a flat mesh from a min and max pos.
	void CreateWaterMesh(ID3D11Device* device, MarchingCubeChunk* chunk, IndexedMesh* waterMesh);

	// Returns a point that is interpolated with ten other points for both normals and possition
	MarchingCubeVoxel Interpolate(MarchingCubeVoxel v1, MarchingCubeVoxel v2, float isoValue)
	{
		MarchingCubeVoxel v;
		float diff;

		diff = (isoValue - v1.density) / (v2.density - v1.density);

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

		////Normalize the normal
		float vectorlength = sqrt((v.normal.x*v.normal.x) + (v.normal.y*v.normal.y) + (v.normal.z*v.normal.z));

		v.normal.x = v.normal.x / vectorlength;
		v.normal.y = v.normal.y / vectorlength;
		v.normal.z = v.normal.z / vectorlength;

		return v;
	}

	float RandomFloat()
	{
		float scale=RAND_MAX+1.0f;
		float base=rand()/scale;
		float fine=rand()/scale;
		return base+fine/scale;
	}

private:
	unsigned int sizeXAxis, sizeYAxis, sizeZAxis;
	float metaballsIsoValue, waterLevel;

	// Tables for edge cases and triangle lookup 
	const static int edgeTable[256];
	const static int triTable[256][16];
};