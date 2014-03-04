#include "LoadingScreen.h"


LoadingScreen::LoadingScreen()
{
}

LoadingScreen::~LoadingScreen()
{
}

void LoadingScreen::Initialize()
{
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	rootWindow = winMgr.createWindow("DefaultWindow", "loadingRoot");
	rootWindow->setVisible(true);
	rootWindow->setSize( CEGUI::USize( CEGUI::UDim( 1.0f, 0.0f ), CEGUI::UDim( 1.0f, 0.0f ) )); //Full screen
	rootWindow->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.0f, 0.0f ), CEGUI::UDim( 0.0f, 0.0f ) )); //Start at top left corner

	/* http://cegui.org.uk/forum/viewtopic.php?p=22795#p22795 */

	textWindow = winMgr.createWindow("TaharezLook/Label", "textWindow");
	rootWindow->addChild(textWindow);

	textWindow->setPosition(CEGUI::UVector2(cegui_absdim(0.0f), cegui_absdim(0.0f)));
	textWindow->setRiseOnClickEnabled(false);
	textWindow->setPixelAligned(true);
	textWindow->setInheritsAlpha(true);
	textWindow->setAlpha(1.0f);
	textWindow->setProperty("HorzFormatting", "CentreAligned");
	textWindow->setProperty("VertFormatting", "CentreAligned");
	textWindow->setWidth(CEGUI::UDim(5.0f, 0.0f));
	textWindow->setHeight(CEGUI::UDim(5.0f, 0.0f));
	textWindow->setFont(&(CEGUI::FontManager::getSingleton().createFromFile("Jura-18.font")));

	textWindow->setText("Loading...");
	textWindow->activate();
}

void LoadingScreen::Enter()
{
}

void LoadingScreen::Exit()
{
}

bool LoadingScreen::Update( float deltaTime )
{
	return true;
}

bool LoadingScreen::Render( float deltaTime )
{
	CEGUI::System::getSingleton().getDefaultGUIContext().clearGeometry();

	//Show mouse cursor
	CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);

	//Set this base window as root.
	CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(rootWindow);

	rootWindow->activate();
	textWindow->activate();
	textWindow->show();

	CEGUI::System::getSingleton().renderAllGUIContexts();

	return true;
}

void LoadingScreen::Clear()
{
	//When exiting, hide and deactivate window
	rootWindow->hide();
}
