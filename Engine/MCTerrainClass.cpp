#include "MCTerrainClass.h"


MCTerrainClass::MCTerrainClass(void)
{
}

MCTerrainClass::MCTerrainClass
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


	noise = new SimplexNoise();
}

MCTerrainClass::~MCTerrainClass(void)
{
// //	delete marchingCubeVertices2; 
// //	marchingCubeVertices2 = 0;

//	noise->~SimplexNoise();
//	delete noise;
}

void MCTerrainClass::MCHeightMap()
{







	//for (z = 16; z < (this->sizeZ - 16); z++)
	//{
	//	for (y = 16; y < (this->sizeY - 16); y++)
	//	{
	//		for (x = 16; x < (this->sizeX - 16); x++)
	//		{

	//			idx = x + y*this->sizeY + z * this->sizeY * this->sizeZ;
	//			

	//			//float r = (float)rand()/(float)RAND_MAX;
	//			float r = noise->noise3D(this->marchingCubeVertices2[idx].posX,marchingCubeVertices2[idx].posY,marchingCubeVertices2[idx].posZ);
	//			
	//			

	//			if(r<0)
	//			{
	//				r=0.000001f;
	//			}
	//			
	//			this->marchingCubeVertices2[idx].flux = r * 100.0f;

	//				

	//			if (this->marchingCubeVertices2[idx].flux > 0.01f)
	//				{
	//					this->marchingCubeVertices2[idx].inside = true;
	//				}
	//				else
	//				{
	//					this->marchingCubeVertices2[idx].inside = false;
	//				}


	//			this->marchingCubeVertices2[idx].normalX = this->marchingCubeVertices2[idx - 1].flux - this->marchingCubeVertices2[idx+1].flux;
	//			this->marchingCubeVertices2[idx].normalY = this->marchingCubeVertices2[idx - this->sizeY].flux - this->marchingCubeVertices2[idx + this->sizeY].flux;
	//			this->marchingCubeVertices2[idx].normalZ = this->marchingCubeVertices2[idx - (this->sizeY * this->sizeZ)].flux - this->marchingCubeVertices2[idx + (this->sizeY * this->sizeZ)].flux;
	//		}
	//	}
	//}
}
