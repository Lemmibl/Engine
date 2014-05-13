#include "GameConsoleWindow.h"

#include "FormattedListboxTextItem.h"
#include "CEGUI/WindowFactoryManager.h"
#include "CEGUI/CommonDialogs/ColourPicker/ColourPicker.h"
#include "CEGUI/CommonDialogs/ColourPicker/Controls.h"

int GameConsoleWindow::iInstanceNumber;            // Don't forget this declaration

//TODO: Take a rootWnd ptr here?
GameConsoleWindow::GameConsoleWindow()
{
	m_ConsoleWindow = NULL;       // Always good practice to initialize a pointer to a NULL value, helps when switching to Release Builds.
	iInstanceNumber = 0;
	sNamePrefix = CEGUI::String("");
	userName = CEGUI::String("");
	colourPicker = nullptr;

	textColour = CEGUI::Colour(1.0f, 1.0f, 1.0f); //Default to white
	errorColour = CEGUI::Colour(1.0f, 0.0f, 0.0f);
	systemColour = CEGUI::Colour(0.0f, 1.0f, 0.0f);
}

GameConsoleWindow::~GameConsoleWindow()
{
	colourPicker = nullptr;
	m_ConsoleWindow = nullptr;
}

void GameConsoleWindow::CreateCEGUIWindow(CEGUI::Window** rootWindow)
{
	// Get a local pointer to the CEGUI Window Manager, Purely for convenience to reduce typing
	CEGUI::WindowManager *pWindowManager = CEGUI::WindowManager::getSingletonPtr();

	// Now before we load anything, lets increase our instance number to ensure no conflicts.  
	// I like the format #_ConsoleRoot so thats how i'm gonna make the prefix.  This simply
	// Increments the iInstanceNumber then puts it + a _ into the sNamePrefix string. 
	sNamePrefix = ++iInstanceNumber + "_";//"";

	// Now that we can ensure that we have a safe prefix, and won't have any naming conflicts lets create the window
	// and assign it to our member window pointer m_ConsoleWindow
	// inLayoutName is the name of your layout file (for example "console.layout"), don't forget to rename inLayoutName by our layout file
	m_ConsoleWindow = pWindowManager->loadLayoutFromFile("customLayouts/ChatRoom.layout");

	// Being a good programmer, its a good idea to ensure that we got a valid window back. 
	if(m_ConsoleWindow)
	{
		if((*rootWindow) != nullptr)
		{
			// Lets add our new window to the Root GUI Window
			//TODO: ... root gui window..
			(*rootWindow)->addChild(m_ConsoleWindow);
		}
		else
		{
			//In case we didn't recieve a real root window ptr, we set this window as root.
			CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(m_ConsoleWindow);

			//Copy root so that we can show/hide this window from outside of the class etc.
			*rootWindow = m_ConsoleWindow;
		}

		auto* sideMenu = m_ConsoleWindow->getChild(sNamePrefix + "SideMenu");

		CEGUI::WindowFactoryManager::addWindowType<CEGUI::ColourPicker>();
		CEGUI::WindowFactoryManager::addWindowType<CEGUI::ColourPickerControls>();

		colourPicker = static_cast<CEGUI::ColourPicker*>(CEGUI::WindowManager::getSingleton().createWindow("Vanilla/ColourPicker"));
		colourPicker->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05f, 0.0f), CEGUI::UDim(0.1f, 0.0f)));
		colourPicker->setSize(CEGUI::USize(CEGUI::UDim(0.5f, 0.0f), CEGUI::UDim(0.04f, 0.0f)));
		static_cast<CEGUI::ColourPicker*>(colourPicker)->setColour(CEGUI::Colour(1.0f, 1.0f, 1.0f));
		

		sideMenu->addChild(colourPicker);

		//Set default username to.... Whatever is in the EditBox at start.
		userName = sideMenu->getChild("NameEditbox")->getText();

		// Now register the handlers for the events (Clicking, typing, etc)
		RegisterHandlers();

		setVisible(false);
		m_bConsole = false;
	}
	else
	{
		// Something bad happened and we didn't successfully create the window lets output the information
		CEGUI::Logger::getSingleton().logEvent("Error: Unable to load the ConsoleWindow from .layout");
	}
}

