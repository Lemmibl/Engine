////////////////////////////////////////////////////////////////////////////////
// Filename: TextureArray.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TEXTUREARRAY_H_
#define _TEXTUREARRAY_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11tex.h>


////////////////////////////////////////////////////////////////////////////////
// Class name: TextureArray
// Description: Container class for an array of textures.
////////////////////////////////////////////////////////////////////////////////
class TextureArray
{
public:
	TextureArray();
	TextureArray(const TextureArray&);
	~TextureArray();

	//bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	ID3D11ShaderResourceView* textures[3];
};

#endif

