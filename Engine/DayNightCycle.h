#pragma once
#include "SkySphere.h"
#include <math.h>
#include "StructsAndEnums.h"
#include "SettingsManager.h"
#include <libconfig.h++>

class DayNightCycle
{
private:
	//Struct containing all of the variables that make up a stage of the day in the day/night system
	//Was made mainly to enable more streamlined code and make lerping between the different stages easier
	struct StageOfDayStruct
	{
		XMFLOAT4 AmbientColor;
		XMFLOAT4 DirectionalLightColor;
		XMFLOAT4 SkysphereColor;
		float LightIntensity;
		float DurationOfStage;
		XMFLOAT3 StartPosition;
		XMFLOAT3 EndPosition;
	};

public:
	DayNightCycle();
	DayNightCycle(const DayNightCycle&);
	~DayNightCycle();

	bool Initialize(StageOfDay startStage);
	float Update(float elapsedTime, DirLight* directionalLight, Skysphere* skysphere);
	void OnSettingsReload(Config* cfg);

	void SetTimePerStage(float time) { timePerStage = time; }
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

	inline XMFLOAT3 SaturateColorVector(XMFLOAT3 color) { color.x /= 255.0f; color.y /= 255.0f; color.z /= 255.0f; return color; }

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