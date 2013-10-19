////////////////////////////////////////////////////////////////////////////////
// Filename: ControllerClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "controllerclass.h"

#pragma region Properties
XMFLOAT3 ControllerClass::GetPosition() 
{ 
	return *position;
}
XMFLOAT3 ControllerClass::GetRotation() 
{ 
	return *rotation;
}

void ControllerClass::SetPosition(XMFLOAT3 position)
{
	*this->position = position;
}

void ControllerClass::SetRotation(XMFLOAT3 rotation)
{
	*this->rotation = rotation;
}
#pragma endregion

#pragma region Matrix-To-WorldDirection XMFLOAT4X4
XMVECTOR ControllerClass::MatrixForward(XMFLOAT4X4* matrix)
{
	return XMVectorSet(matrix->_31, matrix->_32, matrix->_33, 1.0f);
}

XMVECTOR ControllerClass::MatrixBackward(XMFLOAT4X4* matrix)
{
	return XMVectorSet(-matrix->_31, -matrix->_32, -matrix->_33, 1.0f);
}

XMVECTOR ControllerClass::MatrixRight(XMFLOAT4X4* matrix)
{
	return XMVectorSet(matrix->_11, matrix->_12, matrix->_13, 1.0f);
}

XMVECTOR ControllerClass::MatrixLeft(XMFLOAT4X4* matrix)
{
	return XMVectorSet(-matrix->_11, -matrix->_12, -matrix->_13, 1.0f);
}

XMVECTOR ControllerClass::MatrixUp(XMFLOAT4X4* matrix)
{
	return XMVectorSet(matrix->_21, matrix->_22, matrix->_23, 1.0f);
}

XMVECTOR ControllerClass::MatrixDown(XMFLOAT4X4* matrix)
{
	return XMVectorSet(-matrix->_21, -matrix->_22, -matrix->_23, 1.0f);
}
#pragma endregion

ControllerClass::ControllerClass()
{
	frameTime = 0.0f;
	moveSpeed  = 0.0f;
	rotationSpeed = 0.0f;

	rotation = new XMFLOAT3(0.0f, 0.0f, 0.0f);
	position = new XMFLOAT3(0.0f, 0.0f, 0.0f);
}

ControllerClass::ControllerClass(const ControllerClass& other)
{
}

ControllerClass::~ControllerClass()
{
	delete rotation;
	delete position;
	rotation = 0;
	position = 0;

	inputManager = 0;
}

bool ControllerClass::Initialize(InputClass* input, float movespeed, float turnspeed)
{
	inputManager = input;
	moveSpeed = movespeed;
	rotationSpeed = turnspeed;

	SetCursorPos(0, 0);
	prevMousePos = inputManager->GetMousePos();

	return true;
}

void ControllerClass::Update(float frameTime, XMFLOAT4X4* cameraMatrix)
{
	float movementValue;

	if(inputManager->IsKeyPressed(DIK_LSHIFT))
	{
		movementValue = (moveSpeed*3.0f) * frameTime;
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
	//SetCursorPos(0, 0);

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
		movementThisUpdate += movementValue*MatrixForward(cameraMatrix);
	if(inputManager->IsKeyPressed(DIK_S))
		movementThisUpdate += movementValue*MatrixBackward(cameraMatrix);

	if(inputManager->IsKeyPressed(DIK_A))
		movementThisUpdate += movementValue*MatrixLeft(cameraMatrix);
	if(inputManager->IsKeyPressed(DIK_D))
		movementThisUpdate += movementValue*MatrixRight(cameraMatrix);

	if(inputManager->IsKeyPressed(DIK_SPACE))
		movementThisUpdate += movementValue*MatrixUp(cameraMatrix);
	if(inputManager->IsKeyPressed(DIK_C))
		movementThisUpdate += movementValue*MatrixDown(cameraMatrix);

	tempPos = XMLoadFloat3(position);
	tempRot = XMLoadFloat3(rotation);

	tempPos += movementThisUpdate;
	tempRot += rotationThisUpdate;

	float X = XMVectorGetX(tempRot);
	float Y = XMVectorGetY(tempRot);

	if(X >= 89.0f) //We make sure the X rotation doesn't stray off the good path, lest it give in to temptation and sin.
	{
		tempRot = XMVectorSetX(tempRot, 89.0f);
	}
	else if(X <= -89.0f)
	{
		tempRot = XMVectorSetX(tempRot, -89.0f);
	}

	if(Y >= 360.0f || Y <= -360.0f) //We clamp Y rotation to a decent, goodhearted value that heeds the word of god.
	{
		tempRot = XMVectorSetY(tempRot, 0.0f);
	}


	XMStoreFloat3(position, tempPos);
	XMStoreFloat3(rotation, tempRot);

	prevMousePos = inputManager->GetMousePos(); //Add this at the end of the update so that it's kept one step behind the fresh update.

	return;
}