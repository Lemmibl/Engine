#pragma once
#include "types.h"
#include "Utility.h"
#include <windows.h>
#include <vector>
#include "IndexedMesh.h"
#include "Lemmi3DAABB.h"
#include "VegetationManager.h"

class MarchingCubeChunk
{
public:
	MarchingCubeChunk(XMFLOAT3 startPosition, XMFLOAT3 endPosition, XMFLOAT3 stepSize, XMFLOAT3 stepCount);
	~MarchingCubeChunk();

	IndexedMesh* GetTerrainMesh() { return &terrainMesh; }
	IndexedMesh* GetWaterMesh() { return &waterMesh; }

	vector<MarchingCubeVoxel>* GetVoxelField() { return &voxels; }
	vector<unsigned int>* GetIndices() { return &indices; }
	ID3D11ShaderResourceView* GetWindTexture() { return windTexture.p; }
	ID3D11ShaderResourceView** GetWindTexturePP() { return &windTexture.p; }

	unsigned int GetStepCountX() const { return stepCountX; }
	unsigned int GetStepCountY() const { return stepCountY; }
	unsigned int GetStepCountZ() const { return stepCountZ; }

	float GetStepSizeX() const { return stepSizeX ; }
	float GetStepSizeY() const { return stepSizeY ; }
	float GetStepSizeZ() const { return stepSizeZ ; }

	float GetStartPosX() const { return startPosition.x; }
	float GetStartPosY() const { return startPosition.y; }
	float GetStartPosZ() const { return startPosition.z; }

private:
	//voxels is sizeX*sizeY*sizeZ big
	vector<MarchingCubeVoxel> voxels;
	vector<unsigned int> indices;
	CComPtr<ID3D11ShaderResourceView> windTexture;

	//Our mesh that we'll be rendering. Contains all vertex/index data.
	IndexedMesh terrainMesh;
	IndexedMesh waterMesh;

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

	//Lemmi3DAABB boundingBox;

//Potential thing we might add to these chunks in the future:
	//Vector of vegetation quads
	//Bounding box
	//Some sort of bucket for containing everything that is currently inside this chunk. In a sense this means that the chunk would act as a leaf node in some sort of spatial tree/grid.
	//bool wireframe
	// https://code.google.com/p/dx11engine/source/browse/main/trunk/Engine/BoundingBox.h?r=12
};

