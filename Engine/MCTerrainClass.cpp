#include "MCTerrainClass.h"
#include <math.h>       /* sin */

#define PI 3.14159265

MCTerrainClass::MCTerrainClass(void)
{

}



MCTerrainClass::~MCTerrainClass(void)
{
}

void MCTerrainClass::Initialize(int sizeX, int sizeY, int sizeZ, MarchingCubeVertex *marchingCubeVertices, SimplexNoise* simplexNoise)
{
	noise = simplexNoise;
	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->sizeZ = sizeZ;
	this->marchingCubeVertices = marchingCubeVertices;
	static const int a = sizeX;

	densityToBeInside = 0.2f;

	//densityArray3D.resize(sizeZ);
	//for (int i = 0; i < sizeZ; ++i)
	//{
	//	densityArray3D[i].resize(sizeY);
	//	for (int j = 0; j < sizeY; ++j)
	//	{
	//		densityArray3D[i][j].resize(sizeX);
	//	}
	//}

	pulverize = false;
	worldSize = 0;
	worldSizeMargin = 1;
	worldArraySize = sizeX * sizeY;

	XFactor = 1.0f / (2.0f*sizeX);
	YFactor = 1.0f / (2.0f*sizeY);
	ZFactor = 1.0f / (2.0f*sizeZ);
}

