////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

SystemClass::SystemClass()
{
	input = 0;
	graphics = 0;
	timer = 0;
	cpuMeter = 0;
	fpsMeter = 0;
	cameraController = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	input = new InputClass;
	if(!input)
	{
		return false;
	}

	// Initialize the input object.
	result = input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	graphics = new GraphicsClass;
	if(!graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = graphics->Initialize(screenWidth, screenHeight, hwnd);
	if(!result)
	{
		return false;
	}

	fpsMeter = new FpsMeter;
	if(!fpsMeter)
	{
		return false;
	}

	fpsMeter->Initialize();

	cpuMeter = new CpuMeter;
	if(!cpuMeter)
	{
		return false;
	}

	cpuMeter->Initialize();

	// Create the timer object.
	timer = new TimerClass;
	if(!timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	cameraController = new ControllerClass();
	if(!cameraController)
	{
		return false;
	}

	result = cameraController->Initialize(input, graphics->GetCamera(), 0.01f, 0.05f);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the camera controller object.", L"Error", MB_OK);
		return false;
	}

	cameraController->SetPosition(graphics->GetCamera()->GetPositionPtr());
	cameraController->SetRotation(graphics->GetCamera()->GetRotationPtr());

	return true;	
}


void SystemClass::Shutdown()
{
	if(cameraController)
	{
		delete cameraController;
		cameraController = 0;
	}
	
	if(fpsMeter)
	{
		delete fpsMeter;
		fpsMeter = 0;
	}

	if(timer)
	{
		delete timer;
		timer = 0;
	}

	if(cpuMeter)
	{
		cpuMeter->Shutdown();
		delete cpuMeter;
		cpuMeter = 0;
	}

	// Release the graphics object.
	if(graphics)
	{
		graphics->Shutdown();
		delete graphics;
		graphics = 0;
	}

	// Release the input object.
	if(input)
	{
		input->Shutdown();
		delete input;
		input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
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
			// Otherwise do the frame processing.  If frame processing fails then exit.
			result = Frame();
			if(!result)
			{
				MessageBox(hwnd, L"Frame Processing Failed", L"Error", MB_OK);
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

bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;

	timer->Frame();
	fpsMeter->Frame();
	cpuMeter->Frame();

	// Do the input frame processing.
	result = input->Frame();
	if(!result)
	{
		return false;
	}

	// Get the location of the mouse from the input object,
	input->GetMouseLocation(mouseX, mouseY);

	cameraController->SetFrameTime(timer->GetTime());
	cameraController->Update(); //Processes all of the movement for this controller.
	
	// Do the frame processing for the graphics object.
	result = graphics->Frame(fpsMeter->GetFps(), cpuMeter->GetCpuPercentage(), timer->GetTime(), input->IsKeyPressed(DIK_Q), input->IsKeyPressed(DIK_R), input->IsKeyPressed(DIK_F));
	if(!result)
	{
		return false;
	}

	// Finally render the graphics to the screen.
	result = graphics->Render();
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
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

void SystemClass::ShutdownWindows()
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