////////////////////////////////////////////////////////////////////////////////
// Filename: frustumclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "frustumclass.h"

#define ANG2RAD 3.14159265358979323846/180.0

//http://pastebin.com/PEeDKLET
//http://www.gamedev.net/topic/499753-extract-frustum-corners/
//http://www.geometrictools.com/LibMathematics/Intersection/Intersection.html

FrustumClass::FrustumClass()
{
}


FrustumClass::FrustumClass(const FrustumClass& other)
{
}


FrustumClass::~FrustumClass()
{
}

void FrustumClass::SetInternals( float aspectRatio, float FOV, float nearZ, float farZ )
{
	// store the information
	if(aspectRatio != 0)
	{
		this->aspectRatio = aspectRatio;
	}

	if(FOV != 0)
	{
		this->angle = FOV;
	}
	
	if(nearZ != 0)
	{
		this->nearZ = nearZ;
	}

	if(farZ != 0)
	{
		this->farZ = farZ;
	}

	/*
	The height and width of the rectangular boundary on the near plane are defined as follows:

	Hnear = 2 * tan(fov / 2) * nearDist
	Wnear = Hnear * ratio

	The same reasoning can be applied to the far plane:

	Hfar = 2 * tan(fov / 2) * farDist
	Wfar = Hfar * ratio 
	*/

	float tanVal = 2.0f * (float)tan((ANG2RAD * FOV) / 2);

	// compute width and height of the near and far plane sections
	farHeight = tanVal * farZ;
	farWidth = farHeight * aspectRatio;

	//I want an "Orthogonal" frustum
	nearHeight = tanVal * nearZ;
	nearWidth = nearHeight * aspectRatio;
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

	XMFLOAT3* fourPoints = new XMFLOAT3[4]();

	fourPoints[0] = farTopLeft;
	fourPoints[1] = farTopRight;
	fourPoints[2] = farBottomLeft;
	fourPoints[3] = farBottomRight;

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

	XMFLOAT3* fourPoints = new XMFLOAT3[4]();
	
	fourPoints[0] = nearTopLeft;
	fourPoints[1] = nearTopRight;
	fourPoints[2] = nearBottomLeft;
	fourPoints[3] = nearBottomRight;

	return fourPoints;
}

void FrustumClass::CalculateXZBounds( XMVECTOR position, XMVECTOR lookAt, XMVECTOR up )
{
	XMVECTOR farCenter, nearCenter, X, Y, Z;

	// compute the Z axis of camera
	// this axis points in the opposite direction from
	// the looking direction (OpenGL)
	Z = XMVector3Normalize(lookAt);

	// X axis of camera with given "up" vector and Z axis
	X = up * Z;
	X = XMVector3Normalize(X);

	// the real "up" vector is the cross product of Z and X
	Y = Z * X;

	// compute the centers of the far plane
	nearCenter = position + Z * nearZ;

	// compute the centers of the far plane
	farCenter = position + Z * farZ;

	// compute the 4 relevant corners of the frustum
	XMStoreFloat3(&farBottomLeft,	farCenter	- X * farWidth);
	XMStoreFloat3(&farBottomRight,	farCenter	+ X * farWidth);
	XMStoreFloat3(&nearBottomLeft,	nearCenter	- X * nearWidth);
	XMStoreFloat3(&nearBottomRight, nearCenter	+ X * nearWidth);

	/*
	- Y * farHeight		
	- Y * farHeight		
	- Y * nearHeight	
	- Y * nearHeight	
	*/

	farLeft		=	XMFLOAT2(farBottomLeft.x,	farBottomLeft.z);
	farRight	=	XMFLOAT2(farBottomRight.x,	farBottomRight.z);
	nearLeft	=	XMFLOAT2(nearBottomLeft.x,	nearBottomLeft.z);
	nearRight	=	XMFLOAT2(nearBottomRight.x, nearBottomRight.z);
}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX* projectionMatrix, XMMATRIX* viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix, view, proj;

	view = *viewMatrix;
	proj = *projectionMatrix;

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

