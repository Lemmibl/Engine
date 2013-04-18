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
	bool SaveLTreeTextureToFile(ID3D11DeviceContext* deviceContext, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName);

	ID3D11ShaderResourceView** GetVegetationTextureArray();
	ID3D11ShaderResourceView** GetTerrainTextureArray();
	ID3D11ShaderResourceView** GetMaterialTextureArray();

	HRESULT Build2DTextureProgrammatically( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		PixelData* pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV );

private:
	HRESULT Build1DMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		MaterialStruct materialData, int textureWidth, ID3D11Texture1D**);//, ID3D11ShaderResourceView* textureSRV

	HRESULT Build1DMaterialTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, vector<MaterialStruct> materials,
		int materialCount, int textureWidth, ID3D11ShaderResourceView** textureSRV );

	HRESULT Build2DTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArray, int texWidth, int texHeight);

private:
	ID3D11ShaderResourceView* vegetationTextures;
	ID3D11ShaderResourceView* terrainTextures;
	ID3D11ShaderResourceView* materialTextures;

	ID3D11Texture2D* lTreeTexture;
};