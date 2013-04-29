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

bool DayNightCycle::Initialize( float timePerStage, StageOfDay startStage )
{
	this->timePerStage = timePerStage;
	this->currentStageOfDay = startStage;

	if((int)startStage > 0)
	{
		previousFrameStageOfDay = (StageOfDay)(currentStageOfDay-1);
	}
	else
	{
		previousFrameStageOfDay = NIGHT; //Because if start stage is dawn (stage 0), previous stage was night
	}

	//http://www.foszor.com/blog/xna-color-chart/

	//LightSalmon = {R:255 G:160 B:122 A:255}
	//Cornflower Blue = {R:100 G:149 B:237 A:255}
	//Deep Sky Blue = {R:0 G:191 B:255 A:255}
	//Dark Turquoise = {R:0 G:206 B:209 A:255}

	////Initialize colors for the different stages of the day.
	//directionalLightDayColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	//directionalLightNightColor = XMFLOAT4(0.15f, 0.15f, 0.25f, 1.0f);

	//skysphereDawnColor =	XMFLOAT4(1.0f,										ConvertColorValueTo_0_1_Range(160.0f),	ConvertColorValueTo_0_1_Range(122.0f),	1.0f);
	//						
	//skysphereDayColor =		XMFLOAT4(ConvertColorValueTo_0_1_Range(100.0f),		ConvertColorValueTo_0_1_Range(149.0f),	ConvertColorValueTo_0_1_Range(237.0f),	1.0f);
	//skysphereDuskColor =	XMFLOAT4(0.1f,										ConvertColorValueTo_0_1_Range(206.0f),	ConvertColorValueTo_0_1_Range(209.0f),	1.0f);
	//skysphereNightColor =	XMFLOAT4(0.15f,										0.15f,									0.25f,									1.0f);

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

	/************************************************************************/
	//Should light colors / skysphere colors be kept here?
	//Moonshine: 217 206 190
	//Yellow sunshine: 231 187 65                                                            
	/************************************************************************/

	stagesOfDay.reserve(6);

	StageOfDayStruct dawn;
	dawn.AmbientColor =		XMFLOAT4(0.3f,	0.3f,	0.3f,	1.0f);
	dawn.DirectionalLightColor = dawn.AmbientColor;
	dawn.SkysphereColor = XMFLOAT4(1.0f,	ConvertColorValueTo_0_1_Range(160.0f),	ConvertColorValueTo_0_1_Range(122.0f),	1.0f);
	dawn.StartPosition =	XMFLOAT3(150.0f, 45.0f, 30.0f);
	dawn.EndPosition =		XMFLOAT3(75.0f, 65.0f, 50.0f);
	dawn.DurationOfStage = timePerStage;
	dawn.LightIntensity = 0.7f;
	
	stagesOfDay.push_back(dawn);

	StageOfDayStruct morning;
	morning.AmbientColor =	XMFLOAT4(0.5f,	0.5f,	0.5f,	1.0f);
	morning.DirectionalLightColor = morning.AmbientColor;
	morning.SkysphereColor = XMFLOAT4(ConvertColorValueTo_0_1_Range(180.0f),	ConvertColorValueTo_0_1_Range(150.0f), ConvertColorValueTo_0_1_Range(180.0f), 1.0f);
	morning.StartPosition = XMFLOAT3(75.0f, 65.0f, 50.0f);
	morning.EndPosition =	XMFLOAT3(35.0f, 90.0f, 70.0f);
	morning.DurationOfStage = timePerStage;
	morning.LightIntensity = 0.85f;

	stagesOfDay.push_back(morning);

	StageOfDayStruct day;
	day.AmbientColor =		XMFLOAT4(0.7f,	0.7f,	0.7f,	1.0f);
	day.DirectionalLightColor = day.AmbientColor;
	day.SkysphereColor = XMFLOAT4(ConvertColorValueTo_0_1_Range(100.0f),	ConvertColorValueTo_0_1_Range(149.0f),	ConvertColorValueTo_0_1_Range(237.0f),	1.0f);
	day.StartPosition = XMFLOAT3(35.0f, 90.0f, 70.0f);
	day.EndPosition =	XMFLOAT3(-35.0f, 90.0f, 50.0f);
	day.DurationOfStage = timePerStage*3.0f; //Cuz fuck yeah, day!
	day.LightIntensity = 1.0f;

	stagesOfDay.push_back(day);

	StageOfDayStruct dusk;
	dusk.AmbientColor =		XMFLOAT4(0.5f,	0.5f,	0.5f,	1.0f);
	dusk.DirectionalLightColor = dusk.AmbientColor;
	dusk.SkysphereColor = XMFLOAT4(0.2f,	ConvertColorValueTo_0_1_Range(180.0f),	ConvertColorValueTo_0_1_Range(170.0f),	1.0f);
	dusk.StartPosition = XMFLOAT3(-35.0f, 90.0f, 50.0f);
	dusk.EndPosition =	XMFLOAT3(-75.0f, 65.0f, 30.0f);
	dusk.DurationOfStage = timePerStage;
	dusk.LightIntensity = 0.85f;

	stagesOfDay.push_back(dusk);

	StageOfDayStruct evening;
	evening.AmbientColor =	XMFLOAT4(0.3f,	0.3f,	0.3f,	1.0f);
	evening.DirectionalLightColor = evening.AmbientColor;
	evening.SkysphereColor = XMFLOAT4(0.15f,	0.23f,	0.25f,	1.0f);
	evening.StartPosition = XMFLOAT3(-75.0f, 65.0f, 50.0f);
	evening.EndPosition =	XMFLOAT3(-150.0f, 45.0f, 30.0f);
	evening.DurationOfStage = timePerStage;
	evening.LightIntensity = 0.7f;

	stagesOfDay.push_back(evening);

	StageOfDayStruct night;
	night.AmbientColor =	XMFLOAT4(0.15f,	0.15f,	0.25f,	1.0f);
	night.DirectionalLightColor = night.AmbientColor;
	night.SkysphereColor = night.AmbientColor;
	night.StartPosition =	XMFLOAT3(-150.0f, 75.0f, 30.0f);
	night.EndPosition =		XMFLOAT3(150.0f, 75.0f, 30.0f);
	night.DurationOfStage = timePerStage;
	night.LightIntensity = 0.45f;

	stagesOfDay.push_back(night);



	previousStageStruct = stagesOfDay[previousFrameStageOfDay];
	elapsedTime = 0.0f;
	timeOfDay = 0.0f;

	return true;
}

