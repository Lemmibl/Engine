////////////////////////////////////////////////////////////////////////////////
// Filename: controllerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CONTROLLERCLASS_H_
#define _CONTROLLERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "inputclass.h"
#include <windows.h>
#include <xnamath.h>
#include <memory>


////////////////////////////////////////////////////////////////////////////////
// Class name: ControllerClass
////////////////////////////////////////////////////////////////////////////////
class ControllerClass
{
public:
	ControllerClass(std::shared_ptr<InputClass> extInput, float movespeed, float turnspeed);
	~ControllerClass();

	void Update(float frameTime, XMFLOAT4X4* cameraMatrix);

	void SetPosition(const XMFLOAT3);
	void SetRotation(const XMFLOAT3);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

private:
	XMVECTOR MatrixForwardVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixBackwardVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixLeftVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixRightVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixUpVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixDownVector(const XMFLOAT4X4* matrix);

private:
	float frameTime;
	XMFLOAT2 prevMousePos;
	XMFLOAT3 rotation, position;
	float moveSpeed, rotationSpeed;
	std::shared_ptr<InputClass> inputManager;
};

#endif