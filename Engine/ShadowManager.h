#pragma once
#include <d3d11.h>
#include <atlcomcli.h>

class ShadowManager
{
public:
	ShadowManager();
	~ShadowManager();

	bool Initialize(ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext, unsigned int shadowMapWidth, unsigned int shadowMapHeight);

	void SetShadowViewport();

	ID3D11ShaderResourceView* GetShadowmapSRV();
	ID3D11DepthStencilView* GetShadowmapDSV();

private:
	ID3D11Device* device;
	ID3D11DeviceContext*  deviceContext;

	CComPtr<ID3D11Texture2D>			shadowmapTexture;
	CComPtr<ID3D11Texture2D>			shadowmapStencilTexture;
	CComPtr<ID3D11ShaderResourceView>	shadowmapResourceView;
	CComPtr<ID3D11DepthStencilView>		shadowmapDSV;
	D3D11_VIEWPORT						shadowmapViewport;
};

