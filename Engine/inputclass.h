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

namespace Keybinds 
{
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
		RIGHTKEY =	(unsigned int) DIK_RIGHT,
		UPKEY =		(unsigned int)DIK_UP,
		DOWNKEY =	(unsigned int) DIK_DOWN
	};
};

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND hwnd, int, int);
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

private:
	bool ReadKeyboard(HWND hwnd);
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;
	DIMOUSESTATE mouseState;

	unsigned char keyStates[2][256];
	std::vector<unsigned char> currentKeyStates;
	std::vector<unsigned char> previousKeyStates;
	unsigned char lastChar;

	int screenWidth, screenHeight;
	int mouseX, mouseY;
};

#endif

