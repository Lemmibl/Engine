#include "MainMenuScreen.h"

MainMenuScreen::MainMenuScreen()
: GenericScreen()
{
	rootWindow = nullptr;
}

MainMenuScreen::~MainMenuScreen()
{
}

void MainMenuScreen::Enter()
{
	SetActive(true);

	//Show mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(true);

	rootWindow->activate();
	rootWindow->show();
}

void MainMenuScreen::Exit()
{
	SetActive(false);

	//Hide mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//When exiting, hide and deactivate window
	rootWindow->hide();
}

bool MainMenuScreen::Initialize(std::shared_ptr<InputClass> extInput)
{
	input = extInput;

	//Load the layout for this menu
	rootWindow = CEGUI::WindowManager::getSingleton().loadLayoutFromFile("mainMenu.layout");
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
	startGame->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(startGameFunction));

	//Set up open options function and bind it to the Options button
	auto& openOptionsFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to change to options menu
		stateChangeEvent(GameStates::OptionsScreen);

		return true;
	};

	CEGUI::Window* optionsMenu = rootWindow->getChild("Options");
	optionsMenu->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::SubscriberSlot::SubscriberSlot(openOptionsFunction));


	//Set up quit function and bind it to Exit Game button event
	auto& quitFunction = 
		[&](const CEGUI::EventArgs& args) -> bool
	{
		//Throws an event that will be caught by screenManager, signalling to quit the game
		stateChangeEvent(GameStates::QuitGame);

		return true;
	};

	CEGUI::Window* quitGame = rootWindow->getChild("Exit Game");
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
	//Not much going on here either

	return true;
}