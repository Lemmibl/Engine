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
#include <dinput.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DX10math.h>


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
	D3DXVECTOR2 GetMousePos();

private:
	bool ReadKeyboard(HWND hwnd);
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* directInput;
	IDirectInputDevice8* keyboard;
	IDirectInputDevice8* mouse;

	unsigned char keyStates[2][256];
	unsigned char *currentKeyStates;
	unsigned char *previousKeyStates;
	char lastChar;

	DIMOUSESTATE mouseState;

	int screenWidth, screenHeight;
	int mouseX, mouseY;
};

#endif

