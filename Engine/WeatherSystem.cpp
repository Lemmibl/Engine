#include "WeatherSystem.h"

WeatherSystem::WeatherSystem()
:	SettingsDependent(),
	elapsedTime(0.0f),
	updateThreshold(5.0f),
	changeAmplitude(0.2f),
	lerpAmountThisUpdate(0.0f)
{
	//Reseed based on time. Pretty much guaranteed to be a fresh seed.
	srand(static_cast<unsigned int>(time(0)));

	InitializeSettings(this);

	windDirection = XMFLOAT3(RandomFloat(-1, 1), 0.0f, RandomFloat(-1, 1));
	NormalizeFloat3(&windDirection);
}


WeatherSystem::~WeatherSystem()
{
}


void WeatherSystem::Update( float deltaTime )
{
	elapsedTime += deltaTime;

	if(elapsedTime >= updateThreshold)
	{
		//Copy current windDir
		newWindDirection = windDirection;

		//Add new force
		newWindDirection.x += XMConvertToRadians(changeAmplitude * RandomFloat(-1.0f, 1.0f));
		newWindDirection.z += XMConvertToRadians(changeAmplitude * RandomFloat(-1.0f, 1.0f));

		elapsedTime = 0.0f;
		lerpAmountThisUpdate = 0.0f;
	}

	lerpAmountThisUpdate = (deltaTime / updateThreshold); //Calculate amount we'll be moving by this frame //stagesOfDay[currentStageOfDay].DurationOfStage

	windDirection.x = JustALerpFunction(windDirection.x, newWindDirection.x, lerpAmountThisUpdate);
	windDirection.z = JustALerpFunction(windDirection.z, newWindDirection.z, lerpAmountThisUpdate);

	NormalizeFloat3(&windDirection);
}

void WeatherSystem::OnSettingsReload( Config* cfg )
{
	Setting& settings = cfg->getRoot()["windsystem"];

	settings.lookupValue("updateThreshold",	updateThreshold	);
	settings.lookupValue("changeAmplitude",	changeAmplitude	);
}
