#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <random>
#include <xnamath.h>
#include "DODContainer.h"
#include "TextureCreator.h"
#include "TextureAndMaterialStructs.h"
#include "NoiseClass.h"

class MaterialHandler
{
public:
	MaterialHandler();
	~MaterialHandler();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TextureCreator* proceduralTextureHandler, NoiseClass* noise);

	ID3D11ShaderResourceView** GetMaterialTextureArray()	{ return &materialTextureArraySRV.p;	};
	ID3D11ShaderResourceView** GetMaterialLookupTexture()	{ return &materialLookupTableSRV.p;		};

	unsigned short AddOBJMaterial(OBJMaterialStruct mat);
	OBJMaterialStruct& GetMaterial(unsigned short handle) { return modelMaterials[handle]; }

	ID3D11ShaderResourceView** GetTerrainMaterialTexture()			{ return &terrainMaterialTextureSRV.p;			};

private:
	bool CheckForDuplicateMaterials(unsigned int hash, unsigned short& outHandle);
	unsigned int GenerateMaterialHash(OBJMaterialStruct& material);

	void CreateMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, ID3D11ShaderResourceView** textureSRV, 
		MaterialColorSpectrumUINT8 colorSpectrum, float startPosX, float startPosY);

	void CreateMaterialLookupTable(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureSRV, int worldMaxYValue);

	//Produce random number within defined range
	inline UINT8 RandRange(UINT8 min, UINT8 max)
	{
		//Adding +1 to prevent division by zero (modulus by zero)
		return (rand()%((max+1)-min))+min;
	};

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	TextureCreator* texCreator;
	NoiseClass* noise;

	MaterialColorSpectrumUINT8 dirtColor;
	MaterialColorSpectrumUINT8 grassColor;

	CComPtr<ID3D11ShaderResourceView> terrainMaterialTextureSRV;
	CComPtr<ID3D11ShaderResourceView> materialTextureArraySRV;
	CComPtr<ID3D11ShaderResourceView> materialLookupTableSRV;
	DODContainer<OBJMaterialStruct> modelMaterials;

	//A hash to see if material is identical
	std::vector<std::pair<unsigned int, unsigned short>> materialHashAndKeyPairings;
};

