////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "frustumclass.h"

#define ANG2RAD 3.14159265358979323846/180.0


FrustumClass::FrustumClass()
{
}


FrustumClass::FrustumClass(const FrustumClass& other)
{
}


FrustumClass::~FrustumClass()
{
}

void FrustumClass::SetInternals( float aspectRatio, float angle, float nearZ, float farZ )
{
	// store the information
	this->aspectRatio = aspectRatio;
	this->angle = angle;
	this->nearZ = nearZ;
	this->farZ = farZ;

	// compute width and height of the near and far plane sections
	float tang = (float)tan(ANG2RAD * angle * 0.5) ;
	nearHeight = nearZ * tang;
	nearWidth = nearHeight * aspectRatio;
	farHeight = farZ  * tang;
	farWidth = farHeight * aspectRatio;
}

XMFLOAT3* FrustumClass::GetFarFrustumCorners(XMVECTOR position, XMVECTOR lookAt, XMVECTOR up)
{
	XMVECTOR farCenter, X,Y,Z;

	// compute the Z axis of camera
	// this axis points in the opposite direction from
	// the looking direction (OpenGL)
	Z = position + lookAt;
	XMVector3Normalize(Z);

	// X axis of camera with given "up" vector and Z axis
	X = up * Z;
	XMVector3Normalize(X);

	// the real "up" vector is the cross product of Z and X
	Y = Z * X;

	// compute the centers of the far plane
	farCenter = position + Z * farZ;
	
	// compute the 4 corners of the frustum on the far plane
	XMStoreFloat3(&farTopLeft, farCenter + Y * farHeight - X * farWidth);
	XMStoreFloat3(&farTopRight, farCenter + Y * farHeight + X * farWidth);
	XMStoreFloat3(&farBottomLeft, farCenter - Y * farHeight - X * farWidth);
	XMStoreFloat3(&farBottomRight, farCenter - Y * farHeight + X * farWidth);

	XMFLOAT3 fourPoints[4] = {farTopLeft, farTopRight, farBottomLeft, farBottomRight};
	
	return fourPoints;
}

XMFLOAT3* FrustumClass::GetNearFrustumCorners(XMVECTOR position, XMVECTOR lookAt, XMVECTOR up)
{
	XMVECTOR nearCenter, X,Y,Z;

	// compute the Z axis of camera
	// this axis points in the opposite direction from
	// the looking direction (OpenGL)
	Z = position + lookAt;
	XMVector3Normalize(Z);

	// X axis of camera with given "up" vector and Z axis
	X = up * Z;
	XMVector3Normalize(X);

	// the real "up" vector is the cross product of Z and X
	Y = Z * X;

	// compute the centers of the far plane
	nearCenter = position + Z * nearZ;

	// compute the 4 corners of the frustum on the near plane
	XMStoreFloat3(&nearTopLeft, nearCenter + (Y * nearHeight) - (X * nearWidth));
	XMStoreFloat3(&nearTopRight, nearCenter + Y * nearHeight + X * nearWidth);
	XMStoreFloat3(&nearBottomLeft, nearCenter - Y * nearHeight - X * nearWidth);
	XMStoreFloat3(&nearBottomRight, nearCenter - Y * nearHeight + X * nearWidth);

	XMFLOAT3 fourPoints[4] = {nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight};

	return fourPoints;


}

void FrustumClass::ConstructFrustum(float screenDepth, XMFLOAT4X4 projectionMatrix, XMFLOAT4X4 viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix, view, proj;

	view = XMLoadFloat4x4(&viewMatrix);
	proj = XMLoadFloat4x4(&projectionMatrix);

	// Calculate the minimum Z distance in the frustum.
	zMinimum = -proj._43 / proj._33;
	r = screenDepth / (screenDepth - zMinimum);
	proj._33 = r;
	proj._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	matrix = XMMatrixMultiply(view, proj);

	// Calculate near plane of frustum.
	planes[0].a = matrix._14 + matrix._13;
	planes[0].b = matrix._24 + matrix._23;
	planes[0].c = matrix._34 + matrix._33;
	planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&planes[0], &planes[0]);

	// Calculate far plane of frustum.
	planes[1].a = matrix._14 - matrix._13; 
	planes[1].b = matrix._24 - matrix._23;
	planes[1].c = matrix._34 - matrix._33;
	planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&planes[1], &planes[1]);

	// Calculate left plane of frustum.
	planes[2].a = matrix._14 + matrix._11; 
	planes[2].b = matrix._24 + matrix._21;
	planes[2].c = matrix._34 + matrix._31;
	planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&planes[2], &planes[2]);

	// Calculate right plane of frustum.
	planes[3].a = matrix._14 - matrix._11; 
	planes[3].b = matrix._24 - matrix._21;
	planes[3].c = matrix._34 - matrix._31;
	planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&planes[3], &planes[3]);

	// Calculate top plane of frustum.
	planes[4].a = matrix._14 - matrix._12; 
	planes[4].b = matrix._24 - matrix._22;
	planes[4].c = matrix._34 - matrix._32;
	planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&planes[4], &planes[4]);

	// Calculate bottom plane of frustum.
	planes[5].a = matrix._14 + matrix._12;
	planes[5].b = matrix._24 + matrix._22;
	planes[5].c = matrix._34 + matrix._32;
	planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&planes[5], &planes[5]);

	return;
}

bool FrustumClass::CheckPoint(float x, float y, float z)
{
	int i;

	// Check if the point is inside all six planes of the view frustum.
	for(i=0; i<6; i++) 
	{
		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3(x, y, z)) < 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;

	// Check if any one point of the cube is in the view frustum.
	for(i=0; i<6; i++) 
	{
		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool FrustumClass::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	int i;


	// Check if the radius of the sphere is inside the view frustum.
	for(i=0; i<6; i++) 
	{
		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3(xCenter, yCenter, zCenter)) < -radius)
		{
			return false;
		}
	}

	return true;
}

bool FrustumClass::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;


	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for(i=0; i<6; i++)
	{
		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter - zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter - ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + xSize), (yCenter + ySize), (zCenter + zSize))) >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}