#pragma once
#include "GenericScreen.h"
#include "CEGUI/CEGUI.h"
#include "inputclass.h"

class MainMenuScreen : public GenericScreen
{
public:
	MainMenuScreen(std::shared_ptr<InputClass> input);
	~MainMenuScreen();

	virtual void Enter();
	virtual void Exit();

	bool Initialize();

	bool Update(float deltaTime);
	bool Render(float deltaTime);

private:
	CEGUI::Window* rootWindow;
	std::shared_ptr<InputClass> input;
};