void MCTerrainClass::MCHeightMap()
{

	float* worldArray = new float[worldArraySize];

	for(int i = 0; i < worldArraySize; i++)
	{
		float density = noise->Noise2D(marchingCubeVertices[i].position.x, marchingCubeVertices[i].position.z);
		worldArray[i] = density;
	}

	int i = -2;

	for (z = worldSizeMargin; z < (sizeZ - worldSizeMargin); z++)
	{
		i++;
		for (y = worldSizeMargin; y < (sizeY - worldSizeMargin); y++)
		{
			for (x = worldSizeMargin; x < (sizeX - worldSizeMargin); x++)
			{
				i++;

				if (worldArray[i] > (float)y)
				{
				}
				idx = x + y*sizeY + z * sizeY * sizeZ;



				marchingCubeVertices[idx].density = 100.0f;



				if (marchingCubeVertices[idx].density > 0.1f)
				{
					marchingCubeVertices[idx].inside = true;
				}
				else
				{
					marchingCubeVertices[idx].inside = false;
				}


				marchingCubeVertices[idx].normal.x = marchingCubeVertices[idx - 1].density - marchingCubeVertices[idx+1].density;
				marchingCubeVertices[idx].normal.y = marchingCubeVertices[idx - sizeY].density - marchingCubeVertices[idx + sizeY].density;
				marchingCubeVertices[idx].normal.z = marchingCubeVertices[idx - (sizeY * sizeZ)].density - marchingCubeVertices[idx + (sizeY * sizeZ)].density;

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

	for (z = 1; z < (sizeZ-1); z++)
	{
		for (y = 1; y < (sizeY-1); y++)
		{
			for (x = 1; x < (sizeX-1); x++)
			{
				idx = x + (y*sizeY) + (z * sizeY * sizeZ);

				switch ( terrainMode ) {
				case terrainTypesSeaBottom:
					density = 1 + sizeY * 0.1f - y;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/220,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/220) *2.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/220,marchingCubeVertices[idx].position.z/40) *2.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/420,marchingCubeVertices[idx].position.z/20) *2.0f;
					break;
				case terrainTypePlains:
					density = 1 + sizeY * 0.2f - y;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/220,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/220) *10.0f;
					break;
				case terrainTypeHills:
					density = 1 + sizeY * 0.2f - y;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/220,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/220) *2.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/80,marchingCubeVertices[idx].position.y/220,marchingCubeVertices[idx].position.z/40) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/420,marchingCubeVertices[idx].position.z/20) *10.0f;
					break;
				case terrainTypeTerraces:
					density = 1 + sizeY * 0.4f - y;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/220,marchingCubeVertices[idx].position.y/220,marchingCubeVertices[idx].position.z/420) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/20) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/120) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/120) *5.0f;

					//**Toplvl**//
					if(y > 20)
					{
						density -= 12.0f;
					}
					if(y > 25)
					{
						density -= 12.0f;
					}
					if(y > 30)
					{
						density -= 12.0f;
					}
					if(y > 35)
					{
						density -= 12.0f;
					}
					break;

				case terrainTypeDramaticHills:
					density = 1 + sizeY * 0.6f - y;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/80,marchingCubeVertices[idx].position.y/80,marchingCubeVertices[idx].position.z/520) *30.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/40,marchingCubeVertices[idx].position.y/30,marchingCubeVertices[idx].position.z/500) *30.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/180,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/50) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/120) *5.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/80,marchingCubeVertices[idx].position.y/420,marchingCubeVertices[idx].position.z/80) *30.0f;

					//**Toplvl**//
					if(y > 20)
					{
						density -= 12.0f;
					}
					if(y > 25)
					{
						density -= 12.0f;
					}
					if(y > 30)
					{
						density -= 12.0f;
					}
					if(y > 35)
					{
						density -= 12.0f;
					}
					break;
				case terrainTypeFlyingIslands:
					
					density = 0 + sizeY * 0.001f - y*0.1f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/40,marchingCubeVertices[idx].position.y/10,marchingCubeVertices[idx].position.z/25) *10.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/60,marchingCubeVertices[idx].position.y/5,marchingCubeVertices[idx].position.z/35) *10.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/21,marchingCubeVertices[idx].position.y/10,marchingCubeVertices[idx].position.z/43) *10.0f;

					break;
				case terrainTypeAlien:
					density = 1 + sizeY * 0.1f - y;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/220,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/220) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/220,marchingCubeVertices[idx].position.z/40) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/420,marchingCubeVertices[idx].position.z/20) *10.0f;
					break;
				case terrainTypeFancy:
					density = 1 + sizeY * 0.1f - y;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/220,marchingCubeVertices[idx].position.y/220,marchingCubeVertices[idx].position.z/420) *2.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/20) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/120) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/120) *2.0f;
					break;
				case terrainTypeCave:
					density = 7;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/60,marchingCubeVertices[idx].position.y/60,marchingCubeVertices[idx].position.z/60) *20.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/120) *10.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/120,marchingCubeVertices[idx].position.z/20) *10.0f;
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/20) *10.0f;
					break;
				default:


					density = 1 + sizeY * 0.1f - y;

					break;
				}

				if (pulverize == true)
				{
					density += noise->SimplexNoise3D(marchingCubeVertices[idx].position.x*0.2f,marchingCubeVertices[idx].position.y*0.2f,marchingCubeVertices[idx].position.z*0.2f)*2.0f;
				}

				/*
				density = 1 + sizeY * 0.15f - y;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/220,marchingCubeVertices[idx].position.y/220,marchingCubeVertices[idx].position.z/420) *2.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/20) *3.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/20,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/120) *2.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/80,marchingCubeVertices[idx].position.y/80,marchingCubeVertices[idx].position.z/80) *2.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/320,marchingCubeVertices[idx].position.z/20) *3.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/10,marchingCubeVertices[idx].position.y/10,marchingCubeVertices[idx].position.z/10) *3.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/15,marchingCubeVertices[idx].position.y/15,marchingCubeVertices[idx].position.z/30) *5.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/5,marchingCubeVertices[idx].position.y/5,marchingCubeVertices[idx].position.z/5) *5.0f;
				density += noise->noise3D2(marchingCubeVertices[idx].position.x/120,marchingCubeVertices[idx].position.y/20,marchingCubeVertices[idx].position.z/220) *3.0f;
				*/

				/*
				density = 1;// + sizeY * 0.1f - y;
				density += noise->noise3D2(sin (marchingCubeVertices[idx].position.x*PI/180)*0.5f,sin (marchingCubeVertices[idx].position.y*PI/180)*0.5f,sin (marchingCubeVertices[idx].position.z*PI/180)*0.5f) *20.0f;
				density += noise->noise3D2(sin (marchingCubeVertices[idx].position.x*PI/180)*0.5f,sin (marchingCubeVertices[idx].position.y*PI/180)*0.2f,sin (marchingCubeVertices[idx].position.z*PI/180)*0.5f) *20.0f;
				density += noise->noise3D2(sin (marchingCubeVertices[idx].position.x*PI/180),sin (marchingCubeVertices[idx].position.y*PI/180),sin (marchingCubeVertices[idx].position.z*PI/180)) *20.0f;
				density += noise->noise3D2(sin (marchingCubeVertices[idx].position.x*PI/180)*0.1f,sin (marchingCubeVertices[idx].position.y*PI/180)*0.1f,sin (marchingCubeVertices[idx].position.z*PI/180)*0.2f) *20.0f;
				density += noise->noise3D2(sin (marchingCubeVertices[idx].position.x*PI/180)*0.2f,sin (marchingCubeVertices[idx].position.y*PI/180)*0.1f,sin (marchingCubeVertices[idx].position.z*PI/180)*0.1f) *20.0f;
				*/

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

				marchingCubeVertices[idx].density = density;

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

	//maxDensity = maxDensity;
	//minDensity = minDensity;

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

	for (z = 1; z < (sizeZ -1); z++)
	{
		for (y = 1; y < (sizeY -1); y++)
		{
			for (x = 1; x < (sizeX -1); x++)
			{
				idx = x + y*sizeY + z * sizeY * sizeZ;

				if (marchingCubeVertices[idx].density > densityToBeInside)
				{
					marchingCubeVertices[idx].density = (marchingCubeVertices[idx].density ) * densityRangeUpper;
				}

				if (marchingCubeVertices[idx].density < densityToBeInside)
				{
					marchingCubeVertices[idx].density = (marchingCubeVertices[idx].density ) * densityRangeLower;
				}

				if (marchingCubeVertices[idx].density > densityToBeInside)
				{
					marchingCubeVertices[idx].inside = true;
				}
				else
				{
					marchingCubeVertices[idx].inside = false;
				}

				//if(marchingCubeVertices[idx].density < 0)
				//{
				//	densityArray3D[x][y][z] = 0.0f;
				//}
				//else if(marchingCubeVertices[idx].density > 1)
				//{
				//	densityArray3D[x][y][z] = 1.0f;
				//}
				//else
				//{
				//	densityArray3D[x][y][z] = marchingCubeVertices[idx].density;
				//}

				//densityArray3D[x][y][z] = marchingCubeVertices[idx].density;
				
				CreateMCVerts();
			}
		}
	}
}

