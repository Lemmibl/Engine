#include "ScreenManager.h"

ScreenManager::ScreenManager() : SettingsDependent(), timer(), stateToScreenMap(), loadingScreen()
{
	InitializeSettings(this);
	isQuitting = false;
	showCursor = true;
}


ScreenManager::~ScreenManager()
{
	CEGUI::WindowManager::getSingleton().destroyAllWindows();
	CEGUI::System::getSingleton().destroy();
}

bool ScreenManager::Initialize(HWND extHwnd, HINSTANCE hInst,int screenWidth, int screenHeight, int centerPosX, int centerPosY, bool vsyncEnabled, bool fullScreen )
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
	result = input->Initialize(hInst, hwnd, centerPosX, centerPosY, screenWidth, screenHeight);
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
	result = d3D->Initialize(hwnd, vsyncEnabled, fullScreen, nearClip, farClip, screenWidth, screenHeight, 
		static_cast<unsigned int>(shadowMapWidth), static_cast<unsigned int>(shadowMapHeight));
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D. Look in engine.", L"Error", MB_OK);
		return false;
	}

	// Initialize the timer object.
	result = timer.Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	InitializeCEGUI();

	loadingScreen.Initialize();

	std::shared_ptr<GameplayScreen> gameplayScreen = std::make_shared<GameplayScreen>(hwnd, input, d3D);
	//result = gameplayScreen->Initialize();
	//if(!result)
	//{
	//	return false;
	//}

	AddNewScreen(gameplayScreen, GameStates::GameScreen);


	std::shared_ptr<MainMenuScreen> mainMenuScreen = std::make_shared<MainMenuScreen>(input);
	//result = mainMenuScreen->Initialize();
	//if(!result)
	//{
	//	return false;
	//}

	AddNewScreen(mainMenuScreen, GameStates::MainMenuScreen);


	previousState = currentState = GameStates::MainMenuScreen;

	//Decide upon current screen and enter it.
	currentScreen = stateToScreenMap[currentState];
	currentScreen->Enter();

	return true;
}

void ScreenManager::InitializeCEGUI()
{
	// http://static.cegui.org.uk/docs/current/rendering_tutorial.html

	CEGUI::Direct3D11Renderer& ceguiRenderer = CEGUI::Direct3D11Renderer::create(d3D->GetDevice(), d3D->GetDeviceContext());
	CEGUI::System::create(ceguiRenderer);

	// http://static.cegui.org.uk/docs/current/resprov_tutorial.html

	// initialise the required dirs for the DefaultResourceProvider
	CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
	rp->setResourceGroupDirectory("schemes", "../CEGUI/datafiles/schemes/");
	rp->setResourceGroupDirectory("imagesets", "../CEGUI/datafiles/imagesets/");
	rp->setResourceGroupDirectory("fonts", "../CEGUI/datafiles/fonts/");
	rp->setResourceGroupDirectory("layouts", "../CEGUI/datafiles/layouts/");
	rp->setResourceGroupDirectory("looknfeels", "../CEGUI/datafiles/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", "../CEGUI/datafiles/lua_scripts/");

	// set the default resource groups to be used
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	// http://static.cegui.org.uk/docs/current/datafile_tutorial.html

	// create (load) the TaharezLook scheme file
	// (this auto-loads the TaharezLook looknfeel and imageset files)
	CEGUI::SchemeManager::getSingleton().createFromFile("TaharezLook.scheme" );

	// create (load) a font.
	// The first font loaded automatically becomes the default font, but note
	// that the scheme might have already loaded a font, so there may already
	// be a default set - if we want the "DejaVuSans-10" font to definitely
	// be the default, we should set the default explicitly afterwards.
	CEGUI::FontManager::getSingleton().createFromFile("DejaVuSans-10.font");

	//The FontManager automatically sets the first loaded font as the system default. If this is not the default font you require, 
	// or if you can not guarantee the order fonts are loaded, you should set the default explicitly, as in this code:
	CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont("DejaVuSans-10");

	//This uses the TaharezLook imageset which was loaded as part of the TaharezLook scheme we loaded above.
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

	//Finally, if you intend to use tool tips, you should specify the type of the ToolTip based widget that you want used for that purpose. 
	// It is actually possible to set this on a per-window basis, though this is not normally required, and is beyond the scope of this introductory tutorial. 
	// The code to set the default tool tip window type for the initial, default GUIContext looks like this:
	CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultTooltipType("TaharezLook/Tooltip");
}

