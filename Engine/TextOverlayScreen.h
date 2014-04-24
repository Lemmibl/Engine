#pragma once
#include "GenericScreen.h"
#include "CEGUI/CEGUI.h"

class D3DManager;

class TextOverlayScreen : GenericScreen
{
public:
	TextOverlayScreen();
	~TextOverlayScreen();

	void Initialize(D3DManager* d3D);

	virtual bool Enter();
	virtual void Exit();

	virtual bool Update( float deltaTime );
	virtual bool Render( float deltaTime );
	void Clear();

	void SetText(const std::string& text);

private:
	D3DManager* d3DManager;
	CEGUI::Window* rootWindow;
	CEGUI::Window* textWindow;
};

