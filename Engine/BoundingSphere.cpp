#include "BoundingSphere.h"

float BoundingSphere::GetRadius()
{
	return radius;
}

D3DXVECTOR3 BoundingSphere::GetPosition()
{
	return position;
}

void BoundingSphere::SetRadius(const float radius)
{
	this->radius = radius;
}

void BoundingSphere::SetPosition(const D3DXVECTOR3& position)
{
	this->position = position;
}

BoundingSphere::BoundingSphere(float extRadius, D3DXVECTOR3 extPosition)
:	position(extPosition),
	radius(extRadius)
{

	this->position = position;
	this->radius = radius;
}

BoundingSphere::~BoundingSphere()
{
}

bool BoundingSphere::Intersects(BoundingSphere& other)
{
	//If the two radii added together are larger than the distance between the two objects, we have intersection.
	float result, r;
	D3DXVECTOR3 distance;

	//Calculating distance between the two spheres
	distance = position - other.GetPosition();

	//Adding together the radii of the two spheres.
	r = radius+other.radius;

	result = (distance.x*distance.x + distance.y*distance.y + distance.z*distance.z) - (r*r); //Result is squared, I don't want to do sqrt because it's expensive. This works pretty well.

	return (result < 0); //Return the result.
}

bool BoundingSphere::Intersects(FrustumClass& frustum)
{
	bool result = frustum.CheckSphere(position.x, position.y, position.z, radius);
	return result;
}

/*
inline float squared(float v) { return v * v; }
bool doesCubeIntersectSphere(vec3 C1, vec3 C2, vec3 S, float R)
{
float dist_squared = R * R;
//assume C1 and C2 are element-wise sorted, if not, do that now
if (S.X < C1.X) dist_squared -= squared(S.X - C1.X);
else if (S.X > C2.X) dist_squared -= squared(S.X - C2.X);
if (S.Y < C1.Y) dist_squared -= squared(S.Y - C1.Y);
else if (S.Y > C2.Y) dist_squared -= squared(S.Y - C2.Y);
if (S.Z < C1.Z) dist_squared -= squared(S.Z - C1.Z);
else if (S.Z > C2.Z) dist_squared -= squared(S.Z - C2.Z);
return dist_squared > 0;
}


bool BoxIntersectsSphere(Vec3 Bmin, Vec3 Bmax, Vec3 C, float r) {
  float r2 = r * r;
  dmin = 0;
  for( i = 0; i < 3; i++ ) {
	if( C[i] < Bmin[i] ) dmin += SQR( C[i] - Bmin[i] );
	else if( C[i] > Bmax[i] ) dmin += SQR( C[i] - Bmax[i] );     
  }
  return dmin <= r2;
}

*/