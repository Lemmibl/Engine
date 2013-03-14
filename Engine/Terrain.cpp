#include "Terrain.h"


Terrain::Terrain(void)
{
}

Terrain::Terrain
	(
	int sizeX,
	int sizeY,
	int sizeZ,
	MarchingCubeVertex *marchingCubeVertices
	)
{

	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->sizeZ = sizeZ;
	marchingCubeVertices2 = marchingCubeVertices;
}


Terrain::~Terrain(void)
{
}

void Terrain::Initialize()
{
	
}
