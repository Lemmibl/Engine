#include "OBJModel.h"

OBJModel::OBJModel()
: subsetCount(0), textureCount(0), materialCount(0)
{
	meshHandle = 0;

	//Just to initialize them
	meshSubsetIndices.clear();
	textureHandles.clear();
	materialHandles.clear();
}

OBJModel::~OBJModel()
{
}