#include "TerrainNoiseSeeder.h"
#include <math.h>       /* sin */

#define PI 3.14159265

TerrainNoiseSeeder::TerrainNoiseSeeder()
{

}

TerrainNoiseSeeder::~TerrainNoiseSeeder()
{
}

void TerrainNoiseSeeder::Initialize(int sizeX, int sizeY, int sizeZ, NoiseClass* simplexNoise, TerrainTypes terrainType)
{
	terrainMode = terrainType;
	noise = simplexNoise;

	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->sizeZ = sizeZ;

	densityToBeInside = 0.2f;

	pulverize = false;
	worldSize = 0;
	worldSizeMargin = 1;
	worldArraySize = sizeX * sizeY;

	XFactor = 1.0f / (2.0f*sizeX);
	YFactor = 1.0f / (2.0f*sizeY);
	ZFactor = 1.0f / (2.0f*sizeZ);
}

void TerrainNoiseSeeder::MCHeightMap()
{
	//float* worldArray = new float[worldArraySize];

	//for(int i = 0; i < worldArraySize; i++)
	//{
	//	float density = noise->Noise2D(marchingCubeVertices[i].position.x, marchingCubeVertices[i].position.z);
	//	worldArray[i] = density;
	//}

	//int i = -2;

	//for (z = worldSizeMargin; z < (sizeZ - worldSizeMargin); z++)
	//{
	//	i++;
	//	for (y = worldSizeMargin; y < (sizeY - worldSizeMargin); y++)
	//	{
	//		for (x = worldSizeMargin; x < (sizeX - worldSizeMargin); x++)
	//		{
	//			i++;

	//			if (worldArray[i] > (float)y)
	//			{
	//			}
	//			idx = x + y*sizeY + z * sizeY * sizeZ;



	//			(*marchingCubeVertices)[idx].density = 100.0f;



	//			if ((*marchingCubeVertices)[idx].density > 0.1f)
	//			{
	//				(*marchingCubeVertices)[idx].inside = true;
	//			}
	//			else
	//			{
	//				(*marchingCubeVertices)[idx].inside = false;
	//			}


	//			(*marchingCubeVertices)[idx].normal.x = marchingCubeVertices[idx - 1].density - marchingCubeVertices[idx+1].density;
	//			(*marchingCubeVertices)[idx].normal.y = marchingCubeVertices[idx - sizeY].density - marchingCubeVertices[idx + sizeY].density;
	//			(*marchingCubeVertices)[idx].normal.z = marchingCubeVertices[idx - (sizeY * sizeZ)].density - marchingCubeVertices[idx + (sizeY * sizeZ)].density;

	//		}
	//	}
	//}

	//if (worldArray != 0)
	//{
	//	delete worldArray;
	//}
}


