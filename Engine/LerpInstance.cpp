#include "LerpInstance.h"


LerpInstance::LerpInstance(float startVal, float finalVal)
: startValue(startVal),
finalValue(finalVal),
done(false),
timePassed(0.0f),
lerpTimeInterval(1.0f)
{
}

LerpInstance::LerpInstance( float startVal, float finalVal, float lerpInterval )
	: startValue(startVal),
	finalValue(finalVal),
	done(false),
	timePassed(0.0f),
	lerpTimeInterval( 1.0f / lerpInterval) 
{
}

LerpInstance::~LerpInstance()
{
}

void LerpInstance::Update( float deltaTime )
{
	//We only update the value until we've reached the end point, which is 1.0f
	if(timePassed < 1.0f)
	{
		timePassed += deltaTime * lerpTimeInterval;

		currentValue = startValue + timePassed*(finalValue - startValue);
	}
	else
	{
		//After that, it's done.
		done = true;
	}
}

void LerpInstance::ResetTimer()
{
	done = false;
	currentValue = 0.0f;
	timePassed = 0.0f;
}

void LerpInstance::ReverseLerp()
{
	float temp = startValue;
	startValue = finalValue;
	finalValue = temp;

	done = false;
	currentValue = 0.0f;
	timePassed = 0.0f;
}
