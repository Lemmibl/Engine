//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: VegetationShader
////////////////////////////////////////////////////////////////////////////////
class VegetationShader
{
private:

	struct MatrixBufferType
	{
		XMMATRIX WorldViewProjection;
		XMMATRIX WorldView;
		XMMATRIX World;
	};

public:
	VegetationShader();
	VegetationShader(const VegetationShader&);
	~VegetationShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, int vertexCount, int instanceCount, XMMATRIX* worldViewProjection, 
		XMMATRIX* worldView, XMMATRIX* World, ID3D11ShaderResourceView** textures);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* World, ID3D11ShaderResourceView**);
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int vertexCount, int instanceCount);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11SamplerState> samplerState;
};