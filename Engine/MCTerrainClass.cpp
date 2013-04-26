#include "MCTerrainClass.h"


MCTerrainClass::MCTerrainClass(void)
{
}



MCTerrainClass::~MCTerrainClass(void)
{

	noise->~SimplexNoise();
	delete noise;
	noise = 0;
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

	densityToBeInside = 0.2f;
	densityArray1D = new float[sizeX*sizeY*sizeZ];

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
		float density = noise->noise2D(this->marchingCubeVertices[i].posX,marchingCubeVertices[i].posZ);
		worldArray[i] = density;
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



				this->marchingCubeVertices[idx].density = 100.0f;



				if (this->marchingCubeVertices[idx].density > 0.1f)
				{
					this->marchingCubeVertices[idx].inside = true;
				}
				else
				{
					this->marchingCubeVertices[idx].inside = false;
				}


				this->marchingCubeVertices[idx].normalX = this->marchingCubeVertices[idx - 1].density - this->marchingCubeVertices[idx+1].density;
				this->marchingCubeVertices[idx].normalY = this->marchingCubeVertices[idx - this->sizeY].density - this->marchingCubeVertices[idx + this->sizeY].density;
				this->marchingCubeVertices[idx].normalZ = this->marchingCubeVertices[idx - (this->sizeY * this->sizeZ)].density - this->marchingCubeVertices[idx + (this->sizeY * this->sizeZ)].density;

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
	maxDensity = -511.0f;
	minDensity = 511.0f;
	float density;
	for (z = 1; z < (this->sizeZ -1); z++)
	{
		for (y = 1; y < (this->sizeY -1); y++)
		{
			for (x = 1; x < (this->sizeX -1); x++)
			{
				idx = x + y*this->sizeY + z * this->sizeY * this->sizeZ;


				density = 1 + sizeY * 0.4f - y;
				/*density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *4.0f;
				density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *2.0f;
				density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *2.0f;
				density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) + 1.0f) *5.0f;*/
				//density -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/320,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/320) + 1.0f) *10.0f;
				/**shelves

				////***
				***terraces
				density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *2.0f;
				density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *5.0f;
				density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/130,marchingCubeVertices[idx].posZ/30) + 1.0f) *5.0f;
				density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/230,marchingCubeVertices[idx].posZ/30) + 1.0f) *5.0f;
				density -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/300) + 1.0f) *10.0f;
				***/
				
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/220,marchingCubeVertices[idx].posY/220,marchingCubeVertices[idx].posZ/420) *5.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/20) *5.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/320,marchingCubeVertices[idx].posZ/20) *10.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/320,marchingCubeVertices[idx].posZ/20) *10.0f;
				//Hardfloor

				//if(y < 5)
				//{
				//	density += 40.0f;
				//}

				//Toplvl

				/*
				if(y > 10)
				{
				density -= 7.0f;
				}
				if(y > 15)
				{
				density -= 7.0f;
				}
				if(y > 20)
				{
				density -= 7.0f;
				}
				if(y > 25)
				{
				density -= 7.0f;
				}
				*/		

				//density -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/230,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *5.0f;
				//density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) + 1.0f) *5.0f;
				//density += (noise->noise3D2(this->marchingCubeVertices[idx].posX/15,marchingCubeVertices[idx].posY/15,marchingCubeVertices[idx].posZ/15) + 1.0f) *0.5f;
				//density += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//density += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//density += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//density += 4 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/260,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//density -= 10 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.25f;
				//density -= 3 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *0.5f;
				//density -= 3 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/30,marchingCubeVertices[idx].posY/30,marchingCubeVertices[idx].posZ/30) + 1.0f) *0.5f;
				//density += 8 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) + 1.0f) *0.5f;
				//density += 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/180,marchingCubeVertices[idx].posZ/20) + 1.0f) *0.5f;
				//density += 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/140,marchingCubeVertices[idx].posZ/20) + 1.0f) *1.0f;
				//density -= 1 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/10,marchingCubeVertices[idx].posY/10,marchingCubeVertices[idx].posZ/10) + 1.0f) *0.5f;
				//density -= 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/10,marchingCubeVertices[idx].posY/10,marchingCubeVertices[idx].posZ/10) + 1.0f) *0.5f;
				//density -= 2 * (noise->noise3D2(this->marchingCubeVertices[idx].posX/10,marchingCubeVertices[idx].posY/10,marchingCubeVertices[idx].posZ/10) + 1.0f) *0.5f;
				//density -= (noise->noise3D2(this->marchingCubeVertices[idx].posX/5,marchingCubeVertices[idx].posY/5,marchingCubeVertices[idx].posZ/5) + 1.0f) *0.5f;
				//density += (noise->noise3D2(this->marchingCubeVertices[idx].posX,marchingCubeVertices[idx].posY,marchingCubeVertices[idx].posZ) + 0.0f) *0.5f;
				//density += (noise->noise3D2(this->marchingCubeVertices[idx].posX,marchingCubeVertices[idx].posY,marchingCubeVertices[idx].posZ) - 0.3f) *0.5f;


				//density = (noise->noise3D2(this->marchingCubeVertices[idx].posX,marchingCubeVertices[idx].posY,marchingCubeVertices[idx].posZ) + 1.0f) *0.5f;

				/*if(r<0)
				{density = 0.0f;}*/

				//density *= 1000 *3;

				if(density < -2)
				{
					density = -2;
				}
				else if (density > 2)
				{
					density = 2;
				}

				this->marchingCubeVertices[idx].density = density;

				if (maxDensity < density)
				{
					maxDensity = density;
				}
				else if (minDensity > density)
				{
					minDensity = density;
				}
			}
		}
	}

	maxDensity = maxDensity;
	minDensity =  minDensity;
	
	densityRangeUpper = maxDensity - densityToBeInside;
	if (densityRangeUpper != 0)
	{
		densityRangeUpper = 1 / densityRangeUpper;
	}
	densityRangeLower = densityToBeInside - minDensity;
	if (densityRangeUpper != 0)
	{
		densityRangeLower = 1 / densityRangeLower;
	}

	for (z = 1; z < (this->sizeZ -1); z++)
	{
		for (y = 1; y < (this->sizeY -1); y++)
		{
			for (x = 1; x < (this->sizeX -1); x++)
			{
				idx = x + y*this->sizeY + z * this->sizeY * this->sizeZ;
		
				if (this->marchingCubeVertices[idx].density > densityToBeInside)
				{
					this->marchingCubeVertices[idx].density = (this->marchingCubeVertices[idx].density ) * densityRangeUpper;
				}

				if (this->marchingCubeVertices[idx].density < densityToBeInside)
				{
					this->marchingCubeVertices[idx].density = (this->marchingCubeVertices[idx].density ) * densityRangeLower;
				}

				if (marchingCubeVertices[idx].density > densityToBeInside)
				{
					this->marchingCubeVertices[idx].inside = true;
				}
				else
				{
					this->marchingCubeVertices[idx].inside = false;
				}

				this->densityArray1D[idx] = this->marchingCubeVertices[idx].density;
				densityArray3D[z][y][x] = this->marchingCubeVertices[idx].density;


				CreateMCVerts();
			}
		}
	}
}


