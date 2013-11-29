#pragma once
#include "types.h"
#include "Utility.h"
#include <windows.h>
#include <vector>
#include "Mesh.h"
#include "Lemmi3DAABB.h"

class MarchingCubeChunk
{
public:
	MarchingCubeChunk(XMFLOAT3 startPosition, XMFLOAT3 endPosition, XMFLOAT3 stepSize, XMFLOAT3 stepCount);
	~MarchingCubeChunk();

	Mesh* GetMesh() { return &mesh; }
	vector<MarchingCubeVoxel>* GetVoxelField() { return &voxels; }
	vector<unsigned int>* GetIndices() { return &indices; }
	const Lemmi3DAABB* GetBoundingBox() { return &boundingBox; }

	unsigned int GetTotalSize() const { return voxels.size(); }
	unsigned int GetStepCountX() const { return stepCountX; }
	unsigned int GetStepCountY() const { return stepCountY; }
	unsigned int GetStepCountZ() const { return stepCountZ; }

	float GetStepSizeX() const { return stepSizeX ; }
	float GetStepSizeY() const { return stepSizeY ; }
	float GetStepSizeZ() const { return stepSizeZ ; }

	float GetSizeX() const { return stepSizeX * stepCountX; }
	float GetSizeY() const { return stepSizeY * stepCountY; }
	float GetSizeZ() const { return stepSizeZ * stepCountZ; }

private:
	//voxels is sizeX*sizeY*sizeZ big
	vector<MarchingCubeVoxel> voxels;
	vector<unsigned int> indices;

	//Our mesh that we'll be rendering. Contains all vertex/index data.
	Mesh mesh;

	//Should be self explanatory. Defines the bounds of this chunk; where it starts and where it ends.
	XMFLOAT3 startPosition, endPosition;

	// How long each step will be between startPosition and endPosition.
	float stepSizeX;
	float stepSizeY;
	float stepSizeZ;

	// The amount of steps we take between startPosition and endPosition
	unsigned int stepCountX;
	unsigned int stepCountY;
	unsigned int stepCountZ;

	Lemmi3DAABB boundingBox;

//Potential thing we might add to these chunks in the future:
	//Vector of vegetation quads
	//Bounding box
	//Some sort of bucket for containing everything that is currently inside this chunk. In a sense this means that the chunk would act as a leaf node in some sort of spatial tree/grid.
	//bool wireframe
	// https://code.google.com/p/dx11engine/source/browse/main/trunk/Engine/BoundingBox.h?r=12
};

