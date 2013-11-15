#pragma once
#include <windows.h>
#include <xnamath.h>

//Pretty much just generic entity that all other types inherit from
class LemmiEntity
{
public:
	LemmiEntity(XMFLOAT3 position);
	~LemmiEntity();

	XMFLOAT3 position;
};

