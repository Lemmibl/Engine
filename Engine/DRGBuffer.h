////////////////////////////////////////////////////////////////////////////////
// Filename: alphamapshaderclass.h
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
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: AlphaMapShaderClass
////////////////////////////////////////////////////////////////////////////////
class DRGBuffer
{
private:
	struct MatrixBufferType
	{
		XMFLOAT4X4 World;
		XMFLOAT4X4 View;
		XMFLOAT4X4 Projection;
	};

public:
	DRGBuffer();
	DRGBuffer(const DRGBuffer&);
	~DRGBuffer();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, ID3D11ShaderResourceView**);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, 
		XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView** textureArray);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

#endif