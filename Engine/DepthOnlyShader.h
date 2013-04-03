////////////////////////////////////////////////////////////////////////////////
// Filename: DepthOnlyShader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEPTHONLYSHADER_H_
#define _DEPTHONLYSHADER_H_

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
// Class name: DepthOnlyShader
////////////////////////////////////////////////////////////////////////////////
class DepthOnlyShader
{
private:

	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

public:
	DepthOnlyShader();
	DepthOnlyShader(const DepthOnlyShader&);
	~DepthOnlyShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX* , XMMATRIX* , XMMATRIX* );

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX* , XMMATRIX* , XMMATRIX* );
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
};

#endif