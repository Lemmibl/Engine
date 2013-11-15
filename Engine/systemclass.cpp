////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

<<<<<<< master:Engine/systemclass.cpp
SystemClass::SystemClass()
{
	input = 0;
	graphics = 0;
	timer = 0;
	cpuMeter = 0;
	fpsMeter = 0;
=======
Engine::Engine()
:
	renderer(),
	timer(),
	fpsMeter(),
	cpuMeter()
{
>>>>>>> local:Engine/Engine.cpp
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
<<<<<<< master:Engine/systemclass.cpp
	input = new InputClass;
=======
	input = std::make_shared<InputClass>(InputClass());
>>>>>>> local:Engine/Engine.cpp
	if(!input)
	{
		return false;
	}

	// Initialize the input object.
	result = input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
<<<<<<< master:Engine/systemclass.cpp
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
=======
		MessageBox(hwnd, L"Could not initialize the inputmanager. Look in engine.", L"Error", MB_OK);
		return false;
	}

	d3D = std::make_shared<D3DClass>(D3DClass());
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

	cameraController = std::make_shared<ControllerClass>(ControllerClass(input, 0.05f, 0.05f));
	if(!cameraController)
	{
		return false;
	}

	// Create the camera object.
	camera = std::make_shared<CameraClass>(CameraClass(cameraController));
	if(!camera)
	{
		MessageBox(hwnd, L"Could not create the camera object. Look in engine.", L"Error", MB_OK);
		return false;
	}

	// Initialize a base view matrix with the camera for 2D UI rendering.
	camera->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	camera->Update();

	camera->SetPosition(XMFLOAT3(90.0f, 50.0f, 0.0f));
	camera->SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));

	camera->SetPerspectiveProjection(screenWidth, screenHeight, XM_PIDIV4, SCREEN_NEAR, SCREEN_FAR); 


	// Initialize the renderer.
	result = renderer.Initialize(hwnd, camera.get(), input.get(), d3D.get(), screenWidth, screenHeight, 
		shadowMapWidth, shadowMapHeight, SCREEN_FAR, SCREEN_NEAR, toggleDebug);
>>>>>>> local:Engine/Engine.cpp
	if(!result)
	{
		return false;
	}

<<<<<<< master:Engine/systemclass.cpp
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
=======
	fpsMeter.Initialize();
	cpuMeter.Initialize();
>>>>>>> local:Engine/Engine.cpp

	// Initialize the timer object.
	result = timer.Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

<<<<<<< master:Engine/systemclass.cpp
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

=======
	return true;	
}

void Engine::Shutdown()
{
>>>>>>> local:Engine/Engine.cpp
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

<<<<<<< master:Engine/systemclass.cpp
	timer->Frame();
	fpsMeter->Frame();
	cpuMeter->Frame();
=======
	timer.Update();
	fpsMeter.Update();
	cpuMeter.Update();
>>>>>>> local:Engine/Engine.cpp

	// Do the input frame processing.
	result = input->Frame();
	if(!result)
	{
		return false;
	}

<<<<<<< master:Engine/systemclass.cpp
	// Get the location of the mouse from the input object,
	input->GetMouseLocation(mouseX, mouseY);

	cameraController->SetFrameTime(timer->GetTime());
	cameraController->Update(); //Processes all of the movement for this controller.
	
	// Do the frame processing for the graphics object.
	result = graphics->Frame(fpsMeter->GetFps(), cpuMeter->GetCpuPercentage(), timer->GetTime(), input->IsKeyPressed(DIK_Q), input->IsKeyPressed(DIK_R), input->IsKeyPressed(DIK_F));
=======
	cameraController->Update(timer.GetFrameTimeMilliseconds(), camera->GetWorldMatrix()); //Processes all of the movement for this controller.
	camera->Update();
	
	// Do update renderer.
	result = renderer.Update(hwnd, fpsMeter.GetFps(), cpuMeter.GetCpuPercentage(), timer.GetFrameTimeMilliseconds(), timer.GetFrameTimeSeconds());
>>>>>>> local:Engine/Engine.cpp
	if(!result)
	{
		return false;
	}

	// Finally render the graphics to the screen.
<<<<<<< master:Engine/systemclass.cpp
	result = graphics->Render();
=======
	result = renderer.Render(hwnd);
>>>>>>> local:Engine/Engine.cpp
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
	applicationName = L"Lemmi's Engine";

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