#pragma once
#include <atlcomcli.h>
#include <d3d11.h>

class BlendStateManager
{
public:
	BlendStateManager();
	~BlendStateManager();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void TurnOnDefaultBlendState();
	void TurnOnAlphaBlending();
	void TurnOnTransparencyBlending();
	void TurnOnLightBlending();
	void TurnOnShadowBlendState();
	void TurnOffColorBlending();

private:
	CComPtr<ID3D11BlendState> defaultBlendstate;
	CComPtr<ID3D11BlendState> alphaEnableBlendingState;
	CComPtr<ID3D11BlendState> shadowBlendState;
	CComPtr<ID3D11BlendState> lightAddBlendState;
	CComPtr<ID3D11BlendState> colorDisabledBlendState;
	CComPtr<ID3D11BlendState> transparencyBlendstate;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
};

