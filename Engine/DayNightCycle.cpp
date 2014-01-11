#include "DayNightCycle.h"

XMFLOAT4 DayNightCycle::GetAmbientLightColor()
{
	return currentAmbienceColor;
}

DayNightCycle::DayNightCycle()
{

}

DayNightCycle::DayNightCycle( const DayNightCycle& )
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

	//Get settings manager instance and add our function to reload event
	SettingsManager& settings = SettingsManager::GetInstance();
	settings.GetEvent()->Add(*this, &DayNightCycle::OnSettingsReload);

	//Perhaps slightly hacky, but it saves on rewriting code.
	OnSettingsReload(&settings.GetConfig());

	previousStageStruct = stagesOfDay[previousFrameStageOfDay];
	elapsedTime = 0.0f;
	lerpAmountThisStage = 0.0f;

	return true;
}

float DayNightCycle::Update( float deltaTime, DirLight* directionalLight, Skysphere* skysphere )
{
	XMVECTOR startVector, endVector, currentVector;

	currentVector =  XMLoadFloat3(&directionalLight->Position); //Save current position in XMVECTOR format, needed when calculating sum of each frame's lerping

	this->elapsedTime += deltaTime;

	if(lerpAmountThisStage < 1.0f)
	{
		lerpAmountThisStage += (deltaTime / timePerStage); //Calculate amount we'll be moving by this frame //stagesOfDay[currentStageOfDay].DurationOfStage
	}


	//See if we should change stage of day
	if(this->elapsedTime > stagesOfDay[currentStageOfDay].DurationOfStage)
	{
		//Save the old struct before we change to a new one
		previousStageStruct = stagesOfDay[currentStageOfDay];

		//If this has happened, then we switch to next stage of day, because the time has passed
		if(currentStageOfDay == NIGHT) //If it was night, we've reached the end of the enum list, hence...
		{
			currentStageOfDay = DAWN; //We reset
			previousFrameStageOfDay = NIGHT; //Set previous to night... shouldn't be necessary..?
		}
		else
		{
			currentStageOfDay = (StageOfDay)(currentStageOfDay+1);
		}
		
		this->elapsedTime = 0.0f; //Reset elapsed time as well
		lerpAmountThisStage =  0.0f;
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

	//Here we lerp the positions
	currentVector = XMVectorLerp(startVector, endVector, lerpAmountThisStage);
	XMStoreFloat3(&directionalLight->Position, currentVector);

	//Then we extract the start and end colors of this stage to lerp between
	startVector = XMLoadFloat4(&previousStageStruct.AmbientColor);
	endVector = XMLoadFloat4(&stagesOfDay[currentStageOfDay].AmbientColor);

	//Lerp between the two values and save the current value as our current ambience color
	currentVector = XMVectorLerp(startVector, endVector, lerpAmountThisStage);
	XMStoreFloat4(&currentAmbienceColor, currentVector);

	//Currently, all of our different colors are the same, so just apply them right here and now.
	directionalLight->Color = currentAmbienceColor;

	directionalLight->Intensity = lerp(previousStageStruct.LightIntensity, stagesOfDay[currentStageOfDay].LightIntensity, lerpAmountThisStage);

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

	/************************************************************************/
	//Moonshine: 217 206 190
	//Yellow sunshine: 231 187 65                                                            
	/************************************************************************/

	//StageOfDayStruct dawn;
	//dawn.AmbientColor =		XMFLOAT4(0.3f,	0.3f,	0.3f,	1.0f);
	//dawn.DirectionalLightColor = dawn.AmbientColor;
	//dawn.SkysphereColor = XMFLOAT4(1.0f,	ConvertColorValueTo_0_1_Range(160.0f),	ConvertColorValueTo_0_1_Range(122.0f),	1.0f);
	//
	//dawn.StartPosition =	XMFLOAT3(150.0f, 45.0f, 30.0f);
	//dawn.EndPosition =		XMFLOAT3(75.0f, 65.0f, 50.0f);
	//dawn.DurationOfStage = timePerStage;
	//dawn.LightIntensity = 0.7f;
	//
	//stagesOfDay.push_back(dawn);



	//StageOfDayStruct morning;
	//morning.AmbientColor =	XMFLOAT4(0.5f,	0.5f,	0.5f,	1.0f);
	//morning.DirectionalLightColor = morning.AmbientColor;
	//morning.SkysphereColor = XMFLOAT4(ConvertColorValueTo_0_1_Range(180.0f),	ConvertColorValueTo_0_1_Range(150.0f), ConvertColorValueTo_0_1_Range(180.0f), 1.0f);
	//
	//morning.StartPosition = dawn.EndPosition;
	//morning.EndPosition =	XMFLOAT3(35.0f, 90.0f, 70.0f);
	//morning.DurationOfStage = timePerStage;
	//morning.LightIntensity = 0.85f;

	//stagesOfDay.push_back(morning);


	//StageOfDayStruct day;
	//day.AmbientColor =		XMFLOAT4(0.7f,	0.7f,	0.7f,	1.0f);
	//day.DirectionalLightColor = day.AmbientColor;
	//day.SkysphereColor = XMFLOAT4(ConvertColorValueTo_0_1_Range(100.0f),	ConvertColorValueTo_0_1_Range(149.0f),	ConvertColorValueTo_0_1_Range(237.0f),	1.0f);
	//
	//day.StartPosition = morning.EndPosition;
	//day.EndPosition =	XMFLOAT3(-35.0f, 90.0f, 50.0f);
	//day.DurationOfStage = timePerStage*3.0f; //Make day three times as long as the other stages
	//day.LightIntensity = 1.0f;

	//stagesOfDay.push_back(day);



	//StageOfDayStruct dusk;
	//dusk.AmbientColor =		XMFLOAT4(0.5f,	0.5f,	0.5f,	1.0f);
	//dusk.DirectionalLightColor = dusk.AmbientColor;
	//dusk.SkysphereColor = morning.SkysphereColor;//XMFLOAT4(0.2f,	ConvertColorValueTo_0_1_Range(180.0f),	ConvertColorValueTo_0_1_Range(170.0f),	1.0f);
	//
	//dusk.StartPosition = day.EndPosition;
	//dusk.EndPosition =	XMFLOAT3(-75.0f, 65.0f, 30.0f);
	//dusk.DurationOfStage = timePerStage;
	//dusk.LightIntensity = 0.85f;

	//stagesOfDay.push_back(dusk);



	//StageOfDayStruct evening;
	//evening.AmbientColor =	XMFLOAT4(0.3f,	0.3f,	0.3f,	1.0f);
	//evening.DirectionalLightColor = evening.AmbientColor;
	//evening.SkysphereColor = dawn.SkysphereColor;//XMFLOAT4(0.15f,	0.23f,	0.25f,	1.0f);
	//
	//evening.StartPosition = dusk.EndPosition;
	//evening.EndPosition =	XMFLOAT3(-150.0f, 45.0f, 30.0f);
	//evening.DurationOfStage = timePerStage;
	//evening.LightIntensity = 0.7f;

	//stagesOfDay.push_back(evening);



	//StageOfDayStruct night;
	//night.AmbientColor =	XMFLOAT4(0.05f,	0.05f,	0.1f,	0.5f);
	//night.DirectionalLightColor = night.AmbientColor;
	//night.SkysphereColor = night.AmbientColor;
	//
	//night.StartPosition =	evening.EndPosition;
	//night.EndPosition =		dawn.StartPosition;
	//night.DurationOfStage = timePerStage*3.0f; //Make night five times as long as the other stages
	//night.LightIntensity = 0.5f;

	//stagesOfDay.push_back(night);