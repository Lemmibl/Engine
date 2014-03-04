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

class TextureAndMaterialHandler
{
public:
	//TODO: xmfloat3 ambience, diffuse, specular ?
	struct MaterialStruct
	{
		float Kambience;
		float Kdiffuse;
		float Kspecular;
		float smoothness;
		float shouldBeShadowed; //1 to allow shadowing, 0 to disable shadowing
	};

	struct OBJMaterialStruct
	{
		//Just an identifier to help in assigning the right material to the right mesh when loading obj files.
		WCHAR name[MAX_PATH];

		XMFLOAT3 Ambient;
		XMFLOAT3 Diffuse;
		XMFLOAT3 Specular;

		float smoothness, alpha;
		bool transparent;

		//Used for cross referencing and linking the gbuffer stage and post processing stage together 
		// by saving the material ID in a render target during gbuffer stage then reading it during postprocess
		unsigned short materialID;
	};

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
	TextureAndMaterialHandler();
	~TextureAndMaterialHandler();


public:
	unsigned short AddOBJMaterial(OBJMaterialStruct mat);
	OBJMaterialStruct& GetMaterial(unsigned short handle) { return modelMaterials[handle]; }

	unsigned short AddNewtexture(ID3D11Device* device, std::wstring textureFilepath);
	ID3D11ShaderResourceView** GetTexture(unsigned short handle) { return &modelTextures[handle].p; }

	bool GetTextureHandle(std::wstring textureFilepath, unsigned short& handle);


	ID3D11ShaderResourceView** GetVegetationTextureArray()	{ return &vegetationTextureArraySRV.p;	};
	ID3D11ShaderResourceView** GetTerrainTextureArray()		{ return &terrainTextureArraySRV.p;		};
	ID3D11ShaderResourceView** GetMaterialTextureArray()	{ return &materialTextureArraySRV.p;	};
	ID3D11ShaderResourceView** GetMaterialLookupTexture()	{ return &materialLookupTableSRV.p;		};
	ID3D11ShaderResourceView** GetTerrainTexture()			{ return &terrainTextureSRV.p;			};
	ID3D11ShaderResourceView** GetSSAORandomTexture()		{ return &ssaoRandomTextureSRV.p;		};
	ID3D11ShaderResourceView** GetNoiseTexture()			{ return &noiseSRV.p;					};

	ID3D11ShaderResourceView** GetWindTexture()				{ return &windTextureSRV.p;				};
	ID3D11ShaderResourceView** GetWindNormalMap()			{ return &windNormalMapSRV.p;			};

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, NoiseClass* noise, Utility* utility);

	//To access the texture, call GetSSAORandomTexture()
	void RebuildSSAOTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	//To access the texture, call GetNoiseTexture()
	void RebuildSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void RebuildSeamlessSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float startPosX, float startPosY, unsigned int stepsX, unsigned int stepsY);

	//To access the texture, call GetNoiseTexture()
	void RebuildRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	//To access the texture, call GetNoiseTexture()
	void RebuildMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void RebuildTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, TextureID textureID, int width, int height, float startPosX, float startPosY, bool reseedRandomAfterwards);

	HRESULT CreateSeamlessSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv, 
		float startPosX, float startPosY, unsigned int textureWidth, unsigned int textureHeight, float noiseScale);

	HRESULT Create2DNormalMapFromHeightmap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** destTex, unsigned int textureWidth, unsigned int textureHeight);

	void SetupWindtextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float startPosX, float startPosY, 
		unsigned int textureWidth, unsigned int textureHeight, float noiseScale);

	bool SaveLTreeTextureToFile(ID3D11DeviceContext* deviceContext, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName);
	void SaveTextureToFile(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName);

private:
	bool CheckForDuplicateTextures(std::wstring filepath, unsigned short& outHandle);
	bool CheckForDuplicateMaterials(unsigned int hash, unsigned short& outHandle);
	unsigned int GenerateMaterialHash(OBJMaterialStruct& material);

	HRESULT Create2DSSAORandomTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);
	HRESULT CreateMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv);


	void CreateMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, ID3D11ShaderResourceView** textureSRV, 
		MaterialColorSpectrumUINT8 colorSpectrum, float startPosX, float startPosY);

	void CreateMaterialLookupTable(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureSRV, int worldMaxYValue);

	HRESULT Build1DMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		MaterialStruct materialData, int textureWidth, ID3D11Texture1D** texture);

	HRESULT Build1DMaterialTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::vector<MaterialStruct> materials,
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

	DODContainer<CComPtr<ID3D11ShaderResourceView>> modelTextures;
	DODContainer<OBJMaterialStruct> modelMaterials;

	CComPtr<ID3D11ShaderResourceView> windTextureSRV;
	CComPtr<ID3D11ShaderResourceView> windNormalMapSRV;

	CComPtr<ID3D11Texture2D> placeHolderTexture;
	CComPtr<ID3D11Texture2D> windNoiseTexture;

	MaterialColorSpectrumUINT8 dirtColor;
	MaterialColorSpectrumUINT8 grassColor;

	//These pairings are used to see if we're trying to load a texture that has already been loaded! For more information, check out CheckForDuplicateTextures()
	std::vector<std::pair<std::wstring, unsigned short>> textureNameAndKeyPairings;

	//A hash to see if material is identical
	std::vector<std::pair<unsigned int, unsigned short>> materialHashAndKeyPairings;
};