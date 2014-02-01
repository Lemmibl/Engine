#pragma once
#include "GenericScreen.h"
#include "CEGUI/CEGUI.h"
#include "inputclass.h"

class MainMenuScreen : public GenericScreen
{
public:
	MainMenuScreen();
	~MainMenuScreen();

	virtual void Enter();
	virtual void Exit();

	bool Initialize(std::shared_ptr<InputClass> input);

	bool Update(float deltaTime);
	bool Render(float deltaTime);

private:
	CEGUI::Window* rootWindow;
	std::shared_ptr<InputClass> input;
};

