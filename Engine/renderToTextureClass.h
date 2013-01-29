////////////////////////////////////////////////////////////////////////////////
// Filename: renderToTextureClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _RENDERTTOEXTURECLASS_H_
#define _RENDERTTOEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>

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
	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11ShaderResourceView* shaderResourceView;
};

#endif