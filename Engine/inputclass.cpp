////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"

InputClass::InputClass()
{
	directInput = 0;
	keyboard = 0;
	mouse = 0;
	amountOfActiveKeyboardstates = 0;
	amountOfActiveMousestates = 0;
	lockMouseCursor = false;
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenStartPosX, int screenStartPosY, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	lastChar = NULL;

	screenCenter = std::make_pair<int,int>((screenStartPosX + screenWidth/2), (screenStartPosY + screenHeight/2));

	// Initialize the location of the mouse on the screen.
	mouseX = screenCenter.first;
	mouseY = screenCenter.second;

	prevMouseX = mouseX;
	prevMouseY = mouseY;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = keyboard->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = directInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = mouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.

	//DISCL_FOREGROUND|DISCL_EXCLUSIVE
	result = mouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE|DISCL_BACKGROUND );
	if(FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	//Set mouse cursor position
	SetCursorPos(mouseX, mouseY);

	// Read the mouse device.
	result = mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&currentMouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	//Just to set some default values...
	previousMouseState = currentMouseState;

	currentKeyStates.resize(256);
	previousKeyStates.resize(256);

	//Just initialize to default (empty) values
	for(int i = 0; i < 100; i++)
	{
		activeKeyStates[i] = std::make_pair<KeyState, unsigned int>(Null, 0);
	}

	//Just initialize to default (empty) values
	for(int i = 0; i < 10; i++)
	{
		activeMouseStates[i] = std::make_pair<KeyState, CEGUI::MouseButton>(Null, CEGUI::NoButton);
	}


	return true;
}

void InputClass::Shutdown()
{
	// Release the mouse.
	if(mouse)
	{
		mouse->Unacquire();
		mouse->Release();
		mouse = 0;
	}

	// Release the keyboard.
	if(keyboard)
	{
		keyboard->Unacquire();
		keyboard->Release();
		keyboard = 0;
	}

	// Release the main interface to direct input.
	if(directInput)
	{
		directInput->Release();
		directInput = 0;
	}

	return;
}

