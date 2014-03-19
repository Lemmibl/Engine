#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <random>
#include <atlbase.h> // Contains the declaration of CComPtr.
#include "DODContainer.h"
#include "StructsAndEnums.h"
#include "NoiseClass.h"
#include "Utility.h"
#include "TextureCreator.h"

class ProceduralTextureHandler
{
public:
	ProceduralTextureHandler();
	~ProceduralTextureHandler();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TextureCreator* texCreator, NoiseClass* noise, Utility* utility);

	ID3D11ShaderResourceView** GetSSAORandomTexture()		{ return &ssaoRandomTextureSRV.p;		};
	ID3D11ShaderResourceView** GetNoiseTexture()			{ return &noiseSRV.p;					};

	ID3D11ShaderResourceView** GetWindTexture()				{ return &windTextureSRV.p;				};
	ID3D11ShaderResourceView** GetWindNormalMap()			{ return &windNormalMapSRV.p;			};


	void SetupWindtextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float startPosX, float startPosY, 
		unsigned int textureWidth, unsigned int textureHeight, float noiseScale);

	HRESULT CreateSeamlessSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv, 
		float startPosX, float startPosY, unsigned int textureWidth, unsigned int textureHeight, float noiseScale);

	HRESULT Create2DNormalMapFromHeightmap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** destTex, unsigned int textureWidth, unsigned int textureHeight);

	HRESULT Create2DSSAORandomTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);

private:
	//Produce random number within defined range
	inline UINT8 RandRange(UINT8 min, UINT8 max)
	{
		//Adding +1 to prevent division by zero (modulus by zero)
		return (rand()%((max+1)-min))+min;
	};

	inline void MovePointAroundInACircle(float* x, float* y, float originX, float originY, float radius, float scale)
	{
		for (float t = 0; t < 2*XM_PI; t += scale) 
		{
			*x = radius*cos(t) + originX;
			*y = radius*sin(t) + originY;
		}
	}


private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	TextureCreator* texCreator;

	std::shared_ptr<NoiseClass> noise;
	std::shared_ptr<Utility> utility;

	CComPtr<ID3D11Texture2D> placeHolderTexture;

	CComPtr<ID3D11ShaderResourceView> noiseSRV;
	CComPtr<ID3D11ShaderResourceView> ssaoRandomTextureSRV;
	CComPtr<ID3D11ShaderResourceView> windTextureSRV;
	CComPtr<ID3D11ShaderResourceView> windNormalMapSRV;
	CComPtr<ID3D11Texture2D> windNoiseTexture;
};

