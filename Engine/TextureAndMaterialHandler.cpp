#include "TextureAndMaterialHandler.h"

TextureAndMaterialHandler::TextureAndMaterialHandler()
{
}

TextureAndMaterialHandler::~TextureAndMaterialHandler()
{
	//Empty, 'cause CComPtr and shared_ptr
}

bool TextureAndMaterialHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, NoiseClass* simplexNoise, Utility* utility)
{
	HRESULT hResult;

	//Initialize local variable to hold our noise generating class for future use.
	noise = std::make_shared<NoiseClass>(*simplexNoise);
	this->utility = std::make_shared<Utility>(*utility);

	//If this SRV has been initialized before, release it first.
	if(ssaoRandomTextureSRV)
	{
		ssaoRandomTextureSRV.Release();
	}
	CreateRandom2DTexture(device, deviceContext, &ssaoRandomTextureSRV.p);

	//If this SRV has been initialized before, release it first.
	if(noiseSRV)
	{
		noiseSRV.Release();
	}
	CreateSeamlessSimplex2DTexture(device, deviceContext, &noiseSRV.p, 0, 0, 100, 100);

	int terrainTextureCount = 6;
	WCHAR* terrainFilenames[6] = 
	{
		L"../Engine/data/dirt.dds",//dirt //0
		L"../Engine/data/grassTileTest01.dds", //1d
		//L"../Engine/data/randomGrass.dds", //1
		L"../Engine/data/rock4.dds",
		L"../Engine/data/seafloor.dds", //4
		//L"../Engine/data/randomDirt.dds",//4
		L"../Engine/data/snow.dds",
		L"../Engine/data/stone.dds"
	};

	//If this SRV has been initialized before, release it first.
	if(terrainTextureArraySRV != 0)
	{
		terrainTextureArraySRV.Release();
	}

	hResult = Build2DTextureArrayFromFile(device, deviceContext, terrainFilenames, terrainTextureCount, 
		&terrainTextureArraySRV.p, 1024, 1024);
	if(FAILED(hResult))
	{
		return false;
	}

	int vegetationTextureCount = 5;
	WCHAR* vegetationFilenames[5] = 
	{
		L"../Engine/data/Vegetation/yellowGrassQuad2.dds",
		L"../Engine/data/Vegetation/greenGrassQuad2.dds",
		L"../Engine/data/Vegetation/leafbranch.dds",
		L"../Engine/data/Vegetation/flower.dds",
		L"../Engine/data/Vegetation/bush3.dds"
	};

	//If this SRV has been initialized before, release it first.
	if(vegetationTextureArraySRV != 0)
	{
		vegetationTextureArraySRV.Release();
	}

	hResult = Build2DTextureArrayFromFile(device, deviceContext, vegetationFilenames, vegetationTextureCount, 
		&vegetationTextureArraySRV.p, 512, 512);
	if(FAILED(hResult))
	{
		return false;
	}

	vector<MaterialStruct> materials;

	//The lower the smoothness the wider/more spread out the specular is.
	//Meaning the higher the smoothness is the more focused and intense the specular is.
	MaterialStruct grass;
	grass.Kambience = 1.2f;
	grass.Kdiffuse = 1.0f;
	grass.Kspecular = 0.01f;
	grass.smoothness = 1.0f;
	grass.shouldBeShadowed = 10.0f;

	MaterialStruct rock;
	rock.Kambience = 1.2f;
	rock.Kdiffuse = 0.8f;
	rock.Kspecular = 0.6f;
	rock.smoothness = 1.0f;
	rock.shouldBeShadowed = 10.0f;

	MaterialStruct snow;
	snow.Kambience = 1.2f;
	snow.Kdiffuse = 1.0f;
	snow.Kspecular = 1.0f;
	snow.smoothness = 1.0f;
	snow.shouldBeShadowed = 10.0f;

	MaterialStruct dirt;
	dirt.Kambience = 1.2f;
	dirt.Kdiffuse = 0.9f;
	dirt.Kspecular = 0.1f;
	dirt.smoothness = 2.0f;
	dirt.shouldBeShadowed = 10.0f;

	MaterialStruct grassQuads;
	grassQuads.Kambience = 2.0f;
	grassQuads.Kdiffuse = 0.8f;
	grassQuads.Kspecular = 0.1f;
	grassQuads.smoothness = 1.0f;
	grassQuads.shouldBeShadowed = 10.0f;

	MaterialStruct underGround;
	underGround.Kambience = 0.5f;
	underGround.Kdiffuse = 0.5f;
	underGround.Kspecular = 0.2f;
	underGround.smoothness = 8.0f;
	underGround.shouldBeShadowed = 10.0f;

	materials.push_back(grass);
	materials.push_back(rock);
	materials.push_back(snow);
	materials.push_back(dirt);
	materials.push_back(grassQuads);
	materials.push_back(underGround);

	//If this SRV has been initialized before, release it first.
	if(materialTextureArraySRV)
	{
		materialTextureArraySRV.Release();
	}

	Build1DMaterialTextureArray(device, deviceContext, materials, materials.size(), 5, &materialTextureArraySRV.p);

	//Brown generally considered to be 150R, 75B and 0G.
	dirtColor.RedMin = 30;
	dirtColor.RedMax = 80;
	dirtColor.GreenMin = 20;
	dirtColor.GreenMax = 40;
	dirtColor.BlueMin = 15;
	dirtColor.BlueMax = 25;

	grassColor.RedMin = 60;
	grassColor.RedMax = 130;
	grassColor.GreenMin = 110;
	grassColor.GreenMax = 180;
	grassColor.BlueMin = 20;
	grassColor.BlueMax = 35;

	//If this SRV has been initialized before, release it first.
	if(terrainTextureSRV)
	{
		terrainTextureSRV.Release();
	}

	CreateMaterialTexture(device, deviceContext, 1024, 1024, &terrainTextureSRV.p, grassColor, 0.0f, 0.0f);

	CreateMaterialLookupTable(device, deviceContext, &materialLookupTableSRV.p, 380);

	return true;
}

