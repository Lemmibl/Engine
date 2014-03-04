#pragma once
#include "GenericScreen.h"
#include "CEGUI/CEGUI.h"

class LoadingScreen : GenericScreen
{
public:
	LoadingScreen();
	~LoadingScreen();

	void Initialize();

	virtual void Enter();
	virtual void Exit();

	virtual bool Update( float deltaTime );
	virtual bool Render( float deltaTime );
	void Clear();

private:
	CEGUI::Window* rootWindow;
	CEGUI::Window* textWindow;
};

