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
#include <d3dx10math.h>
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

	const D3DXVECTOR3& GetPosition() const;
	const D3DXVECTOR3& GetRotation() const;
	void SetPosition(D3DXVECTOR3*);
	void SetRotation(D3DXVECTOR3*);

private:
	float frameTime;
	D3DXVECTOR2 prevMousePos;
	D3DXVECTOR3 *rotation, *position;
	D3DXVECTOR3 movementThisUpdate, rotationThisUpdate;
	float moveSpeed, rotationSpeed;
	bool externalPosPointer;
	bool externalRotPointer;
	
	CameraClass* camera;
	InputClass* inputManager;
};

#endif