//// check whether an AABB intersects the frustum
//BoundingVolume::TestResult Frustum::testIntersection( shared_ptr<const BoundingBox> box ) const
//{
//	TestResult result = TEST_INSIDE;
//
//	for( uint i = 0; i < 6; i++ )
//	{
//		const float pos = m_planes[i].w;
//		const vec3 normal = vec3(m_planes[i]);
//
//		if( glm::dot(normal, box->getPositiveVertex(normal))+pos < 0.0f )
//		{
//			return TEST_OUTSIDE;
//		}
//
//		if( glm::dot(normal, box->getNegativeVertex(normal))+pos < 0.0f )
//		{
//			result = TEST_INTERSECT;
//		}
//	}
//
//	return result;
//}


/*
 Method 5: If you really don’t care whether a box is partially or fully inside
 All variants of the test before could still distinguish partially inside (intersecting) boxes from ones that are fully inside (this goes for the homogeneous tests as well, 
 using the analog of Cohen-Sutherland clipping outcodes; Google it if you don’t know the technique). I’m now gonna throw that away in the pursuit of even more speed :). 
 
 The test we’re looking at (per-plane) is:

 return dot3(center, plane) + dot3(extent, absPlane) > -plane.w;
 
 The 2 dot products are somewhat annoying. They’re “almost” with the same value, except for sign.
  If we could rewrite both as dot products with “plane”, we could use linearity and do one vector add and one dot product. 
  To do this, we need to get the sign flips into extent – something like:

 return dot3(center + extent * signFlip, plane) > -plane.w;
 where signFlip = (sgn(plane.x), sgn(plane.y), sgn(plane.z)) using the standard signum function and using componentwise multiply for the product “extent * signFlip”. 
 
 For scalar code, you’re best off using this code and precomputing a “signFlip” vector for each “plane” (it replaces “absPlane”).
  For SIMD though, we can usually do some amount of integer math on IEEE floating-point values that makes this much easier: 
  We can extract the signs of “plane” by doing a binary and with the mask 0×80000000 and replace “extent * signFlip” with a XOR:

 vector4 signFlip = componentwise_and(plane, 0x80000000);
 return dot3(center + xor(extent, signFlip), plane) > -plane.w;
 Voila, down to 6 dot products, 6 vector adds, 6 xors and 6 compares to test against 6 planes (plus a very small amount of setup). There’s other tricks you can do, 
 but this particular test is so tight that it’s really hard to get any wins out of common tricks. 
 
 For example, you can exploit that the near and far plane are usually parallel (we implicitly used this for Method 3). 
 This saves some work with the Method 4-style tests, but it’s a wash for this variant.

 Is it possible to do better? Maybe. I have no idea. I can however say that this is the fastest method I know right now.


*/

bool FrustumClass::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	//int i;

	//// Check if any one point of the cube is in the view frustum.
	//for(i=0; i<6; i++) 
	//{
	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter - radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter - radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter - radius), (zCenter + radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter - radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	if(D3DXPlaneDotCoord(&planes[i], &D3DXVECTOR3((xCenter + radius), (yCenter + radius), (zCenter + radius))) >= 0.0f)
	//	{
	//		continue;
	//	}

	//	return false;
	//}

	//return true;


	for(unsigned int iPlane = 0; iPlane < 6; ++iPlane)
	{
		const D3DXPLANE* frustumPlane = &planes[iPlane];

		float d =	xCenter * planes->a + 
					yCenter * planes->b + 
					zCenter * planes->c;

		float r =	xCenter+radius * fabs(planes->a) + 
					yCenter+radius * fabs(planes->b) + 
					zCenter+radius * fabs(planes->c);

		float d_m_r = d - r;
		float d_p_r = d + r;
		 
		if(d_p_r < -planes->d)
		{
			return false; // Outside
			break;
		}
		else if(d_m_r < -planes->d)
		{
			return true; // Intersect
		}
	}

	return false;
}