//This function creates and loads a Texture1D into an external pointer
HRESULT TextureAndMaterialHandler::Build1DMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	MaterialStruct materialData, int textureWidth, ID3D11Texture1D** texture)
{
	HRESULT hResult;
	D3D11_TEXTURE1D_DESC texDesc;
	D3D11_SUBRESOURCE_DATA texInitializeData;

	//Create an array to temporarily hold each element in the texture
	float *dataArray = new float[textureWidth]();

	//Fill up the array
	dataArray[0]	= materialData.Kambience;
	dataArray[1]	= materialData.Kdiffuse;
	dataArray[2]	= materialData.Kspecular;
	dataArray[3]	= materialData.smoothness;
	dataArray[4]	= materialData.shouldBeShadowed;

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
	texInitializeData.SysMemSlicePitch = texInitializeData.SysMemPitch; //They are both the same because it's a 1D texture

	//Create texture object
	hResult = device->CreateTexture1D(&texDesc, &texInitializeData, texture);
	if(FAILED(hResult))
	{
		return false;
	}

	//Clean up
	delete [] dataArray;
	dataArray = 0;

	return S_OK;
}

//Builds a Texture1DArray out of 1D material textures and finally assigns the Texture1DArray to an external shader resource view
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


/*Okay, so this function recieves PixelData, which  is a struct that contains 4 UINT8 (because we use the DXGI_FORMAT_R8G8B8A8_UNORM format.
Then it creates a texture that is textureWidth*textureHeight big and fills it with data. Then it finally assigns the texture to an external shader resource view.*/
HRESULT TextureAndMaterialHandler::Build2DTextureProgrammatically( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	const vector<PixelData>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV )
{
	HRESULT hResult;
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

	if(placeHolderTexture != 0)
	{
		placeHolderTexture.Release();
	}

	//Create texture with the description and the subresource that contains all the pixel data
	hResult = device->CreateTexture2D(&texDesc, &texInitializeData, &placeHolderTexture);
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
	hResult = device->CreateShaderResourceView(placeHolderTexture, &viewDesc, textureSRV);
	if(FAILED(hResult))
	{
		return hResult;
	}

	delete [] dataArray;
	dataArray = 0;

	return S_OK;
}

