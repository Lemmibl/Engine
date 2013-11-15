////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <atlcomcli.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: RenderToTextureClass
////////////////////////////////////////////////////////////////////////////////
class RenderToTextureClass
{
public:
	RenderToTextureClass();
	RenderToTextureClass(const RenderToTextureClass&);
	~RenderToTextureClass();

	bool Initialize(ID3D11Device*,  int textureWidth, int textureHeight);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	CComPtr<ID3D11Texture2D> renderTargetTexture;
	CComPtr<ID3D11RenderTargetView> renderTargetView;
	CComPtr<ID3D11ShaderResourceView> shaderResourceView;
};