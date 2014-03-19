#pragma once
#include "TerrainStructs.h"
#include <stdio.h>
#include <windows.h>
#include <xnamath.h> 

class MetaballsClass
{
public:
	MetaballsClass()
	{
		numberOfMetapoints = 2;

		metapoints[0].power = 1.0f;
		metapoints[0].position.x = 0.0f;
		metapoints[0].position.y = -2.0f;
		metapoints[0].position.z = 0.0f;

		metapoints[1].power = 1.0f;
		metapoints[1].position.x = -8.0f;
		metapoints[1].position.y = 2.0f;
		metapoints[1].position.z = 10.0f;
	}

	void SetIsoValue(float isoValue)
	{
		this->isoValue = isoValue;
	}

	float GetIsoValue() const { return isoValue; }

	//move the metaball
	inline void MoveBall(int idx, float x, float y, float z)
	{
		metapoints[idx].position.x += x;
		metapoints[idx].position.y += y;
		metapoints[idx].position.z += z;
	}


	// Returns a point that is interpolated with ten other points for both normals and possition
	inline MarchingCubeVoxel Interpolate(MarchingCubeVoxel v1, MarchingCubeVoxel v2)
	{
		MarchingCubeVoxel v;
		float diff;

		diff = (isoValue - v1.density) / (v2.density - v1.density);

		// Finds where on the line our point is possitioned
		v.position.x = v1.position.x + (v2.position.x - v1.position.x) * diff;
		v.position.y = v1.position.y + (v2.position.y - v1.position.y) * diff;
		v.position.z = v1.position.z + (v2.position.z - v1.position.z) * diff;
		v.density = (v1.density + v2.density) * 0.5f;

		// calculates the average normal for each point
		v.normal.x = v1.normal.x + (v2.normal.x - v1.normal.x) * diff;
		v.normal.y = v1.normal.y + (v2.normal.y - v1.normal.y) * diff;
		v.normal.z = v1.normal.z + (v2.normal.z - v1.normal.z) * diff;

		//Normalize the normal
		float vectorLength = sqrt((v.normal.x*v.normal.x) + (v.normal.y*v.normal.y) + (v.normal.z*v.normal.z));

		v.normal.x /= vectorLength;
		v.normal.y /= vectorLength;
		v.normal.z /= vectorLength;

		return v;
	}

	
	//gets the densety value of a point in the grid
	inline float Get_vertex_value(MarchingCubeVoxel v)
	{
		float density = 0.0;

		for (i = 0; i < numberOfMetapoints; i++)
		{
			// calculates the length from this point to the given metapoint
			lengthX = metapoints[i].position.x - v.position.x;
			lengthY = metapoints[i].position.y - v.position.y;
			lengthZ = metapoints[i].position.z - v.position.z;
		
			// calculates the power the points are affected by.
			density += fabs(metapoints[i].power) * metapoints[i].power / (
						lengthX * lengthX + 
						lengthY * lengthY +
						lengthZ * lengthZ + 1);
		}

		return density;
	}

private:
	int i;

	// number of metapoints
	int numberOfMetapoints;

	// the length between points
	float lengthX, lengthY, lengthZ;
	float isoValue;

	// the metapoints
	Metapoint metapoints[3];
};