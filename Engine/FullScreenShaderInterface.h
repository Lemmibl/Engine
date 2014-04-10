#pragma once
#include <d3d11.h>
#include <atlcomcli.h>
#include <d3dx11async.h>

class FullScreenShaderInterface
{
	public:
	FullScreenShaderInterface();
	virtual ~FullScreenShaderInterface();

	public:
	void InitializeShader(ID3D11Device* device, HWND hwnd);
	void SetupDrawCall(ID3D11DeviceContext* deviceContext);
	void DrawFullScreenQuad(ID3D11DeviceContext* deviceContext);

	private:
	CComPtr<ID3D11VertexShader> vertexShader;
};

