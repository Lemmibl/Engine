#include "textureclass.h"

TextureClass::TextureClass()
{
}


TextureClass::~TextureClass()
{
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return texture.p;
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	// Load the texture in.
	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &texture.p, NULL);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	//// Release the texture resource.
	//if(texture)
	//{
	//	texture->Release();
	//	texture = 0;
	//}

	return;
}