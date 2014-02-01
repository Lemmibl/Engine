#pragma once
#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include <btBulletDynamicsCommon.h>
#include <memory>
#include "TerrainNoiseSeeder.h"
#include "TerrainManager.h"
#include "d3dmanager.h"
#include "cameraclass.h"
#include "frustumclass.h"
#include "GameRenderer.h"
#include "Lemmi2DAABB.h"
#include "MeshHandler.h"
#include "WeatherSystem.h"
#include "SettingsDependent.h"

//using namespace libconfig;

class GameWorld : SettingsDependent
{
public:
	GameWorld();
	~GameWorld();

	void CleanUp();

	void Initialize(std::shared_ptr<D3DManager> extD3DManager, std::shared_ptr<InputClass> extInput);

	void Update(float deltaTimeSeconds, float deltaTimeMilliseconds);
	void OnSettingsReload(Config* cfg);

	void ResetCamera();
	void InitializeTerrain();


	GameRenderer::RenderableBundle* GetRenderableBundle() {  return &renderableBundle; };
	std::shared_ptr<CameraClass> GetCamera() { return camera; }
	XMFLOAT3* GetWindDirection() { return weatherSystem.GetWindDirection(); }

private:
	void InitializeCollision();
	void InitializeCamera();
	void HandleInput();

	//Do frustum culling and that sort of stuff
	void UpdateVisibility(float deltaTime);

private:
	//Physics related stuff
	XMFLOAT3 gravity;
	float bulletTimestepScale;
	int maxSubSteps;

	//Collision classes
	std::shared_ptr<btBroadphaseInterface> broadphase;
	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;
	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	FrustumClass frustum;
	Lemmi2DAABB frustumAABB;

	//Terrain related objects
	std::shared_ptr<TerrainManager> terrainManager;
	WeatherSystem weatherSystem;

	//Rendering
	MeshHandler meshHandler;
	GameRenderer::RenderableBundle renderableBundle;
	float nearClip, farClip, screenWidth, screenHeight;

	//Misc.
	std::shared_ptr<ControllerClass> cameraController;
	std::shared_ptr<CameraClass> camera;
	std::shared_ptr<InputClass> inputManager;
	std::shared_ptr<D3DManager> d3D;
};


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