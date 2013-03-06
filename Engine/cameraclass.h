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
#include <Xnamath.h>
#include <Xnamath.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

#pragma region get and setters
	void SetPosition(XMFLOAT3);
	void SetRotation(XMFLOAT3);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);
	XMMATRIX GetView();
	XMMATRIX GetProj();
	XMVECTOR* GetPositionPtr();
	XMVECTOR* GetRotationPtr();
	XMVECTOR GetPosition();
	XMVECTOR GetRotation();

	XMVECTOR Forward();
	XMVECTOR Back();
	XMVECTOR Up();
	XMVECTOR Down();
	XMVECTOR Right();
	XMVECTOR Left();

	XMVECTOR ForwardVec(XMMATRIX& matrix);
	XMVECTOR BackVec(XMMATRIX& matrix);
	XMVECTOR LeftVec(XMMATRIX& matrix);
	XMVECTOR RightVec(XMMATRIX& matrix);
	XMVECTOR UpVec(XMMATRIX& matrix);
	XMVECTOR DownVec(XMMATRIX& matrix);
#pragma endregion


	void Update();
	void SetPerspectiveProjection(int screenWidth, int screenHeight, float FOVinDegrees, float zNear, float zFar);

private:
	XMVECTOR position, rotation;
	XMMATRIX world, view, projection;
	float yaw, pitch, roll;

	XMVECTOR up, forward, right; // camera axii.

	double PITCHROOF, PITCHFLOOR;

	XMVECTOR RightVector;
	XMVECTOR UpVector;
	XMVECTOR ForwardVector;
};
#endif