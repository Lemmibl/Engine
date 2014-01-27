#ifndef _ENGINE_H_
#define _ENGINE_H_

#pragma once

#define WIN32_LEAN_AND_MEAN
#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

const bool SHOW_CURSOR = true;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;

//Enable if you want to check for memory leaks.
//#include <vld.h>

#include <windows.h>
#include <memory>
#include <xnamath.h>

#include "SettingsDependent.h"
#include "ScreenManager.h"

class Engine : SettingsDependent
{
public:
	Engine();
	~Engine();

	bool Initialize();
	void Shutdown();
	void MainLoop();
	void OnSettingsReload(Config* cfg);

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR applicationName;
	HINSTANCE hinstance;
	HWND hwnd;

	ScreenManager screenManager;

	int screenWidth, screenHeight;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;

#endif