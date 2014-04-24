////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"

#pragma region Properties
void CameraClass::SetPosition(XMFLOAT3 externalPos)
{
	position = externalPos;
	controller->SetPosition(externalPos);

	return;
}

void CameraClass::SetRotation(XMFLOAT3 externalRot) //lol
{
	rotation = externalRot;
	controller->SetRotation(externalRot);

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
	viewMatrix = XMLoadFloat4x4(&view);
	return;
}

void CameraClass::GetProjectionMatrix(XMMATRIX& projMatrix)
{
	projMatrix =  XMLoadFloat4x4(&projection);
	return;
}


void CameraClass::GetOrthographicProjection( XMMATRIX& orthoMtrx)
{
	orthoMtrx = XMLoadFloat4x4(&orthoProjection);
}

XMFLOAT4X4* CameraClass::GetWorldMatrix()
{
	return &world; 
}

XMMATRIX CameraClass::GetView()
{
	return XMLoadFloat4x4(&view);
}

XMMATRIX CameraClass::GetProj()
{
	return XMLoadFloat4x4(&projection);
}
#pragma endregion

#pragma region XMFloat3 methods
XMFLOAT3 CameraClass::Forward()
{
	return MatrixForward();
}

XMFLOAT3 CameraClass::Back()
{
	return MatrixBackward();
}

XMFLOAT3 CameraClass::Up()
{
	return MatrixUp();
}

XMFLOAT3 CameraClass::Down()
{
	return MatrixDown();
}

XMFLOAT3 CameraClass::Right()
{
	return MatrixRight();
}

XMFLOAT3 CameraClass::Left()
{
	return MatrixLeft();
}
#pragma endregion

#pragma region Vector methods
XMVECTOR CameraClass::ForwardVector()
{
	return XMLoadFloat3(&MatrixForward());
}

XMVECTOR CameraClass::BackVector()
{
	return XMLoadFloat3(&MatrixBackward());
}

XMVECTOR CameraClass::UpVector()
{
	return XMLoadFloat3(&MatrixUp());
}

XMVECTOR CameraClass::DownVector()
{
	return XMLoadFloat3(&MatrixDown());
}

XMVECTOR CameraClass::RightVector()
{
	return XMLoadFloat3(&MatrixRight());
}

XMVECTOR CameraClass::LeftVector()
{
	return XMLoadFloat3(&MatrixLeft());
}
#pragma endregion

#pragma region Matrix methods
XMFLOAT3 CameraClass::MatrixForward()
{
	return XMFLOAT3(world._31, world._32, world._33);
}

XMFLOAT3 CameraClass::MatrixBackward()
{
	return XMFLOAT3(-world._31, -world._32, -world._33);
}

XMFLOAT3 CameraClass::MatrixRight()
{
	return XMFLOAT3(world._11, world._12, world._13);
}

XMFLOAT3 CameraClass::MatrixLeft()
{
	return XMFLOAT3(-world._11, -world._12, -world._13);
}

XMFLOAT3 CameraClass::MatrixUp()
{
	return XMFLOAT3(world._21, world._22, world._23);
}

XMFLOAT3 CameraClass::MatrixDown()
{
	return XMFLOAT3(-world._21, -world._22, -world._23);
}
#pragma endregion

CameraClass::CameraClass(std::shared_ptr<ControllerClass> externalController)
:	controller(externalController),
	PITCHROOF(80.0f*DEG_TO_RAD),
	PITCHFLOOR(-80.0f*DEG_TO_RAD)
{
	position = rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	yaw = pitch = roll = 0.0f;
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPerspectiveProjection(float screenWidth, float screenHeight, float FOVinRadians, float zNear, float zFar)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->nearClip = zNear;
	this->farClip = zFar;

	// Setup the projection matrix.
	float FOV = FOVinRadians;// * (float)DEG_TO_RAD;
	float aspectRatio = screenWidth / screenHeight;

	XMStoreFloat4x4(&orthoProjection, XMMatrixOrthographicLH(screenWidth, screenHeight, zNear, zFar));
	XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(FOV, aspectRatio, zNear, zFar));
}


void CameraClass::Update()
{
	XMVECTOR target, tempRot, tempPos, upVector, forwardVector, up, forward; //right
	XMMATRIX rotationMatrix, tempView;

	upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	position = controller->GetPosition(); //We don't actually move or rotate the camera here, we all that in the controller class.
	rotation = controller->GetRotation();

	tempPos = XMLoadFloat3(&position);
	tempRot = XMLoadFloat3(&rotation);

	pitch = XMVectorGetX(tempRot) * DEG_TO_RAD;
	yaw = XMVectorGetY(tempRot) * DEG_TO_RAD;
	roll = XMVectorGetZ(tempRot) * DEG_TO_RAD;

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll); //Create rotation matrix

	forward = XMVector4Transform(forwardVector, rotationMatrix); //Create forward vector
	up =  XMVector4Transform(upVector, rotationMatrix); //Create up vector
	//right = XMVector3Cross(forward, up); //Create right vector in relation to up and forward vector

	target = tempPos + forward; //Calculate new 'target' for the camera

	tempView = XMMatrixLookAtLH(tempPos, target, up); //Finally, calculate view matrix
	XMStoreFloat4x4(&view, tempView);

	XMVECTOR determinant;
	XMStoreFloat4x4(&world, XMMatrixInverse(&determinant, tempView));

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