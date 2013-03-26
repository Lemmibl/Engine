//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
using namespace std;


class MCubesGBufferShader
{

private:
	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

public:
	MCubesGBufferShader();
	MCubesGBufferShader(const MCubesGBufferShader&);
	~MCubesGBufferShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldMatrix, XMMATRIX* viewMatrix, 
	XMMATRIX* projectionMatrix, ID3D11ShaderResourceView* texture);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, 
		XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix, ID3D11ShaderResourceView* texture);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;

	ID3D11Buffer* matrixBuffer;

	ID3D11SamplerState* samplers[2];
};