HRESULT TextureAndMaterialHandler::Build2DSSAORandomTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<UINT16>& pixelData, 
	int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV )
{
	HRESULT hResult;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SUBRESOURCE_DATA texInitializeData;
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	//Create an array big enough to hold the pixel data for this texture
	UINT16 *dataArray = new UINT16[2 * textureWidth * textureHeight]();

	//Populate the array with color data
	for (int i = 0; i < (2 * textureWidth * textureHeight); i++)
	{
		dataArray[i] = pixelData[i];
	}

	//Set up texture description
	texDesc.Width              = textureWidth;
	texDesc.Height             = textureHeight;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.Format             = DXGI_FORMAT_R16G16_UNORM;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	//Initialize the subresource that will be used to send the pixel data from dataArray to the texture
	ZeroMemory(&texInitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitializeData.pSysMem = dataArray;
	texInitializeData.SysMemPitch = textureWidth*(sizeof(float));//Technically it's sizeof two halfsized floats
	//texInitializeData.SysMemSlicePitch = textureWidth*textureHeight*(sizeof(float)*4);

	if(placeHolderTexture != 0)
	{
		placeHolderTexture.Release();
	}

	//Create texture with the description and the subresource that contains all the pixel data
	hResult = device->CreateTexture2D(&texDesc, &texInitializeData, &placeHolderTexture);
	if(FAILED(hResult))
	{
		return hResult;
	}

	//Set up shader resource view description
	viewDesc.Format = DXGI_FORMAT_R16G16_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = 1;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 1;

	//Initialize the texture shader resource view and fill it with data
	hResult = device->CreateShaderResourceView(placeHolderTexture, &viewDesc, textureSRV);
	if(FAILED(hResult))
	{
		return hResult;
	}

	delete [] dataArray;
	dataArray = 0;

	return S_OK;
}


//Build 2DTextureArray and assigns it to an external shader resource view
HRESULT TextureAndMaterialHandler::Build2DTextureArrayFromFile(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
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
		loadInfo.MipLevels = D3DX11_DEFAULT;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX11_FILTER_LINEAR;
		loadInfo.MipFilter = D3DX11_FILTER_LINEAR;
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

void TextureAndMaterialHandler::CreateMaterialLookupTable( ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureSRV, int worldMaxYValue )
{
	//Definiera FYRA värden för varje Y-koordinat från 0 till WORLD_MAX_Y(180 atm?)
	//Skapa 1DTexture med WORLD_MAX_Y texlar och tilldela värden.

	////IDs for different things. Potential TODO: Read these in from an XML or smth?
	//enum MaterialID
	//{
	//	MATERIAL_GRASS,
	//	MATERIAL_ROCK,
	//	MATERIAL_SNOW,
	//	MATERIAL_DIRT,
	//	MATERIAL_GRASSQUADS,
	//	MATERIAL_UNDERGROUND
	//};

	//enum TextureID
	//{
	//	TEXTURE_DIRT,
	//	TEXTURE_GRASS,
	//	TEXTURE_ROCK,
	//	TEXTURE_SAND,
	//	TEXTURE_SNOW,
	//	TEXTURE_TILEDSTONE
	//}; 

	std::vector<PixelData> pixelData;
	pixelData.resize(worldMaxYValue);

	UINT8 texture1, texture2, material1, material2;

	for(size_t i = 0; i < pixelData.size(); i++)
	{
		if(i <= 19)
		{
			//Only dirt
			texture1 = 0;
			texture2 = 0;

			material1 = 3;
			material2 = 3;
		}
		else if(i <= 20)
		{
			//Dirt and grass
			texture1 = 0;
			texture2 = 1;

			material1 = 3;
			material2 = 0;
		}
		else if(i <= 44)
		{
			//Only grass
			texture1 = 1;
			texture2 = 1;

			material1 = 0;
			material2 = 0;
		}
		else if( i <= 45)
		{
			//Grass and rock
			texture1 = 1;
			texture2 = 2;

			material1 = 0;
			material2 = 1;
		}
		else if(i <= 46)
		{
			//Rock and snow
			texture1 = 2;
			texture2 = 4;

			material1 = 1;
			material2 = 2;
		}
		else if(i <= 190)
		{
			//Dirt and grass
			texture1 = 4;
			texture2 = 4;

			material1 = 2;
			material2 = 2;
		}
		else if(i <= 191)
		{
			//Dirt and grass
			texture1 = 4;
			texture2 = 1;

			material1 = 2;
			material2 = 0;
		}
		else
		{
			//Only snow
			texture1 = 1; 
			texture2 = 1;

			material1 = 0;
			material2 = 0;
		}

		pixelData[i].x = texture1;
		pixelData[i].y = texture2;
		pixelData[i].z = material1;
		pixelData[i].w = material2;
	}

	if(Build2DTextureProgrammatically(device, deviceContext, pixelData, worldMaxYValue, 1, textureSRV) != S_OK)
	{
		MessageBox(GetDesktopWindow(), L"Something went wrong when trying to create material lookup table texture. Look in TextureAndMaterialHandler::CreateMaterialLookupTable.", L"Error", MB_OK);
	}
}


HRESULT TextureAndMaterialHandler::CreateRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv)
{
	int textureWidth, textureHeight;
	textureWidth = 128;
	textureHeight = 128;

	vector<PixelData> pixelData;
	pixelData.resize(textureWidth*textureHeight);

	//Don't use utility.Random(). We do not want floats.
	for(int i = 0; i < textureWidth*textureHeight; i++)
	{
		pixelData[i].x = rand()%255;//%255;
		pixelData[i].y = rand()%255;//%255;
		pixelData[i].z = rand()%255;//%255;
		pixelData[i].w = 1; //Who gives a shit about alpha anyway AMIRITE???
	}

	return Build2DTextureProgrammatically(device, deviceContext, pixelData, textureWidth, textureHeight, srv);
}

HRESULT TextureAndMaterialHandler::CreateSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv)
{
	int textureWidth, textureHeight,i;
	float x,y;
	textureWidth = 512;
	textureHeight = 512;
	i = 0;

	vector<PixelData> pixelData;
	pixelData.resize(textureWidth*textureHeight);

	noise->ReseedRandom();

	for(float yCounter = 0; yCounter < textureHeight; yCounter++)
	{
		for(float xCounter = 0; xCounter < textureWidth; xCounter++)
		{
			y = yCounter;
			x = xCounter;

			if(y < 0)
			{
				y *= -1;
			}

			if(x < 0)
			{
				x *= -1;
			}

			float firstIteration = (noise->SimplexNoise3D(x*0.01f,y*0.01f, 10.0f)*250);
			//firstIteration = (firstIteration > 0) ? firstIteration : -firstIteration;

			float secondIteration = (noise->SimplexNoise3D(x*0.001f,y*0.001f, 10.0f)*250);
			//seccondIteration = (seccondIteration > 0) ? seccondIteration : -seccondIteration;

			float thirdIteration = (noise->SimplexNoise3D(x*0.0005f,y*0.0005f, 10.0f)*250);
			//thirdIteration = (thirdIteration > 0) ? thirdIteration : -thirdIteration;

			pixelData[i].x = (int)(firstIteration);
			pixelData[i].y = (int)(secondIteration);
			pixelData[i].z = (int)(thirdIteration);
			pixelData[i].w = 1; //Alpha.

			i++;
		}
	}

	return Build2DTextureProgrammatically(device, deviceContext, pixelData, textureWidth, textureHeight, srv);
}

