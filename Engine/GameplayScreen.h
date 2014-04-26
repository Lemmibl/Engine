#pragma once
#include "DebugOverlayHUD.h"
#include "GenericScreen.h"
#include "controllerclass.h"
#include "cameraclass.h"
#include "fpsmeter.h"
#include "cpumeter.h"
#include "World.h"
#include "GameRenderer.h"
#include "SettingsDependent.h"
#include "GameStates.h"
#include "CEGUI/CEGUI.h"

class GameplayScreen : public GenericScreen, public SettingsDependent
{
public:
	GameplayScreen(HWND extHwnd, std::shared_ptr<InputClass> extInput, std::shared_ptr<D3DManager> extD3D);
	~GameplayScreen();

	virtual bool Enter();
	bool Initialize();
	virtual bool Update(float deltaTime);
	virtual bool Render(float deltaTime);
	virtual void Exit();

	void OnSettingsReload(Config* cfg);

private:
	std::shared_ptr<InputClass> input;
	std::shared_ptr<D3DManager> d3D;

	FpsMeter fpsMeter;
	CpuMeter cpuMeter;
	std::unique_ptr<GameWorld> world;
	GameRenderer worldRenderer;
	DebugOverlayHUD debugHUD;

	DebugWindowHandle fpsDebugHandle;
	DebugWindowHandle cpuDebugHandle;

	HWND hwnd;
	int cpuUsage, fps;
	float shadowMapWidth, shadowMapHeight, screenWidth, screenHeight, nearClip, farClip;
};

