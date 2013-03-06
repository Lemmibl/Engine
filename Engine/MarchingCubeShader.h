////////////////////////////////////////////////////////////////////////////////
// Filename: marchingcubeshader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MARCHINGCUBESHADER_H_
#define _MARCHINGCUBESHADER_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: MarchingCubeShader
////////////////////////////////////////////////////////////////////////////////
class MarchingCubeShader
{
private:

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	MarchingCubeShader();
	MarchingCubeShader(const MarchingCubeShader&);
	~MarchingCubeShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX world, XMMATRIX view, XMMATRIX proj);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* context, XMMATRIX world, XMMATRIX view, XMMATRIX proj);
	void RenderShader(ID3D11DeviceContext* context, int indexCount);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
};

#endif