HRESULT TextureAndMaterialHandler::CreateMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv)
{
	int textureWidth, textureHeight, i;
	float x,y;
	textureWidth = 512;
	textureHeight = 512;
	i = 0;

	vector<PixelData> pixelData;
	pixelData.resize(textureWidth*textureHeight);

	noise->ReseedRandom();

	for(float yCounter = 0; yCounter < textureHeight; yCounter++)
	{
		for(float xCounter = 0; xCounter < textureWidth; xCounter++)
		{
			y = yCounter;
			x = xCounter;	

			float firstIteration = noise->SimplexNoise3D(x*0.01f, y*0.01f, 10.0f)*250;
			//firstIteration = (firstIteration > 0) ? firstIteration : -firstIteration;

			float seccondIteration = noise->SimplexNoise3D(x*0.001f, y*0.001f, 30.0f)*250;
			//seccondIteration = (seccondIteration > 0) ? seccondIteration : -seccondIteration;

			float thirdIteration = noise->SimplexNoise3D(x*0.0005f, y*0.0005f, 45.0f)*250;
			//thirdIteration = (thirdIteration >= 0) ? thirdIteration : -thirdIteration;

			pixelData[i].x = (int)(firstIteration + seccondIteration + thirdIteration);
			pixelData[i].y = (int)(firstIteration + seccondIteration + thirdIteration);
			pixelData[i].z = (int)(firstIteration + seccondIteration + thirdIteration);
			pixelData[i].w = 1; //Alpha.

			i++;
		}
	}

	return Build2DTextureProgrammatically(device, deviceContext, pixelData, textureWidth, textureHeight, srv);
}

