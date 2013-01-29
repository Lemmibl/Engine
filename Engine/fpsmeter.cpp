///////////////////////////////////////////////////////////////////////////////
// Filename: fpsclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fpsmeter.h"


FpsMeter::FpsMeter()
{
}


FpsMeter::FpsMeter(const FpsMeter& other)
{
}


FpsMeter::~FpsMeter()
{
}

void FpsMeter::Initialize()
{
	fps = 0;
	count = 0;
	startTime = timeGetTime();
	return;
}

void FpsMeter::Frame()
{
	count++;

	if(timeGetTime() >= (startTime + 1000))
	{
		fps = count;
		count = 0;

		startTime = timeGetTime();
	}
}

int FpsMeter::GetFps()
{
	return fps;
}