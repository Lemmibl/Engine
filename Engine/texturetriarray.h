////////////////////////////////////////////////////////////////////////////////
// Filename: TextureTriArray.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTURETRIARRAY_H_
#define _TEXTURETRIARRAY_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11tex.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureTriArray
// Description: Container class for an array of (3) textures.
////////////////////////////////////////////////////////////////////////////////
class TextureTriArray
{
public:
	TextureTriArray();
	TextureTriArray(const TextureTriArray&);
	~TextureTriArray();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	ID3D11ShaderResourceView* textures[3];
};

#endif

