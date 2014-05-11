#pragma once
#include "genericscreen.h"
#include "GameConsoleWindow.h"
#include "CEGUI/CEGUI.h"

class GameConsoleWindow;
class InputClass;

class ChatRoomScreen : public GenericScreen
{
public:
	ChatRoomScreen(std::shared_ptr<InputClass> input);
	~ChatRoomScreen();

	virtual bool Enter();
	virtual void Exit();

	bool Initialize();

	virtual bool Update(float deltaTime);
	virtual bool Render(float deltaTime);

private:
	CEGUI::Window* rootWindow;
	std::shared_ptr<InputClass> inputHandler;
	std::shared_ptr<GameConsoleWindow> consoleWindow;
};
