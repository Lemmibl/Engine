////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

Engine::Engine()
{
	input = 0;
	renderer = 0;
	timer = 0;
	cpuMeter = 0;
	fpsMeter = 0;
	cameraController = 0;
	marchingCubes = 0;
	metaBalls = 0;
	camera = 0;
	d3D = 0;
}

Engine::Engine(const Engine& other)
{
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
	bool result;

	// Initialize values.
	toggleDebug = true;
	screenWidth = 0;
	screenHeight = 0;
	shadowMapWidth = 2048;
	shadowMapHeight = 2048;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	input = new InputClass();
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

	d3D = new D3DClass();
	if(!d3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = d3D->Initialize(hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_NEAR, SCREEN_FAR, 
		screenWidth, screenHeight, shadowMapWidth, shadowMapHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D. Look in engine.", L"Error", MB_OK);
		return false;
	}

	cameraController = new ControllerClass();
	if(!cameraController)
	{
		return false;
	}

	result = cameraController->Initialize(input, 0.01f, 0.05f);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the camera controller object.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	camera = new CameraClass();
	if(!camera)
	{
		MessageBox(hwnd, L"Could not create the camera object. Look in engine.", L"Error", MB_OK);
		return false;
	}

	camera->Initialize(cameraController);

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	camera->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	camera->Update();

	camera->SetPosition(XMFLOAT3(-15.0f, 90.0f, -15.0f));
	camera->SetRotation(XMFLOAT3(40.0f, 40.0f, 0.0f));

	camera->SetPerspectiveProjection(screenWidth, screenHeight, XM_PIDIV4, SCREEN_NEAR, SCREEN_FAR); 


	// Create the renderer object. This object will handle rendering all the graphics for this application. Durp.
	renderer = new Renderer();
	if(!renderer)
	{
		return false;
	}

	// Initialize the renderer.
	result = renderer->Initialize(hwnd, camera, input, d3D, screenWidth, screenHeight, shadowMapWidth, shadowMapHeight, SCREEN_FAR, SCREEN_NEAR);
	if(!result)
	{
		return false;
	}

	fpsMeter = new FpsMeter();
	if(!fpsMeter)
	{
		return false;
	}

	fpsMeter->Initialize();

	cpuMeter = new CpuMeter();
	if(!cpuMeter)
	{
		return false;
	}

	cpuMeter->Initialize();

	// Create the timer object.
	timer = new TimerClass();
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

	return true;	
}

void Engine::Shutdown()
{
	if(d3D)
	{
		d3D->Shutdown();
		delete d3D;
		d3D = 0;
	}

	if(camera)
	{
		delete camera;
		camera = 0;
	}

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
	if(renderer)
	{
		renderer->Shutdown();
		delete renderer;
		renderer = 0;
	}

	if(marchingCubes)
	{
		delete marchingCubes;
		marchingCubes = 0;
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

void Engine::Run()
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
			result = Update();
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

bool Engine::Update()
{
	bool result;

	timer->Frame();
	fpsMeter->Update();
	cpuMeter->Update();

	// Do the input frame processing.
	result = input->Update(hwnd);
	if(!result)
	{
		return false;
	}

	cameraController->Update(timer->GetFrameTime(), camera->GetWorldMatrix()); //Processes all of the movement for this controller.
	camera->Update();
	
	// Do the frame processing for the graphics object.
	result = renderer->Update(fpsMeter->GetFps(), cpuMeter->GetCpuPercentage(), timer->GetFrameTime(), timer->GetFrameTimeSeconds());
	if(!result)
	{
		return false;
	}

	// Finally render the graphics to the screen.
	result = renderer->Render();
	if(!result)
	{
		return false;
	}

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
	applicationName = L"Engine";

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