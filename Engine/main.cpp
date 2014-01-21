////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Engine* system = new Engine();
	bool result;

	if(!system)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = system->Initialize();
	if(result)
	{
		system->MainLoop();
	}

	system->Shutdown();
	delete system;
	system = 0;

	return 0;
}