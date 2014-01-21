#include "DepthStencilManager.h"


DepthStencilManager::DepthStencilManager()
{
}


DepthStencilManager::~DepthStencilManager()
{
}

ID3D11DepthStencilView* DepthStencilManager::GetDepthStencilView()
{
	return depthStencilView.p;
}

bool DepthStencilManager::Initialize( ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext, unsigned int extScreenWidth, unsigned int extScreenHeight)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	screenWidth = extScreenWidth;
	screenHeight = extScreenHeight;
	device = extDevice;
	deviceContext = extDeviceContext;

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
	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer.p);
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
	result = device->CreateDepthStencilState(&depthDisabledStencilDesc, &depthDisabledStencilState.p);
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
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState.p);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state.
	deviceContext->OMSetDepthStencilState(depthStencilState.p, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = device->CreateDepthStencilView(depthStencilBuffer.p, &depthStencilViewDesc, &depthStencilView.p);
	if(FAILED(result))
	{
		return false;
	}

	// Clear the second depth stencil state before setting the parameters.
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

	result = device->CreateDepthStencilState(&depthStencilDesc, &lightDepthStencilState.p);
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

	// Clear the first depth stencil state before setting the parameters.
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

	result = device->CreateDepthStencilState(&depthstencil_desc, &pointLightStencilMethod1State1.p);
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

	// Clear the second depth stencil state before setting the parameters.
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

	result = device->CreateDepthStencilState(&depthstencil_desc, &pointLightStencilMethod1State2.p);
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

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

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

	result = device->CreateDepthStencilState(&depthStencilDesc, &pointLightStencilMethod2State1.p);
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

	// Clear the second depth stencil state before setting the parameters.
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

	result = device->CreateDepthStencilState(&depthStencilDesc, &pointLightStencilMethod2State2.p);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void DepthStencilManager::SetLightStencilMethod1Phase1()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod1State1.p, 0);
	return;
}

void DepthStencilManager::SetLightStencilMethod1Phase2()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod1State2.p, 0);
	return;
}

void DepthStencilManager::SetLightStencilMethod2Phase1()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod2State1.p, 0);
	return;
}

void DepthStencilManager::SetLightStencilMethod2Phase2()
{
	deviceContext->OMSetDepthStencilState(pointLightStencilMethod2State2.p, 0);
	return;
}

void DepthStencilManager::SetDepthDisabledStencilState()
{
	deviceContext->OMSetDepthStencilState(depthDisabledStencilState.p, 1);
	return;
}

void DepthStencilManager::SetDefaultDepthStencilView()
{
	deviceContext->OMSetDepthStencilState(depthStencilState.p, 1);
	return;
}