void MCTerrainClass::CreateMCVerts()
{

	this->marchingCubeVertices[idx].normalX = this->marchingCubeVertices[idx - 1].density/2 - this->marchingCubeVertices[idx+1].density;
	this->marchingCubeVertices[idx].normalY = this->marchingCubeVertices[idx - this->sizeY].density - this->marchingCubeVertices[idx + this->sizeY].density;
	this->marchingCubeVertices[idx].normalZ = this->marchingCubeVertices[idx - (this->sizeY * this->sizeZ)].density - this->marchingCubeVertices[idx + (this->sizeY * this->sizeZ)].density;
}

float MCTerrainClass::GetHighestPositionOfCoordinate(int x, int z)
{


	float j,i = 0;
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
	float j1 = this->getMarchingCubeVertices()[idx].density;
	float j2 = this->getMarchingCubeVertices()[idx2].density;
	j = (j1 + j2)*.50f;


	//j = 0;
	return (i + 1.0f + j);
}

float MCTerrainClass::GetHighestPositionOfCoordinateBruteforce(float x, float z)
{
	float j,i = 0;
	bool tempBool = false;
	for (i = sizeY; !tempBool; i--)
	{
		idx = x + i*this->sizeY + z * this->sizeY * this->sizeZ;
		if(this->getMarchingCubeVertices()[idx].inside)
		{
			tempBool = true;
		} 
	}

	float decimalX = x - int(x);
	float decimalY = y - int(x);
	float totalDensity = 0;
	float densityX = 0;
	float densityZ = 0;
	float densityXZ = 0;

	int idx = x + (i+1)*this->sizeY + z * this->sizeY * this->sizeZ;
	int idx2 = x + (i+2)*this->sizeY + z * this->sizeY * this->sizeZ;
	int idxX1 = (x +1) + (i+1)*this->sizeY + z * this->sizeY * this->sizeZ;
	int idxZ1 = x + (i+1)*this->sizeY + (z + 1) * this->sizeY * this->sizeZ;
	int idxXZ = (x +1) + (i+1)*this->sizeY + (z + 1) * this->sizeY * this->sizeZ;

	//bool b1 = this->getMarchingCubeVertices()[idx].inside;
	//bool b2 = this->getMarchingCubeVertices()[idx2].inside;
	float j1 = this->getMarchingCubeVertices()[idx].density;
	float j2 = this->getMarchingCubeVertices()[idx2].density;
	
	densityX = (this->getMarchingCubeVertices()[idx].density + this->getMarchingCubeVertices()[idxX1].density);
	densityZ = (this->getMarchingCubeVertices()[idx].density + this->getMarchingCubeVertices()[idxZ1].density);
	densityXZ = (this->getMarchingCubeVertices()[idx].density + this->getMarchingCubeVertices()[idxXZ].density);



	j = (j1 + j2)*.50f;


	//j = 0;
	return (i + 1.0f + j);
}