#include "FullScreenShaderInterface.h"

FullScreenShaderInterface::FullScreenShaderInterface()
{
}


FullScreenShaderInterface::~FullScreenShaderInterface()
{
}

void FullScreenShaderInterface::InitializeShader( ID3D11Device* device, HWND hwnd )
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(L"../Engine/Shaders/FullscreenQuadVertexShader.vsh", NULL, NULL, "VS_Quad", "vs_5_0", NULL, 0, NULL, 
		&vertexShaderBuffer, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Error", L"Missing shader file: FullscreenQuadVertexShader", MB_OK);
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if(FAILED(result))
	{
		MessageBox(hwnd, L"Error", L"Couldn't load shader file: FullscreenQuadVertexShader", MB_OK);
	}
}

void FullScreenShaderInterface::SetupDrawCall( ID3D11DeviceContext* deviceContext )
{
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FullScreenShaderInterface::DrawFullScreenQuad( ID3D11DeviceContext* deviceContext )
{
	deviceContext->Draw(3, 0);
}