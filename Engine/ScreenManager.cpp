#include "ScreenManager.h"

ScreenManager::ScreenManager()
{
}


ScreenManager::~ScreenManager()
{
}

ScreenManager::GameStates ScreenManager::GetCurrentState()
{
	return currentState;
}

bool ScreenManager::InitializeStates(HWND extHwnd, std::shared_ptr<InputClass> extInput, std::shared_ptr<D3DManager> extD3D)
{
	bool result;

	std::shared_ptr<GameplayScreen> gameplayScreen = std::make_shared<GameplayScreen>();
	result = gameplayScreen->Initialize(extHwnd, extInput, extD3D);
	if(!result)
	{
		return false;
	}

	/*
	 * TODO: Mainmenu screen, load screen etc.
	 */

	stateToScreenDictionary.insert(std::make_pair(GameScreen, gameplayScreen));

	previousState = currentState = GameScreen;
	currentScreen = gameplayScreen;

	return true;
}

void ScreenManager::ChangeState(GameStates newState)
{
	previousState = currentState;
	currentState = newState;

	currentScreen = stateToScreenDictionary[newState];

	//TODO: Make some sort of loading screen that has an event that you attach the future class to. When future class is fully loaded, 
}

//This is pretty much our core game loop... Heh.
void ScreenManager::UpdateActiveScreen(float deltaTime)
{
	currentScreen->Update(deltaTime);
	currentScreen->Render(deltaTime);
}
