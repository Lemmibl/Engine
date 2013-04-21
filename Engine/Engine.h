////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

/////////////
// GLOBALS //
/////////////
const bool SHOW_CURSOR = true;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_FAR = 150.0f;
const float SCREEN_NEAR = 0.5f;

//////////////
// INCLUDES //
//////////////
#pragma once
#include <windows.h>
#include <xnamath.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "Renderer.h"
#include "fpsmeter.h"
#include "cpumeter.h"
#include "timerclass.h"
#include "controllerclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Engine
////////////////////////////////////////////////////////////////////////////////
class Engine
{
public:
	Engine();
	Engine(const Engine&);
	~Engine();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	D3DClass* d3D;
	InputClass* input;
	Renderer* renderer;
	FpsMeter* fpsMeter;
	CpuMeter* cpuMeter;
	TimerClass* timer;
	ControllerClass* cameraController;
	CameraClass* camera;

	float rotationalValue;
	int screenWidth, screenHeight;
	UINT shadowMapWidth, shadowMapHeight;
	bool toggleDebug;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static Engine* ApplicationHandle = 0;

#endif