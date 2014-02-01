#pragma once
#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include "inputclass.h"
#include <windows.h>
#include <xnamath.h>
#include <memory>
#include <btBulletDynamicsCommon.h>
#include "SettingsDependent.h"

class ControllerClass : public SettingsDependent
{
public:
	ControllerClass(std::shared_ptr<btDynamicsWorld> dynamicsWorld, std::shared_ptr<InputClass> extInput, float turnspeed);
	~ControllerClass();

	void Update(float frameTime, XMFLOAT4X4* cameraMatrix);

	void SetPosition(const XMFLOAT3);
	void SetRotation(const XMFLOAT3);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void ResetBody();
	void OnSettingsReload(Config* cfg);

private:
	XMVECTOR MatrixForwardVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixBackwardVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixLeftVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixRightVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixUpVector(const XMFLOAT4X4* matrix);
	XMVECTOR MatrixDownVector(const XMFLOAT4X4* matrix);

	inline XMFLOAT3 btVector3_to_XMFLOAT3(const btVector3& vec) { return XMFLOAT3(vec.getX(), vec.getY(), vec.getZ()); }
	inline XMFLOAT3 btQuaternion_to_XMFLOAT3(const btQuaternion& quat) { return XMFLOAT3(quat.getX(), quat.getY(), quat.getZ()); }

	inline btVector3 XMVECTOR_to_btVector3(const XMVECTOR& vec) { XMFLOAT3 temp; XMStoreFloat3(&temp, vec); return btVector3(temp.x, temp.y, temp.z); }

private:
	bool noclip;
	float frameTime;
	XMFLOAT2 prevMousePos;
	XMFLOAT3 rotation, position;
	float rotationSpeed, forceScale, sprintModifier, crouchModifier;
	std::shared_ptr<InputClass> inputManager;
	std::shared_ptr<btDynamicsWorld> dynamicsWorld;

	std::shared_ptr<btRigidBody> rigidBody;
	std::shared_ptr<btMotionState> motionState;
	std::shared_ptr<btCollisionShape> collisionShape;

	//Some settings and stats
	XMFLOAT3 startPosition; 

	float collisionRadius;
	float mass;

	float restitution;
	float friction;
	float anisotropicFriction;

	float linearDamping; 
	float angularDamping;
};