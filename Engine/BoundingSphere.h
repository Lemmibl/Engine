////////////////////////////////////////////////////////////////////////////////
// Filename: boundingsphere.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BOUNDINGSPHERE_H_
#define _BOUNDINGSPHERE_H_

//Includes
#include <d3dx10math.h>

//Custom class includes
#include "frustumclass.h"

//Simple 3D collision bounding sphere with basic collision functionality
class BoundingSphere
{
	public:
		BoundingSphere(float, D3DXVECTOR3);
		BoundingSphere(const BoundingSphere&);
		~BoundingSphere();

		bool Intersects(BoundingSphere&);
		bool Intersects(FrustumClass&);
		//bool Intersects(const BoundingBox&);

		float GetRadius();
		D3DXVECTOR3 GetPosition();

		void SetRadius(const float);
		void SetPosition(const D3DXVECTOR3&);

	private:
		float radius;
		D3DXVECTOR3 position;
};
#endif