//Actually not used atm...
HRESULT TextureAndMaterialHandler::Create2DSSAORandomTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv )
{
	int textureWidth, textureHeight;
	textureWidth = 64;
	textureHeight = 64;

	std::vector<UINT16> pixelData;
	pixelData.resize(2 * textureWidth*textureHeight);

	int index = 0;

	//Don't use utility.Random(). We do not want floats.
	for(size_t i = 0; i < pixelData.size(); i++)
	{
		pixelData[i]   = rand()%255;
	}

	return Build2DSSAORandomTexture(device, deviceContext, pixelData, textureWidth, textureHeight, srv);
}

bool TextureAndMaterialHandler::SaveLTreeTextureToFile( ID3D11DeviceContext* deviceContext, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName )
{
	HRESULT hResult;

	hResult = D3DX11SaveTextureToFileA(deviceContext, placeHolderTexture, format, fileName);
	if(FAILED(hResult))
	{
		return false;
	}

	return true;
}

void TextureAndMaterialHandler::SaveTextureToFile( ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName )
{
	HRESULT hResult;

	ID3D11Resource* res;
	ID3D11Texture2D* tex;
	texture->GetResource(&res);
	res->QueryInterface(&tex);

	hResult = D3DX11SaveTextureToFileA(deviceContext, tex, format, fileName);
	if(FAILED(hResult))
	{
		MessageBox(GetDesktopWindow(), L"Something went wrong when trying to save texture to file. Look in TextureAndMaterialHandler::SaveTextureToFile.", L"Error", MB_OK);
	}

	res->Release();
	tex->Release();
	res = 0;
	tex = 0;
}

void TextureAndMaterialHandler::RebuildSSAOTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	//If this SRV has been initialized before, release it first.
	if(ssaoRandomTextureSRV)
	{
		ssaoRandomTextureSRV.Release();
	}

	CreateRandom2DTexture(device, deviceContext, &ssaoRandomTextureSRV.p);
}

void TextureAndMaterialHandler::RebuildSimplex2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	//If this SRV has been initialized before, release it first.
	if(noiseSRV)
	{
		noiseSRV.Release();
	}

	CreateSimplex2DTexture(device, deviceContext, &noiseSRV.p);
}

void TextureAndMaterialHandler::RebuildSeamlessSimplex2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	float startPosX, float startPosY, float stepsX, float stepsY )
{
	//If this SRV has been initialized before, release it first.
	if(noiseSRV)
	{
		noiseSRV.Release();
	}

	CreateSeamlessSimplex2DTexture(device, deviceContext, &noiseSRV.p, startPosX, startPosY, stepsX, stepsY);
}

