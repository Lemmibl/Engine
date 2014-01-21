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

ControllerClass::ControllerClass(std::shared_ptr<btDynamicsWorld> world, std::shared_ptr<InputClass> input, float turnspeed)
:	inputManager(input),
	rotationSpeed(turnspeed),
	prevMousePos(input->GetMousePos()),
	frameTime(0.0f),
	rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	noclip(false)
{
	SetCursorPos(0, 0);
	dynamicsWorld = world;


	//Get settings manager instance and add our function to reload event
	SettingsManager& settings = SettingsManager::GetInstance();
	settings.GetEvent()->Add(*this, (&ControllerClass::OnSettingsReload));

	OnSettingsReload(&settings.GetConfig());
}

ControllerClass::~ControllerClass()
{
}

void ControllerClass::Update(float frameTime, XMFLOAT4X4* cameraMatrix)
{
	//Toggle noclip if E was pressed
	if(inputManager->WasKeyPressed(DIK_R))
	{
		noclip = !noclip;

		if(noclip == true)
		{
			//If we have collision flag 2, we should collide with nothing
			rigidBody->setCollisionFlags(1<<2);
		}
		else
		{
			//0 should be default for everything else
			rigidBody->setCollisionFlags(0);
		}
	}

	//scalar for things like sprint/crouch
	float movementValue;

	//Activate rigid body in case it's been deactivated for some reason
	rigidBody->activate(true);

	//Extract rigid body position
	btTransform trans;
	position = btVector3_to_XMFLOAT3(rigidBody->getCenterOfMassPosition());

	//Scale rotational force
	float rotationalForceScale = rotationSpeed * frameTime;

	XMVECTOR rotationThisUpdate, movementThisUpdate, tempRot;
	XMFLOAT2 mousePos;
	XMFLOAT4 rotationalForce;

	//Null all the values
	rotationThisUpdate = movementThisUpdate = XMVectorZero();
	rotationalForce = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	mousePos = inputManager->GetMousePos();

	//Rotate the controller based on mouse cursor movement
	rotationalForce.y -= (prevMousePos.x - mousePos.x)*rotationSpeed;
	rotationalForce.x -= (prevMousePos.y - mousePos.y)*rotationSpeed;

	//Rotate the controller with the arrow keys
	if(inputManager->IsKeyPressed(Keybinds::DOWNKEY))
		rotationalForce.x += rotationalForceScale;
	if(inputManager->IsKeyPressed(Keybinds::UPKEY))
		rotationalForce.x -= rotationalForceScale;
	if(inputManager->IsKeyPressed(Keybinds::RIGHTKEY))
		rotationalForce.y += rotationalForceScale;
	if(inputManager->IsKeyPressed(Keybinds::LEFTKEY))
		rotationalForce.y -= rotationalForceScale;

	//Rotate with mouse
	rotationThisUpdate = XMLoadFloat4(&rotationalForce);

	//Calculate direction of the controller based on keyboard input
	if(inputManager->IsKeyPressed(Keybinds::FORWARD))
		movementThisUpdate += MatrixForwardVector(cameraMatrix);
	if(inputManager->IsKeyPressed(Keybinds::BACKWARDS))
		movementThisUpdate += MatrixBackwardVector(cameraMatrix);

	if(inputManager->IsKeyPressed(Keybinds::LEFT))
		movementThisUpdate += MatrixLeftVector(cameraMatrix);
	if(inputManager->IsKeyPressed(Keybinds::RIGHT))
		movementThisUpdate += MatrixRightVector(cameraMatrix);

	if(inputManager->IsKeyPressed(Keybinds::UPWARDS))
		movementThisUpdate += MatrixUpVector(cameraMatrix);
	if(inputManager->IsKeyPressed(Keybinds::DOWNWARDS))
		movementThisUpdate += MatrixDownVector(cameraMatrix);

	if(inputManager->IsKeyPressed(Keybinds::SPRINT))
	{
		movementValue = (forceScale*sprintModifier);// * frameTime;
	}
	else if(inputManager->IsKeyPressed(Keybinds::CROUCH))
	{
		movementValue = (forceScale*crouchModifier);// * frameTime;
	}
	else
	{
		//Else we move as usual.
		movementValue = forceScale;
	}


	//Normalize direction vector. Apply a little scaling variable to account for sprinting or "sneaking" and other things. Possibly implement into force scale?
	movementThisUpdate = XMVector3Normalize(movementThisUpdate);

	tempRot = XMLoadFloat3(&rotation);
	tempRot += rotationThisUpdate;

	float X = XMVectorGetX(tempRot);
	float Y = XMVectorGetY(tempRot);

	//We make sure the X rotation doesn't stray off the good path, lest it give in to temptation and sin.
	if(X >= 89.0f)
	{
		tempRot = XMVectorSetX(tempRot, 89.0f);
	}
	else if(X <= -89.0f)
	{
		tempRot = XMVectorSetX(tempRot, -89.0f);
	}

	//We also clamp Y rotation to a decent, goodhearted value that heeds the word of our lord savior.
	if(Y >= 360.0f || Y <= -360.0f)
	{
		tempRot = XMVectorSetY(tempRot, 0.0f);
	}

	//Move our rigid body with forces
	XMFLOAT3 movement;
	XMStoreFloat3(&movement, movementThisUpdate);
	rigidBody->applyForce(btVector3(movement.x*movementValue, movement.y*movementValue, movement.z*movementValue), rigidBody->getCenterOfMassPosition()); //, btVector3(position.x, position.y, position.z)

	//Copy our rigid body position to our XMFLOAT3 position that is used in rendering and that stuff
	rigidBody->getMotionState()->getWorldTransform(trans);
	position = btVector3_to_XMFLOAT3(trans.getOrigin());

	//Store new rotation
	XMStoreFloat3(&rotation, tempRot);

	//Save current mouse position for next update
	prevMousePos = inputManager->GetMousePos(); //Add this at the end of the update so that it's kept one step behind the fresh update.

	return;
}

