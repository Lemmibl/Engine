#pragma once
#include "StructsAndEnums.h"
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
	XMFLOAT4 GetAmbientLightColor();

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
	float timePerStage, elapsedTime;

	//Containers for current lerping value.
	XMFLOAT4 currentSkysphereColor;
	XMFLOAT4 currentAmbienceColor;
	XMFLOAT4 currentDirectionalLightColor;

	vector<StageOfDayStruct> stagesOfDay;

	StageOfDay currentStageOfDay, previousFrameStageOfDay;
	StageOfDayStruct previousStageStruct;
	float lerpAmountThisStage;
};