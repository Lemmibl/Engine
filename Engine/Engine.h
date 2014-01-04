////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN
#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

/////////////
// GLOBALS //
/////////////
const bool SHOW_CURSOR = true;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_FAR = 400.0f;
const float SCREEN_NEAR = 2.0f;

//////////////
// INCLUDES //
//////////////
#pragma once
#include <windows.h>
#include <xnamath.h>
#include <memory>

//Enable if you want to check for memory leaks.
//#include <vld.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include <btBulletDynamicsCommon.h>
#include "inputclass.h"
#include "Renderer.h"
#include "fpsmeter.h"
#include "cpumeter.h"
#include "timerclass.h"
#include "controllerclass.h"
#include "World.h"

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

	shared_ptr<D3DManager> d3D;
	shared_ptr<InputClass> input;
	shared_ptr<ControllerClass> cameraController;
	shared_ptr<CameraClass> camera;

	World world;
	Renderer renderer;
	FpsMeter fpsMeter;
	CpuMeter cpuMeter;
	TimerClass timer;

	//Collision classes
	shared_ptr<btBroadphaseInterface> broadphase;
	shared_ptr<btDefaultCollisionConfiguration> collisionConfiguration;
	shared_ptr<btCollisionDispatcher> dispatcher;
	shared_ptr<btSequentialImpulseConstraintSolver> solver;
	shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld;

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