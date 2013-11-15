//////////////
// INCLUDES //
//////////////
#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: GaussianBlur
////////////////////////////////////////////////////////////////////////////////
class GaussianBlur
{
private:
	struct VertexMatrixBuffer
	{
		XMMATRIX WorldViewProjection;
	};

public:
	GaussianBlur();
	GaussianBlur(const GaussianBlur&);
	~GaussianBlur();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool RenderBlurX(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection,
		ID3D11ShaderResourceView** texture);

	bool RenderBlurY(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection,
		ID3D11ShaderResourceView** texture);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** texture);
	void RenderShaderX(ID3D11DeviceContext*, int);
	void RenderShaderY(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShaderX; //For X blurring
	CComPtr<ID3D11PixelShader> pixelShaderY; //For Y blurring
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> sampler;
	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
};