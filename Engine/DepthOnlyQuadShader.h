#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>

class DepthOnlyQuadShader
{
private:

	struct MatrixBufferType
	{
		XMMATRIX WorldViewProjection;
		XMMATRIX WorldView;
	};

public:
	DepthOnlyQuadShader();
	DepthOnlyQuadShader(const DepthOnlyQuadShader&);
	~DepthOnlyQuadShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int vertexCount, int instanceCount, XMMATRIX* worldViewProjection, XMMATRIX* worldView, ID3D11ShaderResourceView** textures);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX* worldViewProjection, XMMATRIX* worldView, ID3D11ShaderResourceView** textures);
	void RenderShader(ID3D11DeviceContext*, int vertexCount, int instanceCount);

private:
	CComPtr<ID3D11VertexShader>	vertexShader;
	CComPtr<ID3D11PixelShader>	pixelShader;
	CComPtr<ID3D11InputLayout>	layout;
	CComPtr<ID3D11Buffer>		matrixBuffer;
	CComPtr<ID3D11SamplerState> samplerState;
};