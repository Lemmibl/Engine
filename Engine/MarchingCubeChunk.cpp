#include "MarchingCubeChunk.h"

MarchingCubeChunk::MarchingCubeChunk(XMFLOAT3 startPos, XMFLOAT3 endPos, XMFLOAT3 stepSize, XMFLOAT3 stepCount)
:	startPosition(startPos),
	//boundingBox(startPosition, endPosition),
	stepSizeX(stepSize.x),
	stepSizeY(stepSize.y),
	stepSizeZ(stepSize.z),
	stepCountX((unsigned int)stepCount.x),
	stepCountY((unsigned int)stepCount.y),
	stepCountZ((unsigned int)stepCount.z),
	waterMesh(),
	terrainMesh(),
	triMesh(std::make_shared<btTriangleMesh>()),
	waterLevel(0.0f)
{
	extents.x = ((stepCount.x/2) * stepSize.x);
	extents.y = ((stepCount.y/2) * stepSize.y);
	extents.z = ((stepCount.z/2) * stepSize.z);

	centerPosition.x = startPosition.x + extents.x;
	centerPosition.y = startPosition.y + extents.y;
	centerPosition.z = startPosition.z + extents.z;
}

MarchingCubeChunk::~MarchingCubeChunk()
{
}