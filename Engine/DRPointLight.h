////////////////////////////////////////////////////////////////////////////////
// Filename: PointLightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POINTLIGHTSHADERCLASS_H_
#define _POINTLIGHTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11async.h>
#include <fstream>

#include "StructsAndEnums.h"
#include <windows.h>
#include <xnamath.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: PointLightShaderClass
////////////////////////////////////////////////////////////////////////////////
class DRPointLight
{
private:

	struct VertexMatrixBufferType
	{
		XMMATRIX WorldViewProjection;
	};

	struct PixelMatrixBufferType
	{
		XMMATRIX InvertedViewProjection;
	};

	struct LightBufferType
	{
		XMFLOAT4 LightPosition;
		XMFLOAT3 LightColor;
		float LightRadius;
		XMFLOAT3 CameraPosition;
		float LightPower;
	};


public:
	DRPointLight();
	DRPointLight(const DRPointLight&);
	~DRPointLight();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	//This class takes a normal map and a depthmap and should be rendering to the light map.
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* WorldViewProjection, XMMATRIX* inverseViewProjection, 
		PointLight* pointLight, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialArray, XMFLOAT3 cameraPosition);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* WorldViewProjection, XMMATRIX* inverseViewProjection, 
		PointLight* pointLight, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialArray, XMFLOAT3 cameraPosition);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;

	ID3D11SamplerState* samplerState;

	ID3D11Buffer* vertexMatrixBuffer;
	ID3D11Buffer* pixelMatrixBuffer;
	ID3D11Buffer* lightBuffer;
};

#endif