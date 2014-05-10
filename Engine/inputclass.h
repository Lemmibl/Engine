////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////
// INCLUDES //
//////////////
#pragma once
#include <dinput.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <windows.h>
#include <xnamath.h>
#include <vector>
#include "CEGUI/CEGUI.h"

namespace Keybinds 
{
	//TODO: .... a large dictionary that maps string keys to these enums so that I can load keybinds from settings file
	enum
	{
		FORWARD =	(unsigned int)DIK_W,
		BACKWARDS =	(unsigned int)DIK_S,
		LEFT =		(unsigned int)DIK_A,
		RIGHT =		(unsigned int)DIK_D,
		UPWARDS =	(unsigned int)DIK_SPACE,
		DOWNWARDS = (unsigned int)DIK_C,

		CROUCH =	(unsigned int)DIK_LCONTROL,
		SPRINT =	(unsigned int)DIK_LSHIFT,

		LEFTKEY =	(unsigned int)DIK_LEFT,
		RIGHTKEY =	(unsigned int)DIK_RIGHT,
		UPKEY =		(unsigned int)DIK_UP,
		DOWNKEY =	(unsigned int)DIK_DOWN
	};
};

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	enum KeyState
	{
		KeyDown,
		KeyUp,
		MouseClick,
		Null //Not really used except for initialization
	};

	enum MouseState
	{
		DIMOUSE_LEFTBUTTON		= 0,
		DIMOUSE_RIGHTBUTTON		= 1,
		DIMOUSE_MIDDLEBUTTON	= 2,
		DIMOUSE_4BUTTON			= 3,
		DIMOUSE_5BUTTON			= 4,
		DIMOUSE_6BUTTON			= 5,
		DIMOUSE_7BUTTON			= 6,
		DIMOUSE_8BUTTON			= 7,
		MOUSESTATE_LIST_SIZE //Always keep last
	};

	//CEGUI enums
	//enum MouseButton
	//{
	//	LeftButton,
	//	RightButton,
	//	MiddleButton,
	//	X1Button,
	//	X2Button,
	//	MouseButtonCount,
	//	NoButton
	//};

public:
	InputClass();
	~InputClass();

	bool Initialize(HINSTANCE, HWND hwnd, int screenStartPosX, int screenStartPosY, int screenWidth, int screenHeight);
	void Shutdown();
	bool Update(HWND hwnd);

	bool IsEscapePressed();
	bool IsKeyPressed(unsigned char);
	bool IsKeyUp(unsigned char);
	bool WasKeyPressed(unsigned char);

	UINT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	char GetLastChar();

	void GetMouseLocation(int&, int&);
	XMFLOAT2 GetMousePos();
	XMFLOAT2 GetMouseDelta();

	//This is an array. You can find the active count through ActiveKeyboardStateCount()
	std::pair<KeyState, unsigned int>* GetActiveKeyboardStates() { return activeKeyStates; }
	unsigned int ActiveKeyboardStateCount() { return amountOfActiveKeyboardstates; }

	//This is an array. You can find the active count through ActiveMouseStateCount()
	std::pair<KeyState, CEGUI::MouseButton>* GetActiveMouseStates() { return activeMouseStates; }
	unsigned int ActiveMouseStateCount() { return amountOfActiveMousestates; }

private:
	bool ReadKeyboard(HWND hwnd);
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;
	DIMOUSESTATE2 currentMouseState, previousMouseState;

	//TODO: Mouse button... http://www.two-kings.de/tutorials/dinput/dinput03.html

	unsigned char keyStates[256];
	std::pair<KeyState, unsigned int> activeKeyStates[100];
	std::pair<KeyState, CEGUI::MouseButton> activeMouseStates[10];
	std::vector<unsigned char> currentKeyStates;
	std::vector<unsigned char> previousKeyStates;

	unsigned char lastChar;
	int amountOfActiveKeyboardstates, amountOfActiveMousestates;

	int screenWidth, screenHeight;
	int mouseX, mouseY, prevMouseX, prevMouseY;
};

#endif

