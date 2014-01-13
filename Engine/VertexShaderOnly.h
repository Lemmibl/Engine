#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>

class VertexShaderOnly
{
private:

	struct MatrixBufferType
	{
		XMMATRIX WorldViewProjection;
	};

public:
	VertexShaderOnly();
	VertexShaderOnly(const VertexShaderOnly&);
	~VertexShaderOnly();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX* worldViewProjection);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX* worldViewProjection);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11Buffer> matrixBuffer;
};