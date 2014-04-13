#include "SSAOShader.h"


SSAOShader::SSAOShader() : SettingsDependent()
{
	InitializeSettings(this);
}


SSAOShader::~SSAOShader()
{
}

void SSAOShader::OnSettingsReload( Config* cfg )
{
	float screenWidth, screenHeight;

	const Setting& cameraSettings = cfg->getRoot()["camera"];
	cameraSettings.lookupValue("fov", thFov);

	thFov = tan(thFov/2);

	//windowWidth, windowHeight
	const Setting& renderingSettings = cfg->getRoot()["rendering"];
	renderingSettings.lookupValue("windowWidth", screenWidth);
	renderingSettings.lookupValue("windowHeight", screenHeight);

	aspectRatio = (screenWidth / screenHeight);
}
