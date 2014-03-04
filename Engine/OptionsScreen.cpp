#include "OptionsScreen.h"


OptionsScreen::OptionsScreen() : GenericScreen()
{
}


OptionsScreen::~OptionsScreen()
{
}

void OptionsScreen::Enter()
{
	if(!HasBeenInitialized())
	{
		SetInitializedState(true);
	}
}

void OptionsScreen::Exit()
{
}

bool OptionsScreen::Update( float deltaTime )
{

	return true;
}

bool OptionsScreen::Render( float deltaTime )
{

	return true;
}
