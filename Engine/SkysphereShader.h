////////////////////////////////////////////////////////////////////////////////
// Filename: skysphereshader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SKYSPHERESHADER_H_
#define _SKYSPHERESHADER_H_


//////////////
// INCLUDES //
//////////////
#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: SkysphereShader
////////////////////////////////////////////////////////////////////////////////
class SkysphereShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX WorldViewProjection;
	};

	struct GradientBufferType
	{
		XMFLOAT4 ApexColor;
		XMFLOAT4 CenterColor;
		XMFLOAT4 FogColor;
	};

public:
	SkysphereShader();
	SkysphereShader(const SkysphereShader&);
	~SkysphereShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX* worldViewProjection, 
	XMFLOAT4 ApexColor, XMFLOAT4 CenterColor, XMFLOAT4 FogColor, float time);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* context, XMMATRIX* worldViewProjection, 
		XMFLOAT4 ApexColor, XMFLOAT4 CenterColor, XMFLOAT4 FogColor, float time);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> gradientBuffer;
};

#endif