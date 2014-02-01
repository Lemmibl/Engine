#pragma once
#include "SchladetschEvents.h"
#include "inputclass.h"
#include "GameStates.h"

class GenericScreen
{
public:
	GenericScreen();
	virtual ~GenericScreen();

	virtual void Enter() = 0;
	virtual void Exit() = 0;

	virtual bool Update(float deltaTime) = 0;
	virtual bool Render(float deltaTime) = 0;

	Schladetsch::Events::Event<GameStates::Type>* GetStateChangeEvent() { return &stateChangeEvent; }
	const bool IsActive() const { return active; }

protected:
	void SetActive(bool val) { active = val; }
	Schladetsch::Events::Event<GameStates::Type> stateChangeEvent;

private:
	bool active;
};

