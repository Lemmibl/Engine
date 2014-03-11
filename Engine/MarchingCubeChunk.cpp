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
	terrainMesh(),
	waterMesh(),
	triMesh(std::make_shared<btTriangleMesh>()),
	isReady(false)
{
	extents.x = ((stepCount.x/2) * stepSize.x);
	extents.y = ((stepCount.y/2) * stepSize.y);
	extents.z = ((stepCount.z/2) * stepSize.z);

	centerPosition.x = startPosition.x + extents.x;
	centerPosition.y = startPosition.y + extents.y;
	centerPosition.z = startPosition.z + extents.z;

	//unsigned int index = 0;
	//voxels.resize((stepCountX+1) * (stepCountY+1) * (stepCountZ+1));


	//// Set default values for each voxel in the field
	//for(unsigned int z = 0; z <= stepCountZ+1; ++z)
	//{
	//	for(unsigned int y = 0; y <= stepCountY+1; ++y)
	//	{
	//		for(unsigned int x = 0; x <= stepCountX+1; ++x)
	//		{
	//			//Calculate index for this voxel
	//			index = x + (y*stepCountY) + (z * stepCountY * stepCountZ);

	//			//Set default values for this voxel
	//			voxels[index].position.x = startPosition.x + stepSizeX * x;
	//			voxels[index].position.y = startPosition.y + stepSizeY * y;
	//			voxels[index].position.z = startPosition.z + stepSizeZ * z;
	//			voxels[index].density = 0.0f;
	//			voxels[index].inside = false;
	//			voxels[index].normal.x = 0.5f;
	//			voxels[index].normal.y = 0.5f;
	//			voxels[index].normal.z = 0.5f;
	//		}
	//	}
	//}
}

MarchingCubeChunk::~MarchingCubeChunk()
{
	
}