#define WIN32_LEAN_AND_MEAN
#pragma once
#include "CEGUI/CEGUI.h"
#include "genericscreen.h"

class NetworkClient;
class NetworkServer;
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

	bool Handle_TextSent(const CEGUI::EventArgs &e);

	//Event handler for when the GameConsoleWindow receives an update to target IP address.
	bool Handle_ConnectButtonPressed(const CEGUI::EventArgs &e);

private:
	CEGUI::Window* rootWindow;
	std::shared_ptr<InputClass> inputHandler;
	std::shared_ptr<GameConsoleWindow> consoleWindow;
	std::shared_ptr<NetworkClient> netClient;
	std::shared_ptr<NetworkServer> netServer;
	bool connectionActive;
};

