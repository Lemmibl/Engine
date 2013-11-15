#pragma once

/************************************************************************/
/*    A linear interpolation instance. Call update to make it progress. *
 *    Call GetCurrentValue() to get ... current value					*/
/************************************************************************/
class LerpInstance
{
public:
	//Should be self explanatory. Interpolates linearly between start value and final value. Default time interval for this to happen in is 1.0f second.
	explicit LerpInstance(float startValue, float finalValue);

	//Interpolates linearly between start value and final value. Use lerpTimeInterval to control how long it should take for the lerp to finish.
	explicit LerpInstance(float startValue, float finalValue, float lerpTimeInterval);
	~LerpInstance();
	
	//Gets current value that is somewhere between startValue and finalValue
	const float GetCurrentValue() const { return currentValue; };

	//When lerping is done you can decide to ... idk, remove the instance or something.
	const bool IsLerpingDone() const { return done; };

	//Goes between 0 and Time. When timePassed has reached 1.0f, we're done.
	void Update(float deltaTime);

	//Resets the timer so that we can repeat the lerp for whatever reason
	void ResetTimer();

	//Call this to flip start and final values to make it go lerp the other way around.
	void ReverseLerp();

private:
	//Private default constructor that is never implemented
	LerpInstance();

	float lerpTimeInterval, timePassed;
	float startValue, finalValue, currentValue;
	bool done;
};

