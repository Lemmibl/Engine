#pragma once
#include "SettingsManager.h"
#include <libconfig.h++>

class SettingsDependent
{
	public:
	template<class T>
	void InitializeSettings(T* classPointer)
	{
		//Get settings manager instance and add our function to reload event
		SettingsManager& settings = SettingsManager::GetInstance();
		settings.GetEvent()->Add((*classPointer), (&T::OnSettingsReload));

		//Perhaps slightly hacky, but it saves on rewriting code.
		OnSettingsReload(&settings.GetConfig());
	};

	virtual void OnSettingsReload(Config* cfg) = 0;
};