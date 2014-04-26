////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

Engine::Engine()
	: SettingsDependent(), screenManager()
{
}

Engine::~Engine()
{
	Shutdown();
}

bool Engine::Initialize()
{
	bool result;

	// Initialize values.
	screenWidth = 0;
	screenHeight = 0;

	// load in the windows api.
	InitializeWindows(screenWidth, screenHeight, windowCenterPosX, windowCenterPosY);

	//Load settings from file
	InitializeSettings(this);

	result = screenManager.Initialize(hwnd, hinstance, screenWidth, screenHeight, windowCenterPosX, windowCenterPosY, VSYNC_ENABLED, FULL_SCREEN);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the Screen Manager.", L"Error", MB_OK);
		return false;
	}	

	return true;	
}

void Engine::MainLoop()
{
	MSG msg;
	bool done, result;

	//Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	//Loop until there is a quit message from the window or the user.
	done = false;

	while(!done)
	{
		//Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows or engine itself signals to end the application then exit out.
		if(msg.message == WM_QUIT || screenManager.Quitting() == true)
		{
			done = true;
			break;
		}
		else
		{
			//TODO: http://stackoverflow.com/questions/17785022/game-not-recieving-wm-killfocus-message-while-in-fullscreen

			//Otherwise do the frame processing. If frame processing fails then exit.
			result = screenManager.UpdateActiveScreen();
			if(!result)
			{
				MessageBox(hwnd, L"Active screen update failed. Terminating...", L"Error", MB_OK);
				done = true;
				break;
			}
		}
	}

	return;
}

LRESULT CALLBACK Engine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void Engine::InitializeWindows(int& screenWidth, int& screenHeight, int& centerPosX, int& centerPosY)
{
	WNDCLASSEX windowClass;
	DEVMODE devmodeScreenSettings;

	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	applicationName = L"Lemmi's Engine";

	// Setup the windows class with default settings.
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hinstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	windowClass.hIconSm = windowClass.hIcon;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = applicationName;
	windowClass.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&windowClass);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&devmodeScreenSettings, 0, sizeof(devmodeScreenSettings));
		devmodeScreenSettings.dmSize       = sizeof(devmodeScreenSettings);
		devmodeScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		devmodeScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		devmodeScreenSettings.dmBitsPerPel = 32;			
		devmodeScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&devmodeScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		centerPosX = centerPosY = 0;
	}
	else
	{
		// If windowed then set it to this resolution.
		// TODO: Load these values from settings file
		screenWidth  = 1024;
		screenHeight = 768;

		// Place the window in the middle of the screen.
		centerPosX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		centerPosY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW, 
		applicationName, 
		applicationName, 
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_POPUP, // to remove window borders, change to this: WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
		centerPosX, 
		centerPosY, 
		screenWidth, 
		screenHeight, 
		NULL, 
		NULL, 
		hinstance, 
		NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Hide the mouse cursor.
	ShowCursor(SHOW_CURSOR);

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		// Check if the window is being closed.
	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}
		// All other messages pass to the message handler in the system class.
	default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}

void Engine::Shutdown()
{
	// Shutdown the window.
	ShutdownWindow();

	return;
}

void Engine::ShutdownWindow()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(hwnd);
	hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(applicationName, hinstance);
	hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

void Engine::OnSettingsReload(Config* cfg )
{
	//TODO...
}
