#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsDependent.h"

class DepthOnlyShader : public SettingsDependent
{
private:

	struct MatrixBufferType
	{
		XMMATRIX worldViewProjection;
		XMMATRIX worldView;
	};

	struct FarClipBuffer
	{
		float farClip;
		XMFLOAT3 padding; //..yeah... : /
	};

public:
	DepthOnlyShader();
	~DepthOnlyShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void OnSettingsReload(Config* cfg);
	bool Render(ID3D11DeviceContext*, int, XMMATRIX* worldViewProjection, XMMATRIX* worldView);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX* worldViewProjection, XMMATRIX* worldView);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> farClipBuffer;

	float farClip;
	bool bufferNeedsUpdate;
};