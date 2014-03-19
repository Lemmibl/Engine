#include "ProceduralTextureHandler.h"


ProceduralTextureHandler::ProceduralTextureHandler()
{
}


ProceduralTextureHandler::~ProceduralTextureHandler()
{
}

bool ProceduralTextureHandler::Initialize(ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext, TextureCreator* extTexCreator, NoiseClass* extNoise, Utility* extUtility )
{
	device = extDevice;
	deviceContext = extDeviceContext;
	texCreator = extTexCreator;

	//Initialize local variable to hold our noise generating class for future use.
	noise = std::make_shared<NoiseClass>(*extNoise);
	utility = std::make_shared<Utility>(*extUtility);

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
	CreateSeamlessSimplex2DTexture(device, deviceContext, &noiseSRV.p, 0, 0, 1024, 100, 0.5f);

	return true;
}

HRESULT ProceduralTextureHandler::CreateRandom2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv)
{
	int textureWidth, textureHeight;
	textureWidth = 128;
	textureHeight = 128;

	std::vector<PixelData> pixelData;
	pixelData.resize(textureWidth*textureHeight);

	//Don't use utility.Random(). We do not want floats.
	for(int i = 0; i < textureWidth*textureHeight; i++)
	{
		pixelData[i].x = rand()%255;//%255;
		pixelData[i].y = rand()%255;//%255;
		pixelData[i].z = rand()%255;//%255;
		pixelData[i].w = 1; //Who gives a shit about alpha anyway AMIRITE???
	}

	return texCreator->Build32Bit2DTexture(device, deviceContext, pixelData, textureWidth, textureHeight, srv);
}

HRESULT ProceduralTextureHandler::CreateSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv)
{
	int textureWidth, textureHeight,i;
	float x,y;
	textureWidth = 512;
	textureHeight = 512;
	i = 0;

	std::vector<PixelData> pixelData;
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

	return texCreator->Build32Bit2DTexture(device, deviceContext, pixelData, textureWidth, textureHeight, srv);
}

HRESULT ProceduralTextureHandler::CreateMirroredSimplex2DTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv)
{
	int textureWidth, textureHeight, i;
	float x,y;
	textureWidth = 512;
	textureHeight = 512;
	i = 0;

	std::vector<PixelData> pixelData;
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

	return texCreator->Build32Bit2DTexture(device, deviceContext, pixelData, textureWidth, textureHeight, srv);
}

//Credits: http://www.sjeiti.com/creating-tileable-noise-maps/
HRESULT ProceduralTextureHandler::CreateSeamlessSimplex2DTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv, 
	float startPosX, float startPosY, unsigned int textureWidth, unsigned int textureHeight, float noiseScale )
{
	int i;
	i = 0;

	float radius = (textureWidth/2)-2.0f;

	std::vector<UINT8> pixelData;
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
			int noiseResult = (int)((0.5f * noise->SimplexNoise4D(nx*noiseScale, ny*noiseScale, nz*noiseScale, nw*noiseScale) + 0.5f) * 256);

			pixelData[i] = noiseResult;

			i++;
		}
	}

	//Build texture
	if(FAILED(texCreator->Build8Bit2DTexture(device, deviceContext, pixelData, textureWidth, textureHeight, srv)))
	{
		MessageBox(GetDesktopWindow(), L"Something went wrong when calling CreateMaterialTexture. Look in TextureAndMaterialHandler::CreateMaterialTexture.", L"Error", MB_OK);
		return S_FALSE;
	}

	//Reseed the random for next time we'll call this function
	noise->ReseedRandom();

	return S_OK;
}


HRESULT ProceduralTextureHandler::Create2DNormalMapFromHeightmap( ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** destTex, unsigned int textureWidth, unsigned int textureHeight )
{
	HRESULT hResult;
	CComPtr<ID3D11Texture2D> tempTex;
	D3D11_TEXTURE2D_DESC texDesc;

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

	if(placeHolderTexture != 0)
	{ 
		placeHolderTexture.Release();
	}

	//Create texture with the description and the subresource that contains all the pixel data
	hResult = device->CreateTexture2D(&texDesc, NULL, &tempTex.p);
	if(FAILED(hResult))
	{
		return hResult;
	}

	hResult = D3DX11ComputeNormalMap(deviceContext, windNoiseTexture.p, NULL, D3DX_CHANNEL_RED, 30.0f, tempTex.p);
	if(FAILED(hResult))
	{
		return hResult;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	//Set up shader resource view description
	viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = 1;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 1;

	//Initialize the texture shader resource view and fill it with data
	hResult = device->CreateShaderResourceView(tempTex.p, &viewDesc, destTex);
	if(FAILED(hResult))
	{
		return hResult;
	}

	return S_OK;
}

void ProceduralTextureHandler::SetupWindtextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	float startPosX, float startPosY, unsigned int textureWidth, unsigned int textureHeight, float noiseScale)
{
	CreateSeamlessSimplex2DTexture(device, deviceContext, &windTextureSRV.p, startPosX, startPosY, textureWidth, textureHeight, noiseScale);
	Create2DNormalMapFromHeightmap(device, deviceContext, &windNormalMapSRV.p, textureWidth, textureHeight);
}