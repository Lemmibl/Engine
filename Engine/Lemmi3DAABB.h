#pragma once
#include <windows.h>
#include <xnamath.h>

struct Lemmi3DAABB
{
	XMFLOAT3 MinPoint, MaxPoint;

	Lemmi3DAABB(const XMFLOAT3& extMinPoint, const XMFLOAT3& extMaxPoint)
	{
		MinPoint = extMinPoint;
		MaxPoint = extMaxPoint;
	}

	bool Intersects(const Lemmi3DAABB& otherAABB)
	{
		//I do this, to decrease the amount of fetches. 2 vs 6.
		XMFLOAT3 otherMin, otherMax;

		otherMin = otherAABB.MinPoint;
		otherMax = otherAABB.MaxPoint;

		if(otherMin.x > MaxPoint.x || MinPoint.x > otherMax.x)
		{
			return false;
		}

		if(otherMin.y > MaxPoint.y || MinPoint.y > otherMax.y)
		{
			return false;
		}

		if(otherMin.z > MaxPoint.z || MinPoint.z > otherMax.z)
		{
			return false;
		}

		//If everything falls through, then it intersects.
		return true;
	}

	bool Contains(const Lemmi3DAABB& otherAABB)
	{
		//I do this, to decrease the amount of fetches. 2 vs 6.
		XMFLOAT3 otherMin, otherMax;

		otherMin = otherAABB.MinPoint;
		otherMax = otherAABB.MaxPoint;

		if
			(		
			otherMin.x >= MinPoint.x 
			&&	otherMin.y >= MinPoint.y 
			&&	otherMin.z >= MinPoint.z

			&&	otherMax.x <= MaxPoint.x 
			&&	otherMax.y <= MaxPoint.y
			&&	otherMax.z <= MaxPoint.z
			)
		{
			return true;
		}

		return false;
	}

	bool Intersects(float x, float y, float z)
	{
		if(x > MaxPoint.x || MinPoint.x > x)
		{
			return false;
		}

		if(y > MaxPoint.y || MinPoint.y > y)
		{
			return false;
		}

		if(z > MaxPoint.z || MinPoint.z > z)
		{
			return false;
		}

		//If everything falls through, then it intersects.
		return true;
	}

	bool Contains(float x, float y, float z)
	{
		if( x >= MinPoint.x && 
			x <= MaxPoint.x &&

			y >= MinPoint.y	&& 
			y <= MaxPoint.y	&&

			z >= MinPoint.z &&
			z <= MaxPoint.z)
		{
			return true;
		}

		return false;
	}
};