void TerrainNoiseSeeder::Noise3D(unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int endX, unsigned int endY, unsigned int endZ )
{
	maxDensity = -511.0f;
	minDensity = 511.0f;
	float density;

	switch ( terrainMode ) 
	{
	case SeaBottom:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 1 + sizeY * 0.1f - y;
						density += noise->SimplexNoise3D(position.x/220, position.y/20, position.z/220) * 2.0f;
						density += noise->SimplexNoise3D(position.x/120, position.y/220, position.z/40) * 2.0f;
						density += noise->SimplexNoise3D(position.x/20, position.y/420, position.z/20) * 2.0f;

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}

	case Plains:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 1 + sizeY * 0.2f - y;
						density += noise->SimplexNoise3D(position.x/220, position.y/20, position.z/220) * 10.0f;

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}

	case Hills:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 1 + sizeY * 0.2f - y;
						density += noise->SimplexNoise3D(position.x/220, position.y/20,	 position.z/220) *2.0f;
						density += noise->SimplexNoise3D(position.x/80,	 position.y/220, position.z/40) *20.0f;
						density += noise->SimplexNoise3D(position.x/20,	 position.y/420, position.z/20) *10.0f;

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						if(y == 1)
						{
							density += 1000000.0f;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}

	case Terraces:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 1 + sizeY * 0.4f - y;
						density += noise->SimplexNoise3D( position.x/220,	position.y/220,	position.z/420) *20.0f;
						density += noise->SimplexNoise3D( position.x/120,	position.y/20,	position.z/20) *20.0f;
						density += noise->SimplexNoise3D( position.x/20,	position.y/20,	position.z/120) *20.0f;
						density += noise->SimplexNoise3D( position.x/120,	position.y/20,	position.z/120) *5.0f;

						//**Toplvl**//
						if(y > 20)
						{
							density -= 12.0f*((y-20)/5);
						}
						//if(y > 25)
						//{
						//	density -= 12.0f;
						//}
						//if(y > 30)
						//{
						//	density -= 12.0f;
						//}
						//if(y > 35)
						//{
						//	density -= 12.0f;
						//}

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						if(y == 1)
						{
							density += 1000000.0f;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}


	case DramaticHills:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 1 + sizeY * 0.6f - y;

						density += noise->SimplexNoise3D(position.x/80,	 position.y/80,	 position.z/520) *30.0f;
						density += noise->SimplexNoise3D(position.x/40,	 position.y/30,	 position.z/500) *30.0f;
						density += noise->SimplexNoise3D(position.x/180, position.y/20,	 position.z/50) *20.0f;
						density += noise->SimplexNoise3D(position.x/120, position.y/20,	 position.z/120) *5.0f;
						density += noise->SimplexNoise3D(position.x/80,	 position.y/420, position.z/80) *30.0f;

						//**Toplvl**//
						if(y > 20)
						{
							density -= 12.0f*((y-20)/5);
						}
						//if(y > 25)
						//{
						//	density -= 12.0f;
						//}
						//if(y > 30)
						//{
						//	density -= 12.0f;
						//}
						//if(y > 35)
						//{
						//	density -= 12.0f;
						//}

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						if(y == 1)
						{
							density += 1000000.0f;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}


	case FlyingIslands:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 0 + sizeY * 0.001f - y*0.1f;
						density += noise->SimplexNoise3D(position.x/40, position.y/10,	position.z/25) *10.0f;
						density += noise->SimplexNoise3D(position.x/60, position.y/5,	position.z/35) *10.0f;
						density += noise->SimplexNoise3D(position.x/21, position.y/10,	position.z/43) *10.0f;

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}

	case Alien:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 1 + sizeY * 0.1f - y;
						density += noise->SimplexNoise3D(position.x/220, position.y/20,	 position.z/220) *20.0f;
						density += noise->SimplexNoise3D(position.x/120, position.y/220, position.z/40) *20.0f;
						density += noise->SimplexNoise3D(position.x/20,	 position.y/420, position.z/20) *10.0f;

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						if(y == 1)
						{
							density += 1000000.0f;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}


	case Fancy:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 1 + sizeY * 0.1f - y;
						density += noise->SimplexNoise3D(position.x/220,	position.y/220,	position.z/420) *2.0f;
						density += noise->SimplexNoise3D(position.x/120,	position.y/20,	position.z/20) *20.0f;
						density += noise->SimplexNoise3D(position.x/20,		position.y/20,	position.z/120) *20.0f;
						density += noise->SimplexNoise3D(position.x/120,	position.y/20,	position.z/120) *2.0f;

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						if(y == 1)
						{
							density += 1000000.0f;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}

		break;
	case Cave:
		{
			for (y = startY; y < (endY-1); ++y)
			{
				for (z = startZ; z < (endZ-1); ++z)
				{
					for (x = startX; x < (endX-1); ++x)
					{
						idx = x + (y*sizeY) + (z * sizeY * sizeZ);

						const XMFLOAT3& position = (*marchingCubeVertices)[idx].position;

						density = 7;
						density += noise->SimplexNoise3D(position.x/60,	 position.y/60,	 position.z/60) *20.0f;
						density += noise->SimplexNoise3D(position.x/20,	 position.y/20,	 position.z/120) *10.0f;
						density += noise->SimplexNoise3D(position.x/20,	 position.y/120, position.z/20) *10.0f;
						density += noise->SimplexNoise3D(position.x/120, position.y/20,	 position.z/20) *10.0f;

						if(density < -2)
						{
							density = -2;
						}
						else if (density > 2)
						{
							density = 2;
						}

						if(y == 1)
						{
							density += 1000000.0f;
						}

						(*marchingCubeVertices)[idx].density = density;
					}
				}
			}
			break;
		}

	default:
		{
			density = 1 + sizeY * 0.1f - y;

			if(density < -2)
			{
				density = -2;
			}
			else if (density > 2)
			{
				density = 2;
			}

			(*marchingCubeVertices)[idx].density = density;

			break;
		}
	}

	//if (pulverize == true)
	//{
	//	density += noise->SimplexNoise3D((*marchingCubeVertices)[idx].position.x*0.2f,(*marchingCubeVertices)[idx].position.y*0.2f,(*marchingCubeVertices)[idx].position.z*0.2f)*2.0f;
	//}
	//
	//	////**Hardfloor**//
	//if(y == 1)
	//{
	//	density += 1000000.0f;
	//}
	//
	//
	//if(density < -2)
	//{
	//	density = -2;
	//}
	//else if (density > 2)
	//{
	//	density = 4;
	//}
	//
	//(*marchingCubeVertices)[idx].density = density;

	//if (maxDensity < density)
	//{
	//	maxDensity = density;
	//}
	//else if (minDensity > density)
	//{
	//	minDensity = density;
	//}

	//densityRangeUpper = maxDensity - densityToBeInside;
	//if (densityRangeUpper != 0)
	//{
	//	densityRangeUpper = 1 / densityRangeUpper;
	//}
	//densityRangeLower = densityToBeInside - minDensity;
	//if (densityRangeUpper != 0)
	//{
	//	densityRangeLower = 1 / densityRangeLower;
	//}

	for (y = startY; y < (endY-1); ++y)
	{
		for (z = startZ; z < (endZ-1); ++z)
		{
			for (x = startX; x < (endX-1); ++x)
			{
				idx = x + y*sizeY + z * sizeY * sizeZ;

				//if ((*marchingCubeVertices)[idx].density > densityToBeInside)
				//{
				//	(*marchingCubeVertices)[idx].density = ((*marchingCubeVertices)[idx].density ) * densityRangeUpper;
				//}

				//if ((*marchingCubeVertices)[idx].density < densityToBeInside)
				//{
				//	(*marchingCubeVertices)[idx].density = ((*marchingCubeVertices)[idx].density ) * densityRangeLower;
				//}

				if ((*marchingCubeVertices)[idx].density >= densityToBeInside)
				{
					(*marchingCubeVertices)[idx].inside = true;
				}
				else
				{
					(*marchingCubeVertices)[idx].inside = false;
				}

				//if((*marchingCubeVertices)[idx].density < 0)
				//{
				//	densityArray3D[x][y][z] = 0.0f;
				//}
				//else if((*marchingCubeVertices)[idx].density > 1)
				//{
				//	densityArray3D[x][y][z] = 1.0f;
				//}
				//else
				//{
				//	densityArray3D[x][y][z] = (*marchingCubeVertices)[idx].density;
				//}

				//densityArray3D[x][y][z] = (*marchingCubeVertices)[idx].density;

				CreateMCVerts();
			}
		}
	}
}