float DayNightCycle::Update( float elapsedTime, DirLight* directionalLight, Skysphere* skysphere )
{
	XMVECTOR startVector, endVector, currentVector;

	currentVector =  XMLoadFloat3(&directionalLight->Position); //Save current position in XMVECTOR format, needed when calculating sum of each frame's lerping

	this->elapsedTime += elapsedTime;
	float lerpAmountThisFrame = (elapsedTime / stagesOfDay[currentStageOfDay].DurationOfStage); //Calculate amount we'll be moving by this frame

	timeOfDay += lerpAmountThisFrame; //This is for controlling the skysphere lerping.

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
		timeOfDay =  0.0f;
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
	currentVector = XMVectorLerp(startVector, endVector, timeOfDay);
	XMStoreFloat3(&directionalLight->Position, currentVector);

	//Then we extract the start and end colors of this stage to lerp between
	startVector = XMLoadFloat4(&previousStageStruct.AmbientColor);
	endVector = XMLoadFloat4(&stagesOfDay[currentStageOfDay].AmbientColor);

	//Lerp between the two values and save the current value as our current ambience color
	currentVector = XMVectorLerp(startVector, endVector, timeOfDay);
	XMStoreFloat4(&currentAmbienceColor, currentVector);

	//Currently, all of our different colors are the same, so just apply them right here and now.
	directionalLight->Color = currentAmbienceColor;

	directionalLight->Intensity = lerp(previousStageStruct.LightIntensity, stagesOfDay[currentStageOfDay].LightIntensity, timeOfDay);

	previousFrameStageOfDay = currentStageOfDay;

	return timeOfDay;
}


/************************************************************************/
/*					             BACKUP                                 */
/************************************************************************/