bool InputClass::Update(HWND hwnd)
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard(hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not read keyboard. Look in inputclass.", L"Error", MB_OK);
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		MessageBox(hwnd, L"Could not read mouse. Look in inputclass.", L"Error", MB_OK);
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

/*
void Win32AppHelper::doDirectInputEvents(const Win32AppHelper::DirectInputState& dis)
{
// handle direct input based inputs
DIDEVICEOBJECTDATA devDat;
DWORD itemCount = 1;

HRESULT res = dis.keyboardDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &devDat, &itemCount, 0);

if (SUCCEEDED(res))
{
if (itemCount > 0)
{
if (LOBYTE(devDat.dwData) & 0x80)
{
s_samplesFramework->injectKeyDown((CEGUI::Key::Scan)devDat.dwOfs);
}
else
{
s_samplesFramework->injectKeyUp((CEGUI::Key::Scan)devDat.dwOfs);
}

}
}
else
{
// try to re-acquire device if that was the cause of the error.
if ((res == DIERR_NOTACQUIRED) || (res == DIERR_INPUTLOST))
{
dis.keyboardDevice->Acquire();
}

}

}
*/

bool InputClass::ReadKeyboard(HWND hwnd)
{
	if (!keyboard)
	{
		if (FAILED(directInput->CreateDevice(GUID_SysKeyboard, &keyboard, 0))) return false;
		if (FAILED(keyboard->SetDataFormat(&c_dfDIKeyboard))) return false;
		if (FAILED(keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) return false;
		if (FAILED(keyboard->Acquire())) return false;
	}

	//Reset for this frame
	amountOfActiveKeyboardstates = 0;

	//Swap contents
	previousKeyStates.swap(currentKeyStates);

	//Assign new values to currentKeyStates
	if(FAILED(keyboard->GetDeviceState(sizeof(keyStates), (LPVOID)&keyStates)))
	{
		if (FAILED(keyboard->Acquire())) return false;
	}


	//Convert keystates to uint for use with CEGUI
	//Also record any actual events
	for(int i = 0; i < 256; ++i)
	{
		currentKeyStates[i] = keyStates[i];

		if(amountOfActiveKeyboardstates < 100)
		{

			if(currentKeyStates[i] & 0x80)
			{			
				//If key is currently pressed down, add it as an active state
				activeKeyStates[amountOfActiveKeyboardstates].first = KeyDown;

				//Apparently the CEGUI key enums are straight up mapped to the DI8 enums, so I just cast the array index i to a CEGUI key code. Works well.
				activeKeyStates[amountOfActiveKeyboardstates].second = static_cast<unsigned int>(i);

				//Increment in case we get more active key states
				++amountOfActiveKeyboardstates;
			}
			else if(!(currentKeyStates[i] & 0x80) && previousKeyStates[i] & 0x80)
			{
				//OK, so if you've reached this part, it means that the first if failed, meaning that the key isn't currently pressed down. But if previous key state WAS pressed down it means that the key was just released.
				activeKeyStates[amountOfActiveKeyboardstates].first = KeyUp;

				//Apparently the CEGUI key enums are straight up mapped to the DI8 enums, so I just cast the array index i to a CEGUI key code. Works well.
				activeKeyStates[amountOfActiveKeyboardstates].second = static_cast<unsigned int>(i);

				//Increment in case we get more active key states
				++amountOfActiveKeyboardstates;
			}	
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	//Reset counter for this frame
	amountOfActiveMousestates = 0;

	//Save old mouse state
	previousMouseState = currentMouseState;

	// Read the mouse device.
	result = mouse->GetDeviceState(sizeof(DIMOUSESTATE2), (LPVOID)&currentMouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	//Reset mouse positions 
	if(lockMouseCursor)
	{
		SetCursorPos(screenCenter.first, screenCenter.second);
	}

	//Read first three inputs (m1, m2, m3)
	for(unsigned int i = 0; i < 3; i++)
	{
		//Means it's true
		if(currentMouseState.rgbButtons[i] & 0x80)
		{
			activeMouseStates[amountOfActiveMousestates].first = KeyDown;

			//Cast index to mouse button, because they conveniently line up correctly
			activeMouseStates[amountOfActiveMousestates].second = static_cast<CEGUI::MouseButton>(i);

			amountOfActiveMousestates++;
		}
		else if(!(currentMouseState.rgbButtons[i] & 0x80) && previousMouseState.rgbButtons[i] & 0x80)
		{
			activeMouseStates[amountOfActiveMousestates].first = KeyUp;

			//Cast index to mouse button, because they conveniently line up correctly
			activeMouseStates[amountOfActiveMousestates].second = static_cast<CEGUI::MouseButton>(i);

			amountOfActiveMousestates++;
		}

		//Mouse was clicked
		// 
		//if(previousMouseState.rgbButtons[i] & 0x80 && !(currentMouseState.rgbButtons[i] & 0x80))
		//{
		//	//If we've reached this point it means that the current mouse button is up, but the previous mouse button state was down, which means that they mouse button has been released

		//	activeMouseStates[amountOfActiveMousestates].first = MouseClick;

		//	//Cast index to mouse button, because they conveniently line up correctly
		//	activeMouseStates[amountOfActiveMousestates].second = static_cast<CEGUI::MouseButton>(i);

		//	amountOfActiveMousestates++;
		//}

	}

	return true;
}

void InputClass::ProcessInput()
{
	//Save previous mouse positions
	prevMouseX = mouseX;
	prevMouseY = mouseY;

	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	mouseX += currentMouseState.lX;
	mouseY += currentMouseState.lY;

	return;
}

char InputClass::GetLastChar()
{
	return lastChar;
};

bool InputClass::IsEscapePressed()
{
	return (currentKeyStates[DIK_ESCAPE] & 0x80) ? true : false;
}

bool InputClass::IsKeyPressed(unsigned char key)
{
	if(currentKeyStates[key] & 0x80)
	{
		return true;
	}

	return false;
};

bool InputClass::IsKeyUp(unsigned char key)
{
	if(currentKeyStates[key] & 0x80)
	{
		return false;
	}

	return true;
};

bool InputClass::WasKeyPressed(unsigned char key)
{
	if((currentKeyStates[key] & 0x80) && !(previousKeyStates[key] & 0x80))
	{
		return true;
	}

	return false;
};

XMFLOAT2 InputClass::GetMousePos()
{
	return XMFLOAT2((float)mouseX, (float)mouseY);
}

UINT InputClass::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_CHAR) 
		lastChar = static_cast<char>(wParam);

	return uMsg;
}

XMFLOAT2 InputClass::GetMouseDelta()
{
	return XMFLOAT2((float)mouseX - (float)prevMouseX, (float)mouseY - (float)prevMouseY);
}
