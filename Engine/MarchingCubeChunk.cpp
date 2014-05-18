#include "MarchingCubeChunk.h"

MarchingCubeChunk::MarchingCubeChunk(XMFLOAT3 startPos, XMFLOAT3 endPos, XMFLOAT3 extStepSize, XMFLOAT3 extStepCount)
:	startPosition(startPos),
	//boundingBox(startPosition, endPosition),
	stepSize(extStepSize),
	stepCount(extStepCount),
	waterMesh(),
	terrainMesh(),
	triMesh(std::make_shared<btTriangleMesh>()),
	waterLevel(0.0f)
{
	//extents.x = ((stepCount.x/2) * stepSize.x);
	//extents.y = ((stepCount.y/2) * stepSize.y);
	//extents.z = ((stepCount.z/2) * stepSize.z);

	centerPosition.x = startPosition.x + ((extStepCount.x/2) * extStepSize.x);
	centerPosition.y = startPosition.y + ((extStepCount.y/2) * extStepSize.y);
	centerPosition.z = startPosition.z + ((extStepCount.z/2) * extStepSize.z);
}

MarchingCubeChunk::~MarchingCubeChunk()
{
}