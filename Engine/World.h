#pragma once
#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include <btBulletDynamicsCommon.h>
#include <memory>
#include "SettingsDependent.h"
#include <windows.h>
#include <xnamath.h>

class Lemmi2DAABB;
class TerrainManager;
class D3DManager;
class CameraClass;
class FrustumClass;
class GameRenderer;
class WeatherSystem;
class DebugOverlayHUD;
class ControllerClass;
class InputClass;

struct RenderableBundle;

class GameWorld : public SettingsDependent
{
public:
	GameWorld();
	~GameWorld();

	void CleanUp();

	bool Initialize(std::shared_ptr<D3DManager> extD3DManager, std::shared_ptr<InputClass> extInput, std::shared_ptr<GameRenderer> gameRenderer, std::shared_ptr<DebugOverlayHUD> debugHud);

	void Update(float deltaTimeSeconds, float deltaTimeMilliseconds);
	void OnSettingsReload(Config* cfg);

	void ResetCamera();
	bool InitializeTerrain();

	RenderableBundle* GetRenderableBundle() {  return renderableBundle.get(); };
	std::shared_ptr<CameraClass> GetCamera() { return camera; }
	XMFLOAT3* GetWindDirection();

private:
	bool InitializeRenderables();
	bool InitializeCollision();
	bool InitializeCamera();
	
	void HandleInput();

	//Do frustum culling and that sort of stuff
	void UpdateVisibility(float deltaTime);

private:
	//Physics related stuff
	XMFLOAT3 gravity;
	float bulletTimestepScale;
	int maxSubSteps;

	std::shared_ptr<DebugOverlayHUD> debugHUD;

	//Collision classes
	std::shared_ptr<btBroadphaseInterface> broadphase;
	std::shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	std::shared_ptr<btCollisionDispatcher> dispatcher;
	std::shared_ptr<btSequentialImpulseConstraintSolver> solver;
	std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

	std::shared_ptr<FrustumClass> frustum;
	std::shared_ptr<Lemmi2DAABB> frustumAABB;

	//Terrain related objects

	//TODO: pointer? delete it when shutdown ..
	std::shared_ptr<TerrainManager> terrainManager;
	std::shared_ptr<WeatherSystem> weatherSystem;

	//Rendering
	std::shared_ptr<RenderableBundle> renderableBundle;
	float nearClip, farClip, screenWidth, screenHeight;
	unsigned int currentlyActiveChunks, chunksInProduction; 

	//Misc.
	std::shared_ptr<ControllerClass> cameraController;
	std::shared_ptr<CameraClass> camera;
	std::shared_ptr<InputClass> inputManager;
	std::shared_ptr<D3DManager> d3D;
	std::shared_ptr<GameRenderer> renderer;
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