#include "MainMenuScreen.h"

MainMenuScreen::MainMenuScreen(std::shared_ptr<InputClass> extInput)
: GenericScreen()
{
	rootWindow = nullptr;
	input = extInput;
}

MainMenuScreen::~MainMenuScreen()
{
}

bool MainMenuScreen::Enter()
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

void MainMenuScreen::Exit()
{
	SetActive(false);

	//Hide mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//When exiting, hide and deactivate window
	rootWindow->hide();
}

bool MainMenuScreen::Initialize()
{
	//Load the layout for this menu
	rootWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("custom_layouts/mainMenu.layout");
	if(!rootWindow)
	{
		return false;
	}
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	//Set up start game function and bind it to the options button
	auto& startGameFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to quit the game
		stateChangeEvent(GameStates::GameScreen);

		return true;
	};

	CEGUI::Window* startGame = rootWindow->getChild("Start Game");
	if(!startGame)
	{
		return false;
	}
	startGame->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(startGameFunction));

	//Set up open options function and bind it to the Options button
	auto& openChatRoomFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to change to options menu
		stateChangeEvent(GameStates::ChatRoom);

		return true;
	};

	CEGUI::Window* chatRoom = rootWindow->getChild("Chat Room");
	if(!chatRoom)
	{
		return false;
	}
	chatRoom->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(openChatRoomFunction));


	//Set up quit function and bind it to Exit Game button event
	auto& quitFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to quit the game
		stateChangeEvent(GameStates::QuitGame);

		return true;
	};

	CEGUI::Window* quitGame = rootWindow->getChild("Exit Game");
	if(!quitGame)
	{
		return false;
	}
	quitGame->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(quitFunction));

	return true;
}


bool MainMenuScreen::Update( float deltaTime )
{
	//If escape was pressed from the main menu, throw quitGame event
	if(input->WasKeyPressed(DIK_ESCAPE))
	{
		stateChangeEvent(GameStates::QuitGame);
	}

	return true;
}

bool MainMenuScreen::Render( float deltaTime )
{
	//Not much going on here either; it's all managed by internal CEGUI rendering

	return true;
}