#include "TextureCreator.h"


TextureCreator::TextureCreator()
{
}


TextureCreator::~TextureCreator()
{
}

/*Okay, so this function recieves PixelData, which  is a struct that contains 4 UINT8 (because we use the DXGI_FORMAT_R8G8B8A8_UNORM format.
Then it creates a texture that is textureWidth*textureHeight big and fills it with data. Then it finally assigns the texture to an external shader resource view.*/
HRESULT TextureCreator::Build32Bit2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	const std::vector<PixelData>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV )
{
	if((*textureSRV))
	{
		(*textureSRV)->Release();
	}

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

//Build 2DTextureArray and assigns it to an external shader resource view
HRESULT TextureCreator::Load2DTextureArrayFromFiles(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArraySRV, int texWidth, int texHeight)
{
	if((*textureArraySRV))
	{
		(*textureArraySRV)->Release();
	}

	HRESULT hResult;

	//Create a std::vector to temporarily store the textures that we load from the HDD
	std::vector<ID3D11Texture2D*> srcTex;
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

HRESULT TextureCreator::Build16Bit2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::vector<UINT16>& pixelData, 
	int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV )
{
	if((*textureSRV))
	{
		(*textureSRV)->Release();
	}

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

HRESULT TextureCreator::Build8Bit2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	const std::vector<UINT8>& pixelData, int textureWidth, int textureHeight, ID3D11ShaderResourceView** textureSRV )
{
	if((*textureSRV))
	{
		(*textureSRV)->Release();
	}

	HRESULT hResult;
	D3D11_SUBRESOURCE_DATA texInitializeData;
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	D3D11_TEXTURE2D_DESC texDesc;

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
	hResult = device->CreateTexture2D(&texDesc, &texInitializeData, &placeHolderTexture.p);
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
	hResult = device->CreateShaderResourceView(placeHolderTexture.p, &viewDesc, textureSRV);
	if(FAILED(hResult))
	{
		return hResult;
	}

	return S_OK;
}

//This function creates and loads a Texture1D into an external pointer
HRESULT TextureCreator::Build1DMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	ShaderMaterialStruct materialData, int textureWidth, ID3D11Texture1D** texture)
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
HRESULT TextureCreator::Build1DMaterialTextureArray( ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	std::vector<ShaderMaterialStruct> materials, int materialCount, int textureWidth, ID3D11ShaderResourceView** materialSRV )
{
	if((*materialSRV))
	{
		(*materialSRV)->Release();
	}

	HRESULT hResult;

	//Create a std::vector to temporarily store the textures that we load from the HDD
	std::vector<ID3D11Texture1D*> srcTex;
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