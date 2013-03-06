////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


#pragma region Properties
void CameraClass::SetPosition(XMFLOAT3 externalPos)
{
	position = XMLoadFloat3(&externalPos);
	
	return;
}

void CameraClass::SetRotation(XMFLOAT3 externalRot) //lol
{
	rotation = XMLoadFloat3(&externalRot);

	return;
}

XMVECTOR CameraClass::GetPosition()
{
	return position;
}

XMVECTOR CameraClass::GetRotation()
{
	return rotation;
}

XMVECTOR* CameraClass::GetPositionPtr()
{
	return &position;
}

XMVECTOR* CameraClass::GetRotationPtr()
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

XMVECTOR CameraClass::Forward()
{
	return ForwardVec(world);
}

XMVECTOR CameraClass::Back()
{
	return BackVec(world);
}

XMVECTOR CameraClass::Up()
{
	return UpVec(world);
}

XMVECTOR CameraClass::Down()
{
	return DownVec(world);
}

XMVECTOR CameraClass::Right()
{
	return RightVec(world);
}

XMVECTOR CameraClass::Left()
{
	return LeftVec(world);
}

// Returns the forward vector from a transform matrix
XMVECTOR ForwardVec(XMMATRIX& matrix)
{
	return XMLoadFloat3(&XMFLOAT3(matrix._31, matrix._32, matrix._33));
}

// Returns the forward vector from a transform matrix
XMVECTOR BackVec(XMMATRIX& matrix)
{
	return XMLoadFloat3(&XMFLOAT3(-matrix._31, -matrix._32, -matrix._33));
}

// Returns the forward vector from a transform matrix
XMVECTOR RightVec(XMMATRIX& matrix)
{
	return XMLoadFloat3(&XMFLOAT3(matrix._11, matrix._12, matrix._13));
}

// Returns the forward vector from a transform matrix
XMVECTOR LeftVec(XMMATRIX& matrix)
{
	return XMLoadFloat3(&XMFLOAT3(-matrix._11, -matrix._12, -matrix._13));
}

// Returns the forward vector from a transform matrix
XMVECTOR UpVec(XMMATRIX& matrix)
{
	return XMLoadFloat3(&XMFLOAT3(matrix._21, matrix._22, matrix._23));
}

// Returns the forward vector from a transform matrix
XMVECTOR DownVec(XMMATRIX& matrix)
{
	return XMLoadFloat3(&XMFLOAT3(-matrix._21, -matrix._22, -matrix._23));
}
#pragma endregion


CameraClass::CameraClass()
{
	RightVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	UpVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	ForwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	position = rotation = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
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
	XMVECTOR target;
	XMMATRIX rotationMatrix;

	float y = XMVectorGetY(rotation)*DEG_TO_RAD;

	if(y > TWO_PI || y < -TWO_PI) //Just clamping the values to make sure it doesn't go out of control.
	{
		XMVectorSetY(rotation, 0.0f);
	}

	pitch = XMVectorGetX(rotation) * (float)DEG_TO_RAD;
	yaw = XMVectorGetY(rotation) * (float)DEG_TO_RAD;
	roll = XMVectorGetZ(rotation) * (float)DEG_TO_RAD;

	if(pitch > PITCHROOF)
	{
		pitch = (float)PITCHROOF;
		XMVectorSetX(rotation, 80.0f);
	}

	if(pitch < PITCHFLOOR)
	{
		pitch = (float)PITCHFLOOR;
		XMVectorSetX(rotation, -80.0f);
	}

	rotationMatrix = XMMatrixRotationRollPitchYaw(roll, pitch, yaw); //Create rotation matrix

	forward = XMVector4Transform(ForwardVector, rotationMatrix); //Create forward vector
	up =  XMVector4Transform(UpVector, rotationMatrix); //Create up vector
	right = (forward * up); //Create right vector in relation to up and forward vector

	target = position + forward; //Calculate new 'target' for the camera

	view = XMMatrixLookAtLH(position, target, up); //Finally, calculate view matrix

	XMVECTOR determinant;
	world = XMMatrixInverse(&determinant, view);

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