#include <windows.h>
#include <xnamath.h>

//Ultra simple AABB
//http://devmaster.net/forums/topic/7934-aabb-collision/
struct Lemmi2DAABB
{
	Lemmi2DAABB(){}
	~Lemmi2DAABB(){}

	Lemmi2DAABB(XMFLOAT2 minPoint, XMFLOAT2 maxPoint)
	{
		MinPoint = minPoint;
		MaxPoint = maxPoint;
		CenterPosition = XMFLOAT2(((MinPoint.x+MaxPoint.x)*0.5f), ((MinPoint.y+MaxPoint.y)*0.5f));

		xSize = maxPoint.x - minPoint.x;
		ySize = maxPoint.y - minPoint.y;
	}

	bool Intersects(const Lemmi2DAABB& otherAABB)
	{
		if(otherAABB.MinPoint.x > MaxPoint.x || MinPoint.x > otherAABB.MaxPoint.x)
		{
			return false;
		}

		if(otherAABB.MinPoint.y > MaxPoint.y || MinPoint.y > otherAABB.MaxPoint.y)
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

	XMFLOAT2 GetPositiveVertex( const XMFLOAT3 &normal ) const
	{
		XMFLOAT2 positiveVertex = MinPoint;

		if( normal.x >= 0.0f ) positiveVertex.x = MaxPoint.x;
		if( normal.z >= 0.0f ) positiveVertex.y = MaxPoint.y; //YES, it's supposed to be normal.z. I think.

		positiveVertex.x += CenterPosition.x;
		positiveVertex.y += CenterPosition.y;

		return positiveVertex;
	}

	XMFLOAT2 GetNegativeVertex( const XMFLOAT3 &normal ) const
	{
		XMFLOAT2 negativeVertex = MaxPoint;

		if( normal.x >= 0.0f ) negativeVertex.x = MinPoint.x;
		if( normal.z >= 0.0f ) negativeVertex.y = MinPoint.y;

		negativeVertex.x += CenterPosition.x;
		negativeVertex.y += CenterPosition.y;

		return negativeVertex;
	}

	//Moves the minPoint and moves the maxPoint to minPoint+size
	void Move(float x, float y)
	{
		MinPoint.x = x;
		MinPoint.y = y;
		MaxPoint.x = x+xSize;
		MaxPoint.y = y+ySize;
	}

	private:
	XMFLOAT2 MinPoint, MaxPoint, CenterPosition;
	float xSize, ySize;

};