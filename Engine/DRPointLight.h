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

#include "structs.h"
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
		XMFLOAT4X4 World;
		XMFLOAT4X4 View;
		XMFLOAT4X4 Projection;
	};

	struct PixelMatrixBufferType
	{
		XMFLOAT4X4 InvertedViewProjection;
		XMFLOAT3 LightPosition;
	};

	struct LightBufferType
	{
		XMFLOAT3 LightColor;
		int LightRadius;
		float LightPower;
		XMFLOAT3 CameraPosition;
	};


public:
	DRPointLight();
	DRPointLight(const DRPointLight&);
	~DRPointLight();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	//This class takes a normal map and a depthmap and should be rendering to the light map.
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, XMFLOAT4X4 inverseViewProjection, 
		PointLight* pointLight, ID3D11ShaderResourceView** textureArray, XMFLOAT3 cameraPosition);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 proj, XMFLOAT4X4 inverseViewProjection, 
		PointLight* pointLight, ID3D11ShaderResourceView** textureArray, XMFLOAT3 cameraPosition);

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

//fixa buffern tillbaka, lägg till tredje textur att sampla från, ändra rasterizer mode till cull back face ? front face? idk