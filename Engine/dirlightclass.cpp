////////////////////////////////////////////////////////////////////////////////
// Filename: DirLightClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DirLightClass.h"


DirLightClass::DirLightClass()
{
}


DirLightClass::DirLightClass(const DirLightClass& other)
{
}


DirLightClass::~DirLightClass()
{
}


void DirLightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void DirLightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void DirLightClass::SetDirection(float x, float y, float z)
{
	direction = D3DXVECTOR3(x, y, z);
	return;
}

void DirLightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void DirLightClass::SetSpecularPower(float power)
{
	specularPower = power;
	return;
}

D3DXVECTOR4 DirLightClass::GetAmbientColor()
{
	return ambientColor;
}

D3DXVECTOR4 DirLightClass::GetDiffuseColor()
{
	return diffuseColor;
}

D3DXVECTOR3 DirLightClass::GetDirection()
{
	return direction;
}

D3DXVECTOR4 DirLightClass::GetSpecularColor()
{
	return specularColor;
}

float DirLightClass::GetSpecularPower()
{
	return specularPower;
}