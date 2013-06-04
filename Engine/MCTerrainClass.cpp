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

	worldSize = 0;
	worldSizeMargin = 1;
	worldArraySize = sizeX * sizeY;
	noise = new SimplexNoise();
}

void MCTerrainClass::MCHeightMap()
{

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

				
	const unsigned int terrainTypesSeaBottom = 0;
	const unsigned int terrainTypePlains = 1;
	const unsigned int terrainTypeHills = 2;
	
	const unsigned int terrainTypeAlien = 7;
	const unsigned int terrainTypeCave = 8;
	const unsigned int terrainTypeFancy = 9;

switch ( terrainMode ) {
case terrainTypesSeaBottom:
				density = 1 + sizeY * 0.1f - y;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/220,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/220) *2.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/220,marchingCubeVertices[idx].posZ/40) *2.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/420,marchingCubeVertices[idx].posZ/20) *2.0f;
  break;
case terrainTypePlains:
				density = 1 + sizeY * 0.2f - y;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/220,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/220) *10.0f;
  break;
case terrainTypeHills:
				density = 1 + sizeY * 0.2f - y;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/220,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/220) *2.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/80,marchingCubeVertices[idx].posY/220,marchingCubeVertices[idx].posZ/40) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/420,marchingCubeVertices[idx].posZ/20) *10.0f;
  break;
case terrainTypeAlien:
				density = 1 + sizeY * 0.1f - y;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/220,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/220) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/220,marchingCubeVertices[idx].posZ/40) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/420,marchingCubeVertices[idx].posZ/20) *10.0f;
  break;
case terrainTypeCave:
				density = 10;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/120,marchingCubeVertices[idx].posZ/120) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) *10.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/120,marchingCubeVertices[idx].posZ/20) *10.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/20) *10.0f;
  break;
case terrainTypeFancy:
				density = 1 + sizeY * 0.1f - y;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/220,marchingCubeVertices[idx].posY/220,marchingCubeVertices[idx].posZ/420) *2.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/20) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) *2.0f;
  break;
default:
	density = 1 + sizeY * 0.1f - y;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/220,marchingCubeVertices[idx].posY/220,marchingCubeVertices[idx].posZ/420) *2.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/20) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/20,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) *20.0f;
				density += noise->noise3D2(this->marchingCubeVertices[idx].posX/120,marchingCubeVertices[idx].posY/20,marchingCubeVertices[idx].posZ/120) *2.0f;

				//density = 1 + sizeY * 0.1f - y;
  break;
}

				
				
				//**Hardfloor**//
				if(y < 13)
				{
					density += 1000000.0f;
				}


				//**Toplvl**//
				//
				//if(y > 10)
				//{
				//density -= 7.0f;
				//}
				//if(y > 15)
				//{
				//density -= 7.0f;
				//}
				//if(y > 20)
				//{
				//density -= 7.0f;
				//}
				//if(y > 25)
				//{
				//density -= 7.0f;
				//}
						

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
		for (y = 0; y < (this->sizeY -0); y++)
		{
			for (x = 0; x < (this->sizeX -0); x++)
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

				if(this->marchingCubeVertices[idx].density < 0)
				{
					this->densityArray1D[idx] = 0 ;
				}
				else if(this->marchingCubeVertices[idx].density > 1)
				{
					this->densityArray1D[idx] = 1;
				}
				else
				{
					this->densityArray1D[idx] = this->marchingCubeVertices[idx].density;
				}
				
				densityArray3D[z][y][x] = this->marchingCubeVertices[idx].density;


				CreateMCVerts();
			}
		}
	}
}

void MCTerrainClass::CreateMCVerts()
{
	this->marchingCubeVertices[idx].normalX = this->marchingCubeVertices[idx - 1].density - this->marchingCubeVertices[idx+1].density;
	this->marchingCubeVertices[idx].normalY = this->densityArray1D[idx - this->sizeY] - this->densityArray1D[idx + this->sizeY];
	this->marchingCubeVertices[idx].normalZ = this->densityArray1D[idx - (this->sizeY * this->sizeZ)] - this->densityArray1D[idx + (this->sizeY * this->sizeZ)];
}

float MCTerrainClass::GetHighestPositionOfCoordinate(int x, int z)
{
	float j = 0.0f;
	int i = 0;

	bool tempBool = false;
	for (i = sizeY; !tempBool; i--)
	{
		idx = x + i*this->sizeY + z * this->sizeY * this->sizeZ;

		if(this->GetMarchingCubeVertices()[idx].inside)
		{
			tempBool = true;
		} 
	}

	int idx = x + (i+1)*this->sizeY + z * this->sizeY * this->sizeZ;
	int idx2 = x + (i+2)*this->sizeY + z * this->sizeY * this->sizeZ;

	float j1 = this->GetMarchingCubeVertices()[idx].density;
	float j2 = this->GetMarchingCubeVertices()[idx2].density;
	j = (j1 + j2)*0.5f;


	//j = 0;
	return (i + 1.0f + j);
}

float MCTerrainClass::GetHighestPositionOfCoordinateBruteforce(float x, float z)
{
	float j = 0.0f;
	int i = 0;
	bool tempBool = false;

	int xAsInt = (int)x;
	int zAsInt = (int)z;

	for (i = sizeY; !tempBool; i--)
	{
		idx = xAsInt + i*this->sizeY + zAsInt * this->sizeY * this->sizeZ;
		if(this->GetMarchingCubeVertices()[idx].inside)
		{
			tempBool = true;
		} 
	}

	float decimalX = (x - int(x));

	float totalDensity = 0.0f;
	float densityX = 0.0f;
	float densityZ = 0.0f;
	float densityXZ = 0.0f;

	int idx = xAsInt + (i+1)*this->sizeY + zAsInt * this->sizeY * this->sizeZ;
	int idx2 = xAsInt + (i+2)*this->sizeY + zAsInt * this->sizeY * this->sizeZ;
	int idxX1 = (xAsInt +1) + (i+1)*this->sizeY + zAsInt * this->sizeY * this->sizeZ;
	int idxZ1 = xAsInt + (i+1)*this->sizeY + (zAsInt + 1) * this->sizeY * this->sizeZ;
	int idxXZ = (xAsInt +1) + (i+1)*this->sizeY + (zAsInt + 1) * this->sizeY * this->sizeZ;

	float j1 = this->GetMarchingCubeVertices()[idx].density;
	float j2 = this->GetMarchingCubeVertices()[idx2].density;
	
	densityX = (this->GetMarchingCubeVertices()[idx].density + this->GetMarchingCubeVertices()[idxX1].density);
	densityZ = (this->GetMarchingCubeVertices()[idx].density + this->GetMarchingCubeVertices()[idxZ1].density);
	densityXZ = (this->GetMarchingCubeVertices()[idx].density + this->GetMarchingCubeVertices()[idxXZ].density);



	j = (j1 + j2)*0.5f;


	//j = 0;
	return ((float)i + 1.0f + j);
}