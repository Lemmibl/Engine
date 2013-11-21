#pragma once
#include "types.h"
#include "Utility.h"
#include <windows.h>
#include <d3d11.h>
#include <vector>
#include <atlcomcli.h>

class TerrainChunk
{
public:
	TerrainChunk();
	~TerrainChunk();

	unsigned int VertexCount() const { return vertexCount; }
	unsigned int IndexCount() const { return indexCount; }
	unsigned int VertexStride() const { return vertexStride; }

	void VertexCount(unsigned int val) { vertexCount = val; }
	void IndexCount(unsigned int val) { indexCount = val; }

private:
	//Vertices is sizeX*sizeY*sizeZ big
	vector<MarchingCubeVertex> vertices;

	//These buffers represent our mesh
	CComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
	
	//These are relevant for rendering
	unsigned int vertexCount, indexCount, vertexStride; //Stride is == sizeof(MarchingCubeVectors). Total size of vertexBuffer is vertexStride * vertexCount

	XMFLOAT3 startPosition, endPosition;

	// How long each step will be between startPosition and endPosition.
	float spacingX;
	float spacingY;
	float spacingZ;

	// The amount of steps we take between startPosition and endPosition
	unsigned int stepX;
	unsigned int stepY;
	unsigned int stepZ;


//Potential thing we might add to these chunks in the future:
	//Vector of vegetation quads
	//Bounding box
	//Some sort of bucket for containing everything that is currently inside this chunk. In a sense this means that the chunk would act as a leaf node in some sort of spatial tree/grid.
	//bool wireframe
};

