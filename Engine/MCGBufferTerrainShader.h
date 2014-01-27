#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsDependent.h"

class MCGBufferTerrainShader : SettingsDependent
{

private:
	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX WorldView;
		XMMATRIX WorldViewProjection;
	};

	struct VariableBuffer
	{
		float farClip;
		float tighten;
		float textureScale;
		int colorMode;
	};

public:
	MCGBufferTerrainShader();
	~MCGBufferTerrainShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void OnSettingsReload(Config* cfg);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
		XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texAndMatLookupTable, int toggleColor);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
		XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texAndMatLookupTable, int toggleColor);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> variableBuffer;
	CComPtr<ID3D11SamplerState> sampler;

	bool bufferNeedsUpdate;
	VariableBuffer variables;
};