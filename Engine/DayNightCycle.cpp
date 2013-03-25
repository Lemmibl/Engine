#include "DayNightCycle.h"

DayNightCycle::DayNightCycle()
{

}

DayNightCycle::DayNightCycle( const DayNightCycle& )
{

}

DayNightCycle::~DayNightCycle()
{
	delete [] startAndEndPositions;
	startAndEndPositions = 0;
}

bool DayNightCycle::Initialize( float timePerStage, StageOfDay startStage )
{
	this->timePerStage = timePerStage;
	this->currentStageOfDay = startStage;

	//http://www.picturetopeople.org/color_converter.html
	//Or you could just use the normal [0,255] number and divide it by 255 to get the [0,1] counterpart.

	//http://www.foszor.com/blog/xna-color-chart/

	//LightSalmon = {R:255 G:160 B:122 A:255}
	//Cornflower Blue = {R:100 G:149 B:237 A:255}
	//Deep Sky Blue = {R:0 G:191 B:255 A:255}
	//Dark Turquoise = {R:0 G:206 B:209 A:255}

	directionalLightDayColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	directionalLightNightColor = XMFLOAT4(0.2f, 0.2f, 0.3f, 1.0f);

	skysphereDawnColor = XMFLOAT4(1.0f, GetColorValueFromRGB(160.0f), GetColorValueFromRGB(122.0f), 1.0f);
	skysphereDayColor = XMFLOAT4(GetColorValueFromRGB(100.0f), GetColorValueFromRGB(149.0f), GetColorValueFromRGB(237.0f), 1.0f);
	skysphereDuskColor = XMFLOAT4(0.0f, GetColorValueFromRGB(206.0f), GetColorValueFromRGB(209.0f), 1.0f);
	skysphereNightColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);


	startAndEndPositions = new XMFLOAT3[8]; //2 positions for the 4 different stages of the day. We store them as such: 0-3 previous positions, 4-7 future positions

	startAndEndPositions[0] = XMFLOAT3(70.0f, 0.0f, 0.0f); //start pos for dawn
	startAndEndPositions[4] = XMFLOAT3(0.0f, 65.0f, 1.0f); //end pos for dawn

	//which leads into...

	startAndEndPositions[1] = XMFLOAT3(0.0f, 65.0f, 1.0f); //start pos for day
	startAndEndPositions[5] = XMFLOAT3(0.0f, 65.0f, 1.0f); //end pos for day

	//which leads into...

	startAndEndPositions[2] = XMFLOAT3(0.0f, 65.0f, 1.0f); //start pos for dusk
	startAndEndPositions[6] = XMFLOAT3(-70.0f, 0.0f, 0.0f); //end pos for dusk

	//which leads into...

	startAndEndPositions[3] = XMFLOAT3(70.0f, 65.0f, 0.0f); //start pos for night
	startAndEndPositions[7] = XMFLOAT3(-70.0f, 65.0f, 0.0f); //end pos for night

	//repeat: goto start pos for dawn.

	elapsedTime = 0.0f;
	timeOfDay = 0.0f;


	return true;
}

float DayNightCycle::Update( float elapsedTime, DirLight* directionalLight, Skysphere* skysphere )
{
	this->elapsedTime += elapsedTime;
	float lerpAmountThisFrame = clamp((elapsedTime / timePerStage), 0.0f, 1.0f);
	float timeLerpAmountThisFrame = clamp((elapsedTime / 350.0f), 0.0f, 1.0f);

	//	float amount = MathHelper.Clamp(elapsedTime / Duration, 0, 1);
	//	byte value = (byte) MathHelper.Lerp(255, 0, amount);

	timeOfDay += lerp(0.0f, 350.0f, timeLerpAmountThisFrame);

	XMVECTOR oldPosition, futureposition, currentPos, resultPos;

	currentPos =  XMLoadFloat3(&directionalLight->Position);

	//We do approximate comparison because float x float comparisons could get nasty.
	if((int)this->elapsedTime >= timePerStage)
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

	oldPosition = XMLoadFloat3(&startAndEndPositions[currentStageOfDay]);
	futureposition = XMLoadFloat3(&startAndEndPositions[currentStageOfDay+4]);

	/* 
	TODO: .... add dawn -> day and dusk -> night transitions
		
			Possibly change enums to a struct containing enum, duration in float, skysphere color and directional light color (maybe ambient color?)

			http://alvyray.com/Memos/CG/Pixar/spline77.pdf
	*/

	switch(currentStageOfDay)
	{
		case DAWN:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 128.0f;

			skysphere->SetCenterColor(skysphereDawnColor);
			skysphere->SetApexColor(skysphereNightColor);

			resultPos = XMVectorLerp(currentPos, futureposition, lerpAmountThisFrame);
			XMStoreFloat3(&directionalLight->Position, resultPos);
			break;

		case DAY:
			directionalLight->Color = directionalLightDayColor;
				directionalLight->Intensity = 256.0f;

			skysphere->SetCenterColor(skysphereDayColor);
			skysphere->SetApexColor(skysphereDayColor);

			//We don't bother lerping here because sun shouldn't be moving
			break;

		case DUSK:
			directionalLight->Color = directionalLightDayColor;
			directionalLight->Intensity = 128.0f;

			skysphere->SetCenterColor(skysphereDuskColor);
			skysphere->SetApexColor(skysphereDayColor);

			resultPos = XMVectorLerp(currentPos, futureposition, lerpAmountThisFrame);
			XMStoreFloat3(&directionalLight->Position, resultPos);
			break;

		case NIGHT:
			directionalLight->Color = directionalLightNightColor;
			directionalLight->Intensity = 64.0f;

			skysphere->SetCenterColor(skysphereNightColor);
			skysphere->SetApexColor(skysphereNightColor);

			resultPos = XMVectorLerp(currentPos, futureposition, lerpAmountThisFrame);
			XMStoreFloat3(&directionalLight->Position, resultPos);
			break;
	}

	return timeOfDay;
}