void GameConsoleWindow::RegisterHandlers()
{
	// Alright now we need to register the handlers.  We mentioned above we want to acknowledge when the user presses Enter, and 
	// when they click the 'Send' button.  So we need to register each of those events

	// Now for the TextSubmitted, we will be registering the event on the edit box, which is where the users cursor will be when   
	//they press Enter.  I'm not going to break this down as much, because I believe that is very ugly to read, but was a good  
	//way of expressing it.  Here is the function call.
	m_ConsoleWindow->getChild(sNamePrefix + "Console/Editbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted,
		CEGUI::Event::Subscriber(&GameConsoleWindow::Handle_TextSubmitted,this));

	// First lets register the Send button.  Our buttons name is "ConsoleRoot/SendButton", but don't forget we prepended a name to      
	// all the windows which were loaded.  So we need to take that into account here.
	m_ConsoleWindow->getChild(sNamePrefix + "Console/Submit")->subscribeEvent(
		CEGUI::PushButton::EventClicked,
		CEGUI::Event::Subscriber(&GameConsoleWindow::Handle_SendButtonPressed, this)); // Call Handle_SendButtonPressed member of GameConsoleWindow using (this) instance we're in right now

	//Hook up the name edit box with an event so that the user name gets updated when a new name is input.
	m_ConsoleWindow->getChild(sNamePrefix + "SideMenu/NameEditbox")->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&GameConsoleWindow::Handle_UserNameChanged, this));

	//Subscribe to colour accepted event, call Handle_TextColourChanged when event is thrown.
	colourPicker->subscribeEvent(CEGUI::ColourPicker::EventAcceptedColour, CEGUI::Event::Subscriber(&GameConsoleWindow::Handle_TextColourChanged, this));

	//Get ptr to the vertical scrollbar on the history window
	CEGUI::Scrollbar* scrollBar = static_cast<CEGUI::Listbox*>(m_ConsoleWindow->getChild(sNamePrefix + "Console/History"))->getVertScrollbar();

	//Tweak some settings
	scrollBar->setEndLockEnabled(true);

	//These settings control how far you scroll every time you push the up/down buttons
	//scrollBar->setOverlapSize(100.0f);
	scrollBar->setStepSize(5.0f);

	const float scrollStepSize = 0.01f;

	scrollBar->setConfig(0, 0, &scrollStepSize, 0, 0);
}

bool GameConsoleWindow::Handle_TextColourChanged(const CEGUI::EventArgs &e)
{
	textColour = static_cast<CEGUI::ColourPicker*>(colourPicker)->getColour().getARGB();

	return true;
}

bool GameConsoleWindow::Handle_UserNameChanged(const CEGUI::EventArgs &e)
{
	const CEGUI::WindowEventArgs* args = static_cast<const CEGUI::WindowEventArgs*>(&e);

	//Retrieve text that was just entered
	userName = args->window->getText();


	return true;
}

bool GameConsoleWindow::Handle_TextSubmitted(const CEGUI::EventArgs &e)
{
	// The following line of code is not really needed in this particular example, but is good to show.  The EventArgs by itself 
	// only has limited uses. You will find it more useful to cast this to another type of Event.  In this case WindowEventArgs
	// could be much more useful as we are dealing with a CEGUI::Window.  Notably, this will allow you access to the .window
	// member of the argument, which will have a pointer to the window which called the event.  You can imagine that would be
	// useful!
	const CEGUI::WindowEventArgs* args = static_cast<const CEGUI::WindowEventArgs*>(&e);

	// Now we need to get the text that is in the edit box right now.
	CEGUI::String Msg = m_ConsoleWindow->getChild(sNamePrefix + "Console/Editbox")->getText();

	// Since we have that string, lets send it to the TextParser which will handle it from here
	ParseText(Msg);

	// Now that we've finished with the text, we need to ensure that we clear out the Editbox.  This is what we would expect
	// To happen after we press enter
	m_ConsoleWindow->getChild(sNamePrefix + "Console/Editbox")->setText("");

	return true;
}

