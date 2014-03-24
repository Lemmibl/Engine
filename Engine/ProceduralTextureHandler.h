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
private:
	//The different types of operations you can perform in a noise iteration
	enum OperationType
	{
		ADDITION = 0,
		SUBTRACTION,
		MULTIPLICATION,
		DIVISION
	};

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
		float startPosX, float startPosY, unsigned int textureWidth, unsigned int textureHeight, float noiseScale, ID3D11Texture2D** texture);

	HRESULT Create2DNormalMapFromHeightmap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** destTex, 
		unsigned int textureWidth, unsigned int textureHeight, ID3D11Texture2D** texture);

	HRESULT Create2DSSAORandomTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);

	HRESULT CreateTilingCloudTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv, unsigned int textureWidth, unsigned int textureHeight);

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

	void DoNoiseIterations(float startX, float startY, unsigned int textureWidth, unsigned int textureHeight, std::vector<PixelData>& pixelData, OperationType operationType, float noiseScale)
	{
		int i;
		i = 0;

		//TODO: Lambda functions instead of an ugly switch case. Function that recieves two uint8 references and performs and operation on them. add, sub, div, mul

		float radius = (textureWidth/2)-2.0f;

		//Get the right type of operation to perform
		auto operatorFunction = operatorFunctions[((int)operationType)];

		for(float x = startX; x < startX+textureWidth; x++)
		{
			for(float y = startY; y < startY+textureHeight; y++)
			{
				float xScale = x / textureWidth;
				float yScale = y / textureHeight;
				float xPi = xScale * 2 * XM_PI;
				float yPi = yScale * 2 * XM_PI;

				//Produce the four points we'll make noise from
				float nx = radius+sin(xPi);
				float ny = radius+cos(xPi);
				float nz = radius+sin(yPi);
				float nw = radius+cos(yPi);

				//Produce noise, rescale it from [-1, 1] to [0, 1] then multiply to [0, 256] to make full use of the 8bit channels of the texture it'll be stored in.
				int noiseResult = (int)((0.5f * noise->SimplexNoise4D(nx*noiseScale, ny*noiseScale, nz*noiseScale, nw*noiseScale) + 0.5f) * 256);

				operatorFunction(pixelData[i].x, noiseResult);
				operatorFunction(pixelData[i].y, noiseResult);
				operatorFunction(pixelData[i].z, noiseResult);
				operatorFunction(pixelData[i].w, noiseResult);

				i++;
			}
		}	
	}

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	TextureCreator* texCreator;

	std::shared_ptr<NoiseClass> noise;
	std::shared_ptr<Utility> utility;

	std::vector<std::function<void(UINT8& lhs, int& rhs)>> operatorFunctions;

	CComPtr<ID3D11Texture2D> placeHolderTexture;

	CComPtr<ID3D11ShaderResourceView> noiseSRV;
	CComPtr<ID3D11ShaderResourceView> ssaoRandomTextureSRV;
	CComPtr<ID3D11ShaderResourceView> windTextureSRV;
	CComPtr<ID3D11ShaderResourceView> windNormalMapSRV;
	CComPtr<ID3D11Texture2D> windNoiseTexture;
};

