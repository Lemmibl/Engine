#include "TextureAndMaterialHandler.h"

#pragma region Properties
ID3D11ShaderResourceView** TextureAndMaterialHandler::GetVegetationTextures()
{
	return &vegetationTextures;
}

ID3D11ShaderResourceView** TextureAndMaterialHandler::GetTerrainTextures()
{
	return &terrainTextures;
}
#pragma endregion

TextureAndMaterialHandler::TextureAndMaterialHandler()
{
	vegetationTextures = 0;
	terrainTextures = 0;
}

TextureAndMaterialHandler::TextureAndMaterialHandler( const TextureAndMaterialHandler& )
{

}

TextureAndMaterialHandler::~TextureAndMaterialHandler()
{
	if(vegetationTextures != 0)
	{
		vegetationTextures->Release();
		vegetationTextures = 0;
	}

	if(terrainTextures != 0)
	{
		terrainTextures->Release();
		terrainTextures = 0;
	}
}

bool TextureAndMaterialHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	HRESULT hResult;

	int terrainTextureCount = 6;

	WCHAR* terrainFilenames[6] = 
	{
		L"../Engine/data/dirt.dds",
		L"../Engine/data/grass.dds",
		L"../Engine/data/rock.dds",
		L"../Engine/data/seafloor.dds",
		L"../Engine/data/snow.dds",
		L"../Engine/data/stone.dds"
	};


	hResult = Build2DTextureArray(device, deviceContext, terrainFilenames, terrainTextureCount, &terrainTextures, 1024, 1024);
	if(FAILED(hResult))
	{
		return false;
	}

	int vegetationTextureCount = 8;

	WCHAR* vegetationFilenames[8] = 
	{
		L"../Engine/data/Vegetation/grassQuad.dds",
		L"../Engine/data/Vegetation/leafbranch.dds",
		L"../Engine/data/Vegetation/bush.dds",
		L"../Engine/data/Vegetation/tropicalBush.dds",
		L"../Engine/data/Vegetation/flower.dds",
		L"../Engine/data/Vegetation/flower2.dds",
		L"../Engine/data/Vegetation/flower3.dds",
		L"../Engine/data/Vegetation/flower4.dds"
	};

	hResult = Build2DTextureArray(device, deviceContext, vegetationFilenames, vegetationTextureCount, &vegetationTextures, 512, 512);
	if(FAILED(hResult))
	{
		return false;
	}

	return true;
}

HRESULT TextureAndMaterialHandler::Build2DTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArraySRV, int texWidth, int texHeight)
{
	HRESULT hResult;

	//Create a vector to temporarily store the textures that we load from the HDD
	vector<ID3D11Texture2D*> srcTex;
	srcTex.resize(textureCount);

	//For some reason if we try to access the last texture in the vector, we get weird errors, 
	//so we simply add an extra slot at the end of the vector that won't be used.
	for(int i = 0; i < textureCount; i++) 
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.Width = texWidth;		
		loadInfo.Height = texHeight;
		loadInfo.Depth  = 0;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 1;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX11_FILTER_NONE;
		loadInfo.MipFilter = D3DX11_FILTER_NONE;
		loadInfo.pSrcInfo  = 0;


		hResult = D3DX11CreateTextureFromFile(device, filenames[i],
				&loadInfo, 0, (ID3D11Resource**)&srcTex[i], &hResult);
		if(FAILED(hResult))
		{
			return false;
		}
	}

	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = textureCount;
	texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	/*
	You don't want to copy in the array slices one by one after you've created the texture. 
	What you want to do is have all of the data for all slices ready in CPU memory, 
	then provide that data as the initialization data when you create your texture array. 
	One way to do this is to load all of the array slices as STAGING resources, 
	map them to get the pointer to their data, and then use those pointers when initializing the IMMUTABLE Texture2D 
	resource for your texture array.
	*/

	//TODO: Declare a description for a texture array
	ID3D11Texture2D* texArray = 0;
	hResult = device->CreateTexture2D( &texArrayDesc, 0, &texArray);
	if(FAILED(hResult))
	{
		return false;
	}

	// Copy individual texture elements into texture array.
	for(int i = 0; i < textureCount; i++)
	{
		// for each mipmap level...
		for(UINT j = 0; j < texElementDesc.MipLevels; j++)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;

			//Read info from srcTex[i] and put it in mappedTex2D
			hResult = deviceContext->Map(srcTex[i], j, D3D11_MAP_READ, 0, &mappedTex2D);
			if(FAILED(hResult))
			{
				return false;
			}

			//Use mappedTex2D to move the data from srcTex[i] ===> texArray
			deviceContext->UpdateSubresource
			(
				texArray,
				D3D11CalcSubresource(j, i, texElementDesc.MipLevels),
				0, 
				mappedTex2D.pData, //Data from srcTex[i]
				mappedTex2D.RowPitch, //Size of one row (width*texelSize). In this case, our texels are A8R8G8B8 
				mappedTex2D.DepthPitch //Size of one texture (height*width*texelSize)
			);

			deviceContext->Unmap(srcTex[i], j);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = textureCount;

	hResult = device->CreateShaderResourceView(texArray, &viewDesc, textureArraySRV);
	if(FAILED(hResult))
	{
		return false;
	}


	// Cleanup--we only need the resource view.
	texArray->Release();
	texArray = 0;

	for(int i = 0; i < textureCount; ++i)
	{
		srcTex[i]->Release();
		srcTex[i] = 0;
	}


	return S_OK;
};

HRESULT TextureAndMaterialHandler::Build1DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{

	return S_OK;
}
