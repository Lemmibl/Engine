#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsDependent.h"

class SSAOBlur : public SettingsDependent
{
private:
	struct VertexMatrixBuffer
	{
		XMMATRIX WorldViewProjection;
	};

	struct PixelBuffer
	{
		int horizontalBlur;
		float acceptableNormalDifference;
		float acceptableDepthDifference;
		float averagingOffset;
	};

public:
	SSAOBlur();
	~SSAOBlur();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	bool RenderBlurX(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** texture);

	bool RenderBlurY(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** texture);

	virtual void OnSettingsReload(Config* cfg);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFilename, WCHAR* pixelShaderFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** texture, bool blurHorizontal);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> sampler;
	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
	CComPtr<ID3D11Buffer> pixelBuffer;

	float acceptableNormalDifference;
	float acceptableDepthDifference;
	float averagingOffset;
};