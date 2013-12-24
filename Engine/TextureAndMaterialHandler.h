#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include <vector>
#include "MCTerrainClass.h"
#include "Utility.h"
#include "StructsAndEnums.h"
#include <random>
#include <atlbase.h> // Contains the declaration of CComPtr.

class TextureAndMaterialHandler
{
private:
	//We save the pixels in R8G8B8A8. This means that each pixel can have values 1-255.
	struct PixelData
	{
		UINT8 x, y, z, w;
	};

	struct MaterialStruct
	{
		float Kambience;
		float Kdiffuse;
		float Kspecular;
		float smoothness;
		float shouldBeShadowed; //Everything should be 1 except for grassquads, which should be 0
	};

	struct MaterialColorSpectrumUINT8
	{
		UINT8 RedMin, RedMax;
		UINT8 GreenMin, GreenMax;
		UINT8 BlueMin, BlueMax;
	};

public:
	TextureAndMaterialHandler();
	~TextureAndMaterialHandler();


public:
	ID3D11ShaderResourceView** GetVegetationTextureArray()	{ return &vegetationTextureArraySRV.p;	};
	ID3D11ShaderResourceView** GetTerrainTextureArray()		{ return &terrainTextureArraySRV.p;		};
	ID3D11ShaderResourceView** GetMaterialTextureArray()	{ return &materialTextureArraySRV.p;	};
	ID3D11ShaderResourceView** GetMaterialLookupTexture()	{ return &materialLookupTableSRV.p;		};
	ID3D11ShaderResourceView** GetTerrainTexture()			{ return &terrainTextureSRV.p;			};
	ID3D11ShaderResourceView** GetSSAORandomTexture()		{ return &ssaoRandomTextureSRV.p;		};
	ID3D11ShaderResourceView** GetNoiseTexture()			{ return &noiseSRV.p;					};

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, NoiseClass* noise, Utility* utility);

	//To access the texture, call GetSSAORandomTexture()
	void RebuildSSAOTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	//To access the texture, call GetNoiseTexture()
	void RebuildSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void RebuildSeamlessSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float startPosX, float startPosY, float stepsX, float stepsY);

	//To access the texture, call GetNoiseTexture()
	void RebuildRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	//To access the texture, call GetNoiseTexture()
	void RebuildMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void RebuildTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TextureID textureID, int width, int height, float startPosX, float startPosY, bool reseedRandomAfterwards);

	HRESULT CreateSeamlessSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv, 
											float startPosX, float startPosY, float stepsX, float stepsY, float noiseScale);

	HRESULT Create2DNormalMapFromHeightmap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** destTex, float textureWidth, float textureHeight);

	bool SaveLTreeTextureToFile(ID3D11DeviceContext* deviceContext, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName);
	void SaveTextureToFile(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName);

private:

	HRESULT Create2DSSAORandomTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);


	void CreateMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, ID3D11ShaderResourceView** textureSRV, 
		MaterialColorSpectrumUINT8 colorSpectrum, float startPosX, float startPosY);

	void CreateMaterialLookupTable(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureSRV, int worldMaxYValue);

	HRESULT Build1DMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		MaterialStruct materialData, int textureWidth, ID3D11Texture1D** texture);

	HRESULT Build1DMaterialTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<MaterialStruct> materials,
		int materialCount, int textureWidth, ID3D11ShaderResourceView** textureSRV );

	HRESULT Build2DTextureProgrammatically( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		const std::vector<PixelData>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV );

	HRESULT Build8Bit2DTextureProgrammatically( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		const std::vector<UINT8>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV );

	HRESULT Build2DSSAORandomTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		const std::vector<UINT16>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV );

	HRESULT Build2DTextureArrayFromFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArray, int texWidth, int texHeight);

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
	std::shared_ptr<NoiseClass> noise;
	std::shared_ptr<Utility> utility;
	
	CComPtr<ID3D11ShaderResourceView> vegetationTextureArraySRV;
	CComPtr<ID3D11ShaderResourceView> terrainTextureArraySRV;
	CComPtr<ID3D11ShaderResourceView> materialTextureArraySRV;
	CComPtr<ID3D11ShaderResourceView> materialLookupTableSRV;
	CComPtr<ID3D11ShaderResourceView> terrainTextureSRV;
	CComPtr<ID3D11ShaderResourceView> noiseSRV;
	CComPtr<ID3D11ShaderResourceView> ssaoRandomTextureSRV;

	CComPtr<ID3D11Texture2D> placeHolderTexture;
	CComPtr<ID3D11Texture2D> windNoiseTexture;

	MaterialColorSpectrumUINT8 dirtColor;
	MaterialColorSpectrumUINT8 grassColor;

};