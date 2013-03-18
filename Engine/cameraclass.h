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
#include <windows.h>
#include <xnamath.h>

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
	XMFLOAT3* GetPositionPtr();
	XMFLOAT3* GetRotationPtr();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	XMFLOAT3 Forward();
	XMFLOAT3 Back();
	XMFLOAT3 Up();
	XMFLOAT3 Down();
	XMFLOAT3 Right();
	XMFLOAT3 Left();

	XMVECTOR ForwardVector();
	XMVECTOR BackVector();
	XMVECTOR LeftVector();
	XMVECTOR RightVector();
	XMVECTOR UpVector();
	XMVECTOR DownVector();
#pragma endregion


	void Update();
	void SetPerspectiveProjection(int screenWidth, int screenHeight, float FOVinDegrees, float zNear, float zFar);

private:
	XMFLOAT3 MatrixForward(XMMATRIX& matrix);
	XMFLOAT3 MatrixBackward(XMMATRIX& matrix);
	XMFLOAT3 MatrixLeft(XMMATRIX& matrix);
	XMFLOAT3 MatrixRight(XMMATRIX& matrix);
	XMFLOAT3 MatrixUp(XMMATRIX& matrix);
	XMFLOAT3 MatrixDown(XMMATRIX& matrix);

	XMFLOAT3 position, rotation;
	XMFLOAT4X4 world, view, projection;
	float yaw, pitch, roll;

	float PITCHROOF, PITCHFLOOR;
};
#endif