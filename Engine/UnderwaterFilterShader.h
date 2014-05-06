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

#include "ShaderInputStructs.h"

class UnderwaterFilterShader : public SettingsDependent
{
private:
	struct VertexMatrixBuffer
	{
		XMMATRIX WorldViewProjection;
	};

	struct PixelBuffer
	{
		XMFLOAT4 FogColor;

		XMFLOAT3 ColorMapDampeningScale;  // = float3(0.3f, 0.3f, 0.0f);
		float fogStart;

		XMFLOAT3 FogColorDampeningScale; // = float3(0.15f, 0.1f, 0.0f);
		float fogEnd;

		float cameraHeight;
		float waterDepthFogScaling; // = 0.3f;
		float waterVisionRange; // = 0.04f;
		float waterMinVisionRange; // = 0.2f;

		float waterMaxVisionRange; // = 1.0f;
		float farClip;
		float waterLevel;
		float PADDING;
	};

	/*
	float sampleRadius;	//Controls sampling radius. 0.1f to 1.0f are pretty ok values.
	float intensity; 	//AO intensity. The higher this value is, the darker the occluded parts will be. 1.0f to 10.0f values is pretty ok values.
	float scale;		//Scales distance between occluders and occludee. Still a little unsure as to what values would be good to use.
	float bias;			//Cutoff value. The higher this value is, the harsher we are with cutting off low AO values. 0.01f to 0.4f values are pretty ok.
	 */

public:
	UnderwaterFilterShader();
	~UnderwaterFilterShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void OnSettingsReload(Config* cfg);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, ShaderInputStructs::WaterFilterInput* input);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, ShaderInputStructs::WaterFilterInput* input);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> sampler;

	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
	CComPtr<ID3D11Buffer> pixelBuffer;

	float farClip;
	float waterLevel;
	float fogStart;
	float fogEnd;

	float waterDepthFogScaling; // = 0.3f;
	float waterVisionRange; // = 0.04f;
	float waterMinVisionRange; // = 0.2f;

	float waterMaxVisionRange; // = 1.0f;
	XMFLOAT3 ColorMapDampeningScale;
	XMFLOAT3 FogColorDampeningScale;
};