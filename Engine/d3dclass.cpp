////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

#pragma region Properties
ID3D11Device* D3DClass::GetDevice()
{
	return device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = XMLoadFloat4x4(&this->projectionMatrix);
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = XMLoadFloat4x4(&this->worldMatrix);
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = XMLoadFloat4x4(&this->orthoMatrix);
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, videoCardDescription);
	memory = videoCardMemory;
	return;
}

ID3D11DepthStencilView* D3DClass::GetDepthStencilView()
{
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);
	return depthStencilView;
}

ID3D11DepthStencilView* D3DClass::GetShadowmapDSV()
{
	return shadowmapDSV;
}

ID3D11ShaderResourceView* D3DClass::GetShadowmapSRV()
{
	return shadowmapResourceView;
}

ID3D11RenderTargetView* D3DClass::GetBackBuffer()
{
	return renderTargetView;
}
#pragma endregion

D3DClass::D3DClass()
{
	swapChain = 0;
	device = 0;
	deviceContext = 0;
	renderTargetView = 0;
	depthStencilBuffer = 0;
	depthStencilState = 0;
	depthStencilView = 0;
	rasterState = 0;
	backfaceCullingCW = 0;
	frontfaceCullingCW = 0;

	depthDisabledStencilState = 0;
	lightDepthStencilState = 0;
	pointLightStencilMethod1State1 = 0;
	pointLightStencilMethod1State2 = 0;

	pointLightStencilMethod2State1 = 0;
	pointLightStencilMethod2State2 = 0;

	shadowmapDSV = 0;
	shadowmapTexture = 0;
	shadowmapResourceView = 0;

	alphaEnableBlendingState = 0;
	alphaDisableBlendingState = 0;
	lightAddBlendState = 0;
	colorDisabledBlendState = 0;
	previousBlendState = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(HWND hwnd, bool vsync, bool fullscreen, float screenNear, float screenFar, 
	int screenWidth, int screenHeight, UINT shadowMapWidth, UINT shadowMapHeight)
{
	unsigned int numModes, i, numerator, denominator, stringLength;
	int error;
	float fieldOfView, screenAspect;
	HRESULT result;
	D3D_FEATURE_LEVEL featureLevel; //enum

	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;

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
	error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
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
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
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
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	#pragma region Viewport and matrix setup
	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	deviceContext->RSSetViewports(1, &viewport);

	// Setup the projection matrix.
	fieldOfView = XM_PIDIV4;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	 XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH((FLOAT)fieldOfView, (FLOAT)screenAspect, (FLOAT)screenNear, (FLOAT)screenFar));

	// Initialize the world matrix to the identity matrix.
	 XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());

	// Create an orthographic projection matrix for 2D rendering.
	 XMStoreFloat4x4(&orthoMatrix, XMMatrixOrthographicLH((FLOAT)screenWidth, (FLOAT)screenHeight, (FLOAT)screenNear, (FLOAT)screenFar));
	#pragma endregion

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = device->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if(FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = device->CreateDepthStencilState(&depthStencilDesc, &lightDepthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	//ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	
	/* http://www.altdevblogaday.com/2011/08/08/stencil-buffer-optimisation-for-deferred-lights/ */

	//Pass 1:

	//The important bits we’re doing here are leaving depth test on as normal, setting depth writes off, 
	//enabling stencil and setting the stencil depthfail operation for both front and back faces to D3D11_STENCIL_OP_INVERT. 
	//If your graphics API does not support an INVERT operator, you can use INCREMENT on the back faces and DECREMENT on the front faces.

	//Then we render the light geometry with back face culling off and no pixel shader. 
	//It is important that the stencil buffer is completely clear before this is done. 
	//The stencil buffer is then filled as in the next screenshot.

	D3D11_DEPTH_STENCIL_DESC depthstencil_desc;
	ZeroMemory(&depthstencil_desc, sizeof(depthstencil_desc));

	depthstencil_desc.DepthEnable = TRUE;
	depthstencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthstencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	depthstencil_desc.StencilEnable = TRUE;
	depthstencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthstencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthstencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
	depthstencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthstencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthstencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
	depthstencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	result = device->CreateDepthStencilState(&depthstencil_desc, &pointLightStencilMethod1State1);
	if(FAILED(result))
	{
		return false;
	}

	//Pass 2:

	//The important bits here are that depth testing is off, and the stencil test for back faces is set to D3D11_COMPARISON_NOT_EQUAL, 
	//which will mean only pixels with a non-zero stencil will be shaded. 

	//We also set the StencilPassOp for back faces to D3D_STENCIL_OP_ZERO so the stencil buffer is cleared for the next light. 
	//The FrontFace settings are ignored as we’re only going to render back faces in this stage.
	//Then we render the light geometry with the full pixel shader and front faces culled. 
	//The visual result is exactly the same.

	//The next and last stage is to render the geometry again with the following depth stencil state.

	ZeroMemory(&depthstencil_desc, sizeof(depthstencil_desc));

	depthstencil_desc.DepthEnable = FALSE;
	depthstencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthstencil_desc.DepthFunc = D3D11_COMPARISON_GREATER;

	depthstencil_desc.StencilEnable = TRUE;
	depthstencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthstencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthstencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
	depthstencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	depthstencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthstencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	depthstencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	depthstencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	result = device->CreateDepthStencilState(&depthstencil_desc, &pointLightStencilMethod1State2);
	if(FAILED(result))
	{
		return false;
	}



	/*Pass 1 ************* Method from Killzone 2 whitepaper
	

	Marks pixels in front of the far light boundary

	Render back-faces of light volume
	Depth test GREATER-EQUAL
	Write to stencil on depth pass
	Skipped for very small distant lights
	*/

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	result = device->CreateDepthStencilState(&depthStencilDesc, &pointLightStencilMethod2State1);
	if(FAILED(result))
	{
		return false;
	}

	
	/*Pass 2 *************
	Find amount of lit pixels inside the volume
	Start pixel query
	Render front faces of light volume
	Depth test LESS-EQUAL
	Don’t write anything – only EQUAL stencil test
	*/

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	result = device->CreateDepthStencilState(&depthStencilDesc, &pointLightStencilMethod2State2);
	if(FAILED(result))
	{
		return false;
	}



	// Setup the raster description which will determine how and what polygons will be drawn.
	// By default DirectX already has a rasterizer state set up and working the exact same as the one below, but you have no control to change it unless you set up one yourself. 
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;//D3D11_CULL_NONE
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	deviceContext->RSSetState(rasterState);

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
	result = device->CreateRasterizerState(&rasterDesc, &backfaceCullingCW);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the raster description which will determine how and what polygons will be drawn.
	// Another special rasterizer state for the light stage of deferred rendering
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = device->CreateRasterizerState(&rasterDesc, &frontfaceCullingCW);
	if(FAILED(result))
	{
		return false;	
	}

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
	result = device->CreateRasterizerState(&rasterDesc, &noCullingCW);
	if(FAILED(result))
	{
		return false;
	}


	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	//To create an alpha enabled blend state description change BlendEnable to TRUE and DestBlend to D3D11_BLEND_INV_SRC_ALPHA. 
	//The other settings are set to their default values which can be looked up in the Windows DirectX Graphics Documentation. 
	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState); //Enable
	if(FAILED(result))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaDisableBlendingState); //Disable
	if(FAILED(result))
	{
		return false;
	}

	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	for (UINT i = 0; i < 8; ++i)
	{
		blendStateDescription.RenderTarget[i].BlendEnable = true;
		blendStateDescription.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendStateDescription.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
	}

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &lightAddBlendState); //Additive blend state for light mapping
	if(FAILED(result))
	{
		return false;
	}

	blendStateDescription.AlphaToCoverageEnable = false;
	blendStateDescription.IndependentBlendEnable = false;
	for (UINT i = 0; i < 8; ++i)
	{
		blendStateDescription.RenderTarget[i].BlendEnable = false;
		blendStateDescription.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendStateDescription.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].RenderTargetWriteMask = 0;
		blendStateDescription.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendStateDescription.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
	}


	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &colorDisabledBlendState); //Color disabled state for pointlight phase
	if(FAILED(result))
	{
		return false;
	}

