#pragma once
#include "metaballsClass.h"
#include "types.h"
#include <windows.h>
#include <stdio.h>


class MarchingCubesClass
{
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	private:
	int vertexCount, indexCount;
	int x,y,z, idx;

	/* Properties for the grid, what x,y,z coordinates in object space the grid will start at. */
	float startX;
	float startY;
	float startZ;

	/* Properties for the grid, what x,y,z coordinates in object space the grid will end at. */
	float endX;
	float endY;
	float endZ;

	/* How long each step will be between start(x,y,z) and end(x,y,z). */
	float stepX;
	float stepY;
	float stepZ;

	/* Size of the grid. Is calculated in the constructor. */
	int sizeX;
	int sizeY;
	int sizeZ;

	short lookup;
	float metaballsIsoValue;

	/* Whether we want to draw with wireframe or not. */
	bool wireframe;

	/* Pointer to a metaball object */
	MetaballsClass *mb;

	/* Tables for edge cases and triangle lookup */
	const static int edgeTable[256];
	const static int triTable[256][16];
	
	MarchingCubeVectors *things;
	
	/* Stores the points that we generate */
	MarchingCubeVertex *marchingCubeVertices;

	/* Stores the points from a simple cube */
	MarchingCubeVertex verts[12];



public:
	MarchingCubesClass(	float startX, float startY, float startZ, float endX, float endY, float endZ,
			float stepX, float stepY, float stepZ);

	virtual ~MarchingCubesClass();

	/* Calculates whether the points are inside or outside. */
	void ComputeMetaBalls();

	/* Converts the metaballs into a mesh for directx to use */
	void CalculateMesh(ID3D11Device* device);

	bool Render(ID3D11DeviceContext* context);

	int GetIndexCount() { return indexCount; }

	/* Setter for metaball pointer and isovalue */
	void SetMetaBalls(MetaballsClass *mb, float isoValue)
	{
		this->mb = mb;
		this->mb->SetIsoValue(isoValue);
		this->metaballsIsoValue = isoValue;
	}

	void SetWireframe(bool s)
	{
		this->wireframe = s;
	}



};