//Inserts a new screen into the map, together with its' corresponding gameState key.
//Also attaches the screen's game state changing event to our ChangeState function.
void ScreenManager::AddNewScreen(std::shared_ptr<GenericScreen> screen, GameStates::Type gameState )
{
	//Technically speaking not a key, but an iterator into the map, but for all intents and purposes, I use it as a key here.
	auto& key = stateToScreenMap.find(gameState);

	//If this key doesn't already exist in the map
	if(key == stateToScreenMap.end())
	{
		//Append event
		screen->GetStateChangeEvent()->Add(*this, &ScreenManager::ChangeState);

		//Insert into map
		stateToScreenMap.insert(std::make_pair(gameState, screen));
	}
}

void ScreenManager::ChangeState(GameStates::Type newState)
{
	if(newState == GameStates::QuitGame)
	{
		Quit();
	}
	else
	{
		// Else we smoothly handle a transition between two states. 
		if(currentState != newState)
		{
			//Look for the new screen to see if it actually exists in the map
			auto& screen = stateToScreenMap.find(newState);

			//If screen actually exists
			if(screen != stateToScreenMap.end())
			{
				//Exit old screen first
				currentScreen->Exit();

				//Save old state
				previousState = currentState;

				//Set new state
				currentState = newState;

				//Change current screen to the new screen related to new state
				currentScreen = screen->second;

				//Render "Loading..." screen once before entering the current screen, so will be what is displayed during loading/entering
				DrawLoadingScreen();

				//Call enter
				currentScreen->Enter();
			}
		}
	}
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

	result = UpdateInputs();
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

void ScreenManager::DrawLoadingScreen()
{
	//Just do a quick pass to render loading screen text
	d3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);
	loadingScreen.Render(0.0f);
	d3D->PresentFrame();

	//Then clear the text (hide it), so that when we've finished loading, it won't show up
	loadingScreen.Clear();
}

bool ScreenManager::UpdateInputs()
{
	unsigned int keyPressCount, mouseClickCount;

	//Get singleton
	CEGUI::System::getSingleton().injectTimePulse(timer.GetFrameTimeSeconds());
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(input->GetMouseDelta().x, input->GetMouseDelta().y);

	keyPressCount = input->ActiveKeyboardStateCount();
	mouseClickCount = input->ActiveMouseStateCount();

	//if(keyPressCount > 0)
	//{
	//	auto keyArray = input->GetActiveKeyboardStates();

	//	for(int i = 0; i < keyPressCount; i++)
	//	{
	//		result = CEGUI::System::getSingleton().getDefaultGUIContext().injectChar((unsigned int)(keyArray[i].second)); //static_cast<CEGUI::utf32>

	//		switch(keyArray[i].first)
	//		{
	//		case InputClass::KeyDown:
	//			result = CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown((CEGUI::Key::Scan)keyArray[i].second);
	//			break;

	//		case InputClass::KeyUp:
	//			result = CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp((CEGUI::Key::Scan)(keyArray[i].second));
	//			break;

	//		};
	//	}
	//}

	if(mouseClickCount > 0)
	{
		auto mouseArray = input->GetActiveMouseStates();

		for(unsigned int i = 0; i < mouseClickCount; i++)
		{
			switch(mouseArray[i].first)
			{
			case InputClass::KeyDown:
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(mouseArray[i].second);

			case InputClass::KeyUp:
				CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(mouseArray[i].second);

				//case InputClass::MouseClick:
				//	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonClick(mouseArray[i].second);
			}

			if(mouseArray[i].second == CEGUI::RightButton)
			{
				//If right click was pressed then toggle the drawing of the mouse cursor.
				if(mouseArray[i].first == InputClass::KeyUp)
				{
					showCursor = !showCursor;

					//Retarded hack to toggle hiding and showing of mouse cursor.
					//Apparently the showCursor uses some sort of internal reference  counter, where each false decrements it with 1 and every true increments it with one.
					//If the program started with cursor showing, it'll be set to +1. You need to be -1 or lower for it to hide........ So Every toggle needs to change it with 2.
					//http://us.generation-nt.com/answer/showcursor-false-should-hide-cursor-help-10591742.html
					if(showCursor)
					{
						ShowCursor(TRUE);
						ShowCursor(TRUE);
					}
					else
					{
						ShowCursor(FALSE);
						ShowCursor(FALSE);
					}
				}
			}
		}
	}

	return true;
}

