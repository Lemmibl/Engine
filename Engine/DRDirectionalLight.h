#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include "StructsAndEnums.h"
#include <atlcomcli.h>
#include "SettingsDependent.h"

class DRDirLight : public SettingsDependent
{
public:
	struct DirectionalLightInput
	{
		XMMATRIX* worldViewProjection;
		XMMATRIX* worldView;
		XMMATRIX* world; 
		XMMATRIX* view;
		XMMATRIX* invertedView; 
		XMMATRIX* invertedProjection; 
		XMMATRIX* lightView;
		XMMATRIX* lightProj;
		XMMATRIX* lightViewProj;
		XMMATRIX* lightViewAndInvertedCameraView; 
		DirLight* dirLight; 
		ID3D11ShaderResourceView** textureArray;
		ID3D11ShaderResourceView** materialTextureArray; 
		XMFLOAT4 ambienceColor;
		XMFLOAT3 cameraPosition; 
	};

private:
	struct PixelMatrixBuffer
	{
		XMMATRIX View;
		XMMATRIX InvertedView;
		XMMATRIX LightView;
		XMMATRIX LightViewProj;
	};

	struct VertexMatrixBuffer
	{
		XMMATRIX WorldViewProjection;
		XMMATRIX WorldView;
		XMMATRIX InvertedProjection;
		float thFOV;
		float aspectRatio;
		XMFLOAT2 PADDING;
	};

	/*
	XMMATRIX WorldView;
	XMMATRIX World;
	XMMATRIX InvertedViewProjection;
	XMFLOAT4 CameraPosition;
	 */

	struct PositionalBuffer
	{
		XMFLOAT4 LightDirection;
		XMFLOAT4 CameraPosition;
	};

	struct LightBuffer
	{
		XMFLOAT4 DiffuseColor;
		XMFLOAT4 AmbienceColor;
	};

public:
	DRDirLight();
	DRDirLight(const DRDirLight&);
	~DRDirLight();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectionalLightInput& input);

	void OnSettingsReload(Config* cfg);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectionalLightInput& input);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> samplers[3];
	CComPtr<ID3D11Buffer> lightBuffer;
	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
	CComPtr<ID3D11Buffer> pixelMatrixBuffer;
	CComPtr<ID3D11Buffer> positionalBuffer;
	float cameraFarClip, thFOV, aspectRatio;
};