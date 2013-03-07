////////////////////////////////////////////////////////////////////////////////
// Filename: DRFinalComposition.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DRFINALCOMPOSITION_H_
#define _DRFINALCOMPOSITION_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>

using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: DRFinalComposition
////////////////////////////////////////////////////////////////////////////////
class DRFinalComposition
{
private:
	struct PixelMatrixBuffer
	{
		XMFLOAT4X4 LightViewProjection;
		XMFLOAT4X4 InvertedViewProjection;
	};

	struct VertexMatrixBuffer
	{
		XMFLOAT4X4 World;
		XMFLOAT4X4 View;
		XMFLOAT4X4 Projection;
	};

public:
	DRFinalComposition();
	DRFinalComposition(const DRFinalComposition&);
	~DRFinalComposition();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 projection, XMFLOAT4X4 invertedViewProjection, XMFLOAT4X4 lightViewProj,
		ID3D11ShaderResourceView** textureArray);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 projection, XMFLOAT4X4 invertedViewProjection, XMFLOAT4X4 lightViewProj,
		ID3D11ShaderResourceView** textureArray);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sampler;

	ID3D11Buffer* vertexMatrixBuffer;
	ID3D11Buffer* pixelMatrixBuffer;

};

#endif