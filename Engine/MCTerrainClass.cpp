#include "MCTerrainClass.h"


MCTerrainClass::MCTerrainClass(void)
{
}



MCTerrainClass::~MCTerrainClass(void)
{
	// //	delete marchingCubeVertices; 
	// //	marchingCubeVertices = 0;

	//	noise->~SimplexNoise();
	//	delete noise;
}

void MCTerrainClass::Initialize
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
	this->marchingCubeVertices = marchingCubeVertices;
	static const int a = sizeX;

	marchingCubesFluxArray = new int[sizeX*sizeY*sizeZ];

	densityArray3D.resize(sizeZ);
	for (int i = 0; i < sizeZ; ++i)
	{
		densityArray3D[i].resize(sizeY);
		for (int j = 0; j < sizeY; ++j)
		{
			densityArray3D[i][j].resize(sizeX);
		}
	}

	//static const float[a][sizeY][sizeZ] katt;

	worldSize = 0;
	worldSizeMargin = 1;
	worldArraySize = sizeX * sizeY;
	noise = new SimplexNoise();
}

void MCTerrainClass::MCHeightMap()
{
	//int worldArray[worldArraySize];


	float* worldArray = new float[worldArraySize];

	for(int i = 0; i < worldArraySize; i++)
	{
		float r = noise->noise2D(this->marchingCubeVertices[i].posX,marchingCubeVertices[i].posZ);
		worldArray[i] = r;
	}

	int i = -2;

	for (z = worldSizeMargin; z < (this->sizeZ - worldSizeMargin); z++)
	{
		i++;
		for (y = worldSizeMargin; y < (this->sizeY - worldSizeMargin); y++)
		{
			for (x = worldSizeMargin; x < (this->sizeX - worldSizeMargin); x++)
			{
				i++;

				if (worldArray[i] > (float)y)
				{
				}
				idx = x + y*this->sizeY + z * this->sizeY * this->sizeZ;



				this->marchingCubeVertices[idx].flux = 100.0f;



				if (this->marchingCubeVertices[idx].flux > 0.1f)
				{
					this->marchingCubeVertices[idx].inside = true;
				}
				else
				{
					this->marchingCubeVertices[idx].inside = false;
				}


				this->marchingCubeVertices[idx].normalX = this->marchingCubeVertices[idx - 1].flux - this->marchingCubeVertices[idx+1].flux;
				this->marchingCubeVertices[idx].normalY = this->marchingCubeVertices[idx - this->sizeY].flux - this->marchingCubeVertices[idx + this->sizeY].flux;
				this->marchingCubeVertices[idx].normalZ = this->marchingCubeVertices[idx - (this->sizeY * this->sizeZ)].flux - this->marchingCubeVertices[idx + (this->sizeY * this->sizeZ)].flux;

			}
		}
	}

	if (worldArray != 0)
	{
		delete worldArray;
	}

}