void TextureAndMaterialHandler::RebuildRandom2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	//If this SRV has been initialized before, release it first.
	if(noiseSRV)
	{
		noiseSRV.Release();
	}

	CreateRandom2DTexture(device, deviceContext, &noiseSRV.p);
}

void TextureAndMaterialHandler::RebuildMirroredSimplex2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext )
{
	//If this SRV has been initialized before, release it first.
	if(noiseSRV)
	{
		noiseSRV.Release();
	}

	CreateMirroredSimplex2DTexture(device, deviceContext, &noiseSRV.p);
}

//Sorry for the vague name. This function is for randomly creating textures for different "materials"... Grass, dirt, rock. 
//They will hopefully look pretty much like minecraft's default textures.
void TextureAndMaterialHandler::CreateMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, ID3D11ShaderResourceView** textureSRV, 
	MaterialColorSpectrumUINT8 colorSpectrum, float startPosX=0.0f, float startPosY=0.0f)
{
	std::vector<PixelData> pixelData;
	pixelData.resize(width * height);

	//for(size_t i = 0; i < pixelData.size(); i++)
	//{
	//	pixelData[i].x = RandRange(colorSpectrum.RedMin, colorSpectrum.RedMax);
	//	pixelData[i].y = RandRange(colorSpectrum.GreenMin, colorSpectrum.GreenMax);
	//	pixelData[i].z = RandRange(colorSpectrum.BlueMin, colorSpectrum.BlueMax);
	//	pixelData[i].w = 1.0f;
	//}

	int i = 0;

	//float randZ = utility->RandomFloat();


	float x, y;

	//for(float yCounter = (-height*0.5f); yCounter < height*0.5f; yCounter++)
	//{
	//	for(float xCounter = (-width*0.5f); xCounter < width*0.5f; xCounter++)
	//	{


	for(float yCounter = startPosY; yCounter < startPosY+height; yCounter++)
	{
		for(float xCounter = startPosX; xCounter < startPosX+width; xCounter++)
		{
			y = abs(yCounter);
			x = abs(xCounter);

			float firstIteration = noise->SimplexNoise3DZeroToOne(x*0.01f, y*0.01f, 25.0f);
			float secondIteration = noise->SimplexNoise3DZeroToOne(y*0.008f, x*0.008f, 50.0f);

			float multiplier = (secondIteration+firstIteration)*0.5f;

			pixelData[i].x = (UINT8)((float)multiplier * (colorSpectrum.RedMax-colorSpectrum.RedMin)+colorSpectrum.RedMin);		//
			pixelData[i].y = (UINT8)((float)multiplier * (colorSpectrum.GreenMax-colorSpectrum.GreenMin)+colorSpectrum.GreenMin);	//
			pixelData[i].z = (UINT8)((float)multiplier * (colorSpectrum.BlueMax-colorSpectrum.BlueMin)+colorSpectrum.BlueMin);		//
			pixelData[i].w = 1; //Alpha.

			i++;
		}
	}

	noise->ReseedRandom();

	if(FAILED(Build2DTextureProgrammatically(device, deviceContext, pixelData, width, height, textureSRV)))
	{
		MessageBox(GetDesktopWindow(), L"Something went wrong when calling CreateMaterialTexture. Look in TextureAndMaterialHandler::CreateMaterialTexture.", L"Error", MB_OK);
	}
}

void TextureAndMaterialHandler::RebuildTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, TextureID textureID, 
	int width, int height, float startPosX, float startPosY, bool reseedRandomAfterwards )
{
	if(terrainTextureSRV)
	{
		terrainTextureSRV.Release();
	}

	switch(textureID)
	{
		case TEXTURE_DIRT:
		CreateMaterialTexture(device, deviceContext, width, height, &terrainTextureSRV.p, dirtColor, startPosX, startPosY);
		break;

		case TEXTURE_GRASS:
		CreateMaterialTexture(device, deviceContext, width, height, &terrainTextureSRV.p, grassColor,  startPosX, startPosY);
		break;

		case TEXTURE_ROCK:
		break;

		case TEXTURE_SNOW:
		break;

		case TEXTURE_SAND:
		break;

		default:
		break;
		//Meh.
	}

	if(reseedRandomAfterwards)
	{
		noise->ReseedRandom();
	}
}


