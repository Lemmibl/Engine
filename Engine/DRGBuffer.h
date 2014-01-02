////////////////////////////////////////////////////////////////////////////////
// Filename: DRGBuffer.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DRGBUFFER_H_
#define _DRGBUFFER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: DRGBuffer
////////////////////////////////////////////////////////////////////////////////
class DRGBuffer
{

private:

	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

	struct PixelShaderBufferType
	{
		float FarZ;
		XMFLOAT3 Padding;
	};

public:
	DRGBuffer();
	DRGBuffer(const DRGBuffer&);
	~DRGBuffer();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX*, XMMATRIX*, XMMATRIX*, ID3D11ShaderResourceView* texture, float FarZ);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, 
		XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix, ID3D11ShaderResourceView* texture, float FarZ);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;

	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> pixelFarZBuffer;

	CComPtr<ID3D11SamplerState> sampler;
};

#endif