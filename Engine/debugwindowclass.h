////////////////////////////////////////////////////////////////////////////////
// Filename: debugwindowclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEBUGWINDOWCLASS_H_
#define _DEBUGWINDOWCLASS_H_


//////////////
// INCLUDES //
//////////////
#pragma once

#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <atlcomcli.h>
#include <vector>


////////////////////////////////////////////////////////////////////////////////
// Class name: DebugWindowClass
////////////////////////////////////////////////////////////////////////////////
class DebugWindowClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	DebugWindowClass();
	DebugWindowClass(const DebugWindowClass&);
	~DebugWindowClass();

	bool Initialize(ID3D11Device* device, int screenWidth, int screenHeight, int bitmapWidth, int bitmapHeight);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	CComPtr<ID3D11Buffer> vertexBuffer;
	CComPtr<ID3D11Buffer> indexBuffer;
	int vertexCount, indexCount;
	int screenWidth, screenHeight;
	int bitmapWidth, bitmapHeight;
	int previousPosX, previousPosY;
};

#endif