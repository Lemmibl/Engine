//http://stackoverflow.com/questions/514194/using-enum-inside-types-compiler-warning-c4482-c

#pragma once
#include <windows.h>

namespace GameStates
{
	typedef int Type;
	enum
	{
		MainMenuScreen = 0,
		GameScreen,
		LoadingScreen,
		OptionsScreen,
		QuitGame,
		GAMESTATES_LIST_SIZE //Always keep last
	};
};
