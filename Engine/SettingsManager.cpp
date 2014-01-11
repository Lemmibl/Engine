#include "SettingsManager.h"


SettingsManager::SettingsManager()
{
	//Load config file upon construction
	cfg.readFile(filepath.c_str());
}

void SettingsManager::ReloadSettings()
{
	//Reload file
	cfg.readFile(filepath.c_str());

	//Shoot off event that we should re-initialize all values that are stored in settings.
	loadEvent(&cfg);
} 
