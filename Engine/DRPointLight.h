#pragma once
#include <d3d11.h>
#include <d3dx11async.h>
#include <fstream>

#include "StructsAndEnums.h"
#include <windows.h>
#include <xnamath.h>
#include <atlcomcli.h>

class DRPointLightShader
{
private:

	struct VertexMatrixBufferType
	{
		XMMATRIX WorldViewProjection;
		XMMATRIX WorldView;
		XMMATRIX World;
		XMFLOAT3 CameraPosition;
		float padding;
	};

	struct PixelMatrixBufferType
	{
		XMMATRIX InvertedView;
		XMMATRIX InvertedProjection;
	};

	struct LightBufferType
	{
		XMFLOAT4 LightPosition;
		XMFLOAT4 LightColor; //.w channel will contain LightRadius
		//float LightRadius;
		XMFLOAT4 CameraPosition; //.w channel will contain LightPower
		//float LightPower;
	};


public:
	DRPointLightShader();
	DRPointLightShader(const DRPointLightShader&);
	~DRPointLightShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	//This class takes a normal map and a depthmap and should be rendering to the light map.
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* world, XMMATRIX* invertedView,
		XMMATRIX* invertedProjection, PointLight* pointLight, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialArray, XMFLOAT3 cameraPosition);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* WorldViewProjection, XMMATRIX* worldView, XMMATRIX* world, XMMATRIX* invertedView, 
		XMMATRIX* invertedProjection, PointLight* pointLight, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialArray, XMFLOAT3 cameraPosition);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> samplerState;

	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
	CComPtr<ID3D11Buffer> pixelMatrixBuffer;
	CComPtr<ID3D11Buffer> lightBuffer;
};