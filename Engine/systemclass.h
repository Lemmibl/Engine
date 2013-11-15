////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

//////////////
// INCLUDES //
//////////////
#include <windows.h>
<<<<<<< master:Engine/systemclass.h
=======
#include <xnamath.h>
#include <memory>
>>>>>>> local:Engine/Engine.h

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "graphicsclass.h"
#include "fpsmeter.h"
#include "cpumeter.h"
#include "timerclass.h"
#include "controllerclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

<<<<<<< master:Engine/systemclass.h
	InputClass* input;
	GraphicsClass* graphics;
	FpsMeter* fpsMeter;
	CpuMeter* cpuMeter;
	TimerClass* timer;
	ControllerClass* cameraController;
=======
	shared_ptr<D3DClass> d3D;
	shared_ptr<InputClass> input;
	shared_ptr<ControllerClass> cameraController;
	shared_ptr<CameraClass> camera;
	Renderer renderer;
	FpsMeter fpsMeter;
	CpuMeter cpuMeter;
	TimerClass timer;

>>>>>>> local:Engine/Engine.h

	float rotationalValue;
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


/////////////
// GLOBALS //
/////////////
static SystemClass* ApplicationHandle = 0;

#endif