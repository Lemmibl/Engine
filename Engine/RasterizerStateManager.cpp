#include "RasterizerStateManager.h"


RasterizerStateManager::RasterizerStateManager()
{
}


RasterizerStateManager::~RasterizerStateManager()
{
}

bool RasterizerStateManager::Initialize( ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext )
{
	D3D11_RASTERIZER_DESC rasterDesc;
	HRESULT result;
	device = extDevice;
	deviceContext = extDeviceContext;

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	// Setup the raster description which will determine how and what polygons will be drawn.
	// By default DirectX already has a rasterizer state set up and working the exact same as the one below, but you have no control to change it unless you set up one yourself. 
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;//D3D11_CULL_NONE
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &wireFrameState.p);
	if(FAILED(result))
	{
		return false;
	}
	
	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	// Setup the raster description which will determine how and what polygons will be drawn.
	// Special rasterizer state for the light stage of deferred rendering
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &backfaceCullingCW.p);
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	deviceContext->RSSetState(backfaceCullingCW);

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	// Setup the raster description which will determine how and what polygons will be drawn.
	// Special rasterizer state for the light stage of deferred rendering
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 1;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 1;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &depthBiasState.p);
	if(FAILED(result))
	{
		return false;
	}

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	// Setup the raster description which will determine how and what polygons will be drawn.
	// Another special rasterizer state for the light stage of deferred rendering
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &frontfaceCullingCW.p);
	if(FAILED(result))
	{
		return false;	
	}

	ZeroMemory(&rasterDesc, sizeof(rasterDesc));

	// Setup the raster description which will determine how and what polygons will be drawn.
	// Another special rasterizer state for the light stage of deferred rendering
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &noCullingCW.p);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void RasterizerStateManager::SetBackFaceCullingRasterizer()
{
	deviceContext->RSSetState(backfaceCullingCW.p);

	return;
}

void RasterizerStateManager::SetFrontFaceCullingRasterizer()
{
	deviceContext->RSSetState(frontfaceCullingCW.p);

	return;
}

void RasterizerStateManager::SetDepthBiasRasterizer()
{
	deviceContext->RSSetState(depthBiasState.p);

	return;
}

void RasterizerStateManager::SetWireframeRasterizer()
{
	deviceContext->RSSetState(wireFrameState);

	return;
}

void RasterizerStateManager::SetNoCullRasterizer()
{
	deviceContext->RSSetState(noCullingCW.p);

	return;
}