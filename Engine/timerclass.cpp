///////////////////////////////////////////////////////////////////////////////
// Filename: timerclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "timerclass.h"


TimerClass::TimerClass()
{
}


TimerClass::TimerClass(const TimerClass& other)
{
}


TimerClass::~TimerClass()
{
}

bool TimerClass::Initialize()
{
	// Check to see if this system supports high performance timers.
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	if(frequency == 0)
	{
		return false;
	}

	// Find out how many times the frequency counter ticks every millisecond.
	ticksPerMs = (float)(frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	return true;
}

void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;


	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);

	timeDifference = (float)(currentTime - startTime);

	frameTime = timeDifference / ticksPerMs;

	startTime = currentTime;

	return;
}

float TimerClass::GetTime()
{
	return frameTime;
}