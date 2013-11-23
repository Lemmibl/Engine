#pragma once
#include "types.h"
#include "Utility.h"
#include <windows.h>
#include <vector>
#include "Mesh.h"
#include "Lemmi3DAABB.h"

class TerrainChunk
{
public:
	TerrainChunk(XMFLOAT3 startPosition, XMFLOAT3 endPosition, XMFLOAT3 stepSize, XMFLOAT3 stepCount, size_t vertexStride);
	~TerrainChunk();

	Mesh* GetMesh() { return &mesh; }
	Lemmi3DAABB* GetBoundingBox() const { return &boundingBox; }

	unsigned int GetStepsX() const { return stepsX; }
	unsigned int GetStepsY() const { return stepsY; }
	unsigned int GetStepsZ() const { return stepsZ; }

	float GetStepSizeX() const { return stepSizeX ; }
	float GetStepSizeY() const { return stepSizeY ; }
	float GetStepSizeZ() const { return stepSizeZ ; }

	void Render(ID3D11DeviceContext* deviceContext);

private:
	//Vertices is sizeX*sizeY*sizeZ big
	vector<MarchingCubeVertex> vertices;

	//Our mesh that we'll be rendering. Contains all vertex/index data.
	Mesh mesh;

	//Should be self explanatory. Defines the bounds of this chunk; where it starts and where it ends.
	XMFLOAT3 startPosition, endPosition;

	// How long each step will be between startPosition and endPosition.
	float stepSizeX;
	float stepSizeY;
	float stepSizeZ;

	// The amount of steps we take between startPosition and endPosition
	unsigned int stepsX;
	unsigned int stepsY;
	unsigned int stepsZ;

	Lemmi3DAABB boundingBox;

//Potential thing we might add to these chunks in the future:
	//Vector of vegetation quads
	//Bounding box
	//Some sort of bucket for containing everything that is currently inside this chunk. In a sense this means that the chunk would act as a leaf node in some sort of spatial tree/grid.
	//bool wireframe
	// https://code.google.com/p/dx11engine/source/browse/main/trunk/Engine/BoundingBox.h?r=12
};