//Credits: http://www.sjeiti.com/creating-tileable-noise-maps/
HRESULT TextureAndMaterialHandler::CreateSeamlessSimplex2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	ID3D11ShaderResourceView** srv, float startPosX, float startPosY, float stepsX, float stepsY)
{
	int textureWidth, textureHeight,i;
	textureWidth = stepsX;
	textureHeight = stepsY;
	i = 0;

	float noiseScale = 0.5f;
	float radius = (textureWidth/2)-2.0f;

	vector<UINT8> pixelData;
	pixelData.resize(textureWidth*textureHeight);

	for(float x = startPosX; x < startPosX+textureWidth; x++)
	{
		for(float y = startPosY; y < startPosY+textureHeight; y++)
		{
			float xScale = x / textureWidth;
			float yScale = y / textureHeight;
			float xPi = xScale * 2 * XM_PI;
			float yPi = yScale * 2 * XM_PI;

			//Produce the four points we'll make noise from
			float nx = radius+sin(xPi);
			float ny = radius+cos(xPi);
			float nz = radius+sin(yPi);
			float nw = radius+cos(yPi);

			//Produce noise, rescale it from [-1, 1] to [0, 1] then multiply to [0, 256] to make full use of the 8bit channels of the texture it'll be stored in.
			int noiseResult = (0.5f * noise->SimplexNoise4D(nx*noiseScale, ny*noiseScale, nz*noiseScale, nw*noiseScale) + 0.5f) * 256;

			pixelData[i] = noiseResult;

			i++;
		}
	}

	//Build texture
	if(FAILED(Build8Bit2DTextureProgrammatically(device, deviceContext, pixelData, textureWidth, textureHeight, srv)))
	{
		MessageBox(GetDesktopWindow(), L"Something went wrong when calling CreateMaterialTexture. Look in TextureAndMaterialHandler::CreateMaterialTexture.", L"Error", MB_OK);
		return S_FALSE;
	}

	//Reseed the random for next time we'll call this function
	noise->ReseedRandom();

	return S_OK;
}

HRESULT TextureAndMaterialHandler::Build8Bit2DTextureProgrammatically( ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<UINT8>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV )
{
	HRESULT hResult;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_SUBRESOURCE_DATA texInitializeData;
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	//Set up texture description
	texDesc.Width              = textureWidth;
	texDesc.Height             = textureHeight;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.Format             = DXGI_FORMAT_R8_UNORM;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags     = 0;
	texDesc.MiscFlags          = 0;

	//Initialize the subresource that will be used to send the pixel data from dataArray to the texture
	ZeroMemory(&texInitializeData, sizeof(D3D11_SUBRESOURCE_DATA));
	texInitializeData.pSysMem = pixelData.data();
	texInitializeData.SysMemPitch = textureWidth*(sizeof(UINT8));
	//texInitializeData.SysMemSlicePitch = textureWidth*textureHeight*(sizeof(float)*4);

	if(placeHolderTexture != 0)
	{
		placeHolderTexture.Release();
	}

	//Create texture with the description and the subresource that contains all the pixel data
	hResult = device->CreateTexture2D(&texDesc, &texInitializeData, &placeHolderTexture);
	if(FAILED(hResult))
	{
		return hResult;
	}

	//Set up shader resource view description
	viewDesc.Format = DXGI_FORMAT_R8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = 1;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 1;

	//Initialize the texture shader resource view and fill it with data
	hResult = device->CreateShaderResourceView(placeHolderTexture, &viewDesc, textureSRV);
	if(FAILED(hResult))
	{
		return hResult;
	}

	return S_OK;
}


