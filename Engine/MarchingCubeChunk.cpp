#include "MarchingCubeChunk.h"

MarchingCubeChunk::MarchingCubeChunk(XMFLOAT3 startPos, XMFLOAT3 endPos, XMFLOAT3 stepSize, XMFLOAT3 stepCount)
:	startPosition(startPos),
	endPosition(endPos),
	//boundingBox(startPosition, endPosition),
	stepSizeX(stepSize.x),
	stepSizeY(stepSize.y),
	stepSizeZ(stepSize.z),
	stepCountX((unsigned int)stepCount.x),
	stepCountY((unsigned int)stepCount.y),
	stepCountZ((unsigned int)stepCount.z),
	mesh()
{
	voxels.resize((stepCountX+1) * (stepCountY+1) * (stepCountZ+1));
	indices.resize(voxels.size());

	int index = 0;

	// Set default values for each voxel in the field
	for(unsigned int z = 0; z <= stepCountZ; ++z)
	{
		for(unsigned int y = 0; y <= stepCountY; ++y)
		{
			for(unsigned int x = 0; x <= stepCountX; ++x)
			{
				//Calculate index for this voxel
				index = x + y*stepCountY + z * stepCountY * stepCountZ;

				//Set default values for this voxel
				voxels[index].position.x = startPosition.x + stepSizeX * x;
				voxels[index].position.y = startPosition.y + stepSizeY * y;
				voxels[index].position.z = startPosition.z + stepSizeZ * z;
				voxels[index].density = 0.0f;
				voxels[index].inside = false;
				voxels[index].normal.x = 0.0f;
				voxels[index].normal.y = 0.0f;
				voxels[index].normal.z = 0.0f;
			}
		}
	}
}

MarchingCubeChunk::~MarchingCubeChunk()
{
	
}