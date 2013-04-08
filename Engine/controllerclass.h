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

////////////////////////////////////////////////////////////////////////////////
// Class name: ControllerClass
////////////////////////////////////////////////////////////////////////////////
class ControllerClass
{
public:
	ControllerClass();
	ControllerClass(const ControllerClass&);
	~ControllerClass();

	bool Initialize(InputClass* extInput, float movespeed, float turnspeed);
	void Update(float frameTime, XMFLOAT4X4* cameraMatrix);

	void SetPosition(XMFLOAT3);
	void SetRotation(XMFLOAT3);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

private:
	XMVECTOR MatrixForward(XMFLOAT4X4* matrix);
	XMVECTOR MatrixBackward(XMFLOAT4X4* matrix);
	XMVECTOR MatrixLeft(XMFLOAT4X4* matrix);
	XMVECTOR MatrixRight(XMFLOAT4X4* matrix);
	XMVECTOR MatrixUp(XMFLOAT4X4* matrix);
	XMVECTOR MatrixDown(XMFLOAT4X4* matrix);

private:
	float frameTime;
	XMFLOAT2 prevMousePos;
	XMFLOAT3 *rotation, *position;
	float moveSpeed, rotationSpeed;
	InputClass* inputManager;
};

#endif