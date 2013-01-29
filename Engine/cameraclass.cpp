////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


#pragma region Properties
void CameraClass::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	
	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;

	return;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return position;
}

D3DXVECTOR3 CameraClass::GetRotation()
{
	return rotation;
}

void CameraClass::SetPositionPtr( D3DXVECTOR3& extposition)
{
	position = extposition;
	return;
}

void CameraClass::SetRotationPtr( D3DXVECTOR3& extrotation)
{
	rotation = extrotation;
	return;
}

D3DXVECTOR3* CameraClass::GetPositionPtr()
{
	return &position;
}

D3DXVECTOR3* CameraClass::GetRotationPtr()
{
	return &rotation;
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;
	return;
}

void CameraClass::GetProjectionMatrix(D3DXMATRIX& projMatrix)
{
	projMatrix = this->projectionMatrix;
	return;
}

D3DXMATRIX CameraClass::GetView()
{
	return viewMatrix;
}

D3DXMATRIX CameraClass::GetProj()
{
	return projectionMatrix;
}
#pragma endregion


CameraClass::CameraClass()
{
	rightVector =	D3DXVECTOR3(1.0f,0.0f,0.0f);
	upVector =		D3DXVECTOR3(0.0f,1.0f,0.0f);
	forwardVector = D3DXVECTOR3(0.0f,0.0f,1.0f);

	position = rotation = D3DXVECTOR3(0.0f,0.0f,0.0f);
	yaw = pitch = roll = 0.0f;

	right =		D3DXVECTOR3(1.0f,0.0f,0.0f);
	up =		D3DXVECTOR3(0.0f,1.0f,0.0f);
	forward =	D3DXVECTOR3(0.0f,0.0f,1.0f);

	PITCHROOF = 80.0f*0.01745329251994329576923690768489;
	PITCHFLOOR = -80.0f*0.01745329251994329576923690768489;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll); //Create rotation matrix
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}

void CameraClass::SetPerspectiveProjection(int screenWidth, int screenHeight, float FOVinDegrees, float zNear, float zFar)
{

	// Setup the projection matrix.
	float FOV = FOVinDegrees;// * (float)DEG_TO_RAD;
	float aspectRatio = (float)screenWidth / (float)screenHeight;

	D3DXMatrixPerspectiveFovLH( &projectionMatrix, FOV, aspectRatio, zNear, zFar );
}


void CameraClass::Render()
{
	D3DXVECTOR3 target;

	if(rotation.y*DEG_TO_RAD > TWO_PI) //Just clamping the values to make sure it doesn't go out of control.
	{
		rotation.y = 0.0f;
	}

	if(rotation.y*DEG_TO_RAD < -TWO_PI)
	{
		rotation.y = 0.0f;
	}

	pitch = rotation.x * (float)DEG_TO_RAD;
	yaw = rotation.y * (float)DEG_TO_RAD;
	roll = rotation.z * (float)DEG_TO_RAD;

	if(pitch > PITCHROOF)
	{
		pitch = (float)PITCHROOF;
		rotation.x = 80.0f;
	}

	if(pitch < PITCHFLOOR)
	{
		pitch = (float)PITCHFLOOR;
		rotation.x = -80.0f;
	}

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll); //Create rotation matrix

	D3DXVec3TransformCoord(&forward, &forwardVector, &rotationMatrix); //Create forward vector
	D3DXVec3TransformCoord(&up, &upVector, &rotationMatrix); //Create up vector
	D3DXVec3Cross(&right, &forward, &up); //Create right vector in relation to up and forward vector

	target = position + forward; //Calculate new 'target' for the camera

	D3DXMatrixLookAtLH(&viewMatrix, &position, &target, &up); //Finally, calculate view matrix

	return;
}

/*
void camera::updateView()
{
//create rotation matrix
D3DXMatrixRotationYawPitchRoll( &rotationMatrix, heading, pitch, 0 );

//create new view and up vectors
D3DXVec3TransformCoord( &view, &dV, &rotationMatrix );
D3DXVec3TransformCoord( &up, &dU, &rotationMatrix );

//create new forward and strafe vectors
D3DXVec3Normalize( &forward, &view );
D3DXVec3Cross( &strafeRight, &up, &view );
D3DXVec3Normalize( &strafeRight, &strafeRight );

//take into account eye position
view = position + view;

//update view matrix
D3DXMatrixLookAtLH( &viewMatrix, &eye, &view, &up );
}
*/


/*
private void UpdateViewMatrix()
{
Matrix cameraRotation = Matrix.CreateRotationX(updownRot) * Matrix.CreateRotationY(leftrightRot);

Vector3 cameraOriginalTarget = new Vector3(0, 0, -1);
Vector3 cameraOriginalUpVector = new Vector3(0, 1, 0);

Vector3 cameraRotatedTarget = Vector3.Transform(cameraOriginalTarget, cameraRotation);
Vector3 cameraFinalTarget = cameraPosition + cameraRotatedTarget;

Vector3 cameraRotatedUpVector = Vector3.Transform(cameraOriginalUpVector, cameraRotation);

viewMatrix = Matrix.CreateLookAt(cameraPosition, cameraFinalTarget, cameraRotatedUpVector);
}
*/