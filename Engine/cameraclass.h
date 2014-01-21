#pragma once

#define TWO_PI 6.283185307179586476925286766559f
#define DEG_TO_RAD 0.01745329251994329576923690768489f

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <xnamath.h>
#include "controllerclass.h"
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	CameraClass(std::shared_ptr<ControllerClass> externalController);
	~CameraClass();

	#pragma region get and setters
	void SetPosition(XMFLOAT3);
	void SetRotation(XMFLOAT3);

	void GetViewMatrix(XMMATRIX&);
	void GetProjectionMatrix(XMMATRIX&);
	void GetOrthographicProjection(XMMATRIX&);

	XMMATRIX GetView();
	XMMATRIX GetProj();
	XMFLOAT4X4* GetWorldMatrix();

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

	float GetScreenWidth() { return screenWidth; };
	float GetScreenHeight() { return screenHeight; };

	float GetNearClip() { return nearClip; };
	float GetFarClip() { return farClip; };
	#pragma endregion

	void Update();
	void SetPerspectiveProjection(float screenWidth, float screenHeight, float FOVinDegrees, float zNear, float zFar);

private:
	XMFLOAT3 MatrixForward();
	XMFLOAT3 MatrixBackward();
	XMFLOAT3 MatrixLeft();
	XMFLOAT3 MatrixRight();
	XMFLOAT3 MatrixUp();
	XMFLOAT3 MatrixDown();

	std::shared_ptr<ControllerClass> controller;
	XMFLOAT3 position, rotation;
	XMFLOAT4X4 world, view, projection, orthoProjection;
	float yaw, pitch, roll;
	float screenWidth, screenHeight;
	float nearClip, farClip;

	const float PITCHROOF, PITCHFLOOR;
};