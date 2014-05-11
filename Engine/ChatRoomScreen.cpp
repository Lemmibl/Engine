#include "ChatRoomScreen.h"

#include "GameConsoleWindow.h"
#include "inputclass.h"

ChatRoomScreen::ChatRoomScreen(std::shared_ptr<InputClass> input) : GenericScreen()
{
	inputHandler = input;
}

ChatRoomScreen::~ChatRoomScreen()
{
	rootWindow = nullptr;
}

bool ChatRoomScreen::Enter()
{
	SetActive(true);
	bool result;

	if(!HasBeenInitialized())
	{
		result = Initialize();
		if(!result)
		{
			return false;
		}

		SetInitializedState(true);
	}

	//Show mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	//Set this base window as root.
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	rootWindow->activate();
	rootWindow->show();

	return true;
}

bool ChatRoomScreen::Initialize()
{
	rootWindow = nullptr;

	consoleWindow = std::make_shared<GameConsoleWindow>();
	consoleWindow->CreateCEGUIWindow(&rootWindow);

	consoleWindow->setVisible(true);

	return true;
}

void ChatRoomScreen::Exit()
{
	SetActive(false);

	//Hide mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//When exiting, hide and deactivate window
	rootWindow->hide();
}

bool ChatRoomScreen::Update( float deltaTime )
{
	//If escape was pressed, go back to main menu
	if(inputHandler->WasKeyPressed(DIK_ESCAPE))
	{
		stateChangeEvent(GameStates::MainMenuScreen);
	}

	return true;
}

bool ChatRoomScreen::Render( float deltaTime )
{
	//rootWindow->render();

	return true;
}