/*
#include "DayNightCycle.h"

DayNightCycle::DayNightCycle()
{

}

DayNightCycle::DayNightCycle( const DayNightCycle& )
{

}

DayNightCycle::~DayNightCycle()
{
}

bool DayNightCycle::Initialize( float timePerStage, StageOfDay startStage )
{
	this->timePerStage = timePerStage;
	this->currentStageOfDay = startStage;
	previousFrameStageOfDay = (StageOfDay)(currentStageOfDay-1);

	//http://www.picturetopeople.org/color_converter.html
	//Or you could just use the normal [0,255] number and divide it by 255 to get the [0,1] counterpart.

	//http://www.foszor.com/blog/xna-color-chart/

	//LightSalmon = {R:255 G:160 B:122 A:255}
	//Cornflower Blue = {R:100 G:149 B:237 A:255}
	//Deep Sky Blue = {R:0 G:191 B:255 A:255}
	//Dark Turquoise = {R:0 G:206 B:209 A:255}

	//Initialize colors for the different stages of the day.
	directionalLightDayColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	directionalLightNightColor = XMFLOAT4(0.15f, 0.15f, 0.25f, 1.0f);

	skysphereDawnColor = XMFLOAT4(1.0f, ConvertColorValueTo_0_1_Range(160.0f), ConvertColorValueTo_0_1_Range(122.0f), 1.0f);
	skysphereDayColor = XMFLOAT4(ConvertColorValueTo_0_1_Range(100.0f), ConvertColorValueTo_0_1_Range(149.0f), ConvertColorValueTo_0_1_Range(237.0f), 1.0f);
	skysphereDuskColor = XMFLOAT4(0.0f, ConvertColorValueTo_0_1_Range(206.0f), ConvertColorValueTo_0_1_Range(209.0f), 1.0f);
	skysphereNightColor = XMFLOAT4(0.15f, 0.15f, 0.25f, 1.0f);


	stagesOfDay.reserve(8);

	StageOfDayStruct dawn;
	dawn.AmbientColor = skysphereDawnColor;
	dawn.DirectionalLightColor = skysphereDawnColor;
	dawn.SkysphereColor = skysphereDawnColor;
	//dawn.StartPosition;
	//dawn.EndPosition;
	//dawn.DurationOfStage;
	//dawn.LightIntensity;

	//Tiny optimization so that we won't have to resize vector every time we add a new variable
	startAndEndPositions.reserve(12); 

	//2 positions for the 6 different stages of the day. We store them as such: 0-5 previous positions, 6-11 future positions
	startAndEndPositions.push_back(XMFLOAT3(150.0f, 45.0f, 30.0f)); //start pos for dawn
	startAndEndPositions.push_back(XMFLOAT3(75.0f, 65.0f, 50.0f)); //end pos for dawn

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(75.0f, 65.0f, 70.0f)); //start pos for morning
	startAndEndPositions.push_back(XMFLOAT3(35.0f, 80.0f, 90.0f)); //end pos for morning

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(35.0f, 90.0f, 90.0f)); //start pos for day
	startAndEndPositions.push_back(XMFLOAT3(-35.0f, 90.0f, 90.0f)); //end pos for day

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(-35.0f, 80.0f, 90.0f)); //start pos for dusk
	startAndEndPositions.push_back(XMFLOAT3(-75.0f, 65.0f, 70.0f)); //end pos for dusk

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(-75.0f, 65.0f, 50.0f)); //start pos for evening
	startAndEndPositions.push_back(XMFLOAT3(-150.0f, 45.0f, 30.0f)); //end pos for evening

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(-150.0f, 120.0f, 0.0f)); //start pos for night
	startAndEndPositions.push_back(XMFLOAT3(150.0f, 120.0f, 0.0f)); //end pos for night

	//which ends and repeats; goto start pos for dawn. 

	elapsedTime = 0.0f;
	timeOfDay = 0.0f;


	return true;
}

float DayNightCycle::Update( float elapsedTime, DirLight* directionalLight, Skysphere* skysphere )
{
	XMVECTOR startPosition, endPosition, currentPosition;

	currentPosition =  XMLoadFloat3(&directionalLight->Position); //Save current position in XMVECTOR format, needed when calculating sum of each frame's lerping

	this->elapsedTime += elapsedTime;
	float lerpAmountThisFrame = (elapsedTime / timePerStage);//clamp( , 0.0f, 1.0f) //Calculate amount we'll be moving by this frame

	timeOfDay += lerpAmountThisFrame; //This is for controlling the skysphere lerping.

	//See if we should change stage of day
	if(this->elapsedTime > timePerStage)
	{
		//If this has happened, then we switch to next stage of day, because the time has passed
		if(currentStageOfDay == NIGHT) //If it was night, we've reached the end of the enum list, hence...
		{
			currentStageOfDay = DAWN; //We reset
		}
		else
		{
			currentStageOfDay = (StageOfDay)(currentStageOfDay+1);
		}
		
		this->elapsedTime = 0.0f; //Reset elapsed time as well
		timeOfDay =  0.0f;
	}

	//Extract start and end position of this stage of day so that we can lerp the directional light position 
	//between them as to create the illusion of the sun moving during the course of the day
	startPosition = XMLoadFloat3(&startAndEndPositions[currentStageOfDay*2]);
	endPosition = XMLoadFloat3(&startAndEndPositions[currentStageOfDay*2+1]);


	if(currentStageOfDay != previousFrameStageOfDay) //Tiny optimization
	{
		switch(currentStageOfDay)
		{
		case DAWN:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 0.7f;

			skysphere->SetCenterColor(skysphereDawnColor);
			skysphere->SetApexColor(skysphereNightColor);
			break;

		case MORNING:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 0.85f;

			skysphere->SetCenterColor(skysphereDayColor);
			skysphere->SetApexColor(skysphereDawnColor);
			break;

		case DAY:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 1.0f;

			skysphere->SetCenterColor(skysphereDayColor);
			skysphere->SetApexColor(skysphereDayColor);
			break;

		case DUSK:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 0.85f;

			skysphere->SetCenterColor(skysphereDuskColor);
			skysphere->SetApexColor(skysphereDayColor);
			break;

		case EVENING:
			directionalLight->Color = directionalLightNightColor;
			directionalLight->Intensity = 0.7f;

			skysphere->SetCenterColor(skysphereNightColor);
			skysphere->SetApexColor(skysphereDuskColor);
			break;

		case NIGHT:
			directionalLight->Color = directionalLightNightColor;
			directionalLight->Intensity = 0.55f;

			skysphere->SetCenterColor(skysphereNightColor);
			skysphere->SetApexColor(skysphereNightColor);
			break;
		}
	}

	currentPosition = XMVectorLerp(currentPosition, endPosition, lerpAmountThisFrame);
	XMStoreFloat3(&directionalLight->Position, currentPosition);

	previousFrameStageOfDay = currentStageOfDay;

	return timeOfDay;
}

*/