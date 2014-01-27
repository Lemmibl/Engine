#pragma once
#include "GameplayScreen.h"
#include "SettingsDependent.h"
#include "inputclass.h"
#include "timerclass.h"
#include <map>
#include "CEGUI/CEGUI.h"
#include "CEGUI/RendererModules/Direct3D11/Renderer.h"


class ScreenManager : public SettingsDependent
{
public:
enum GameStates
{
	MainMenuScreen = 0,
	GameScreen,
	LoadingScreen,
	OptionsScreen
};

public:
	ScreenManager();
	~ScreenManager();

	bool Initialize(HWND extHwnd, HINSTANCE hInst, int screenWidth, int screenHeight, bool vsynEnabled, bool fullScreen);

	GameStates GetCurrentState();
	void ChangeState(GameStates state);
	bool UpdateActiveScreen();

	bool Quit() { return input->WasKeyPressed(DIK_ESCAPE); }

	virtual void OnSettingsReload(Config* cfg);

private:
	HWND hwnd;
	float nearClip, farClip, shadowMapWidth, shadowMapHeight;

	std::shared_ptr<D3DManager> d3D;
	std::shared_ptr<InputClass> input;
	TimerClass timer;

	GameStates previousState, currentState;
	std::shared_ptr<GenericScreen> currentScreen;

	std::map<GameStates, std::shared_ptr<GenericScreen>> stateToScreenDictionary;
};

