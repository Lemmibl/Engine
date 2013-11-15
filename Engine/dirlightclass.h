////////////////////////////////////////////////////////////////////////////////
// Filename: dirlightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DIRLIGHTCLASS_H_
#define _DIRLIGHTCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: DirLightClass
////////////////////////////////////////////////////////////////////////////////
class DirLightClass
{
public:
	DirLightClass();
	DirLightClass(const DirLightClass&);
	~DirLightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();


private:
	D3DXVECTOR4 ambientColor;
	D3DXVECTOR4 diffuseColor;
	D3DXVECTOR3 direction;
	D3DXVECTOR4 specularColor;
	float specularPower;
};

#endif