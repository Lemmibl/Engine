////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.01745329251994329576923690768489

//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetPositionPtr(D3DXVECTOR3&);

	void SetRotation(float, float, float);
	void SetRotationPtr(D3DXVECTOR3&);

	D3DXVECTOR3* GetPositionPtr();
	D3DXVECTOR3* GetRotationPtr();

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	D3DXVECTOR3 GetUp() const { return up; }
	D3DXVECTOR3 GetForward() const { return forward; }
	D3DXVECTOR3 GetRight() const { return right; }

	void Render();
	void GetViewMatrix(D3DXMATRIX&);
	void GetProjectionMatrix(D3DXMATRIX&);
	D3DXMATRIX GetView();
	D3DXMATRIX GetProj();
	void SetPerspectiveProjection(int screenWidth, int screenHeight, float FOVinDegrees, float zNear, float zFar);

private:
	D3DXVECTOR3 position, rotation;
	D3DXMATRIX viewMatrix, projectionMatrix, rotationMatrix;
	float yaw, pitch, roll;

	D3DXVECTOR3 upVector, forwardVector, rightVector;
	D3DXVECTOR3 up, forward, right; // camera axii.

	double PITCHROOF, PITCHFLOOR;
};
#endif