#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include <vector>
#include <atlcomcli.h>
#include "TextureAndMaterialStructs.h"

class TextureCreator
{
public:
	TextureCreator();
	~TextureCreator();

	HRESULT Build32Bit2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		const std::vector<PixelData>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV);

	HRESULT Load2DTextureArrayFromFiles(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArray, int texWidth, int texHeight);

	HRESULT Build8Bit2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		const std::vector<UINT8>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV );

	HRESULT Build16Bit2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		const std::vector<UINT16>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV );

	HRESULT Build1DMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		ShaderMaterialStruct materialData, int textureWidth, ID3D11Texture1D** texture);

	HRESULT Build1DMaterialTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<ShaderMaterialStruct> materials,
		int materialCount, int textureWidth, ID3D11ShaderResourceView** textureSRV );

private:
	CComPtr<ID3D11Texture2D> placeHolderTexture;
};

