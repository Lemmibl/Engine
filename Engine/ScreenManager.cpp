#include "ScreenManager.h"

ScreenManager::ScreenManager() : timer(), SettingsDependent()
{
	InitializeSettings(this);
}


ScreenManager::~ScreenManager()
{
}

ScreenManager::GameStates ScreenManager::GetCurrentState()
{
	return currentState;
}

bool ScreenManager::Initialize( HWND extHwnd, HINSTANCE hInst, int screenWidth, int screenHeight, bool vsynEnabled, bool fullScreen )
{
	bool result;

	hwnd = extHwnd;

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	input = std::make_shared<InputClass>();
	if(!input)
	{
		return false;
	}

	// Initialize the input object.
	result = input->Initialize(hInst, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the inputmanager. Look in engine.", L"Error", MB_OK);
		return false;
	}

	//Create shared pointer containing a d3dmanager
	d3D = std::make_shared<D3DManager>();
	if(!d3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = d3D->Initialize(hwnd, vsynEnabled, fullScreen, nearClip, farClip, screenWidth, screenHeight, 
		static_cast<unsigned int>(shadowMapWidth), static_cast<unsigned int>(shadowMapHeight));
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D. Look in engine.", L"Error", MB_OK);
		return false;
	}

	CEGUI::Direct3D11Renderer& ceguiRenderer = CEGUI::Direct3D11Renderer::create(d3D->GetDevice(), d3D->GetDeviceContext());
	CEGUI::System::create(ceguiRenderer);

	// Initialize the timer object.
	result = timer.Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	std::shared_ptr<GameplayScreen> gameplayScreen = std::make_shared<GameplayScreen>();
	result = gameplayScreen->Initialize(hwnd, input, d3D);
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
bool ScreenManager::UpdateActiveScreen()
{
	bool result;

	timer.Update();

	// Do the input frame processing.
	result = input->Update(hwnd);
	if(!result)
	{
		return false;
	}

	result = currentScreen->Update(timer.GetFrameTimeSeconds());
	if(!result)
	{
		return false;
	}

	result = currentScreen->Render(timer.GetFrameTimeSeconds());
	if(!result)
	{
		return false;
	}

	// draw GUI
	CEGUI::System::getSingleton().renderAllGUIContexts();

	d3D->PresentFrame();

	return true;
}

void ScreenManager::OnSettingsReload( Config* cfg )
{
	const Setting& settings = cfg->getRoot()["rendering"];

	settings.lookupValue("nearClip", nearClip);
	settings.lookupValue("farClip", farClip);

	settings.lookupValue("shadowmapWidth", shadowMapWidth);
	settings.lookupValue("shadowmapHeight", shadowMapHeight);
}
