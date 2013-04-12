#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include <vector>
#include "MCTerrainClass.h"
#include "structs.h"

class TextureAndMaterialHandler
{
public:
	TextureAndMaterialHandler();
	TextureAndMaterialHandler(const TextureAndMaterialHandler&);
	~TextureAndMaterialHandler();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	ID3D11ShaderResourceView** GetVegetationTextures();
	ID3D11ShaderResourceView** GetTerrainTextures();

private:
	HRESULT Build2DTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArray, int texWidth, int texHeight);

	HRESULT Build1DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

private:
	ID3D11ShaderResourceView* vegetationTextures;
	ID3D11ShaderResourceView* terrainTextures;

};