#ifndef MCUBES_METABALLS_H
#define MCUBES_METABALLS_H
#include "customStructs.h"
#include <stdio.h>
#include <math.h>

class metaballsClass
{
	

private:
	int i;
	// number of metapoints
	int numberOfMetapoints;

	// the length between points
	double lengthX, lengthY, lengthZ;

	// the metapoints
	metapoint metapoints[3];


public:
	metaballsClass()
	{
		numberOfMetapoints = 2;
		metapoints[0].power = 1.5;
		metapoints[0].posX = 0.0;
		metapoints[0].posY = -2.0;
		metapoints[0].posZ = 0.0;

		metapoints[1].power = 1.5;
		metapoints[1].posX = -4.0;
		metapoints[1].posY = 2.0;
		metapoints[1].posZ = 4.0;
	}

	~metaballsClass();

	double isoValue;

	void setIsoValue(double isoValue)
	{
		this->isoValue = isoValue;
	}

	//move the metaball
	inline void moveBall(int idx, double x, double y, double z)
	{
		metapoints[idx].posX += x;
		metapoints[idx].posY += y;
		metapoints[idx].posZ += z;
	}


	// Returns a point that is interpolated with ten other points for both normals and possition
	inline marchingCubeVertex interpolate(marchingCubeVertex v1, marchingCubeVertex v2)
	{
		marchingCubeVertex v;
		double diff;

		diff = (this->isoValue - v1.flux) / (v2.flux - v1.flux);

		// Finds where on the line our point is possitioned
		v.posX = v1.posX + (v2.posX - v1.posX) * diff;
		v.posY = v1.posY + (v2.posY - v1.posY) * diff;
		v.posZ = v1.posZ + (v2.posZ - v1.posZ) * diff;
		v.flux = (v1.flux + v2.flux) * 0.5;

		// calculates the average normal for each point
		v.normalX = v1.normalX + (v2.normalX - v1.normalX) * diff;
		v.normalY = v1.normalY + (v2.normalY - v1.normalY) * diff;
		v.normalZ = v1.normalZ + (v2.normalZ - v1.normalZ) * diff;

		return v;
	}

	/* henter ut flux-verdi for et gitt punkt i gridden */
	// ???
	inline double get_vertex_value(marchingCubeVertex v)
	{
		double flux = 0.0;

		for (i = 0; i < numberOfMetapoints; i++)
		{
			// calculates the length from this point to the given metapoint
			lengthX = metapoints[i].posX - v.posX;
			lengthY = metapoints[i].posY - v.posY;
			lengthZ = metapoints[i].posZ - v.posZ;
		
			// calculates the power the points are affected by.
			flux += fabs(metapoints[i].power) * metapoints[i].power / (
						lengthX * lengthX + 
						lengthY * lengthY +
						lengthZ * lengthZ + 1);
		}

		return flux;
	}
};
#endif