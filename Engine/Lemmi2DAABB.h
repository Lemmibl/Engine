#include <windows.h>
#include <xnamath.h>

//Ultra simple AABB
//http://devmaster.net/forums/topic/7934-aabb-collision/
struct Lemmi2DAABB
{
	XMFLOAT2 MinPoint, MaxPoint;

	Lemmi2DAABB(XMFLOAT2 minPoint, XMFLOAT2 maxPoint)
	{
		this->MinPoint = minPoint;
		this->MaxPoint = maxPoint;
	}

	bool Intersects(const Lemmi2DAABB& otherAABB)
	{
		XMFLOAT2 otherMinPoint, otherMaxPoint;

		//I do this to minimize fetches. 2 vs 4
		otherMinPoint = otherAABB.MinPoint;
		otherMaxPoint = otherAABB.MaxPoint;

		if(otherMinPoint.x > MaxPoint.x || MinPoint.x > otherMaxPoint.x)
		{
			return false;
		}

		if(otherMinPoint.y > MaxPoint.y || MinPoint.y > otherMaxPoint.y)
		{
			return false;
		}

		//If everything falls through, then it intersects.
		return true;
	}

	bool Contains(const Lemmi2DAABB& otherAABB)
	{
		XMFLOAT2 otherMinPoint, otherMaxPoint;

		//I do this to minimize fetches. 2 vs 4
		otherMinPoint = otherAABB.MinPoint;
		otherMaxPoint = otherAABB.MaxPoint;

		if( otherMinPoint.x >= MinPoint.x 
		&&	otherMinPoint.y >= MinPoint.y 
		&&	otherMaxPoint.x <= MaxPoint.x 
		&&	otherMaxPoint.y <= MaxPoint.y)
		{
			return true;
		}

		return false;
	}

	bool Intersects(float x, float y)
	{
		if(x > MaxPoint.x || MinPoint.x > x)
		{
			return false;
		}

		if(y > MaxPoint.y || MinPoint.y > y)
		{
			return false;
		}

		//If everything falls through, then it intersects.
		return true;
	}

	bool Contains(float x, float y)
	{
		if( x >= MinPoint.x && 
			x <= MaxPoint.x &&
			y >= MinPoint.y		&& 
			y <= MaxPoint.y	)
		{
			return true;
		}

		return false;
	}
};