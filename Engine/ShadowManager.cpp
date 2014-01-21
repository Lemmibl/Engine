#include "ShadowManager.h"


ShadowManager::ShadowManager()
{
}


ShadowManager::~ShadowManager()
{
}

void ShadowManager::SetShadowViewport()
{
	deviceContext->RSSetViewports(1, &shadowmapViewport);
}

ID3D11DepthStencilView* ShadowManager::GetShadowmapDSV()
{
	return shadowmapDSV.p;
}

ID3D11ShaderResourceView* ShadowManager::GetShadowmapSRV()
{
	return shadowmapResourceView.p;
}

bool ShadowManager::Initialize( ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext, unsigned int shadowMapWidth, unsigned int shadowMapHeight )
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	device =  extDevice;
	deviceContext = extDeviceContext;

	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = shadowMapWidth;
	texDesc.Height = shadowMapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//create texture and depth/resource views
	result = device->CreateTexture2D(&texDesc, NULL, &shadowmapStencilTexture.p);
	if(FAILED(result))
	{
		return false;
	}

	// Create the depth stencil view desc
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;

	result = device->CreateDepthStencilView(shadowmapStencilTexture.p, &dsvDesc, &shadowmapDSV.p);
	if(FAILED(result))
	{
		return false;
	}

	texDesc.Width = shadowMapWidth;
	texDesc.Height = shadowMapHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//create texture and depth/resource views
	result = device->CreateTexture2D(&texDesc, NULL, &shadowmapTexture.p);
	if(FAILED(result))
	{
		return false;
	}

	//create shader resource view desc
	srvDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(shadowmapTexture, &srvDesc, &shadowmapResourceView.p);
	if(FAILED(result))
	{
		return false;
	}

	//Create shadowmap viewport
	shadowmapViewport.Width = (FLOAT)shadowMapWidth;
	shadowmapViewport.Height = (FLOAT)shadowMapHeight;
	shadowmapViewport.MinDepth = 0.0f;
	shadowmapViewport.MaxDepth = 1.0f;
	shadowmapViewport.TopLeftX = 0.0f;
	shadowmapViewport.TopLeftY = 0.0f;

	return true;
}