#include "ChatRoomScreen.h"

#include "NetworkClient.h"
#include "GameConsoleWindow.h"
#include "inputclass.h"
#include "NetworkServer.h"

ChatRoomScreen::ChatRoomScreen(std::shared_ptr<InputClass> input) : GenericScreen()
{
	inputHandler = input;
	connectionActive = false;
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

	auto* connectButton = rootWindow->getChild("SideMenu/ConnectButton");

	connectButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatRoomScreen::Handle_ConnectButtonPressed, this));

	// First lets register the Send button.  Our buttons name is "ConsoleRoot/SendButton", but don't forget we prepended a name to      
	// all the windows which were loaded.  So we need to take that into account here.
	auto* editBox = rootWindow->getChild("Console/Editbox");
	editBox->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ChatRoomScreen::Handle_TextSent, this));

	auto* sendButton = rootWindow->getChild("Console/Submit");
	sendButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatRoomScreen::Handle_TextSent, this));

	consoleWindow->setVisible(true);

	netClient = std::make_shared<NetworkClient>(consoleWindow.get());
	netServer = std::make_shared<NetworkServer>(consoleWindow.get());

	NetworkServer::ServerSettings settings;

	if(netServer->Initialize(settings))
	{
		consoleWindow->PrintText("Server is online.");
	}

	return true;
}

void ChatRoomScreen::Exit()
{
	SetActive(false);

	//Hide mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//When exiting, hide and deactivate window
	rootWindow->hide();

	//Close connection
	if(connectionActive)
	{
		netClient->Shutdown();
	}
}

bool ChatRoomScreen::Update(float deltaTime)
{
	//If escape was pressed, go back to main menu
	if(inputHandler->WasKeyPressed(DIK_ESCAPE))
	{
		stateChangeEvent(GameStates::MainMenuScreen);
	}

	if(connectionActive)
	{
		//Update our network client. If update returns false, it means something went wrong or we disconnected from the host.
		connectionActive = netClient->Update();
	}

	netServer->Update();

	return true;
}

bool ChatRoomScreen::Render( float deltaTime )
{
	//rootWindow->render();

	return true;
}

bool ChatRoomScreen::Handle_ConnectButtonPressed( const CEGUI::EventArgs &e )
{
	auto& ipString = rootWindow->getChild("SideMenu/IPEditBox")->getText();

	if(connectionActive)
	{
		rootWindow->getChild("SideMenu/ConnectButton")->setText("Connect");

		netClient->SendDisconnectPacket();
		connectionActive = false;
	}
	else
	{
		rootWindow->getChild("SideMenu/ConnectButton")->setText("Disconnect");

		//Temp without IP.
		connectionActive = netClient->Connect();
	}

	return true;
}

bool ChatRoomScreen::Handle_TextSent( const CEGUI::EventArgs &e )
{
	if(connectionActive)
	{
		const CEGUI::WindowEventArgs* args = static_cast<const CEGUI::WindowEventArgs*>(&e);

		auto* editbox = rootWindow->getChild("Console/Editbox");

		netClient->SendTextPacket(editbox->getText().c_str());

		editbox->setText("");
	}

	return true;
}