void ControllerClass::OnSettingsReload(Config* cfg)
{
	const Setting& settings = cfg->getRoot()["camera"];

	//Just initialize the variables to some value in case reading from settings file fails.
	float positionX = 0.0f; 
	float positionY = 50.0f;
	float positionZ = 0.0f;

	float collisionRadius = 2.5f;
	float mass = 1.0f;

	float restitution = 0.0f;
	float friction = 10.0f;
	float anisotropicFriction = 2.0f;

	float linearDamping = 0.8f; 
	float angularDamping = 5.0f;

	settings.lookupValue("positionX", positionX);
	settings.lookupValue("positionY", positionY);
	settings.lookupValue("positionZ", positionZ);

	settings.lookupValue("turnSpeed", rotationSpeed);
	settings.lookupValue("sprintModifier", sprintModifier);
	settings.lookupValue("sneakModifier", crouchModifier);

	settings.lookupValue("collisionRadius", collisionRadius);
	settings.lookupValue("mass", mass);

	settings.lookupValue("forceScale", forceScale);
	settings.lookupValue("restitution", restitution);
	settings.lookupValue("friction", friction);
	settings.lookupValue("anisotropicFriction", anisotropicFriction);

	settings.lookupValue("lineardamping", linearDamping);
	settings.lookupValue("angulardamping", angularDamping);


	if(rigidBody)
	{
		dynamicsWorld->removeRigidBody(rigidBody.get());
	}

	//Set up all collision related objects
	collisionShape = std::make_shared<btSphereShape>(collisionRadius);
	 
	btVector3 fallInertia(0, 0, 0);
	collisionShape->calculateLocalInertia(mass, fallInertia);

	//Don't change motionstate
	if(!motionState)
	{
		motionState = std::make_shared<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), btVector3(positionX, positionY, positionZ))); //positionX, positionY, positionZ
	}
	
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState.get(), collisionShape.get(), fallInertia);

	rigidBody = std::make_shared<btRigidBody>(rigidBodyCI);

	//Add it to the world
	dynamicsWorld->addRigidBody(rigidBody.get());

	//http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=8900&view=next
	//This is a flying controller, it shouldn't be affected by gravity
	rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));

	//Self explanatory.
	rigidBody->setFriction(friction);
	rigidBody->setAnisotropicFriction(btVector3(anisotropicFriction, anisotropicFriction, anisotropicFriction));

	//Linear damping is ... air friction, ish. Angular friction is how 
	rigidBody->setDamping(linearDamping, angularDamping);

	//Bounciness.
	rigidBody->setRestitution(restitution);

	//rigidBody->setLinearVelocity(btVector3(1.0f, 1.0f, 1.0f));
	//rigidBody->setCcdSweptSphereRadius(collisionRadius * 0.9f);
}
