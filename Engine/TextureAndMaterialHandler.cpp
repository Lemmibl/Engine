#include "TextureAndMaterialHandler.h"

#pragma region Properties
ID3D11ShaderResourceView** TextureAndMaterialHandler::GetVegetationTextureArray()
{
	return &vegetationTextures;
}

ID3D11ShaderResourceView** TextureAndMaterialHandler::GetTerrainTextureArray()
{
	return &terrainTextures;
}

ID3D11ShaderResourceView** TextureAndMaterialHandler::GetMaterialTextureArray()
{
	return &materialTextures;
}
#pragma endregion

TextureAndMaterialHandler::TextureAndMaterialHandler()
{
	vegetationTextures = 0;
	terrainTextures = 0;
	materialTextures = 0;
}

TextureAndMaterialHandler::TextureAndMaterialHandler( const TextureAndMaterialHandler& )
{

}

TextureAndMaterialHandler::~TextureAndMaterialHandler()
{
	if(vegetationTextures)
	{
		vegetationTextures->Release();
		vegetationTextures = 0;
	}

	if(terrainTextures)
	{
		terrainTextures->Release();
		terrainTextures = 0;
	}

	if(materialTextures)
	{
		 materialTextures->Release();
		 materialTextures = 0;
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

	hResult = Build2DTextureArray(device, deviceContext, terrainFilenames, terrainTextureCount, 
		&terrainTextures, 1024, 1024);
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

	hResult = Build2DTextureArray(device, deviceContext, vegetationFilenames, vegetationTextureCount, 
		&vegetationTextures, 512, 512);
	if(FAILED(hResult))
	{
		return false;
	}

	vector<MaterialStruct> materials;

	//The lower the smoothness the wider/more spread out the specular is.
	//Meaning the higher the smoothness is the more focused and intense the specular is.

	MaterialStruct grass;
	grass.Kambience = 0.8f;
	grass.Kdiffuse = 1.0f;
	grass.Kspecular = 0.0f;
	grass.smoothness = 256.0f;

	MaterialStruct rock;
	rock.Kambience = 0.5f;
	rock.Kdiffuse = 0.8f;
	rock.Kspecular = 0.6f;
	rock.smoothness = 256.0f;

	MaterialStruct snow;
	snow.Kambience = 1.0f;
	snow.Kdiffuse = 1.0f;
	snow.Kspecular = 1.0f;
	snow.smoothness = 1.0f;

	MaterialStruct dirt;
	dirt.Kambience = 0.8f;
	dirt.Kdiffuse = 0.9f;
	dirt.Kspecular = 0.1f;
	dirt.smoothness = 2.0f;

	materials.push_back(grass);
	materials.push_back(rock);
	materials.push_back(snow);
	materials.push_back(dirt);

	Build1DMaterialTextureArray(device, deviceContext, materials, 4, 4, &materialTextures);

	return true;
}

HRESULT TextureAndMaterialHandler::Build1DMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	MaterialStruct materialData, int textureWidth, ID3D11Texture1D** texture)
{
	//, ID3D11ShaderResourceView* textureSRV
	
	HRESULT hResult;
	D3D11_TEXTURE1D_DESC texDesc;
	D3D11_SUBRESOURCE_DATA texInitializeData;
	//D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	//Create an array to hold each element in the texture
	float *dataArray = new float[textureWidth]();

	//Fill up the array ... try to find a way to make this dynamic?
	dataArray[0]	= materialData.Kambience;
	dataArray[1]	= materialData.Kdiffuse;
	dataArray[2]	= materialData.Kspecular;
	dataArray[3]	= materialData.smoothness; 

	//Initialize texture description
	texDesc.Width				= textureWidth;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.Format				= DXGI_FORMAT_R32_FLOAT;
	texDesc.Usage				= D3D11_USAGE_STAGING;
	texDesc.BindFlags			= 0;
	texDesc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	texDesc.MiscFlags			= 0;

	//Initialize texture object
	ZeroMemory(&texInitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitializeData.pSysMem = dataArray;
	texInitializeData.SysMemPitch = textureWidth*sizeof(float);
	texInitializeData.SysMemSlicePitch = textureWidth*sizeof(float); //It's the same as it's a 1D texture

	//Create texture object
	hResult = device->CreateTexture1D(&texDesc, &texInitializeData, texture);
	if(FAILED(hResult))
	{
		return false;
	}

	////Initialize shader resource view description
	//viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	//viewDesc.Texture1D.MostDetailedMip = 0;
	//viewDesc.Texture1D.MipLevels = 1;

	////Create shader resource view
	//hResult = device->CreateShaderResourceView(texture, &viewDesc, &textureSRV);
	//if(FAILED(hResult))
	//{
	//	return false;
	//}

	//Clean up
	delete [] dataArray;
	dataArray = 0;

	return S_OK;
}


HRESULT TextureAndMaterialHandler::Build1DMaterialTextureArray( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	vector<MaterialStruct> materials, int materialCount, int textureWidth, ID3D11ShaderResourceView** materialSRV )
{
	HRESULT hResult;

	//Create a vector to temporarily store the textures that we load from the HDD
	vector<ID3D11Texture1D*> srcTex;
	srcTex.resize(materialCount);

	//For each texture, load it from harddrive and put it in a texture2D object
	for(int i = 0; i < materialCount; i++) 
	{
		hResult = Build1DMaterialTexture(device, deviceContext, materials[i], textureWidth, &srcTex[i]);
		if(FAILED(hResult))
		{
			return false;
		}
	}

	// Load description of textures that we loaded in above, to be used when initializing texArray below.
	D3D11_TEXTURE1D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);


	//Declare a description for the texture array
	D3D11_TEXTURE1D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = materialCount;
	texArrayDesc.Format             = DXGI_FORMAT_R32_FLOAT;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	//Create texture array to be filled up below
	ID3D11Texture1D* texArray = 0;
	hResult = device->CreateTexture1D( &texArrayDesc, 0, &texArray);
	if(FAILED(hResult))
	{
		return false;
	}

	// Copy individual texture elements into the texture array.
	for(int i = 0; i < materialCount; i++)
	{
		// for each mipmap level...
		for(UINT j = 0; j < texElementDesc.MipLevels; j++)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex1D;

			//Read info from srcTex[i] and put it in mappedTex2D
			hResult = deviceContext->Map(srcTex[i], j, D3D11_MAP_READ, 0, &mappedTex1D);
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
				mappedTex1D.pData, //Data from srcTex[i]
				mappedTex1D.RowPitch, //Size of one row (width*texelSize). In this case, our texels are A8R8G8B8 
				mappedTex1D.DepthPitch //Size of one texture (height*width*texelSize)
				);

			deviceContext->Unmap(srcTex[i], j);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = materialCount;

	hResult = device->CreateShaderResourceView(texArray, &viewDesc, materialSRV);
	if(FAILED(hResult))
	{
		return false;
	}

	// Cleanup--we only need the resource view.
	texArray->Release();
	texArray = 0;

	for(int i = 0; i < materialCount; ++i)
	{
		srcTex[i]->Release();
		srcTex[i] = 0;
	}

	return S_OK;
}


