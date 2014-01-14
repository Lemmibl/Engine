#pragma once
#include <windows.h>
#include <xnamath.h>
#include <random>

class WeatherSystem
{
public:
	WeatherSystem();
	~WeatherSystem();

	void Update(float deltaTime);

	XMFLOAT3* GetWindDirection() { return &windDirection; }

private:
	inline float JustALerpFunction(float a, float b, float t)
	{
		return ((1.0f-t)*a + t*b);
	}

	inline void NormalizeFloat3(XMFLOAT3* vec) 
	{  
		float length = sqrt((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));

		vec->x = vec->x/length;
		vec->y = vec->y/length;
		vec->z = vec->z/length;
	};

	inline float RandomFloat(float a, float b) 
	{
		const float random = ((float) rand()) / (float) RAND_MAX;
		const float diff = b - a;
		const float r = random * diff;
		return a + r;
	}

private:
	XMFLOAT3 windDirection, newWindDirection;
	float updateThreshold, elapsedTime, changeAmplitude, lerpAmountThisUpdate;
};

