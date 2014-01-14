#include "WeatherSystem.h"


WeatherSystem::WeatherSystem()
:	elapsedTime(0.0f),
	updateThreshold(5.0f),
	changeAmplitude(0.2f),
	lerpAmountThisUpdate(0.0f)
{
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
		newWindDirection.x += (changeAmplitude * RandomFloat(-1.0f, 1.0f));
		newWindDirection.z += (changeAmplitude * RandomFloat(-1.0f, 1.0f));

		NormalizeFloat3(&newWindDirection);

		elapsedTime = 0.0f;
		lerpAmountThisUpdate = 0.0f;
	}
	else
	{

		lerpAmountThisUpdate = (deltaTime / updateThreshold); //Calculate amount we'll be moving by this frame //stagesOfDay[currentStageOfDay].DurationOfStage


		windDirection.x = JustALerpFunction(windDirection.x, newWindDirection.x, lerpAmountThisUpdate);
		windDirection.y = JustALerpFunction(windDirection.y, newWindDirection.y, lerpAmountThisUpdate);
		windDirection.z = JustALerpFunction(windDirection.z, newWindDirection.z, lerpAmountThisUpdate);

		NormalizeFloat3(&windDirection);
	}
}
