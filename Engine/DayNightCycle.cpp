#include "DayNightCycle.h"

DayNightCycle::DayNightCycle()
{

}

DayNightCycle::DayNightCycle( const DayNightCycle& )
{

}

DayNightCycle::~DayNightCycle()
{
	if(startAndEndPositions)
	{
		delete [] startAndEndPositions;
		startAndEndPositions = 0;
	}
}

bool DayNightCycle::Initialize( float timePerStage, StageOfDay startStage )
{
	this->timePerStage = timePerStage;
	this->currentStageOfDay = startStage;
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
	directionalLightNightColor = XMFLOAT4(0.2f, 0.2f, 0.3f, 1.0f);

	skysphereDawnColor = XMFLOAT4(1.0f, GetColorValueFromRGB(160.0f), GetColorValueFromRGB(122.0f), 1.0f);
	skysphereDayColor = XMFLOAT4(GetColorValueFromRGB(100.0f), GetColorValueFromRGB(149.0f), GetColorValueFromRGB(237.0f), 1.0f);
	skysphereDuskColor = XMFLOAT4(0.0f, GetColorValueFromRGB(206.0f), GetColorValueFromRGB(209.0f), 1.0f);
	skysphereNightColor = XMFLOAT4(0.05f, 0.05f, 0.1f, 1.0f);

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

	//2 positions for the 6 different stages of the day. We store them as such: 0-5 previous positions, 6-11 future positions
	startAndEndPositions = new XMFLOAT3[12];

	startAndEndPositions[0] = XMFLOAT3(150.0f, 0.0f, 0.0f); //start pos for dawn
	startAndEndPositions[1] = XMFLOAT3(75.0f, 70.0f, 35.0f); //end pos for dawn

	//which leads into...

	startAndEndPositions[2] = XMFLOAT3(75.0f, 70.0f, 35.0f); //start pos for morning
	startAndEndPositions[3] = XMFLOAT3(35.0f, 90.0f, 70.0f); //end pos for morning

	//which leads into...

	startAndEndPositions[4] = XMFLOAT3(35.0f, 120.0f, 70.0f); //start pos for day
	startAndEndPositions[5] = XMFLOAT3(-35.0f, 120.0f, 70.0f); //end pos for day

	//which leads into...

	startAndEndPositions[6] = XMFLOAT3(-35.0f, 90.0f, 70.0f); //start pos for dusk
	startAndEndPositions[7] = XMFLOAT3(-75.0f, 70.0f, 35.0f); //end pos for dusk

	//which leads into...

	startAndEndPositions[8] = XMFLOAT3(-75.0f, 70.0f, 35.0f); //start pos for evening
	startAndEndPositions[9] = XMFLOAT3(-150.0f, 35.0f, 0.0f); //end pos for evening

	//which leads into...

	startAndEndPositions[10] = XMFLOAT3(0.0f, 120.0f, 0.0f); //start pos for night
	startAndEndPositions[11] = XMFLOAT3(0.0f, 120.0f, 0.0f); //end pos for night

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

	//Extract start and end position of this stage of day so that we can lerp between them as to
	//create the illusion of the sun moving during the course of the day
	startPosition = XMLoadFloat3(&startAndEndPositions[currentStageOfDay*2]);
	endPosition = XMLoadFloat3(&startAndEndPositions[currentStageOfDay*2+1]);

	/* 
			//TODO?:
			//Possibly change enums to a struct containing: enum, duration in float, skysphere color 
			//and directional light color (maybe ambient color?)

			http://alvyray.com/Memos/CG/Pixar/spline77.pdf
	*/

	switch(currentStageOfDay)
	{
		case DAWN:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 128.0f;

			skysphere->SetCenterColor(skysphereDawnColor);
			skysphere->SetApexColor(skysphereNightColor);

			currentPosition = XMVectorLerp(currentPosition, endPosition, lerpAmountThisFrame);

			if(shouldLightMove >= 5.0f)
			{
				XMStoreFloat3(&directionalLight->Position, currentPosition);
				shouldLightMove = 0.0f;
			}
			break;

		case MORNING:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 64.0f;

			skysphere->SetCenterColor(skysphereDayColor);
			skysphere->SetApexColor(skysphereDawnColor);

			currentPosition = XMVectorLerp(currentPosition, endPosition, lerpAmountThisFrame);

			if(shouldLightMove >= 5.0f)
			{
				XMStoreFloat3(&directionalLight->Position, currentPosition);
				shouldLightMove = 0.0f;
			}
			break;

		case DAY:
			directionalLight->Color = directionalLightDayColor;
				directionalLight->Intensity = 32.0f;

			skysphere->SetCenterColor(skysphereDayColor);
			skysphere->SetApexColor(skysphereDayColor);

			currentPosition = XMVectorLerp(currentPosition, endPosition, lerpAmountThisFrame);

			if(shouldLightMove >= 5.0f)
			{
				XMStoreFloat3(&directionalLight->Position, currentPosition);
				shouldLightMove = 0.0f;
			}
			break;

		case DUSK:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 64.0f;

			skysphere->SetCenterColor(skysphereDuskColor);
			skysphere->SetApexColor(skysphereDayColor);

			currentPosition = XMVectorLerp(currentPosition, endPosition, lerpAmountThisFrame);

			if(shouldLightMove >= 5.0f)
			{
				XMStoreFloat3(&directionalLight->Position, currentPosition);
				shouldLightMove = 0.0f;
			}
			break;

		case EVENING:
			directionalLight->Color = directionalLightNightColor;
			directionalLight->Intensity = 128.0f;

			skysphere->SetCenterColor(skysphereNightColor);
			skysphere->SetApexColor(skysphereDuskColor);

			currentPosition = XMVectorLerp(currentPosition, endPosition, lerpAmountThisFrame);

			if(shouldLightMove >= 5.0f)
			{
				XMStoreFloat3(&directionalLight->Position, currentPosition);
				shouldLightMove = 0.0f;
			}
			break;

		case NIGHT:
			directionalLight->Color = directionalLightNightColor;
			directionalLight->Intensity = 256.0f;

			skysphere->SetCenterColor(skysphereNightColor);
			skysphere->SetApexColor(skysphereNightColor);

			currentPosition = XMVectorLerp(currentPosition, endPosition, lerpAmountThisFrame);

			if(shouldLightMove >= 5.0f)
			{
				XMStoreFloat3(&directionalLight->Position, currentPosition);
				shouldLightMove = 0.0f;
			}
			break;
	}

	return timeOfDay;
}