void MCTerrainClass::CreateMCVerts()
{
	marchingCubeVertices[idx].normal.x = (marchingCubeVertices[idx - 1].density -				marchingCubeVertices[idx+1].density)					* XFactor;
	marchingCubeVertices[idx].normal.y = (marchingCubeVertices[idx - sizeY].density -			marchingCubeVertices[idx + sizeY].density)				* YFactor;
	marchingCubeVertices[idx].normal.z = (marchingCubeVertices[idx - (sizeY * sizeZ)].density -	marchingCubeVertices[idx + (sizeY * sizeZ)].density)	* ZFactor;

	//float normalX = ((densityArray3D[x-1][y][z]	- densityArray3D[x+1][y][z])) * XFactor;
	//float normalY = ((densityArray3D[x][y-1][z]	- densityArray3D[x][y+1][z])) * YFactor;
	//float normalZ = ((densityArray3D[x][y][z-1]	- densityArray3D[x][y][z+1])) * ZFactor;

	//marchingCubeVertices[idx].normal.x = normalX;
	//marchingCubeVertices[idx].normal.y = normalY;
	//marchingCubeVertices[idx].normal.z = normalZ;

	////Normalize results.
	//float vectorLength = (normalX*normalX) + (normalY*normalY) + (normalZ*normalZ);

	//marchingCubeVertices[idx].normal.x = normalX/vectorLength;
	//marchingCubeVertices[idx].normal.y = normalY/vectorLength;
	//marchingCubeVertices[idx].normal.z = normalZ/vectorLength;
}

float MCTerrainClass::GetHighestPositionOfCoordinate(int x, int z)
{
	float j = 0.0f;
	int i = 0;
	int idxPreCalc = x + z * sizeY * sizeZ;

	//So what this function does is that it takes a point in our voxel field, then it starts from the absolute highest point in the field, 
	// and goes downward until it hits a point with a density that is high enough to be considered solid.
	for (i = sizeY; i > 0; i--)
	{
		idx = i*sizeY + idxPreCalc;

		//When we find our first solid voxel, we break, because we've found the right point
		if(marchingCubeVertices[idx].inside)
		{
			break;
		} 
	}

	//Now we want the point below it
	int idx2 = idx+sizeY;

	//Calculate an average
	j = (marchingCubeVertices[idx].density + marchingCubeVertices[idx2].density)*0.5f;

	//Then we add some fuckin' magic numbers because we're BAD BOYS who don't PLAY BY THE RULES
	return (i + 0.5f + j);
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
		idx = xAsInt + i*sizeY + zAsInt * sizeY * sizeZ;
		if(marchingCubeVertices[idx].inside)
		{
			tempBool = true;
		} 
	}

	float decimalX = (x - int(x));

	float totalDensity = 0.0f;
	float densityX = 0.0f;
	float densityZ = 0.0f;
	float densityXZ = 0.0f;

	int idx = xAsInt + (i+1)*sizeY + zAsInt * sizeY * sizeZ;
	int idx2 = xAsInt + (i+2)*sizeY + zAsInt * sizeY * sizeZ;
	int idxX1 = (xAsInt +1) + (i+1)*sizeY + zAsInt * sizeY * sizeZ;
	int idxZ1 = xAsInt + (i+1)*sizeY + (zAsInt + 1) * sizeY * sizeZ;
	int idxXZ = (xAsInt +1) + (i+1)*sizeY + (zAsInt + 1) * sizeY * sizeZ;

	float j1 = marchingCubeVertices[idx].density;
	float j2 = marchingCubeVertices[idx2].density;

	densityX = (marchingCubeVertices[idx].density + marchingCubeVertices[idxX1].density);
	densityZ = (marchingCubeVertices[idx].density + marchingCubeVertices[idxZ1].density);
	densityXZ = (marchingCubeVertices[idx].density + marchingCubeVertices[idxXZ].density);



	j = (j1 + j2)*0.5f;


	//j = 0;
	return ((float)i + 1.0f + j);
}