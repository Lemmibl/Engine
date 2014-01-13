#pragma once
#include <d3d11.h>
#include <d3dx11tex.h>
#include <atlcomcli.h>

class TextureClass
{
public:
	TextureClass();
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	CComPtr<ID3D11ShaderResourceView> texture;
};