void MCTerrainClass::Noise3D()
{
	float max, min;
	max = 0;
	min = 1;

	for (z = 1; z < (this->sizeZ -1); z++)
	{
		for (y = 1; y < (this->sizeY -1); y++)
		{
			for (x = 1; x < (this->sizeX -1); x++)
			{
				idx = x + y*this->sizeY + z * this->sizeY * this->sizeZ;


				float r =  + sizeY * 0.2f - y;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *4.0f;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *2.0f;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *2.0f;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) + 1.0f) *5.0f;
				//r -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/320,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/320) + 1.0f) *10.0f;
				/**shelves

				////***
				//***terraces
				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *2.0f;
				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *5.0f;
				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/130,marchingCubeVertices[idx].posZ/30) + 1.0f) *5.0f;
				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/230,marchingCubeVertices[idx].posZ/30) + 1.0f) *5.0f;
				r -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/300) + 1.0f) *10.0f;
				//***/



				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/80,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/80) + 1.0f) *2.0f;
				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/80,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/80) + 1.0f) *2.0f;
				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) + 1.0f) *5.0f;
				r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/300) + 1.0f) *5.0f;
				//Hardfloor

				//if(y < 5)
				//{
				//	r += 40.0f;
				//}

				///Toplvl

				/*
				if(y > 10)
				{
				r -= 7.0f;
				}
				if(y > 15)
				{
				r -= 7.0f;
				}
				if(y > 20)
				{
				r -= 7.0f;
				}
				if(y > 25)
				{
				r -= 7.0f;
				}
				*/

				//r -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/230,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *5.0f;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) + 1.0f) *5.0f;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX/15,marchingCubeVertices[idx].posY/15,marchingCubeVertices[idx].posZ/15) + 1.0f) *0.5f;
				//r += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//r += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//r += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//r += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//r -= 10 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//r -= 3 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *0.5f;
				//r -= 3 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *0.5f;
				//r += 8 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) + 1.0f) *0.5f;
				//r += 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/180,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.5f;
				//r += 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/140,marchingCubeVertices[idx].posZ/20) + 1.0f) *1.0f;
				//r -= 1 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/10,marchingCubeVertices[idx].posY/10,marchingCubeVertices[idx].posZ/10) + 1.0f) *0.5f;
				//r -= 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/10,marchingCubeVertices[idx].posY/10,marchingCubeVertices[idx].posZ/10) + 1.0f) *0.5f;
				//r -= 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/10,marchingCubeVertices[idx].posY/10,marchingCubeVertices[idx].posZ/10) + 1.0f) *0.5f;
				//r -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/5,marchingCubeVertices[idx].posY/5,marchingCubeVertices[idx].posZ/5) + 1.0f) *0.5f;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX,marchingCubeVertices[idx].posY,marchingCubeVertices[idx].posZ) + 0.0f) *0.5f;
				//r += (noise->noise3D2(this->marchingCubeVertices[idx].posX,marchingCubeVertices[idx].posY,marchingCubeVertices[idx].posZ) - 0.3f) *0.5f;


				//r = (noise->noise3D2(this->marchingCubeVertices[idx].posX,marchingCubeVertices[idx].posY,marchingCubeVertices[idx].posZ) + 1.0f) *0.5f;

				/*if(r<0)
				{r = 0.0f;}*/


				//r *= 1000 *3;

				if(max < r)
				{max = r;}
				if(min > r)
				{min = r;}


				/*
				if (r<0)
				{
				r = 0;
				}
				else if (r>1)
				{
				r = 1;
				}*/

				this->marchingCubeVertices[idx].flux = r;

				if (r > 0.2f)
				{
					this->marchingCubeVertices[idx].inside = true;
				}
				else
				{
					this->marchingCubeVertices[idx].inside = false;
				}

				densityArray3D[z][y][x] = r;


				CreateMCVerts();

			}
		}
	}
	max = max;
	min = min;
}


void MCTerrainClass::CreateMCVerts()
{
	
				this->marchingCubeVertices[idx].normalX = this->marchingCubeVertices[idx - 1].flux - this->marchingCubeVertices[idx+1].flux;
				this->marchingCubeVertices[idx].normalY = this->marchingCubeVertices[idx - this->sizeY].flux - this->marchingCubeVertices[idx + this->sizeY].flux;
				this->marchingCubeVertices[idx].normalZ = this->marchingCubeVertices[idx - (this->sizeY * this->sizeZ)].flux - this->marchingCubeVertices[idx + (this->sizeY * this->sizeZ)].flux;
}

float MCTerrainClass::GetHighestPositionOfCoordinate(int x, int z)
{
	int i = 0;
	float j = 0;
	bool tempBool = false;
	for (i = sizeY; !tempBool; i--)
	{
		idx = x + i*this->sizeY + z * this->sizeY * this->sizeZ;


		if(this->getMarchingCubeVertices()[idx].inside)
		{
			tempBool = true;
		}

	}

	int idx = x + (i+1)*this->sizeY + z * this->sizeY * this->sizeZ;
	int idx2 = x + (i+2)*this->sizeY + z * this->sizeY * this->sizeZ;

	//bool b1 = this->getMarchingCubeVertices()[idx].inside;
	//bool b2 = this->getMarchingCubeVertices()[idx2].inside;
	float j1 = this->getMarchingCubeVertices()[idx].flux;
	float j2 = this->getMarchingCubeVertices()[idx2].flux;
	j = (j1 + j2)*0.5f;

	return i + 1.1f + j;
}