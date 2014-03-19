#include "GameplayScreen.h"

GameplayScreen::GameplayScreen(HWND extHwnd, std::shared_ptr<InputClass> extInput, std::shared_ptr<D3DManager> extD3D) 
: GenericScreen(), SettingsDependent(), world(), worldRenderer(), fpsMeter(), cpuMeter(), debugHUD()
{
	input = extInput;
	d3D = extD3D;
	hwnd = extHwnd;

	shadowMapWidth = 1024;
	shadowMapHeight = 1024;
	cpuUsage = 0;
	fps = 0;
	temp = false;
}


GameplayScreen::~GameplayScreen()
{
}

void GameplayScreen::Enter()
{
	SetActive(true);

	if(!HasBeenInitialized())
	{
		Initialize();
		SetInitializedState(true);
	}

	debugHUD.SetHUDVisibility(false);

	//Reset camera
	world.ResetCamera();

	//Reset terrain.
	world.InitializeTerrain();
}

bool GameplayScreen::Initialize()
{
	bool result;

	//Load settings from file
	InitializeSettings(this);

	debugHUD.Initialize(0.0f, 0.0f);

	//Initialize world
	world.Initialize(d3D, input, &worldRenderer, &debugHUD);

	// Initialize the renderer.
	result = worldRenderer.Initialize(hwnd, world.GetCamera(), input, d3D, world.GetMeshHandler(), &debugHUD, (unsigned int)screenWidth, (unsigned int)screenHeight, 
		(unsigned int)shadowMapWidth, (unsigned int)shadowMapHeight, nearClip, farClip);
	if(!result)
	{
		return false;
	}

	fpsMeter.Initialize();
	cpuMeter.Initialize();

	debugHUD.SetHUDVisibility(true);

	debugHUD.AddNewWindow("FPS ", &fps, DataTypeEnumMappings::Int32, fpsDebugHandle);
	debugHUD.AddNewWindow("CPU Usage % ", &cpuUsage, DataTypeEnumMappings::Int32, cpuDebugHandle);
	debugHUD.AddNewWindowWithoutHandle("Wind direction ", world.GetWindDirection(), DataTypeEnumMappings::Float3);

	return true;
}


bool GameplayScreen::Update(float deltaTime)
{
	float deltaTimeMilliseconds = deltaTime*1000.0f;
	
	fpsMeter.Update();
	fps = fpsMeter.GetFps();

	cpuMeter.Update();
	cpuUsage = cpuMeter.GetCpuPercentage();

	if(input->WasKeyPressed(DIK_ESCAPE))
	{
		//If we press escape from gameplay screen, change state back to main menu screen
		stateChangeEvent(GameStates::MainMenuScreen);
	}

	//Here. Do world update stuff here.
	world.Update(deltaTime, deltaTimeMilliseconds);


	// Do update renderer.
	if(!worldRenderer.Update(hwnd, fps, cpuUsage, deltaTimeMilliseconds, deltaTime, world.GetWindDirection()))
	{
		return false;
	}

	return true;
}

bool GameplayScreen::Render(float deltaTime)
{
	// Finally render the graphics to the screen.
	if(!worldRenderer.Render(hwnd, world.GetRenderableBundle()))
	{
		return false;
	}

	return true;
}

void GameplayScreen::Exit()
{
	debugHUD.SetHUDVisibility(false);

	world.CleanUp();
	SetActive(false);
}

void GameplayScreen::OnSettingsReload( Config* cfg )
{
	const Setting& settings = cfg->getRoot()["rendering"];

	//Initialize variables to some default value
	screenWidth = screenHeight = shadowMapWidth = shadowMapHeight = nearClip = farClip = 0.0f;

	settings.lookupValue("windowWidth",		screenWidth);
	settings.lookupValue("windowHeight",	screenHeight);
	settings.lookupValue("shadowmapWidth",	shadowMapWidth);
	settings.lookupValue("shadowmapHeight", shadowMapHeight);
	settings.lookupValue("nearClip",		nearClip);
	settings.lookupValue("farClip",			farClip);
}