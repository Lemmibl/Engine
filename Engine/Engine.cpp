////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

Engine::Engine()
: timer(), screenManager()
{
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
	bool result;

	// Initialize values.
	screenWidth = 0;
	screenHeight = 0;
	float shadowMapWidth = 1024;
	float shadowMapHeight = 1024;

	// load in the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	input = std::make_shared<InputClass>();
	if(!input)
	{
		return false;
	}

	// Initialize the input object.
	result = input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
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
	result = d3D->Initialize(hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR, 
		screenWidth, screenHeight, (UINT)shadowMapWidth, (UINT)shadowMapHeight);
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
	
	result = screenManager.InitializeStates(hwnd, input, d3D);
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

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Otherwise do the frame processing. If frame processing fails then exit.
			result = Update();
			if(!result)
			{
				MessageBox(hwnd, L"Main update loop failed.", L"Error", MB_OK);
				done = true;
			}
		}
		// Check if the user pressed escape and wants to quit.
		if(input->IsEscapePressed() == true)
		{
			done = true;
		}
	}

	return;
}

bool Engine::Update()
{
	bool result;

	timer.Update();

	// Do the input frame processing.
	result = input->Update(hwnd);
	if(!result)
	{
		return false;
	}

	screenManager.UpdateActiveScreen(timer.GetFrameTimeSeconds());

	return true;
}

LRESULT CALLBACK Engine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void Engine::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX windowClass;
	DEVMODE devmodeScreenSettings;
	int posX, posY;

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
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to this resolution.
		screenWidth  = 1024;
		screenHeight = 768;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW, 
		applicationName, 
		applicationName, 
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_POPUP, // to remove window borders, change to this: WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
		posX, 
		posY, 
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
	ShutdownWindows();

	return;
}

void Engine::ShutdownWindows()
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
