#pragma once
#include "genericscreen.h"
class OptionsScreen : public GenericScreen
{
public:
	OptionsScreen();
	~OptionsScreen();

	virtual void Enter();
	virtual void Exit();

	virtual bool Update(float deltaTime);
	virtual bool Render(float deltaTime);

private:

};

