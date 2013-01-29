////////////////////////////////////////////////////////////////////////////////
// Filename: multitextureshader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MULTITEXTURESHADER_H_
#define _MULTITEXTURESHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: MultiTextureShader
////////////////////////////////////////////////////////////////////////////////
class MultiTextureShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct TextureMultiplierBufferType
	{
		D3DXVECTOR2 multipliers;
	};

public:
	MultiTextureShader();
	MultiTextureShader(const MultiTextureShader&);
	~MultiTextureShader();

	bool Initialize(ID3D11Device*, HWND, float, float);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR2);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* multiplierBuffer;
	ID3D11SamplerState* sampleState;
	D3DXVECTOR2 textureMultipliers;
	D3DXVECTOR2 textureAlphas;
};

#endif