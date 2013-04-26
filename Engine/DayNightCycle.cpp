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
	shouldLightMove = 0;

	//http://www.picturetopeople.org/color_converter.html
	//Or you could just use the normal [0,255] number and divide it by 255 to get the [0,1] counterpart.

	//http://www.foszor.com/blog/xna-color-chart/

	//LightSalmon = {R:255 G:160 B:122 A:255}
	//Cornflower Blue = {R:100 G:149 B:237 A:255}
	//Deep Sky Blue = {R:0 G:191 B:255 A:255}
	//Dark Turquoise = {R:0 G:206 B:209 A:255}

	//Initialize colors for the different stages of the day.
	directionalLightDayColor = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	directionalLightNightColor = XMFLOAT4(0.4f, 0.4f, 0.5f, 1.0f);

	skysphereDawnColor = XMFLOAT4(1.0f, GetColorValueFromRGB(160.0f), GetColorValueFromRGB(122.0f), 1.0f);
	skysphereDayColor = XMFLOAT4(GetColorValueFromRGB(100.0f), GetColorValueFromRGB(149.0f), GetColorValueFromRGB(237.0f), 1.0f);
	skysphereDuskColor = XMFLOAT4(0.0f, GetColorValueFromRGB(206.0f), GetColorValueFromRGB(209.0f), 1.0f);
	skysphereNightColor = XMFLOAT4(0.15f, 0.15f, 0.25f, 1.0f);

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

	//Tiny optimization so that we won't have to resize vector every time we add a new variable
	startAndEndPositions.reserve(12); 

	//2 positions for the 6 different stages of the day. We store them as such: 0-5 previous positions, 6-11 future positions
	startAndEndPositions.push_back(XMFLOAT3(150.0f, 0.0f, 0.0f)); //start pos for dawn
	startAndEndPositions.push_back(XMFLOAT3(75.0f, 35.0f, 35.0f)); //end pos for dawn

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(75.0f, 35.0f, 35.0f)); //start pos for morning
	startAndEndPositions.push_back(XMFLOAT3(35.0f, 70.0f, 70.0f)); //end pos for morning

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(35.0f, 70.0f, 70.0f)); //start pos for day
	startAndEndPositions.push_back(XMFLOAT3(-35.0f, 70.0f, 70.0f)); //end pos for day

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(-35.0f, 70.0f, 70.0f)); //start pos for dusk
	startAndEndPositions.push_back(XMFLOAT3(-75.0f, 35.0f, 35.0f)); //end pos for dusk

	//which leads into...

	startAndEndPositions.push_back(XMFLOAT3(-75.0f, 35.0f, 35.0f)); //start pos for evening
	startAndEndPositions.push_back(XMFLOAT3(-150.0f, 0.0f, 0.0f)); //end pos for evening

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
	shouldLightMove += elapsedTime;
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

	/* 
			//TODO?:
			//Possibly change enums to a struct containing: enum, duration in float, skysphere color 
			//and directional light color (maybe ambient color?)

			http://alvyray.com/Memos/CG/Pixar/spline77.pdf
	*/

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
			directionalLight->Intensity = 1.0f;

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
