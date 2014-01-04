#include "controllerclass.h"

#pragma region Properties
XMFLOAT3 ControllerClass::GetPosition() 
{ 
	return position;
}

XMFLOAT3 ControllerClass::GetRotation() 
{ 
	return rotation;
}

void ControllerClass::SetPosition(const XMFLOAT3 pos)
{
	return; //position = pos;
}

void ControllerClass::SetRotation(const XMFLOAT3 rot)
{
	rotation = rot;
}
#pragma endregion

#pragma region Matrix-To-Vector conversion functions
XMVECTOR ControllerClass::MatrixForwardVector(const XMFLOAT4X4* matrix)
{
	return XMVectorSet(matrix->_31, matrix->_32, matrix->_33, 1.0f);
}

XMVECTOR ControllerClass::MatrixBackwardVector(const XMFLOAT4X4* matrix)
{
	return XMVectorSet(-matrix->_31, -matrix->_32, -matrix->_33, 1.0f);
}

XMVECTOR ControllerClass::MatrixRightVector(const XMFLOAT4X4* matrix)
{
	return XMVectorSet(matrix->_11, matrix->_12, matrix->_13, 1.0f);
}

XMVECTOR ControllerClass::MatrixLeftVector(const XMFLOAT4X4* matrix)
{
	return XMVectorSet(-matrix->_11, -matrix->_12, -matrix->_13, 1.0f);
}

XMVECTOR ControllerClass::MatrixUpVector(const XMFLOAT4X4* matrix)
{
	return XMVectorSet(matrix->_21, matrix->_22, matrix->_23, 1.0f);
}

XMVECTOR ControllerClass::MatrixDownVector(const XMFLOAT4X4* matrix)
{
	return XMVectorSet(-matrix->_21, -matrix->_22, -matrix->_23, 1.0f);
}
#pragma endregion

//http://gamedev.stackexchange.com/questions/53723/first-person-camera-with-bullet-physics

ControllerClass::ControllerClass(std::shared_ptr<btDynamicsWorld> world, std::shared_ptr<InputClass> input, float movespeed, float turnspeed)
:	inputManager(input),
	moveSpeed(movespeed),
	rotationSpeed(turnspeed),
	prevMousePos(input->GetMousePos()),
	frameTime(0.0f),
	rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	position(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	SetCursorPos(0, 0);

	dynamicsWorld = world;

	//Set up all collision related objects
	collisionShape = std::make_shared<btSphereShape>(2.5f);

	forceScale = 50.0f;
	btScalar mass = 1.0f;
	btVector3 fallInertia(0, 0, 0);
	collisionShape->calculateLocalInertia(mass,fallInertia);

	motionState = std::make_shared<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(0, 50, 0)));

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState.get(), collisionShape.get(), fallInertia);

	rigidBody = std::make_shared<btRigidBody>(rigidBodyCI);

	//Add it to the world
	dynamicsWorld->addRigidBody(rigidBody.get());

	//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=8900&view=next
	rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	rigidBody->setFriction(0.5f);
	rigidBody->setDamping(0.5f, 0.5f);
	rigidBody->setRestitution(0.2f);
}

ControllerClass::~ControllerClass()
{
}

void ControllerClass::Update(float frameTime, XMFLOAT4X4* cameraMatrix)
{
	float movementValue;

	rigidBody->activate(true);

	btTransform trans;
	rigidBody->getMotionState()->getWorldTransform(trans);

	position = btVector3_to_XMFLOAT3(trans.getOrigin());

	movementValue = moveSpeed * frameTime;

	if(inputManager->IsKeyPressed(Keybinds::SPRINT))
	{
		movementValue = (moveSpeed*5.0f) * frameTime;
	}
	else if(inputManager->IsKeyPressed(Keybinds::CROUCH))
	{
		movementValue = (moveSpeed*0.3f) * frameTime;
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
	if(inputManager->IsKeyPressed(Keybinds::DOWNKEY))
		rotationalForce.x += rotationValue;
	if(inputManager->IsKeyPressed(Keybinds::UPKEY))
		rotationalForce.x -= rotationValue;
	if(inputManager->IsKeyPressed(Keybinds::RIGHTKEY))
		rotationalForce.y += rotationValue;
	if(inputManager->IsKeyPressed(Keybinds::LEFTKEY))
		rotationalForce.y -= rotationValue;

	//Rotate with mouse
	rotationThisUpdate = XMLoadFloat4(&rotationalForce);

	//Move the controller with WASD, C and Space.
	if(inputManager->IsKeyPressed(Keybinds::FORWARD))
		movementThisUpdate += movementValue*MatrixForwardVector(cameraMatrix);
	if(inputManager->IsKeyPressed(Keybinds::BACKWARDS))
		movementThisUpdate += movementValue*MatrixBackwardVector(cameraMatrix);

	if(inputManager->IsKeyPressed(Keybinds::LEFT))
		movementThisUpdate += movementValue*MatrixLeftVector(cameraMatrix);
	if(inputManager->IsKeyPressed(Keybinds::RIGHT))
		movementThisUpdate += movementValue*MatrixRightVector(cameraMatrix);

	if(inputManager->IsKeyPressed(Keybinds::UPWARDS))
		movementThisUpdate += movementValue*MatrixUpVector(cameraMatrix);
	if(inputManager->IsKeyPressed(Keybinds::DOWNWARDS))
		movementThisUpdate += movementValue*MatrixDownVector(cameraMatrix);

	tempPos = XMLoadFloat3(&position);
	tempRot = XMLoadFloat3(&rotation);

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

	if(Y >= 360.0f || Y <= -360.0f) //We clamp Y rotation to a decent, goodhearted value that heeds the word of our lord savior.
	{
		tempRot = XMVectorSetY(tempRot, 0.0f);
	}

	//apply force before updating position
	//rigidBody->translate(XMVECTOR_to_btVector3(movementThisUpdate)); //getMotionState()->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), XMVECTOR_to_btVector3(movementThisUpdate))); //

	XMFLOAT3 movement;
	XMStoreFloat3(&movement, movementThisUpdate);
	rigidBody->setLinearVelocity(btVector3(movement.x*forceScale, movement.y*forceScale, movement.z*forceScale)); //, btVector3(position.x, position.y, position.z)

	XMStoreFloat3(&position, tempPos);
	XMStoreFloat3(&rotation, tempRot);

	prevMousePos = inputManager->GetMousePos(); //Add this at the end of the update so that it's kept one step behind the fresh update.

	return;
}