bool GameConsoleWindow::Handle_SendButtonPressed(const CEGUI::EventArgs &e)
{
	CEGUI::String Msg = m_ConsoleWindow->getChild(sNamePrefix + "Console/Editbox")->getText();
	ParseText(Msg);
	m_ConsoleWindow->getChild(sNamePrefix + "Console/Editbox")->setText("");

	return true;
}


void GameConsoleWindow::PrintText(CEGUI::String inMsg)
{
	//Pass through string to internal function, and we use the "internal system" colour!
	OutputText(inMsg, systemColour);
}


void GameConsoleWindow::ParseText(CEGUI::String inMsg)
{

	// I personally like working with std::string. So i'm going to convert it here.
	std::string inString = inMsg.c_str();

	if (inString.length() >= 1) // Be sure we got a string longer than 0
	{
		if (inString.at(0) == '/') // Check if the first letter is a 'command'
		{
			std::string::size_type commandEnd = inString.find(" ", 1);
			std::string command = inString.substr(1, commandEnd - 1);
			//std::string commandArgs = inString.substr(commandEnd + 1, inString.length() - (commandEnd + 1));

			//convert command to lower case
			for(std::string::size_type i=0; i < command.length(); i++)
			{
				command[i] = tolower(command[i]);
			}

			// Begin processing

			//if (command == "say")
			//{
			//	////"Optimal" way to replace the /say command with a "You:"
			//	//inString[0] = 'Y';
			//	//inString[1] = 'o';
			//	//inString[2] = 'u';
			//	//inString[3] = ':';

			//	CEGUI::String outString = userName + ": " + inString.substr(4); // Create a new string with your "name" and only add the part of the string that isn't a command
			//	OutputText(outString, textColour);
			//}
			//else 
			if (command == "help")
			{
				OutputText("Do you want help? There is none! Despair!",  systemColour);
			}
			else if (command == "yell")
			{
				OutputText("You: AIEEEEEE!", textColour);
			}
			else
			{
				std::string outString = "<" + inString + "> is an invalid command.";
				OutputText(outString, errorColour); // With red ANGRY colors!
			}
		} // End if /
		else
		{
			CEGUI::String outString = userName + ": " + inString; // Create a new string with your "name" and only add the part of the string that isn't a command
			OutputText(outString, textColour); // no commands, just output what was written
		}
	} 
}

void GameConsoleWindow::OutputText(CEGUI::String inMsg, CEGUI::Colour colour)
{

	// Get a pointer to the ChatBox so we don't have to use this ugly getChild function everytime.
	CEGUI::Listbox *outputWindow = static_cast<CEGUI::Listbox*>(m_ConsoleWindow->getChild(sNamePrefix + "Console/History"));

	CEGUI::FormattedListboxTextItem* newItem=0; // This will hold the actual text and will be the listbox segment / item

	newItem = new CEGUI::FormattedListboxTextItem(inMsg, colour); // instance new item with colour
	outputWindow->addItem(newItem); // Add the new FormattedListBoxTextItem to the ListBox
}

void GameConsoleWindow::setVisible(bool visible)
{
	m_ConsoleWindow->setVisible(visible);
	m_bConsole = visible;

	CEGUI::Editbox* Editbox = static_cast<CEGUI::Editbox*>(m_ConsoleWindow->getChild(sNamePrefix + "Console/Editbox"));

	if(visible)
		Editbox->activate();
	else
		Editbox->deactivate();
}

bool GameConsoleWindow::isVisible()
{
	return m_ConsoleWindow->isVisible();
}