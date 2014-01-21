#pragma once
#include "GameplayScreen.h"
#include <map>

class ScreenManager
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

	bool InitializeStates(HWND extHwnd, std::shared_ptr<InputClass> extInput, std::shared_ptr<D3DManager> extD3D);

	GameStates GetCurrentState();
	void ChangeState(GameStates state);
	void UpdateActiveScreen(float deltaTime);

private:
	GameStates previousState, currentState;
	std::shared_ptr<GenericScreen> currentScreen;

	std::map<GameStates, std::shared_ptr<GenericScreen>> stateToScreenDictionary;
};