#pragma region Shadowmap things
	D3D11_TEXTURE2D_DESC texDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

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
	result = device->CreateTexture2D(&texDesc, NULL, &shadowmapTexture);
	if(FAILED(result))
	{
		return false;
	}

	// Create the depth stencil view desc
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0;

	result = device->CreateDepthStencilView(shadowmapTexture, &dsvDesc, &shadowmapDSV);
	if(FAILED(result))
	{
		return false;
	}


	////create shader resource view desc
	//srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	//srvDesc.Texture2D.MostDetailedMip = 0;

	//result = device->CreateShaderResourceView(shadowmapTexture, &srvDesc, &shadowmapResourceView);
	//if(FAILED(result))
	//{
	//	return false;
	//}


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
	result = device->CreateTexture2D(&texDesc, NULL, &shadowmapTexture);
	if(FAILED(result))
	{
		return false;
	}

	//create shader resource view desc
	srvDesc.Format = DXGI_FORMAT_R16G16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(shadowmapTexture, &srvDesc, &shadowmapResourceView);
	if(FAILED(result))
	{
		return false;
	}

	//Create shadowmap viewport
	shadowmapViewport.Width = (float)shadowMapWidth;
	shadowmapViewport.Height = (float)shadowMapHeight;
	shadowmapViewport.MinDepth = 0.0f;
	shadowmapViewport.MaxDepth = 1.0f;
	shadowmapViewport.TopLeftX = 0.0f;
	shadowmapViewport.TopLeftY = 0.0f;
#pragma endregion


	return true;
}

