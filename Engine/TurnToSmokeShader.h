#pragma once
#include <windows.h>
#include <xnamath.h>

class TurnToSmokeShader
{
private:
	struct GeomshaderStruct
	{
		XMFLOAT3 WindDirection;
		float deltaTime;
		float lifeTime;
		float windSpeed;
	};

public:
	TurnToSmokeShader(void);
	~TurnToSmokeShader(void);
};