/*Okay, so this function recieves PixelData, which  is a struct that contains 4 UINT8 (because we use the DXGI_FORMAT_R8G8B8A8_UNORM format*/
HRESULT TextureAndMaterialHandler::Build2DTextureProgrammatically( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	PixelData* pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV )
{
	HRESULT hResult;
	ID3D11Texture2D* texture;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SUBRESOURCE_DATA texInitializeData;
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	//Create an array big enough to hold the pixel data for this texture
	UINT8 *dataArray = new UINT8[4 * textureWidth * textureHeight]();

	int index = 0;

	//Populate the array with color data
	for (int i = 0; i < (4 * textureWidth * textureHeight); i += 4)
	{
		dataArray[i	]	= pixelData[index].x;
		dataArray[i+1]	= pixelData[index].y;
		dataArray[i+2]	= pixelData[index].z;
		dataArray[i+3]	= pixelData[index].w;

		index++;
	}

	//pixelData is now no longer needed
	delete [] pixelData;
	pixelData = 0;

	//Set up texture description
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

	//Initialize the subresource that will be used to send the pixel data from dataArray to the texture
	ZeroMemory(&texInitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitializeData.pSysMem = dataArray;
	texInitializeData.SysMemPitch = textureWidth*(sizeof(UINT8)*4);
	//texInitializeData.SysMemSlicePitch = textureWidth*textureHeight*(sizeof(float)*4);

	//Create texture with the description and the subresource that contains all the pixel data
	hResult = device->CreateTexture2D(&texDesc, &texInitializeData, &texture);
	if(FAILED(hResult))
	{
		return hResult;
	}

	//Set up shader resource view description
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = 1;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 1;

	//Initialize the texture shader resource view and fill it with data
	hResult = device->CreateShaderResourceView(texture, &viewDesc, textureSRV);
	if(FAILED(hResult))
	{
		return hResult;
	}

	//Clean up everything. We don't need the texture object anymore, because we have the resource view.
	texture->Release();
	texture = 0;

	delete [] dataArray;
	dataArray = 0;

	return S_OK;
}


HRESULT TextureAndMaterialHandler::Build2DTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArraySRV, int texWidth, int texHeight)
{
	HRESULT hResult;

	//Create a vector to temporarily store the textures that we load from the HDD
	vector<ID3D11Texture2D*> srcTex;
	srcTex.resize(textureCount);

	//For each texture, load it from harddrive and put it in a texture2D object
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

	// Load description of textures that we loaded in above, to be used when initializing texArray below.
	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);


	//Declare a description for the texture array
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

	//Create texture array to be filled up below
	ID3D11Texture2D* texArray = 0;
	hResult = device->CreateTexture2D( &texArrayDesc, 0, &texArray);
	if(FAILED(hResult))
	{
		return false;
	}

	// Copy individual texture elements into the texture array.
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