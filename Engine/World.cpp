#include "World.h"

static const btScalar bulletTimeStepScale = 1.0f / 60.0f;

World::World()
:	frustum(), renderableBundle(), frustumAABB(XMFLOAT2(-1, -1), XMFLOAT2(1, 1))
{
}

World::~World()
{
}

void World::Initialize( shared_ptr<D3DManager> extD3DManager, shared_ptr<CameraClass> extCamera, shared_ptr<InputClass> extInput, shared_ptr<btDiscreteDynamicsWorld> collisionWorld)
{
	inputManager = extInput;
	camera = extCamera;
	d3D = extD3DManager;
	dynamicsWorld = collisionWorld;

	//1.77f is 16:9 aspect ratio
	frustum.SetInternals((float)camera->GetScreenWidth() / (float)camera->GetScreenHeight(), XM_PIDIV2, camera->GetNearClip(), camera->GetFarClip());

	//Load mesh from hdd
	meshHandler.LoadIndexedMeshFromFile(d3D->GetDevice(), "../Engine/data/sphere.txt", (&renderableBundle.testSphere.mesh));

	InitializeCollisionStuff();
	InitializeTerrain();
}

void World::InitializeCollisionStuff()
{
}

void World::InitializeTerrain()
{
	//Initialize terrain manager
	terrainManager = make_shared<TerrainManager>(d3D->GetDevice(), d3D->GetDeviceContext(), dynamicsWorld, GetDesktopWindow(), camera->GetPosition());

	//Get the meshes from current terrain
	renderableBundle.terrainChunks = terrainManager->GetActiveChunks();
}

void World::Update( float deltaTime)
{
	//Advance bullet world simulation stepping
	dynamicsWorld->stepSimulation(deltaTime, 15);

	HandleInput();
	UpdateVisibility();
}

void World::HandleInput()
{
	if(inputManager->IsKeyPressed(DIK_G))
	{
		SettingsManager::GetInstance().ReloadSettings();
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD1) || inputManager->WasKeyPressed(DIK_F1))
	{
		terrainManager->SetTerrainType(MCTerrainClass::SeaBottom);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD2) || inputManager->WasKeyPressed(DIK_F2))
	{
		terrainManager->SetTerrainType(MCTerrainClass::Plains);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD3) || inputManager->WasKeyPressed(DIK_F3))
	{
		terrainManager->SetTerrainType(MCTerrainClass::Hills);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD4) || inputManager->WasKeyPressed(DIK_F4))
	{
		terrainManager->SetTerrainType(MCTerrainClass::Terraces);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD5) || inputManager->WasKeyPressed(DIK_F5))
	{
		terrainManager->SetTerrainType(MCTerrainClass::DramaticHills);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD6) || inputManager->WasKeyPressed(DIK_F6))
	{
		terrainManager->SetTerrainType(MCTerrainClass::FlyingIslands);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD7) || inputManager->WasKeyPressed(DIK_F7))
	{
		terrainManager->SetTerrainType(MCTerrainClass::Alien);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD8) || inputManager->WasKeyPressed(DIK_F8))
	{
		terrainManager->SetTerrainType(MCTerrainClass::Fancy);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD9) || inputManager->WasKeyPressed(DIK_F9))
	{
		terrainManager->SetTerrainType(MCTerrainClass::Cave);
	}
}

void World::UpdateVisibility()
{
	frustum.ConstructFrustum(camera->GetFarClip(), &camera->GetProj(), &camera->GetView());
	frustum.CalculateFrustumExtents(&frustumAABB, XMLoadFloat3(&camera->GetPosition()), camera->ForwardVector(), camera->UpVector());

	if(terrainManager->UpdateAgainstAABB(d3D->GetDevice(), d3D->GetDeviceContext(), &frustumAABB))
	{
		renderableBundle.terrainChunks.clear();
		renderableBundle.terrainChunks = terrainManager->GetActiveChunks();
	}
}

//TODO: camera body that collides with meshes
//		Generate meshes from marching cubes class and add them to dynamicsWorld in the smoothest way possible.
//		

//TODO: Keep list of renderables here? Then we have a GetRenderableBundle or something along those lines that is sent to renderer
// RenderableBundle should probably just contain a vector of terrainchunks
//
// Further idea: Maybe renderable bundle should keep an array or a big collection of textures... or should everything be fetched from textureAndMaterial manager?
// Maybe all sorts of meshes should keep a textureID handle. TextureAndMaterialManager has a... LoadTexture(string filePath); that returns a handle
// 
//TextureAndMaterial handler has an unsorted map or smth that takes string keys and returns handles. The keys are the strings that were used to load the textures from the HDD.
//
// This all means that whenever we add a texture, it first checks the map to see if that exact texture has already been added, if that's the case, return handle to that texture. 
// If that's NOT the case then we do the whole DOD container thing where we put it in the next free array slot and assign a new index etcetc.
// 
//Could probably apply exactly the same pattern with models loaded in from hdd. Could call it MeshHandler.
// 


//So regarding the whole RenderableBundle thing, maybe just make the whole thing into a full blown class/manager that has different functions for adding/removing meshes, coupled with DOD containers behind.