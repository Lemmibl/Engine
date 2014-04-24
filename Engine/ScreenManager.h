#pragma once
#include <functional>
#include "MainMenuScreen.h"
#include "GameplayScreen.h"
#include "SettingsDependent.h"
#include "inputclass.h"
#include "timerclass.h"
#include <map>
#include "CEGUI/CEGUI.h"
#include "CEGUI/RendererModules/Direct3D11/Renderer.h"
#include "GameStates.h"
#include "TextOverlayScreen.h"

class ScreenManager : public SettingsDependent
{
public:
	ScreenManager();
	~ScreenManager();

	bool Initialize(HWND extHwnd, HINSTANCE hInst, int screenWidth, int screenHeight, int centerPosX, int centerPosY, bool vsyncEnabled, bool fullScreen);
	void InitializeCEGUI();
	void AddNewScreen(std::shared_ptr<GenericScreen> screen, GameStates::Type gameState, std::wstring name);

	//Provides smooth transition out of current state and into new state
	void ChangeState(GameStates::Type state);

	bool UpdateActiveScreen();
	bool UpdateInputs();

	virtual void OnSettingsReload(Config* cfg);

	CEGUI::utf32 KeycodeToUTF32(unsigned int scanCode);

	//Returns true if escape was pressed this frame
	bool Quitting() { return isQuitting; }
	void Quit() { isQuitting = true; }

	GameStates::Type GetCurrentState() { return currentState; }

private:
	void DrawOverlayText(const std::string& text);

	HWND hwnd;
	float nearClip, farClip, shadowMapWidth, shadowMapHeight;

	std::shared_ptr<D3DManager> d3D;
	std::shared_ptr<InputClass> input;
	TimerClass timer;

	//TODO: Lambda system again... this time in debug mode to see what's happening
	GameStates::Type previousState, currentState;
	std::shared_ptr<GenericScreen> currentScreen;

	std::map<GameStates::Type, std::wstring> stateNames;
	std::map<GameStates::Type, std::shared_ptr<GenericScreen>> stateToScreenMap;
	TextOverlayScreen overlayTextScreen;

	bool isQuitting, showCursor, paused;
};

