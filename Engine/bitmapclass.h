////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_


//////////////
// INCLUDES //
//////////////
#pragma once

#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include <atlcomcli.h>
#include <memory>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
// Class name: BitmapClass
////////////////////////////////////////////////////////////////////////////////
class BitmapClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	CComPtr<ID3D11Buffer> vertexBuffer;
	CComPtr<ID3D11Buffer> indexBuffer;
	int vertexCount, indexCount;
	std::shared_ptr<TextureClass> texture;

	int screenWidth, screenHeight;
	int screenCenterX, screenCenterY;
	int bitmapWidth, bitmapHeight;
	int previousPosX, previousPosY;
};

#endif