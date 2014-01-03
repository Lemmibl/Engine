#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include <memory>
#include <btBulletDynamicsCommon.h>
#include "MCTerrainClass.h"
#include "TerrainManager.h"
#include "d3dmanager.h"
#include "cameraclass.h"
#include "frustumclass.h"
#include "Renderer.h"
#include "Lemmi2DAABB.h"
#include "MeshHandler.h"

using namespace std;

class World
{
public:
	World();
	~World();

	void Initialize(shared_ptr<D3DManager> extD3DManager, shared_ptr<CameraClass> extCamera, shared_ptr<InputClass> extInput);

	void Update(float deltaTime);

	Renderer::RenderableBundle* GetRenderableBundle() {  return &renderableBundle; };


private:
	void InitializeCollisionStuff();
	void InitializeTerrain();
	void HandleInput();

	//Do frustum culling and that sort of stuff
	void UpdateVisibility();

//////////////////////////////////////////////////////////////////////////
//
//  World will contain:
//		TerrainManager
//			*	TerrainManager will contain all of the terrain meshes, vegetation, procedural generation and the likes
//			
//		EntityManager
//			*	EntityManager will contain all of the entities, obviously. Handle logic like spawning/death, culling before rendering and that stuff. Or maybe world will do the culling?
//			
//		CollisionManager
//			*	http://bulletphysics.org/mediawiki-1.5.8/index.php/Hello_World
//		
//		PhysicsManager
//			*	Might not even have a physics manager. Could also be responsible for movement and the likes, I guess? Windgeneration maybe?
//		
//		LightManager
//			*	Owns all instances of directional, point and spot lights. When it's time to render, we send a pointer of this manager to the renderer.
//			*	Probably also owns the mesh for point/spot light
//		
//		
//		This class might also contain:
//		Day/Night cycle
//		Skysphere? Maybe it belongs in renderer
//		LOD system/function
//		General culling solutions
//		A queue or some sort of container for all objects that will be rendered this frame
//		
//		
//
//		Maybe this class also tracks events and other sorts of things that might actually appear somewhere in the next years. Gameplay and such.
// 
//		In general, it will contain functions bridging all of the managers together
// 
//////////////////////////////////////////////////////////////////////////

private:
	//Collision classes
	shared_ptr<btBroadphaseInterface> broadphase;
	shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	shared_ptr<btCollisionDispatcher> dispatcher;
	shared_ptr<btSequentialImpulseConstraintSolver> solver;
	shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	//Collision objects
	//shared_ptr<btCollisionShape> groundShape;
	shared_ptr<btCollisionShape> fallShape;

	//shared_ptr<btDefaultMotionState> groundMotionState;
	shared_ptr<btDefaultMotionState> fallMotionState;

	//shared_ptr<btRigidBody> groundRigidBody;
	shared_ptr<btRigidBody> fallRigidBody;

	//Terrain related objects
	shared_ptr<TerrainManager> terrainManager;

	//Rendering
	MeshHandler meshHandler;
	Renderer::RenderableBundle renderableBundle;

	//Misc.
	FrustumClass frustum;
	Lemmi2DAABB frustumAABB;

	shared_ptr<InputClass> inputManager;
	shared_ptr<CameraClass> camera;
	shared_ptr<D3DManager> d3D;
};