void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(swapChain)
	{
		swapChain->SetFullscreenState(false, NULL);
	}

	if(depthDisabledStencilState)
	{
		depthDisabledStencilState->Release();
		depthDisabledStencilState = 0;
	}

	if(pointLightStencilMethod1State1)
	{
		pointLightStencilMethod1State1->Release();
		pointLightStencilMethod1State1 = 0;
	}

	if(pointLightStencilMethod1State2)
	{
		pointLightStencilMethod1State2->Release();
		pointLightStencilMethod1State2 = 0;
	}

	if(pointLightStencilMethod2State1)
	{
		pointLightStencilMethod2State1->Release();
		pointLightStencilMethod2State1 = 0;
	}

	if(pointLightStencilMethod2State2)
	{
		pointLightStencilMethod2State2->Release();
		pointLightStencilMethod2State2 = 0;
	}

	if(alphaEnableBlendingState)
	{
		alphaEnableBlendingState->Release();
		alphaEnableBlendingState = 0;
	}

	if(alphaDisableBlendingState)
	{
		alphaDisableBlendingState->Release();
		alphaDisableBlendingState = 0;
	}

	if(lightAddBlendState)
	{
		lightAddBlendState->Release();
		lightAddBlendState = 0;
	}

	if(colorDisabledBlendState)
	{
		colorDisabledBlendState->Release();
		colorDisabledBlendState = 0;
	}

	if(previousBlendState)
	{
		previousBlendState->Release();
		previousBlendState = 0;
	}

	if(rasterState)
	{
		rasterState->Release();
		rasterState = 0;
	}

	if(backfaceCullingCW)
	{
		backfaceCullingCW->Release();
		backfaceCullingCW = 0;
	}

	if(frontfaceCullingCW)
	{
		frontfaceCullingCW->Release();
		frontfaceCullingCW = 0;
	}

	if(depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}

	if(depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = 0;
	}

	if(depthStencilBuffer)
	{
		depthStencilBuffer->Release();
		depthStencilBuffer = 0;
	}

	if(renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = 0;
	}

	if(deviceContext)
	{
		deviceContext->Release();
		deviceContext = 0;
	}

	if(device)
	{
		device->Release();
		device = 0;
	}

	if(swapChain)
	{
		swapChain->Release();
		swapChain = 0;
	}

	if(lightDepthStencilState)
	{
		lightDepthStencilState->Release();
		lightDepthStencilState = 0;
	}

	if(shadowmapDSV)
	{
		shadowmapDSV->Release();
		shadowmapDSV = 0;
	}

	if(shadowmapTexture)
	{
		shadowmapTexture->Release();
		shadowmapTexture = 0;
	}

	if(shadowmapResourceView)
	{
		shadowmapResourceView->Release();
		shadowmapResourceView = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
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

	return;
}

void D3DClass::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	return;
}

void D3DClass::SetDefaultViewport()
{
	deviceContext->RSSetViewports(1, &viewport);
}

void D3DClass::SetShadowViewport()
{
	deviceContext->RSSetViewports(1, &shadowmapViewport);
}

#pragma region Rasterizer functions
void D3DClass::ChangeRasterizerState( ID3D11RasterizerState* rasterizerState )
{
	deviceContext->RSSetState(rasterizerState);

	return;
}

void D3DClass::SetBackFaceCullingRasterizer()
{
	deviceContext->RSSetState(backfaceCullingCW);

	return;
}

void D3DClass::ResetRasterizerState()
{
	deviceContext->RSSetState(rasterState);

	return;
}

void D3DClass::SetFrontFaceCullingRasterizer()
{
	deviceContext->RSSetState(frontfaceCullingCW);

	return;
}

void D3DClass::SetNoCullRasterizer()
{
	deviceContext->RSSetState(noCullingCW);

	return;
}
#pragma endregion

#pragma region Blend functions
void D3DClass::TurnOnLightBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(lightAddBlendState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOffColorBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(colorDisabledBlendState, blendFactor, 0xffffffff);
}

void D3DClass::TurnOnAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}

void D3DClass::TurnOffAlphaBlending()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn off the alpha blending.
	deviceContext->OMSetBlendState(alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}


void D3DClass::ChangeBlendState(ID3D11BlendState* blendState)
{
	float blendFactor[4];
	UINT sampleMask;

	// Get the old blendstate
	deviceContext->OMGetBlendState(&previousBlendState, blendFactor, &sampleMask);

	// Turn on the new blendstate
	deviceContext->OMSetBlendState(blendState, blendFactor, sampleMask);
}

void D3DClass::ResetBlendState()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(previousBlendState, blendFactor, 0xffffffff);
}
#pragma endregion

#pragma region Stencil functions
//void D3DClass::SetShadowMapStencilState()
//{
//	deviceContext->OMSetDepthStencilState(lightDepthStencilState, 1);
//	return;
//}

void D3DClass::SetLightStencilMethod1Phase1()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod1State1, 0);
	return;
}

void D3DClass::SetLightStencilMethod1Phase2()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod1State2, 0);
	return;
}

void D3DClass::SetLightStencilMethod2Phase1()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod2State1, 0);
	return;
}

void D3DClass::SetLightStencilMethod2Phase2()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod2State2, 0);
	return;
}

void D3DClass::TurnZBufferOn()
{
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);
	return;
}

void D3DClass::TurnZBufferOff()
{
	deviceContext->OMSetDepthStencilState(depthDisabledStencilState, 1);
	return;
}
#pragma endregion