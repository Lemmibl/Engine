//1.) Check each corner point of the rectangle against the frustum. 
//	If any point is in the frustum the rectangle is in view. 
//	If no point is in the frustum, move to step #2.
//
//2.) Now check if all 4 corner points of the rectangle are on the outside of any one of the 6 frustum planes. 
//	If all 4 points are outside of one of the planes then the rectangle is not in view. 
//	But if all 4 points are not outside any of the planes the rectangle is in view.

//float3 AABB::ExtremePoint(const float3 &direction) const
//{
//	float3 pt;
//	pt.x = (direction.x >= 0.f ? maxPoint.x : minPoint.x);
//	pt.y = (direction.y >= 0.f ? maxPoint.y : minPoint.y);
//	pt.z = (direction.z >= 0.f ? maxPoint.z : minPoint.z);
//	return pt;
//}

////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FRUSTUMCLASS_H_
#define _FRUSTUMCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>
#include <Xnamath.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: FrustumClass
////////////////////////////////////////////////////////////////////////////////
class FrustumClass
{
public:
	FrustumClass();
	FrustumClass(const FrustumClass&);
	~FrustumClass();

	void SetInternals(float aspectRatio, float angle, float nearZ, float farZ);

	//Returns the four frustum corners in a float4. [0] = top left, [1] = top right, [2] = bottom left, [3] = bottom right
	XMVECTOR* GetFarFrustumCorners(XMVECTOR position, XMVECTOR lookAt, XMVECTOR up);

	//Returns the four frustum corners in a float4. [0] = top left, [1] = top right, [2] = bottom left, [3] = bottom right
	XMVECTOR* GetNearFrustumCorners(XMVECTOR position, XMVECTOR lookAt, XMVECTOR up);

	void ConstructFrustum(float, XMMATRIX, XMMATRIX);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:
	D3DXPLANE planes[6];
	float aspectRatio, angle, nearZ, farZ;
	float nearHeight, nearWidth, farHeight, farWidth;
	XMVECTOR nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	XMVECTOR farTopLeft, farTopRight, farBottomLeft, farBottomRight;
};

#endif