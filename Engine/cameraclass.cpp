////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


#pragma region Properties
void CameraClass::SetPosition(XMFLOAT3 externalPos)
{
	position = externalPos;
	
	return;
}

void CameraClass::SetRotation(XMFLOAT3 externalRot) //lol
{
	rotation = externalRot;

	return;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return position;
}

XMFLOAT3 CameraClass::GetRotation()
{
	return rotation;
}

XMFLOAT3* CameraClass::GetPositionPtr()
{
	return &position;
}

XMFLOAT3* CameraClass::GetRotationPtr()
{
	return &rotation;
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->view;
	return;
}

void CameraClass::GetProjectionMatrix(XMMATRIX& projMatrix)
{
	projMatrix = this->projection;
	return;
}

XMMATRIX CameraClass::GetView()
{
	return view;
}

XMMATRIX CameraClass::GetProj()
{
	return projection;
}

#pragma region XMFloat3 methods
XMFLOAT3 CameraClass::Forward()
{
	return MatrixForward(world);
}

XMFLOAT3 CameraClass::Back()
{
	return MatrixBackward(world);
}

XMFLOAT3 CameraClass::Up()
{
	return MatrixUp(world);
}

XMFLOAT3 CameraClass::Down()
{
	return MatrixDown(world);
}

XMFLOAT3 CameraClass::Right()
{
	return MatrixRight(world);
}

XMFLOAT3 CameraClass::Left()
{
	return MatrixLeft(world);
}
#pragma endregion XMFloat3 methods

XMVECTOR CameraClass::ForwardVector()
{
	return XMLoadFloat3(&MatrixForward(world));
}

XMVECTOR CameraClass::BackVector()
{
	return XMLoadFloat3(&MatrixBackward(world));
}

XMVECTOR CameraClass::UpVector()
{
	return XMLoadFloat3(&MatrixUp(world));
}

XMVECTOR CameraClass::DownVector()
{
	return XMLoadFloat3(&MatrixDown(world));
}

XMVECTOR CameraClass::RightVector()
{
	return XMLoadFloat3(&MatrixRight(world));
}

XMVECTOR CameraClass::LeftVector()
{
	return XMLoadFloat3(&MatrixLeft(world));
}

#pragma region Matrix methods
// Returns the forward vector from a transform matrix
XMFLOAT3 MatrixForward(XMMATRIX& matrix)
{
	return XMFLOAT3(matrix._31, matrix._32, matrix._33);
}

// Returns the forward vector from a transform matrix
XMFLOAT3 MatrixBackward(XMMATRIX& matrix)
{
	return XMFLOAT3(-matrix._31, -matrix._32, -matrix._33);
}

// Returns the forward vector from a transform matrix
XMFLOAT3 MatrixRight(XMMATRIX& matrix)
{
	return XMFLOAT3(matrix._11, matrix._12, matrix._13);
}

// Returns the forward vector from a transform matrix
XMFLOAT3 MatrixLeft(XMMATRIX& matrix)
{
	return XMFLOAT3(-matrix._11, -matrix._12, -matrix._13);
}

// Returns the forward vector from a transform matrix
XMFLOAT3 MatrixUp(XMMATRIX& matrix)
{
	return XMFLOAT3(matrix._21, matrix._22, matrix._23);
}

// Returns the forward vector from a transform matrix
XMFLOAT3 MatrixDown(XMMATRIX& matrix)
{
	return XMFLOAT3(-matrix._21, -matrix._22, -matrix._23);
}
#pragma endregion

#pragma endregion


CameraClass::CameraClass()
{
	upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	position = rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	yaw = pitch = roll = 0.0f;


	PITCHROOF = 80.0f*0.01745329251994329576923690768489;
	PITCHFLOOR = -80.0f*0.01745329251994329576923690768489;
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

	projection = XMMatrixPerspectiveFovLH(FOV, aspectRatio, zNear, zFar);
}


void CameraClass::Update()
{
	XMVECTOR target, tempRot, tempPos;
	XMMATRIX rotationMatrix;

	tempPos = XMLoadFloat3(&position);
	tempRot = XMLoadFloat3(&rotation);

	float y = XMVectorGetY(tempRot)*DEG_TO_RAD;

	if(y > TWO_PI || y < -TWO_PI) //Just clamping the values to make sure it doesn't go out of control.
	{
		XMVectorSetY(tempRot, 0.0f);
	}

	pitch = XMVectorGetX(tempRot) * (float)DEG_TO_RAD;
	yaw = XMVectorGetY(tempRot) * (float)DEG_TO_RAD;
	roll = XMVectorGetZ(tempRot) * (float)DEG_TO_RAD;

	if(pitch > PITCHROOF)
	{
		pitch = (float)PITCHROOF;
		XMVectorSetX(tempRot, 80.0f);
	}

	if(pitch < PITCHFLOOR)
	{
		pitch = (float)PITCHFLOOR;
		XMVectorSetX(tempRot, -80.0f);
	}

	rotationMatrix = XMMatrixRotationRollPitchYaw(roll, pitch, yaw); //Create rotation matrix

	forward = XMVector4Transform(forwardVector, rotationMatrix); //Create forward vector
	up =  XMVector4Transform(upVector, rotationMatrix); //Create up vector
	right = (forward * up); //Create right vector in relation to up and forward vector

	target = tempPos + forward; //Calculate new 'target' for the camera

	view = XMMatrixLookAtLH(tempPos, target, up); //Finally, calculate view matrix

	XMVECTOR determinant;
	world = XMMatrixInverse(&determinant, view);

	 XMStoreFloat3(&position, tempPos);
	 XMStoreFloat3(&rotation, tempRot);

	return;
}

/*
void camera::updateView()
{
//create rotation matrix
XMMATRIXRotationYawPitchRoll( &rotationMatrix, heading, pitch, 0 );

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
XMMATRIXLookAtLH( &viewMatrix, &eye, &view, &up );
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