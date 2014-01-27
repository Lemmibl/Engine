#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsDependent.h"

class DRCompose : public SettingsDependent
{
private:
	struct VertexMatrixBuffer
	{
		XMMATRIX WorldViewProjection;
		XMMATRIX WorldView;
	};

	struct PixelMatrixBuffer
	{
		XMMATRIX View;
		XMMATRIX InvertedProjection;
		XMFLOAT4 FogColor;
	};

	struct VariableBuffer
	{
		int toggleSSAO;
		float sampleRadius;	//Controls sampling radius. 0.1f to 1.0f are pretty ok values.
		float intensity; 	//AO intensity. The higher this value is, the darker the occluded parts will be. 1.0f to 10.0f values is pretty ok values.
		float scale;		//Scales distance between occluders and occludee. Still a little unsure as to what values would be good to use.
		float bias;			//Cutoff value. The higher this value is, the harsher we are with cutting off low AO values. 0.01f to 0.4f values are pretty ok.
		float fogStart;
		float fogEnd;
		float farClip;
		XMFLOAT2 randomSize;
		XMFLOAT2 screenSize;
	};


public:
	DRCompose();
	~DRCompose();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void OnSettingsReload(Config* cfg);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invViewProjection,
		XMFLOAT4* fogColor, float fogMinimum, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* randomTexture, int toggle);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invViewProjection, 
		XMFLOAT4* fogColor, float fogMinimum, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* randomTexture, int toggle);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> samplers[2];

	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
	CComPtr<ID3D11Buffer> pixelMatrixBuffer;
	CComPtr<ID3D11Buffer> variableBuffer;

	bool bufferNeedsUpdate;
	VariableBuffer variables;
};