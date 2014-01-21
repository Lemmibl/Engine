#include "BlendStateManager.h"


BlendStateManager::BlendStateManager(void)
{
}


BlendStateManager::~BlendStateManager(void)
{
}

bool BlendStateManager::Initialize(ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext)
{
	HRESULT result;
	D3D11_BLEND_DESC blendStateDescription;

	device = extDevice;
	deviceContext = extDeviceContext;

	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	blendStateDescription.AlphaToCoverageEnable = FALSE;
	blendStateDescription.IndependentBlendEnable = FALSE;

	// Create an transparency enabled blend state description.
	for (UINT i = 0; i < 8; ++i)
	{
		blendStateDescription.RenderTarget[i].BlendEnable = FALSE;
		blendStateDescription.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &defaultBlendstate.p);
	if(FAILED(result))
	{
		return false;
	}

	blendStateDescription.IndependentBlendEnable = TRUE;

	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	for(UINT i = 1; i < 8; i++)
	{
		blendStateDescription.RenderTarget[i].BlendEnable = FALSE;
		blendStateDescription.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &transparencyBlendstate.p);
	if(FAILED(result))
	{
		return false;
	}

	//To create an alpha enabled blend state description change BlendEnable to TRUE and DestBlend to D3D11_BLEND_INV_SRC_ALPHA. 
	//The other settings are set to their default values which can be looked up in the Windows DirectX Graphics Documentation. 
	// Create an alpha enabled blend state description.
	for (UINT i = 0; i < 8; ++i)
	{
		blendStateDescription.RenderTarget[i].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].RenderTargetWriteMask = 0x0F;
	}

	blendStateDescription.AlphaToCoverageEnable = true;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState.p); //Enable
	if(FAILED(result))
	{
		return false;
	}

	//To create an alpha enabled blend state description change BlendEnable to TRUE and DestBlend to D3D11_BLEND_INV_SRC_ALPHA. 
	//The other settings are set to their default values which can be looked up in the Windows DirectX Graphics Documentation. 
	// Create an alpha enabled blend state description.
	for (UINT i = 0; i < 8; ++i)
	{
		blendStateDescription.RenderTarget[i].BlendEnable = TRUE;
		blendStateDescription.RenderTarget[i].SrcBlend = D3D11_BLEND_ZERO;
		blendStateDescription.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
		blendStateDescription.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		blendStateDescription.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendStateDescription.RenderTarget[i].RenderTargetWriteMask = 0x0F;
	}

	blendStateDescription.AlphaToCoverageEnable = true;

	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &shadowBlendState.p); //Enable
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
	result = device->CreateBlendState(&blendStateDescription, &lightAddBlendState.p); //Additive blend state for light mapping
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
	result = device->CreateBlendState(&blendStateDescription, &colorDisabledBlendState.p); //Color disabled state for pointlight phase
	if(FAILED(result))
	{
		return false;
	}

	TurnOnDefaultBlendState();

	return true;
}

#pragma region Blend functions
void BlendStateManager::TurnOnLightBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(lightAddBlendState.p, blendFactor, 0xffffffff);
}

void BlendStateManager::TurnOffColorBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(colorDisabledBlendState.p, blendFactor, 0xffffffff);
}

void BlendStateManager::TurnOnAlphaBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(alphaEnableBlendingState.p, blendFactor, 0xffffffff);

	return;
}

void BlendStateManager::TurnOnShadowBlendState()
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(shadowBlendState.p, blendFactor, 0xffffffff);
}

void BlendStateManager::TurnOnDefaultBlendState()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(defaultBlendstate.p, blendFactor, 0xffffffff);
}

void BlendStateManager::TurnOnTransparencyBlending()
{
	float blendFactor[4];

	// Setup the blend factor.
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(transparencyBlendstate.p, blendFactor, 0xffffffff);
}
#pragma endregion