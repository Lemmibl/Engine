#pragma once
#include "CEGUI/CEGUI.h"
#include "CEGUI/String.h"

class ColourPicker;

class GameConsoleWindow
{
private:
	#define SystemColour CEGUI::Colour(0xFF00FF00) //Colour used by the system itself. Used for stuff like: /help
	#define ErrorColour CEGUI::Colour(0xFFFF0000) //Generally an angry red to denote that something is WRONG. Used for stuff like: "<" + inString + "> is an invalid command.";

public:
	GameConsoleWindow();                   // Constructor
	~GameConsoleWindow();				   // Destructor
	void setVisible(bool visible);         // Hide or show the console
	bool isVisible();                      // return true if console is visible, false if is hidden

	void CreateCEGUIWindow(CEGUI::Window** rootWindow = nullptr);	// The function which will load in the CEGUI Window and register event handlers

	//Print external text. Alternatively with colour. Default is system colours.
	void PrintText(CEGUI::String inMsg, CEGUI::Colour textColour = SystemColour);

	CEGUI::String GetUserNameString() { return userName; }
	CEGUI::Colour GetUserTextColour() { return userTextColour; }

private:
	void RegisterHandlers();                                   // Register our handler functions
	bool Handle_TextSubmitted(const CEGUI::EventArgs &e);      // Handle when we press Enter after typing
	bool Handle_SendButtonPressed(const CEGUI::EventArgs &e);  // Handle when we press the Send button 
	bool Handle_TextColourChanged(const CEGUI::EventArgs &e);
	bool Handle_UserNameChanged(const CEGUI::EventArgs &e);
	
	void ParseText(CEGUI::String inMsg);	// Parse the text the user submitted.
	void OutputText(CEGUI::String inMsg, CEGUI::Colour colour = SystemColour); // Post the message to the ChatHistory listbox with white as default text colo(u)r

	CEGUI::Window* colourPicker;
	CEGUI::Window* m_ConsoleWindow;								// This will be a pointer to the ConsoleRoot window.
	CEGUI::Colour userTextColour;									//Text colour being the colour that the user can choose themselves
	CEGUI::String sNamePrefix;                                  // This will be the prefix name we give the layout
	CEGUI::String userName;
	static int iInstanceNumber;                                 // This will be the instance number for this class.
	bool m_bConsole;
};