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

HRESULT TextureAndMaterialHandler::Build1DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	MaterialStruct materialData, int textureWidth )
{
	//float *texArray = new float[textureWidth]();

	//for (int i = 0; i < (textureWidth * textureHeight); i += 4)
	//{
	//	texArray[i]		= pixelData->at(i).x;
	//	texArray[i+1]	= pixelData->at(i).y;
	//	texArray[i+2]	= pixelData->at(i).z;
	//	texArray[i+3]	= pixelData->at(i).w;
	//}

	//pixelData = 0;

	//D3D11_TEXTURE2D_DESC texDesc;
	//texDesc.Width              = textureWidth;
	//texDesc.Height             = textureHeight;
	//texDesc.MipLevels          = 1;
	//texDesc.ArraySize          = 1;
	//texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	//texDesc.SampleDesc.Count   = 1;
	//texDesc.SampleDesc.Quality = 0;
	//texDesc.Usage              = D3D11_USAGE_DEFAULT;
	//texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	//texDesc.CPUAccessFlags     = 0;
	//texDesc.MiscFlags          = 0;

	//D3D11_SUBRESOURCE_DATA texInitializeData;
	//ZeroMemory(&texInitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	//texInitializeData.pSysMem = texArray;

	//device->CreateTexture2D(&texDesc, &texInitializeData, &texture);

	//D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	//viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//viewDesc.Texture2DArray.MostDetailedMip = 0;
	//viewDesc.Texture2DArray.MipLevels = 1;
	//viewDesc.Texture2DArray.FirstArraySlice = 0;
	//viewDesc.Texture2DArray.ArraySize = 1;

	//device->CreateShaderResourceView(texture, &viewDesc, &textureSRV);

	return S_OK;
}

HRESULT TextureAndMaterialHandler::Build1DTextureArray( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	vector<MaterialStruct>* materials, int materialCount )
{

	return S_OK;
}

HRESULT TextureAndMaterialHandler::Build2DTextureProgrammatically( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	PixelData* pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView* textureSRV )
{
	HRESULT hResult;
	ID3D11Texture2D* texture;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SUBRESOURCE_DATA texInitializeData;
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;


	//http://stackoverflow.com/questions/14802205/creating-texture-programmatically-directx
	int *texArray = new int[4 * textureWidth * textureHeight]();

	for (int i = 0; i < (textureWidth * textureHeight); i += 4)
	{
		texArray[i	]	= pixelData[i].x;
		texArray[i+1]	= pixelData[i].y;
		texArray[i+2]	= pixelData[i].z;
		texArray[i+3]	= pixelData[i].w;
	}

	delete [] pixelData;
	pixelData = 0;

	texDesc.Width              = textureWidth;
	texDesc.Height             = textureHeight;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;


	ZeroMemory(&texInitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitializeData.pSysMem = texArray;


	hResult = device->CreateTexture2D(&texDesc, &texInitializeData, &texture);
	if(FAILED(hResult))
	{
		return hResult;
	}

	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = 1;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 1;

	hResult = device->CreateShaderResourceView(texture, &viewDesc, &textureSRV);
	if(FAILED(hResult))
	{
		return hResult;
	}

	pixelData = 0;

	texture->Release();
	texture = 0;

	delete [] texArray;
	texArray = 0;

	return S_OK;
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
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
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