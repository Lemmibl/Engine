#pragma once
#include "structs.h"
#include "SkySphere.h"
#include <math.h>

class DayNightCycle
{
public:
	DayNightCycle();
	DayNightCycle(const DayNightCycle&);
	~DayNightCycle();

	bool Initialize(float timePerStage, StageOfDay startStage);
	float Update(float elapsedTime, DirLight* directionalLight, Skysphere* skysphere);

	StageOfDay GetCurrentStageOfDay() { return currentStageOfDay; }

private:
	inline float clamp(float x, float a, float b)
	{
		return (x < a ? a : (x > b ? b : x));
	}

	inline float lerp(float a, float b, float t)
	{
		return ((1.0f-t)*a + t*b);
	}

	//Well, I don't know what else to call it.
	inline float ConvertColorValueTo_0_1_Range(float val)
	{
		return (val/255.0f);
	}

private:
	StageOfDay currentStageOfDay, previousFrameStageOfDay;
	float timePerStage, elapsedTime;

	XMFLOAT4 directionalLightDayColor;
	XMFLOAT4 directionalLightNightColor;

	XMFLOAT4 ambientDayColor;
	XMFLOAT4 ambientNightColor;

	XMFLOAT4 skysphereDawnColor;
	XMFLOAT4 skysphereDayColor;
	XMFLOAT4 skysphereDuskColor;
	XMFLOAT4 skysphereNightColor;

	//Containers for current lerping value.
	XMFLOAT4 currentSkysphereColor;
	XMFLOAT4 currentAmbienceColor;
	XMFLOAT4 currentDirectionalLightColor;

	vector<XMFLOAT3> startAndEndPositions;

	float timeOfDay, shouldLightMove;

	//Should light colors / skysphere colors be kept here?
	//Moonshine: 217 206 190
	//Yellow sunshine: 231 187 65
};