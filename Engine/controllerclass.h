////////////////////////////////////////////////////////////////////////////////
// Filename: controllerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CONTROLLERCLASS_H_
#define _CONTROLLERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <xnamath.h>
#include "inputclass.h"
#include <Xnamath.h>
#include "cameraclass.h"



////////////////////////////////////////////////////////////////////////////////
// Class name: ControllerClass
////////////////////////////////////////////////////////////////////////////////
class ControllerClass
{
public:
	ControllerClass();
	ControllerClass(const ControllerClass&);
	~ControllerClass();

	bool Initialize(InputClass* extInput, CameraClass *extCam, float movespeed, float turnspeed);
	void Update(float frameTime);

	const XMFLOAT3& GetPosition() const;
	const XMFLOAT3& GetRotation() const;
	void SetPosition(XMFLOAT3*);
	void SetRotation(XMFLOAT3*);

private:
	float frameTime;
	XMFLOAT2 prevMousePos;
	XMFLOAT3 *rotation, *position;
	XMFLOAT3 movementThisUpdate, rotationThisUpdate;
	float moveSpeed, rotationSpeed;
	bool externalPosPointer;
	bool externalRotPointer;
	
	CameraClass* camera;
	InputClass* inputManager;
};

#endif