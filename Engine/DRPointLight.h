////////////////////////////////////////////////////////////////////////////////
// Filename: PointLightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _POINTLIGHTSHADERCLASS_H_
#define _POINTLIGHTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

#include "structs.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: PointLightShaderClass
////////////////////////////////////////////////////////////////////////////////
class DRPointLight
{
private:

	struct VertexMatrixBufferType
	{
		D3DXMATRIX World;
		D3DXMATRIX View;
		D3DXMATRIX Projection;
	};

	struct PixelMatrixBufferType
	{
		D3DXMATRIX InvertedViewProjection;
		float4 LightPosition;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 LightColor;
		int LightRadius;
		float LightPower;
		D3DXVECTOR2 Padding;
		D3DXVECTOR4 CameraPosition;
	};


public:
	DRPointLight();
	DRPointLight(const DRPointLight&);
	~DRPointLight();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	//This class takes a normal map and a depthmap and should be rendering to the light map.
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj, D3DXMATRIX inverseViewProjection, 
		PointLight* pointLight, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 cameraPosition);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj, D3DXMATRIX inverseViewProjection, 
		PointLight* pointLight, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 cameraPosition);

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