bool FrustumClass::Check2DAABB(Lemmi2DAABB* aabb)
{
	XMFLOAT2 minPoint, maxPoint;

	//We need to make sure that "maxPoint" actually is the farthest away from the camera, 
	//it gets weird if we rotate camera 180 degrees the wrong way
	minPoint = XMFLOAT2(min(min(nearLeft.x, nearRight.x), min(farRight.x, farLeft.x)), min(min(nearLeft.y, nearRight.y), min(farRight.y, farLeft.y)));
	maxPoint = XMFLOAT2(max(max(nearLeft.x, nearRight.x), max(farRight.x, farLeft.x)), max(max(nearLeft.y, nearRight.y), max(farRight.y, farLeft.y)));

	Lemmi2DAABB thisAABB = Lemmi2DAABB(minPoint, maxPoint);

	if(thisAABB.Intersects((const Lemmi2DAABB) *aabb))
	{
		return true;
	}

	return false;
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

void FrustumClass::CalculateFrustumExtents( Lemmi2DAABB* outAABB, XMVECTOR position, XMVECTOR lookAt, XMVECTOR up)
{
	XMVECTOR farCenter, nearCenter, X, Y, Z;

	// compute the Z axis of camera
	// this axis points in the opposite direction from
	// the looking direction (OpenGL)
	Z = XMVector3Normalize(lookAt);

	// X axis of camera with given "up" vector and Z axis
	X = up * Z;
	X = XMVector3Normalize(X);

	// the real "up" vector is the cross product of Z and X
	Y = Z * X;

	// compute the centers of the near plane
	nearCenter = position + Z * -nearZ;

	// compute the centers of the far plane
	farCenter = position + Z * farZ;

	// compute the 4 relevant corners of the frustum
	XMStoreFloat3(&farBottomLeft,	farCenter	- X * farWidth);
	XMStoreFloat3(&farBottomRight,	farCenter	+ X * farWidth);
	XMStoreFloat3(&nearBottomLeft,	nearCenter	- X * farWidth); //nearWidth
	XMStoreFloat3(&nearBottomRight, nearCenter	+ X * farWidth); //nearWidth

	float minX, minZ, maxX, maxZ;

	minX = min(min(farBottomLeft.x, farBottomRight.x), min(nearBottomLeft.x, nearBottomRight.x));
	minZ = min(min(farBottomLeft.z, farBottomRight.z), min(nearBottomLeft.z, nearBottomRight.z));

	maxX = max(max(farBottomLeft.x, farBottomRight.x), max(nearBottomLeft.x, nearBottomRight.x));
	maxZ = max(max(farBottomLeft.z, farBottomRight.z), max(nearBottomLeft.z, nearBottomRight.z));


	//This is what is being returned
	outAABB->Resize(fabs(minX - maxX), fabs(minZ - maxZ));
	outAABB->Move(minX, minZ); // = Lemmi2DAABB(XMFLOAT2(minX, minZ), XMFLOAT2(maxX, maxZ));
}

/*
struct AABB {

vec3x min;

vec3x max;

};



struct Frustum {

vec4f planes[6];



Frustum(const mat4f &m)

{

const vec4f r1(m.elem[0][0], m.elem[0][1], m.elem[0][2], m.elem[0][3]);

const vec4f r2(m.elem[1][0], m.elem[1][1], m.elem[1][2], m.elem[1][3]);

const vec4f r3(m.elem[2][0], m.elem[2][1], m.elem[2][2], m.elem[2][3]);

const vec4f r4(m.elem[3][0], m.elem[3][1], m.elem[3][2], m.elem[3][3]);



planes[0] = r4 + r1;

planes[1] = r4 - r1;

planes[2] = r4 + r2;

planes[3] = r4 - r2;

planes[4] = r4 + r3;

planes[5] = r4 - r3;

}

};



int aabb_vs_frustum(const AABB &aabb, const Frustum &f)

{

int result = 1;



for (int i = 0; i < 6; ++i) {

const vec4f &plane = f.planes[i];



const vec3f pv(

plane.x > 0 ? aabb.max.x : aabb.min.x,

plane.y > 0 ? aabb.max.y : aabb.min.y,

plane.z > 0 ? aabb.max.z : aabb.min.z

);



const vec3f nv(

plane.x < 0 ? aabb.max.x : aabb.min.x,

plane.y < 0 ? aabb.max.y : aabb.min.y,

plane.z < 0 ? aabb.max.z : aabb.min.z

);



const float n = dot(vec4f(pv, 1.0f), plane);

if (n < 0) return -1;



const float m = dot(vec4f(nv, 1.0f), plane);

if (m < 0) result = 0;

}



return result;

}
*/