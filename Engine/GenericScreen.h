#pragma once
#include "inputclass.h"

class GenericScreen
{
public:
	GenericScreen();
	virtual ~GenericScreen();

	virtual void Enter() = 0;
	virtual void Exit() = 0;

	virtual bool Update(float deltaTime) = 0;
	virtual bool Render(float deltaTime) = 0;

	const bool IsActive() const { return active; }

protected:
	bool active;
};

