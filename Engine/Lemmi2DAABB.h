#pragma once
#include <windows.h>
#include <xnamath.h>

//Ultra simple AABB
//http://devmaster.net/forums/topic/7934-aabb-collision/
class Lemmi2DAABB
{
	public:

	Lemmi2DAABB(){}
	Lemmi2DAABB(XMFLOAT2 minPoint, XMFLOAT2 maxPoint)
	{
		this->minPoint = minPoint;
		this->maxPoint = maxPoint;
		XMStoreFloat(&(radius), XMVector2Length(XMVectorSet(((maxPoint.x-minPoint.x)*0.5f), ((maxPoint.y-minPoint.y)*0.5f), 0.0f, 1.0f)));

		xSize = maxPoint.x - minPoint.x;
		ySize = maxPoint.y - minPoint.y;
	}

	~Lemmi2DAABB(){}

	XMFLOAT2 MinPoint() const { return minPoint; }
	XMFLOAT2 MaxPoint() const { return maxPoint; }


	bool Intersects(const Lemmi2DAABB& otherAABB)
	{
		if(otherAABB.MinPoint().x > maxPoint.x || minPoint.x > otherAABB.MaxPoint().x)
		{
			return false;
		}

		if(otherAABB.MinPoint().y > maxPoint.y || minPoint.y > otherAABB.MaxPoint().y)
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
		otherMinPoint = otherAABB.MinPoint();
		otherMaxPoint = otherAABB.MaxPoint();

		if( otherMinPoint.x >= minPoint.x 
			&&	otherMinPoint.y >= minPoint.y 
			&&	otherMaxPoint.x <= maxPoint.x 
			&&	otherMaxPoint.y <= maxPoint.y)
		{
			return true;
		}

		return false;
	}

	bool Intersects(float x, float y)
	{
		if(x > maxPoint.x || minPoint.x > x)
		{
			return false;
		}

		if(y > maxPoint.y || minPoint.y > y)
		{
			return false;
		}

		//If everything falls through, then it intersects.
		return true;
	}

	bool Contains(float x, float y)
	{
		if( x >= minPoint.x && 
			x <= maxPoint.x &&
			y >= minPoint.y	&& 
			y <= maxPoint.y	)
		{
			return true;
		}

		return false;
	}

	XMFLOAT2 GetPositiveVertex( const XMFLOAT3 &normal ) const
	{
		XMFLOAT2 positiveVertex = minPoint;

		if( normal.x >= 0.0f ) positiveVertex.x = maxPoint.x;
		if( normal.z >= 0.0f ) positiveVertex.y = maxPoint.y; //YES, it's supposed to be normal.z. I think.

		positiveVertex.x += centerPosition.x;
		positiveVertex.y += centerPosition.y;

		return positiveVertex;
	}

	XMFLOAT2 GetNegativeVertex( const XMFLOAT3 &normal ) const
	{
		XMFLOAT2 negativeVertex = maxPoint;

		if( normal.x >= 0.0f ) negativeVertex.x = minPoint.x;
		if( normal.z >= 0.0f ) negativeVertex.y = minPoint.y;

		negativeVertex.x += centerPosition.x;
		negativeVertex.y += centerPosition.y;

		return negativeVertex;
	}

	//Moves the minPoint and moves the maxPoint to minPoint+size
	void Move(float x, float y)
	{
		minPoint.x = x;
		minPoint.y = y;
		maxPoint.x = x+xSize;
		maxPoint.y = y+ySize;
	}

	private:
	XMFLOAT2 minPoint, maxPoint, centerPosition;

	float xSize, ySize, radius;

};