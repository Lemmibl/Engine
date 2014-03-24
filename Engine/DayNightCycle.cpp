#include "DayNightCycle.h"

XMFLOAT4 DayNightCycle::GetAmbientLightColor()
{
	return currentAmbienceColor;
}

DayNightCycle::DayNightCycle() : SettingsDependent()
{

}

DayNightCycle::~DayNightCycle()
{
}

bool DayNightCycle::Initialize(StageOfDay startStage )
{
	//Other sanity checks
	if( (int)startStage >= DAWN && (int)startStage <= NIGHT)
	{
		this->currentStageOfDay = startStage;
	}
	else
	{
		//Just some default value in case we go out of bounds
		this->currentStageOfDay = DAY;
	}

	if(this->currentStageOfDay != NIGHT)
	{
		previousFrameStageOfDay = (StageOfDay)(currentStageOfDay-1);
	}
	else
	{
		previousFrameStageOfDay = NIGHT; //Because if start stage is dawn (stage 0), previous stage was night
	}

	InitializeSettings(this);

	previousStageStruct = stagesOfDay[previousFrameStageOfDay];
	elapsedTime = 0.0f;
	lerpAmountThisStage = 0.0f;

	return true;
}

float DayNightCycle::Update( float deltaTime, DirLight* directionalLight, Skysphere* skysphere, XMFLOAT3* cameraPosition)
{
	XMVECTOR startVector, endVector, currentVector;
	XMFLOAT4 apexColor, centerColor;

	currentVector =  XMLoadFloat3(&directionalLight->Position); //Save current position in XMVECTOR format, needed when calculating sum of each frame's lerping

	elapsedTime += deltaTime;

	if(lerpAmountThisStage < 1.1f)
	{
		lerpAmountThisStage += (deltaTime / stagesOfDay[currentStageOfDay].DurationOfStage); //Calculate amount we'll be moving by this frame
	}


	//See if we should change stage of day
	if(elapsedTime >= stagesOfDay[currentStageOfDay].DurationOfStage)
	{
		//Save the old struct before we change to a new one
		previousStageStruct = stagesOfDay[currentStageOfDay];

		//If this has happened, then we switch to next stage of day, because the time has passed
		if(currentStageOfDay == NIGHT) //If it was night, we've reached the end of the enum list, hence...
		{
			currentStageOfDay = DAWN; //We reset
			previousFrameStageOfDay = NIGHT; //Set previous to night
		}
		else
		{
			currentStageOfDay = (StageOfDay)(currentStageOfDay+1);
		}
		
		this->elapsedTime = 0.0f; //Reset elapsed time as well
		lerpAmountThisStage = 0.0f;
	}

	if(currentStageOfDay != previousFrameStageOfDay) //Tiny optimization
	{
		skysphere->SetCenterColor(stagesOfDay[currentStageOfDay].SkysphereColor);
		skysphere->SetApexColor(stagesOfDay[previousFrameStageOfDay].SkysphereColor);
	}

	//Extract start and end position of this stage of day so that we can lerp the directional light position 
	//between them as to create the illusion of the sun moving during the course of the day
	startVector = XMLoadFloat3(&stagesOfDay[currentStageOfDay].StartPosition);
	endVector = XMLoadFloat3(&stagesOfDay[currentStageOfDay].EndPosition);

	//Here we lerp the positions and add camera position
	currentVector = XMVectorLerp(startVector, endVector, lerpAmountThisStage) + XMLoadFloat3(cameraPosition);
	XMStoreFloat3(&directionalLight->Position, currentVector);

	//Then we extract the start and end colors of this stage to lerp between
	startVector = XMLoadFloat4(&previousStageStruct.AmbientColor);
	endVector = XMLoadFloat4(&stagesOfDay[currentStageOfDay].AmbientColor);

	//Lerp between the two values and save the current value as our current ambience color
	currentVector = XMVectorLerp(startVector, endVector, lerpAmountThisStage);

	directionalLight->Intensity = lerp(previousStageStruct.LightIntensity, stagesOfDay[currentStageOfDay].LightIntensity, lerpAmountThisStage);

	//scale ambient color with light intensity
	XMStoreFloat4(&currentAmbienceColor, currentVector * directionalLight->Intensity);


	//Currently, all of our different colors are the same, so just apply them right here and now.
	directionalLight->Color = currentAmbienceColor;

	previousFrameStageOfDay = currentStageOfDay;

	return lerpAmountThisStage;
}

void DayNightCycle::OnSettingsReload(Config* cfg )
{
	//Set time per stage
	cfg->getRoot()["daycycle"].lookupValue("timePerStage", timePerStage);

	//Load in array with stage-of-day structs
	const Setting &dayStages = cfg->getRoot()["daycycle"]["stagesOfDay"];
	int count = dayStages.getLength();

	 /*
	Day cycle goes in this order:

	DAWN
	MORNING
	DAY
	DUSK
	EVENING
	NIGHT

	then repeats
	*/

	for(int i = 0; i < count; ++i)
	{
		const Setting &stage = dayStages[i];

		//Set up temporary variables
		XMFLOAT4 lightColor;
		XMFLOAT3 skysphereColor;
		XMFLOAT3 startPosition;
		XMFLOAT3 endPosition;

		float durationMultiplier;
		float lightIntensity;

		//Read in variables from settings file
		stage.lookupValue("startPosX", startPosition.x);
		stage.lookupValue("startPosY", startPosition.y);
		stage.lookupValue("startPosZ", startPosition.z);

		stage.lookupValue("endPosX", endPosition.x);
		stage.lookupValue("endPosY", endPosition.y);
		stage.lookupValue("endPosZ", endPosition.z);

		stage.lookupValue("lightColorR", lightColor.x);
		stage.lookupValue("lightColorG", lightColor.y);
		stage.lookupValue("lightColorB", lightColor.z);
		lightColor.w = 1.0f;

		stage.lookupValue("skysphereColorR", skysphereColor.x);
		stage.lookupValue("skysphereColorG", skysphereColor.y);
		stage.lookupValue("skysphereColorB", skysphereColor.z);

		stage.lookupValue("durationMultiplier", durationMultiplier);
		stage.lookupValue("lightIntensity", lightIntensity);

		//Assign variables to a struct
		StageOfDayStruct temp;
		temp.AmbientColor =				lightColor;
		temp.DirectionalLightColor =	lightColor;
		temp.SkysphereColor =			XMFLOAT4(ConvertColorValueTo_0_1_Range(skysphereColor.x),	ConvertColorValueTo_0_1_Range(skysphereColor.y),	ConvertColorValueTo_0_1_Range(skysphereColor.z), 1.0f);

		temp.StartPosition =			startPosition;
		temp.EndPosition =				endPosition;
		temp.DurationOfStage =			timePerStage * durationMultiplier;
		temp.LightIntensity =			lightIntensity;

		//Add the struct...
		stagesOfDay.push_back(temp);
	}
}