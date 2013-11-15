////////////////////////////////////////////////////////////////////////////////
// Filename: DRFinalComposition.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DRFINALCOMPOSITION_H_
#define _DRFINALCOMPOSITION_H_


//////////////
// INCLUDES //
//////////////
////////////// 
#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: DRFinalComposition
////////////////////////////////////////////////////////////////////////////////
class DRFinalComposition
{
private:
	struct PixelMatrixBuffer
	{
		D3DXMATRIX LightViewProjection;
		D3DXMATRIX InvertedViewProjection;
	};

	struct VertexMatrixBuffer
	{
		D3DXMATRIX World;
		D3DXMATRIX View;
		D3DXMATRIX Projection;
	};

public:
	DRFinalComposition();
	DRFinalComposition(const DRFinalComposition&);
	~DRFinalComposition();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, D3DXMATRIX invertedViewProjection, D3DXMATRIX lightViewProj,
		ID3D11ShaderResourceView** textureArray);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, D3DXMATRIX invertedViewProjection, D3DXMATRIX lightViewProj,
		ID3D11ShaderResourceView** textureArray);
	void RenderShader(ID3D11DeviceContext*, int);

private:
<<<<<<< master:Engine/DRFinalComposition.h
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampler;

	ID3D11Buffer* vertexMatrixBuffer;
	ID3D11Buffer* pixelMatrixBuffer;

=======
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11SamplerState> samplers[2];

	CComPtr<ID3D11Buffer> vertexMatrixBuffer;
	CComPtr<ID3D11Buffer> pixelMatrixBuffer;
>>>>>>> local:Engine/DRCompose.h
};

#endif