////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dmanager.h"

#pragma region Properties
ID3D11Device* D3DManager::GetDevice()
{
	return device;
}

ID3D11DeviceContext* D3DManager::GetDeviceContext()
{
	return deviceContext;
}

void D3DManager::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
	return;
}

//ID3D11RenderTargetView* D3DManager::GetBackBuffer()
//{
//	return renderTargetView.p;
//}
#pragma endregion

D3DManager::D3DManager()
{
}

D3DManager::~D3DManager()
{
}

bool D3DManager::Initialize(HWND hwnd, bool vsync, bool fullscreen, float screenNear, float screenFar, 
	int screenWidth, int screenHeight, UINT shadowMapWidth, UINT shadowMapHeight)
{
	unsigned int numModes, i, numerator, denominator, stringLength;
	int errorCode;
	HRESULT result;
	D3D_FEATURE_LEVEL featureLevel; //enum

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D* backBufferPtr;

	this->hwnd = hwnd;

	// Store the vsync setting.
	vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if(displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	errorCode = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if(errorCode != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if(fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	//Using D3D11_CREATE_DEVICE_BGRA_SUPPORT flag because of CEGUI. -> http://cegui.org.uk/forum/viewtopic.php?f=1&t=5490
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain.p, &device.p, NULL, &deviceContext.p);
	if(FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer.
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView.p);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	deviceContext->RSSetViewports(1, &viewport);

	if(!blendStateManager.Initialize(device, deviceContext))
	{
		MessageBox(hwnd, L"Could not initialize the blend state manager.", L"Error", MB_OK);
		return false;
	}

	if(!rasterizerStateManager.Initialize(device, deviceContext))
	{
		MessageBox(hwnd, L"Could not initialize the rasterizer state manager.", L"Error", MB_OK);
		return false;
	}

	if(!shadowManager.Initialize(device, deviceContext, shadowMapWidth, shadowMapHeight))
	{
		MessageBox(hwnd, L"Could not initialize the shadow manager.", L"Error", MB_OK);
		return false;
	}

	if(!depthStencilManager.Initialize(device, deviceContext, static_cast<unsigned int>(screenWidth), static_cast<unsigned int>(screenHeight)))
	{
		MessageBox(hwnd, L"Could not initialize the depth stencil manager.", L"Error", MB_OK);
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &renderTargetView.p, depthStencilManager.GetDepthStencilView());

	return true;
}

void D3DManager::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(swapChain)
	{
		swapChain->SetFullscreenState(false, NULL);
	}
}

void D3DManager::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(renderTargetView.p, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilManager.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3DManager::PresentFrame()
{
	// Present the back buffer to the screen since rendering is complete.
	if(vsync_enabled)
	{
		// Lock to screen refresh rate.
		swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		swapChain->Present(0, 0);
	}

}

void D3DManager::ResetBackBufferRenderTarget()
{
	auto temp = depthStencilManager.GetDepthStencilView();

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(temp, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &renderTargetView.p, temp);
}

void D3DManager::SetDefaultViewport()
{
	deviceContext->RSSetViewports(1, &viewport);
}
