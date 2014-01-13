#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>

class SkysphereShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX WorldViewProjection;
		float cameraYPos;
		XMFLOAT3 padding;
	};

	struct GradientBufferType
	{
		XMFLOAT4 ApexColor;
		XMFLOAT4 CenterColor;
		XMFLOAT4 FogColor;
	};

public:
	SkysphereShader();
	~SkysphereShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX* worldViewProjection, float cameraYPos, XMFLOAT4 ApexColor, XMFLOAT4 CenterColor, XMFLOAT4 FogColor, float time);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* context, XMMATRIX* worldViewProjection, float cameraYPos, XMFLOAT4 ApexColor, XMFLOAT4 CenterColor, XMFLOAT4 FogColor, float time);

	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> gradientBuffer;
};