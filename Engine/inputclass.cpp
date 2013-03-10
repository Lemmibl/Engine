////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"

InputClass::InputClass()
{
	directInput = 0;
	keyboard = 0;
	mouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor.
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	mouseX = 0;
	mouseY = 0;

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
	result = mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = mouse->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
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

	currentKeyStates = new unsigned char[256];
	previousKeyStates = new unsigned char[256];

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
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard(HWND hwnd)
{
	if (!keyboard)
	{
		if (FAILED(directInput->CreateDevice(GUID_SysKeyboard, &keyboard, 0))) return false;
		if (FAILED(keyboard->SetDataFormat(&c_dfDIKeyboard))) return false;
		if (FAILED(keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) return false;
		if (FAILED(keyboard->Acquire())) return false;
	}

	unsigned char *tempKeyStates = previousKeyStates;
	previousKeyStates = currentKeyStates;
	currentKeyStates = tempKeyStates;

	if (FAILED(keyboard->GetDeviceState(256, currentKeyStates)))
	{
		if (FAILED(keyboard->Acquire())) return false;
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouseState);
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
	
	return true;
}

void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	mouseX += mouseState.lX;
	mouseY += mouseState.lY;

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
	return (currentKeyStates[key] & 0x80) ? true : false;
};

bool InputClass::IsKeyUp(unsigned char key)
{
	return (currentKeyStates[key] & 0x80) ? false : true;
};

bool InputClass::WasKeyPressed(unsigned char key)
{
	return ((currentKeyStates[key] & 0x80) && !(previousKeyStates[key] & 0x80)) ? true : false;
};

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = this->mouseX;
	mouseY = this->mouseY;
	return;
}

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

/*
	private:
	IDirectInput8* p_Object;
	IDirectInputDevice8* p_KeyboardDevice;
	unsigned char p_KeyStates[2][256];
	unsigned char *p_CurKeyStates;
	unsigned char *p_PrevKeyStates;
	char p_LastChar;

	bool Update(void)
	{
	if (!p_KeyboardDevice)
	{
	if (FAILED(p_Object->CreateDevice(GUID_SysKeyboard, &p_KeyboardDevice, 0))) return false;
	if (FAILED(p_KeyboardDevice->SetDataFormat(&c_dfDIKeyboard))) return false;
	if (FAILED(p_KeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))) return false;
	if (FAILED(p_KeyboardDevice->Acquire())) return false;
	}

	unsigned char *tempKeyStates = p_PrevKeyStates;
	p_PrevKeyStates = p_CurKeyStates;
	p_CurKeyStates = tempKeyStates;

	if (FAILED(p_KeyboardDevice->GetDeviceState(256, p_CurKeyStates)))
	{
	if (FAILED(p_KeyboardDevice->Acquire())) return false;
	}

	return true;
	};
	
	public:
	char GetLastChar(void) const
	{
		return p_LastChar;
	};
	
	bool IsKeyDown(Keys key) const
	{
		return (p_CurKeyStates[key] & 0x80) ? true : false;
	};
	
	bool IsKeyUp(Keys key) const
	{
		return (p_CurKeyStates[key] & 0x80) ? false : true;
	};
	
	bool IsKeyPressed(Keys key) const
	{
		return ((p_CurKeyStates[key] & 0x80) && !(p_PrevKeyStates[key] & 0x80)) ? true : false;
	};

	UINT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CHAR) 
		p_LastChar = static_cast<char>(wParam);

		return uMsg;
	};
*/