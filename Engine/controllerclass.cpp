////////////////////////////////////////////////////////////////////////////////
// Filename: ControllerClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "controllerclass.h"

#pragma region Properties
const XMFLOAT3& ControllerClass::GetPosition() const
{
	return *position;
}

const XMFLOAT3& ControllerClass::GetRotation() const
{
	return *rotation;
}

void ControllerClass::SetPosition(XMFLOAT3* positionptr)
{
	position = positionptr;
	externalPosPointer = true;
	return;
}

void ControllerClass::SetRotation(XMFLOAT3* rotationptr)
{
	rotation = rotationptr;
	externalRotPointer = true;
	return;
}
#pragma endregion

ControllerClass::ControllerClass()
{
	frameTime = 0.0f;
	moveSpeed  = 0.0f;
	rotationSpeed = 0.0f;
	externalRotPointer = externalPosPointer = false;

	//movementThisUpdate = rotationThisUpdate = XMFLOAT3(0.0f, 0.0f, 0.0f);

	rotation = new XMFLOAT3(0.0f, 0.0f, 0.0f);
	position = new XMFLOAT3(0.0f, 0.0f, 0.0f);
}

ControllerClass::ControllerClass(const ControllerClass& other)
{
}

ControllerClass::~ControllerClass()
{
	if(!externalRotPointer)
		delete rotation;

	if(!externalPosPointer)
		delete position;

	rotation = 0;
	position = 0;
	inputManager = 0;
	camera = 0;
}

bool ControllerClass::Initialize(InputClass* input, CameraClass *extCam, float movespeed, float turnspeed)
{
	inputManager = input;
	camera = extCam;
	moveSpeed = movespeed;
	rotationSpeed = turnspeed;

	SetCursorPos(0, 0);
	prevMousePos = inputManager->GetMousePos();

	return true;
}

void ControllerClass::Update(float frameTime)
{
	float movementValue;

	if(inputManager->IsKeyPressed(DIK_LSHIFT))
	{
		movementValue = (moveSpeed*3) * frameTime;
	}
	else
	{
		movementValue = moveSpeed * frameTime;
	}

	float rotationValue = rotationSpeed * frameTime;
	XMVECTOR rotationThisUpdate, movementThisUpdate, tempPos, tempRot;
	XMFLOAT2 mousePos;
	XMFLOAT4 rotationalForce;
	rotationThisUpdate = movementThisUpdate = XMVectorZero();
	rotationalForce = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	mousePos = inputManager->GetMousePos();
	SetCursorPos(0, 0);

	rotationalForce.y -= (prevMousePos.x - mousePos.x)*0.06f;
	rotationalForce.x -= (prevMousePos.y - mousePos.y)*0.06f;

	//Rotate the controller with the arrow keys
	if(inputManager->IsKeyPressed(DIK_DOWN))
		rotationalForce.x += rotationValue;
	if(inputManager->IsKeyPressed(DIK_UP))
		rotationalForce.x -= rotationValue;
	if(inputManager->IsKeyPressed(DIK_RIGHT))
		rotationalForce.y += rotationValue;
	if(inputManager->IsKeyPressed(DIK_LEFT))
		rotationalForce.y -= rotationValue;

	//Rotate with mouse
	rotationThisUpdate = XMLoadFloat4(&rotationalForce);

	//Move the controller with WASD, LCtrl and Space.
	if(inputManager->IsKeyPressed(DIK_W))
		movementThisUpdate += (camera->ForwardVector()*movementValue);
	if(inputManager->IsKeyPressed(DIK_S))
		movementThisUpdate -= (camera->ForwardVector()*movementValue);

	if(inputManager->IsKeyPressed(DIK_A))
		movementThisUpdate -= (camera->RightVector()*movementValue);
	if(inputManager->IsKeyPressed(DIK_D))
		movementThisUpdate += (camera->RightVector()*movementValue);

	if(inputManager->IsKeyPressed(DIK_SPACE))
		movementThisUpdate += (camera->UpVector()*movementValue);
	if(inputManager->IsKeyPressed(DIK_LCONTROL))
		movementThisUpdate -= (camera->UpVector()*movementValue);

	tempPos = XMLoadFloat3(position);
	tempRot = XMLoadFloat3(rotation);

	tempPos += movementThisUpdate;
	tempRot += rotationThisUpdate;

	XMStoreFloat3(position, tempPos);
	XMStoreFloat3(rotation, tempRot);

	prevMousePos = inputManager->GetMousePos(); //Add this at the end of the update so that it's kept one step behind the fresh update.

	return;
}