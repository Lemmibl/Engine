#pragma once
#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include "types.h"
#include "Utility.h"
#include <windows.h>
#include <vector>
#include "IndexedMesh.h"
#include "Lemmi3DAABB.h"
#include "VegetationManager.h"
#include "BulletDynamics\Dynamics\btRigidBody.h"
#include "BulletCollision\CollisionShapes\btTriangleMesh.h"

class MarchingCubeChunk
{
public:
	MarchingCubeChunk(XMFLOAT3 startPosition, XMFLOAT3 endPosition, XMFLOAT3 stepSize, XMFLOAT3 stepCount);
	~MarchingCubeChunk();

	IndexedMesh* GetTerrainMesh() { return &terrainMesh; }
	IndexedMesh* GetWaterMesh() { return &waterMesh; }

	std::vector<MarchingCubeVoxel>* GetVoxelField() { return &voxels; }
	std::vector<unsigned int>* GetIndices() { return &indices; }

	XMFLOAT3 GetPosition() const { return centerPosition; }

	unsigned int GetStepCountX() const { return stepCountX; }
	unsigned int GetStepCountY() const { return stepCountY; }
	unsigned int GetStepCountZ() const { return stepCountZ; }

	float GetStepSizeX() const { return stepSizeX ; }
	float GetStepSizeY() const { return stepSizeY ; }
	float GetStepSizeZ() const { return stepSizeZ ; }

	float GetStartPosX() const { return startPosition.x; }
	float GetStartPosY() const { return startPosition.y; }
	float GetStartPosZ() const { return startPosition.z; }

	float GetCenterPosX() const { return centerPosition.x; }
	float GetCenterPosY() const { return centerPosition.y; }
	float GetCenterPosZ() const { return centerPosition.z; }

	float GetExtentsX() const { return extents.x; }
	float GetExtentsY() const { return extents.y; }
	float GetExtentsZ() const { return extents.z; }

	btRigidBody* GetRigidBody() const { return rigidBody.get(); }
	btCollisionShape* GetCollisionShape() const { return collisionShape.get(); }
	btTriangleMesh* GetTriMesh() { return triMesh.get(); }

	void SetRigidBody(std::shared_ptr<btRigidBody> body) { rigidBody = body; }
	void SetCollisionShape(std::shared_ptr<btCollisionShape> shape) { collisionShape = shape; }

private:
	//voxels is sizeX*sizeY*sizeZ big
	std::vector<MarchingCubeVoxel> voxels;
	std::vector<unsigned int> indices;

	//Our mesh that we'll be rendering. Contains all vertex/index data.
	IndexedMesh terrainMesh;
	IndexedMesh waterMesh;

	//Should be self explanatory. Defines the bounds of this chunk; where it starts and where it ends.
	XMFLOAT3 startPosition, centerPosition, extents;

	// How long each step will be between startPosition and endPosition.
	float stepSizeX;
	float stepSizeY;
	float stepSizeZ;

	// The amount of steps we take between startPosition and endPosition
	unsigned int stepCountX;
	unsigned int stepCountY;
	unsigned int stepCountZ;

	std::shared_ptr<btTriangleMesh> triMesh;
	std::shared_ptr<btCollisionShape> collisionShape; //triMesh
	std::shared_ptr<btRigidBody> rigidBody;
};

