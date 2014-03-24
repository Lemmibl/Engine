////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Engine* engine = new Engine();
	bool result;

	if(!engine)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = engine->Initialize();
	if(result)
	{
		engine->MainLoop();
	}

	engine->Shutdown();
	delete engine;
	engine = 0;

	return 0;
}