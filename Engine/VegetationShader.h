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
// Class name: VegetationShader
////////////////////////////////////////////////////////////////////////////////
class VegetationShader
{
private:

	struct MatrixBufferType
	{
		XMMATRIX WorldViewProjection;
	};

public:
	VegetationShader();
	VegetationShader(const VegetationShader&);
	~VegetationShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int vertexCount, int instanceCount, XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textures);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX* worldViewProjection, ID3D11ShaderResourceView**);
	void RenderShader(ID3D11DeviceContext*, int vertexCount, int instanceCount);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* samplerState;
};