void TerrainNoiseSeeder::CreateMCVerts()
{
	(*marchingCubeVertices)[idx].normal.x = ((*marchingCubeVertices)[idx - 1].density -					(*marchingCubeVertices)[idx+1].density)					* XFactor;
	(*marchingCubeVertices)[idx].normal.y = ((*marchingCubeVertices)[idx - sizeY].density -				(*marchingCubeVertices)[idx + sizeY].density)			* YFactor;
	(*marchingCubeVertices)[idx].normal.z = ((*marchingCubeVertices)[idx - (sizeY * sizeZ)].density -	(*marchingCubeVertices)[idx + (sizeY * sizeZ)].density)	* ZFactor;

	//Normalize results.
	float vectorLength = ((*marchingCubeVertices)[idx].normal.x*(*marchingCubeVertices)[idx].normal.x) + ((*marchingCubeVertices)[idx].normal.y*(*marchingCubeVertices)[idx].normal.y) + ((*marchingCubeVertices)[idx].normal.z*(*marchingCubeVertices)[idx].normal.z);

	(*marchingCubeVertices)[idx].normal.x = (*marchingCubeVertices)[idx].normal.x/vectorLength;
	(*marchingCubeVertices)[idx].normal.y = (*marchingCubeVertices)[idx].normal.y/vectorLength;
	(*marchingCubeVertices)[idx].normal.z = (*marchingCubeVertices)[idx].normal.z/vectorLength;
}

float TerrainNoiseSeeder::GetHighestPositionOfCoordinate(int x, int z)
{
	int idx;
	float j = 0.0f;
	int i = 0;
	int idxPreCalc = x + z * sizeY * sizeZ;

	//So what this function does is that it takes a point in our voxel field, then it starts from the absolute highest point in the field, 
	// and goes downward until it hits a point with a density that is high enough to be considered solid.
	for (i = sizeY; i > 0; i--)
	{
		idx = i*sizeY + idxPreCalc;

		//When we find our first solid voxel, we break, because we've found the right point
		if((*marchingCubeVertices)[idx].inside)
		{
			break;
		} 
	}

	//Now we want the point below it
	int idx2 = idx+sizeY;

	//Calculate an average
	j = ((*marchingCubeVertices)[idx].density + (*marchingCubeVertices)[idx2].density)*0.5f;

	//Then we add some fuckin' magic numbers because we're BAD BOYS who don't PLAY BY THE RULES
	return (i + 0.5f + j);
}