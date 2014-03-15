#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include "types.h"
#include <windows.h>
#include <vector>
#include "IndexedMesh.h"
#include "BulletDynamics\Dynamics\btRigidBody.h"
#include "BulletCollision\CollisionShapes\btTriangleMesh.h"
#include <xnamath.h>
#include "customStructs.h"

class MarchingCubeChunk
{
public:
	MarchingCubeChunk(XMFLOAT3 startPosition, XMFLOAT3 endPosition, XMFLOAT3 stepSize, XMFLOAT3 stepCount);
	~MarchingCubeChunk();

	IndexedMesh* GetTerrainMesh() { return &terrainMesh; }
	IndexedMesh* GetWaterMesh() { return &waterMesh; }

	std::vector<unsigned int>* GetIndices() { return &indices; }
	std::vector<MarchingCubeVectors>* GetVertices() { return &vertices; }

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

	std::shared_ptr<btRigidBody> GetRigidBody() const { return rigidBody; }
	btTriangleMesh* GetTriMesh() { return triMesh.get(); }
	std::shared_ptr<btCollisionShape> GetCollisionShape() const { return collisionShape; }

	void SetRigidBody(std::shared_ptr<btRigidBody> body) { rigidBody = body; }
	void SetCollisionShape(std::shared_ptr<btCollisionShape> shape) { collisionShape = shape; }

	void SetWaterLevel(float val) { waterLevel = val; }
	float GetWaterLevel() { return waterLevel; }

	std::pair<int, int> GetKey() { return key; }
	void SetKey(int x, int y) { key = std::make_pair<int, int>(x,y); }

private:
	//voxels is sizeX*sizeY*sizeZ big
	std::vector<unsigned int> indices;
	std::vector<MarchingCubeVectors> vertices;

	//What key is used to fetch this chunk from the big map in terrainManager
	std::pair<int, int> key;

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

	//Decides at what height the water mesh is going to be created.
	float waterLevel;

	//Collision objects
	std::shared_ptr<btRigidBody> rigidBody;
	std::shared_ptr<btTriangleMesh> triMesh;
	std::shared_ptr<btCollisionShape> collisionShape;
};

