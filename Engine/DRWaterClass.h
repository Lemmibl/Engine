//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>

class DRWaterClass
{

private:
	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX WorldView;
		XMMATRIX WorldViewProjection;
	};

	struct GeometryTimeBuffer
	{
		XMMATRIX WorldViewProjection;
		XMMATRIX World;
		float DeltaTime;
	};

public:
	DRWaterClass();
	DRWaterClass(const DRWaterClass&);
	~DRWaterClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
		XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** waterTexture, ID3D11ShaderResourceView** offsetNoiseTexture, float cameraFarclip, float deltaTime);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
		XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** waterTexture, ID3D11ShaderResourceView** offsetNoiseTexture, float cameraFarclip, float deltaTime);

	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11GeometryShader> geometryShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;
	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> timeBuffer;
	CComPtr<ID3D11SamplerState> sampler;
};