//TODO: move this function to some more appropriate place
CEGUI::utf32 ScreenManager::KeycodeToUTF32( unsigned int scanCode )
{
	CEGUI::utf32 utf = 0;

	BYTE keyboardState[256];
	unsigned char ucBuffer[3];
	static WCHAR deadKey = '\0';

	// Retrieve the keyboard layout in order to perform the necessary convertions
	HKL hklKeyboardLayout = GetKeyboardLayout(0); // 0 means current thread 
	// This seemingly cannot fail 
	// If this value is cached then the application must respond to WM_INPUTLANGCHANGE 

	// Retrieve the keyboard state
	// Handles CAPS-lock and SHIFT states
	if (GetKeyboardState(keyboardState) == FALSE)
		return utf;

	/* 0. Convert virtual-key code into a scan code
	1. Convert scan code into a virtual-key code
	Does not distinguish between left- and right-hand keys.
	2. Convert virtual-key code into an unshifted character value
	in the low order word of the return value. Dead keys (diacritics)
	are indicated by setting the top bit of the return value.
	3. Windows NT/2000/XP: Convert scan code into a virtual-key
	Distinguishes between left- and right-hand keys.*/
	UINT virtualKey = MapVirtualKeyEx(scanCode, 3, hklKeyboardLayout);
	if (virtualKey == 0) // No translation possible
		return utf;

	/* Parameter 5:
	0. No menu is active
	1. A menu is active
	Return values:
	Negative. Returned a dead key
	0. No translation available
	1. A translation exists 
	2. Dead-key could not be combined with character 	*/
	int ascii = ToAsciiEx(virtualKey, scanCode, keyboardState, (LPWORD) ucBuffer, 0, hklKeyboardLayout);
	if(deadKey != '\0' && ascii == 1)
	{
		// A dead key is stored and we have just converted a character key
		// Combine the two into a single character
		WCHAR wcBuffer[3];
		WCHAR out[3];
		wcBuffer[0] = ucBuffer[0];
		wcBuffer[1] = deadKey;
		wcBuffer[2] = '\0';
		if( FoldStringW(MAP_PRECOMPOSED, (LPWSTR) wcBuffer, 3, (LPWSTR) out, 3) )
			utf = out[0];
		else
		{
			// FoldStringW failed
			DWORD dw = GetLastError();
			switch(dw)
			{
			case ERROR_INSUFFICIENT_BUFFER:
			case ERROR_INVALID_FLAGS:
			case ERROR_INVALID_PARAMETER:
				break;
			}
		}
		deadKey = '\0';
	}
	else if (ascii == 1)
	{
		// We have a single character
		utf = ucBuffer[0];
		deadKey = '\0';
	}
	else
	{
		// Convert a non-combining diacritical mark into a combining diacritical mark
		switch(ucBuffer[0])
		{
		case 0x5E: // Circumflex accent: â
			deadKey = 0x302;
			break;
		case 0x60: // Grave accent: � 
			deadKey = 0x300;
			break;
		case 0xA8: // Diaeresis: ü
			deadKey = 0x308;
			break;
		case 0xB4: // Acute accent: é
			deadKey = 0x301;
			break;
		case 0xB8: // Cedilla: ç
			deadKey = 0x327;
			break;
		default:
			deadKey = ucBuffer[0];
		}
	}

	return utf;
}

void ScreenManager::OnSettingsReload( Config* cfg )
{
	const Setting& settings = cfg->getRoot()["rendering"];

	settings.lookupValue("nearClip", nearClip);
	settings.lookupValue("farClip", farClip);

	settings.lookupValue("shadowmapWidth", shadowMapWidth);
	settings.lookupValue("shadowmapHeight", shadowMapHeight);
}