#pragma once
#include "GenericScreen.h"
#include "controllerclass.h"
#include "cameraclass.h"
#include "fpsmeter.h"
#include "cpumeter.h"
#include "World.h"
#include "GameRenderer.h"
#include "SettingsDependent.h"

class GameplayScreen: public GenericScreen, SettingsDependent
{
public:
	GameplayScreen();
	~GameplayScreen();

	virtual void Enter();
	bool Initialize(HWND extHwnd, std::shared_ptr<InputClass> extInput, std::shared_ptr<D3DManager> extD3D);
	virtual bool Update(float deltaTime);
	virtual bool Render(float deltaTime);
	virtual void Exit();

	void OnSettingsReload(Config* cfg);



private:
	std::shared_ptr<ControllerClass> cameraController;
	std::shared_ptr<CameraClass> camera;
	std::shared_ptr<InputClass> input;
	std::shared_ptr<D3DManager> d3D;

	FpsMeter fpsMeter;
	CpuMeter cpuMeter;
	GameWorld world;
	GameRenderer worldRenderer;

	float rotationalValue;
	HWND hwnd;
	float shadowMapWidth, shadowMapHeight, screenWidth, screenHeight, nearClip, farClip;
};

