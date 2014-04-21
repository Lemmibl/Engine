#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsDependent.h"
#include <stdio.h>
#include <d3dcompiler.h>
#include <vector>

class SSAOShader : public SettingsDependent
{
public:
	struct SSAOShaderInput
	{
		ID3D11ShaderResourceView** rtTextureArray;
		XMMATRIX* worldViewProjection;
		XMMATRIX* view;
		XMMATRIX* projection;
	};

private:
	struct VertexShaderBuffer
	{
		XMMATRIX worldViewProjection;
		float thFOV;
		float aspectRatio;
		XMFLOAT2 PADDING;
	};

	struct PixelShaderBuffer 
	{
		XMMATRIX View;
		XMMATRIX Projection;
		float thFOV;
		float aspectRatio;
		XMFLOAT2 PADDING;
		XMFLOAT4 samplingRays[14];
	};

public:
	SSAOShader();
	~SSAOShader();

	virtual void OnSettingsReload(Config* cfg);

	bool Initialize(ID3D11Device* device, HWND hwnd);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, SSAOShaderInput& input);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderName, WCHAR* pixelShaderName);
	void OutputShaderErrorMessage(ID3D10Blob* blob, HWND hwnd, WCHAR* filename);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, SSAOShaderInput& input);

	void RenderShader(ID3D11DeviceContext*, int);

	void BuildSamplingRays();

	float RandomFloat(float min, float max) 
	{
			float random = ((float) rand()) / (float) RAND_MAX;
			float diff = max - min;
			float r = random * diff;
			return min + r;
	}

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> sampler;

	CComPtr<ID3D11Buffer> vertexShaderBuffer;
	CComPtr<ID3D11Buffer> pixelShaderBuffer;
	//CComPtr<ID3D11Buffer> variableBuffer;
	float thFov, aspectRatio;
	std::vector<XMFLOAT4> samplingRays;
	static const unsigned int rayCount = 14;
	bool matrixCreated;
	XMFLOAT4X4 projMatrix;
};

