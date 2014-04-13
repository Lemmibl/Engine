#pragma once
#include "settingsdependent.h"
#include <d3d11.h>
#include <atlcomcli.h>
#include <xnamath.h>

class SSAOShader : public SettingsDependent
{
public:
	struct SSAOShaderInput
	{
		ID3D11ShaderResourceView** rtTextureArray;
		XMMATRIX* worldViewProjection;
	};

private:
	struct VertexShaderBuffer
	{
		XMMATRIX worldViewProjection;
		float thFOV;
		float aspectRatio;
		XMFLOAT2 PADDING;
	};

public:
	SSAOShader();
	~SSAOShader();

	virtual void OnSettingsReload(Config* cfg);

	bool Initialize(ID3D11Device* device, HWND hwnd);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, SSAOShaderInput& input);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderName, WCHAR* pixelShaderName);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* filename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, SSAOShaderInput& input);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> sampler;

	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
	CComPtr<ID3D11Buffer> pixelMatrixBuffer;
	CComPtr<ID3D11Buffer> variableBuffer;
	float thFov, aspectRatio;
};

