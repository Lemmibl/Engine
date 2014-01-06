#include "SettingsManager.h"


SettingsManager::SettingsManager()
{
	auto text = filepath.c_str();
	cfg.readFile(text);

	const Setting &thing = cfg.getRoot()["camera"];

	float posX, posY, posZ;
	thing.lookupValue("positionX", posX);
	thing.lookupValue("positionY", posY);
	thing.lookupValue("positionZ", posZ);
}

void SettingsManager::ReloadSettings()
{
	auto text = filepath.c_str();
	cfg.readFile(text);

	loadEvent(&cfg);
} 
