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

	movementThisUpdate = rotationThisUpdate = XMFLOAT3(0.0f, 0.0f, 0.0f);

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
	float movementValue = moveSpeed * frameTime;
	float rotationValue = rotationSpeed * frameTime;

	XMFLOAT2 mousePos;
	rotationThisUpdate = movementThisUpdate = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mousePos = inputManager->GetMousePos();
	SetCursorPos(0, 0);

	//Rotate with mouse
	rotationThisUpdate.y -= (prevMousePos.x - mousePos.x)*0.06f;//*rotationValue
	rotationThisUpdate.x -= (prevMousePos.y - mousePos.y)*0.06f;//*rotationValue

	//Rotate the controller with the arrow keys
	if(inputManager->IsKeyPressed(DIK_DOWN))
		rotationThisUpdate.x += rotationValue;
	if(inputManager->IsKeyPressed(DIK_UP))
		rotationThisUpdate.x -= rotationValue;
	if(inputManager->IsKeyPressed(DIK_RIGHT))
		rotationThisUpdate.y += rotationValue;
	if(inputManager->IsKeyPressed(DIK_LEFT))
		rotationThisUpdate.y -= rotationValue;

	//Move the controller with WASD, LCtrl and Space.
	if(inputManager->IsKeyPressed(DIK_W))
		movementThisUpdate += (camera->Forward()*movementValue);
	if(inputManager->IsKeyPressed(DIK_S))
		movementThisUpdate -= (camera->Forward()*movementValue);

	if(inputManager->IsKeyPressed(DIK_A))
		movementThisUpdate += (camera->Right()*movementValue);
	if(inputManager->IsKeyPressed(DIK_D))
		movementThisUpdate -= (camera->Right()*movementValue);

	if(inputManager->IsKeyPressed(DIK_SPACE))
		movementThisUpdate += (camera->Up()*movementValue);
	if(inputManager->IsKeyPressed(DIK_LCONTROL))
		movementThisUpdate -= (camera->Up()*movementValue);

	*position += movementThisUpdate;
	*rotation += rotationThisUpdate;

	prevMousePos = inputManager->GetMousePos(); //Add this at the end of the update so that it's kept one step behind the fresh update.

	return;
}