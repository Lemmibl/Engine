#include "TextureHandler.h"

TextureHandler::TextureHandler()
	: modelTextures(64)
{
}

TextureHandler::~TextureHandler()
{
	//Empty, 'cause CComPtr and shared_ptr
}

bool TextureHandler::Initialize(ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext)
{
	HRESULT hResult;

	 device = extDevice;
	 deviceContext = extDeviceContext;

	std::vector<WCHAR*> terrainFilenames;

	terrainFilenames.push_back(L"../Engine/data/dirt.dds");//dirt //0
	terrainFilenames.push_back(L"../Engine/data/grassTileTest01.dds"); //1   grassGreenYellow
	//terrainFilenames.push_back(L"../Engine/data/randomGrass.dds"); //1
	terrainFilenames.push_back(L"../Engine/data/rock4.dds");
	terrainFilenames.push_back(L"../Engine/data/seafloor.dds"); //4
	//terrainFilenames.push_back(L"../Engine/data/randomDirt.dds");//4
	terrainFilenames.push_back(L"../Engine/data/snow.dds");
	terrainFilenames.push_back(L"../Engine/data/stone.dds");


	//If this SRV has been initialized before, release it first.
	if(terrainTextureArraySRV != 0)
	{
		terrainTextureArraySRV.Release();
	}

	hResult = texCreator.Load2DTextureArrayFromFiles(device, deviceContext, terrainFilenames.data(), terrainFilenames.size(), 
		&terrainTextureArraySRV.p, 1024, 1024);
	if(FAILED(hResult))
	{
		return false;
	}

	std::vector<WCHAR*> vegetationFilenames;
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants01.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants02.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants03.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants04.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants08.dds");

	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants05.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants06.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants07.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants08.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants08.dds");

	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants08.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants09.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants10.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants11.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants12.dds");

	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants13.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants14.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants15.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants16.dds");
	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants17.dds");

	vegetationFilenames.push_back(	L"../Engine/data/CGTextures/Vegetation/Plants08.dds");

	//If this SRV has been initialized before, release it first.
	if(vegetationTextureArraySRV != 0)
	{
		vegetationTextureArraySRV.Release();
	}

	hResult = texCreator.Load2DTextureArrayFromFiles(device, deviceContext, vegetationFilenames.data(), vegetationFilenames.size(), 
		&vegetationTextureArraySRV.p, 512, 512);
	if(FAILED(hResult))
	{
		return false;
	}

	return true;
}


bool TextureHandler::Load2DTextureFromFile( ID3D11Device* device, std::wstring textureFilepath, ID3D11ShaderResourceView** srv )
{
	HRESULT result;
	
	D3DX11CreateShaderResourceViewFromFile(device, textureFilepath.c_str(), NULL, NULL, srv, &result);
	if(FAILED(result))
	{
		MessageBox(GetDesktopWindow(), (L"Couldn't load this 2D texture: " + *textureFilepath.c_str()), L"Error", MB_OK);
		return false;
	}

	return true;
}

bool TextureHandler::Load2DCubemapTextureFromFile( ID3D11Device* device, std::wstring textureFilepath, ID3D11ShaderResourceView** srv)
{
	HRESULT result;

	//Tell D3D we will be loading a cube texture
	D3DX11_IMAGE_LOAD_INFO loadInfo;
	loadInfo.Width = 1024;
	loadInfo.Height = 1024;
	loadInfo.Depth = 0;
	loadInfo.FirstMipLevel = 0;
	loadInfo.MipLevels = 0;
	loadInfo.Usage = D3D11_USAGE_IMMUTABLE;
	loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	loadInfo.CpuAccessFlags = 0;
	loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	loadInfo.Filter = D3DX11_FILTER_LINEAR;
	loadInfo.MipFilter = D3DX11_FILTER_LINEAR;
	loadInfo.pSrcInfo = NULL; 

	//Load the texture
	ID3D11Texture2D* SMTexture = 0;

	result = D3DX11CreateTextureFromFile(device, textureFilepath.c_str(), &loadInfo, NULL, (ID3D11Resource**)&SMTexture, NULL);
	if(FAILED(result))
	{
		return false;
	}

	//Create the textures description
	D3D11_TEXTURE2D_DESC SMTextureDesc;
	SMTexture->GetDesc(&SMTextureDesc);

	//Tell D3D We have a cube texture, which is an array of 2D textures
	D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	SMViewDesc.Format = SMTextureDesc.Format;
	SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SMViewDesc.TextureCube.MipLevels = SMTextureDesc.MipLevels;
	SMViewDesc.TextureCube.MostDetailedMip = 0;

	//Create the Resource view
	result = device->CreateShaderResourceView(SMTexture, &SMViewDesc, srv);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureHandler::SaveTextureToFile( ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, D3DX11_IMAGE_FILE_FORMAT format, LPCSTR fileName )
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

unsigned short TextureHandler::AddNewtexture(ID3D11Device* device, std::wstring textureFilepath )
{
	unsigned short newHandle = 0;

	//If there ISNT a duplicate texture
	if(!CheckForDuplicateTextures(textureFilepath, newHandle))
	{
		//We allocate a new slot in our modelTextures container
		if(modelTextures.AddNewObject(newHandle))
		{
			HRESULT result;

			if(modelTextures[newHandle] != 0)
			{
				modelTextures[newHandle].Release();
			}

			result = D3DX11CreateShaderResourceViewFromFile(device, textureFilepath.c_str(),
				NULL, NULL, &modelTextures[newHandle].p, NULL );

			if(FAILED(result))
			{
				std::wstring message;
				message += L"Could not load texture file: ";
				message += textureFilepath.c_str();

				MessageBox(0, message.c_str(), L"Error", MB_OK);
			}
			else
			{
				//So if EVERYTHING went through, add the filepath and related handle so that we can check for duplicates of this texture in the future
				textureNameAndKeyPairings.push_back(std::make_pair<std::wstring, unsigned short>(textureFilepath, newHandle));
			}
		} 
	}

	return newHandle; 
}

//Returns true if there's a duplicate found. This means that the outHandle will be assigned a value.
bool TextureHandler::CheckForDuplicateTextures(std::wstring filepath, unsigned short& outHandle)
{

	for(unsigned int i = 0; i < textureNameAndKeyPairings.size(); i++)
	{
		auto& pair = textureNameAndKeyPairings[i];

		//If this is true, it means we've been trying to add a duplicate texture
		if(filepath == pair.first)
		{
			//So don't add a duplicate texture, instead just return the handle to the identical texture that is already loaded in
			outHandle = pair.second;

			//Break loop
			return true;
		}
	}

	return false;
}

bool TextureHandler::GetTextureHandle( std::wstring textureFilepath, unsigned short& handle )
{
	return CheckForDuplicateTextures(textureFilepath, handle);
}


