////////////////////////////////////////////////////////////////////////////////
// Filename: marchingcubeshader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MARCHINGCUBESHADER_H_
#define _MARCHINGCUBESHADER_H_


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
// Class name: MarchingCubeShader
////////////////////////////////////////////////////////////////////////////////
class MarchingCubeShader
{
private:

	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

public:
	MarchingCubeShader();
	MarchingCubeShader(const MarchingCubeShader&);
	~MarchingCubeShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* context, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj);
	void RenderShader(ID3D11DeviceContext* context, int indexCount);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
};

#endif