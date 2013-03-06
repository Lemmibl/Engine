///////////////////////////////////////////////////////////////////////////////
// Filename: cpuclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "cpumeter.h"


CpuMeter::CpuMeter()
{
}


CpuMeter::CpuMeter(const CpuMeter& other)
{
}


CpuMeter::~CpuMeter()
{
}

void CpuMeter::Initialize()
{
	PDH_STATUS status;


	// Initialize the flag indicating whether this object can read the system cpu usage or not.
	canReadCpu = true;

	// Create a query object to poll cpu usage.
	status = PdhOpenQuery(NULL, 0, &queryHandle);
	if(status != ERROR_SUCCESS)
	{
		canReadCpu = false;
	}

	// Set query object to poll all cpus in the system.
	status = PdhAddCounter(queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &counterHandle);
	if(status != ERROR_SUCCESS)
	{
		canReadCpu = false;
	}

	lastSampleTime = GetTickCount(); 

	cpuUsage = 0;

	return;
}

void CpuMeter::Shutdown()
{
	if(canReadCpu)
	{
		PdhCloseQuery(queryHandle);
	}

	return;
}

void CpuMeter::Update()
{
	PDH_FMT_COUNTERVALUE value; 

	if(canReadCpu)
	{
		if((lastSampleTime + 1000) < GetTickCount())
		{
			lastSampleTime = GetTickCount(); 

			PdhCollectQueryData(queryHandle);

			PdhGetFormattedCounterValue(counterHandle, PDH_FMT_LONG, NULL, &value);

			cpuUsage = value.longValue;
		}
	}

	return;
}

int CpuMeter::GetCpuPercentage()
{
	int usage;

	if(canReadCpu)
	{
		usage = (int)cpuUsage;
	}
	else
	{
		usage = 0;
	}

	return usage;
}