#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include <vector>
#include "TerrainNoiseSeeder.h"
#include "Utility.h"
#include "StructsAndEnums.h"
#include <random>
#include <atlbase.h> // Contains the declaration of CComPtr.
#include "DODContainer.h"

#include "MaterialHandler.h"
#include "TextureCreator.h"
#include "ProceduralTextureHandler.h"

class TextureHandler
{
private:
	//We save the pixels in R8G8B8A8. This means that each pixel can have values 1-255.
	struct PixelData
	{
		UINT8 x, y, z, w;
	};

	//Used for my failed procedurally generated texture prototype
	struct MaterialColorSpectrumUINT8
	{
		UINT8 RedMin, RedMax;
		UINT8 GreenMin, GreenMax;
		UINT8 BlueMin, BlueMax;
	};

public:
	TextureHandler();
	~TextureHandler();

public:
	unsigned short AddNewtexture(ID3D11Device* device, std::wstring textureFilepath);
	ID3D11ShaderResourceView** GetTexture(unsigned short handle) { return &modelTextures[handle].p; }

	bool Load2DTextureFromFile(ID3D11Device* device, std::wstring textureFilepath, ID3D11ShaderResourceView** srv);
	bool Load2DCubemapTextureFromFile(ID3D11Device* device, std::wstring textureFilepath, ID3D11ShaderResourceView** srv);

	bool GetTextureHandle(std::wstring textureFilepath, unsigned short& handle);

	ID3D11ShaderResourceView** GetVegetationTextureArray()	{ return &vegetationTextureArraySRV.p;	};
	ID3D11ShaderResourceView** GetTerrainTextureArray()		{ return &terrainTextureArraySRV.p;		};

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void SaveTextureToFile(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName);

private:
	bool CheckForDuplicateTextures(std::wstring filepath, unsigned short& outHandle);


private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	MaterialHandler matHandler;
	TextureCreator texCreator;
	ProceduralTextureHandler proceduralTexHandler;

	CComPtr<ID3D11ShaderResourceView> terrainTextureArraySRV;
	CComPtr<ID3D11ShaderResourceView> vegetationTextureArraySRV;

	DODContainer<CComPtr<ID3D11ShaderResourceView>> modelTextures;

	//These pairings are used to see if we're trying to load a texture that has already been loaded! For more information, check out CheckForDuplicateTextures()
	std::vector<std::pair<std::wstring, unsigned short>> textureNameAndKeyPairings;
};