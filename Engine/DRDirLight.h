////////////////////////////////////////////////////////////////////////////////
// Filename: dirlightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DRDIRLIGHT_H_
#define _DRDIRLIGHT_H_


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
// Class name: DRDirLight
////////////////////////////////////////////////////////////////////////////////
class DRDirLight
{
private:
	struct PixelMatrixBuffer
	{
		D3DXMATRIX InvertedViewProjection;
		D3DXMATRIX LightView;
		D3DXMATRIX LightProjection;
	};

	struct VertexMatrixBuffer
	{
		D3DXMATRIX World;
		D3DXMATRIX View;
		D3DXMATRIX Projection;
	};

	struct PositionalBuffer
	{
		float4 LightDirection;
		float4 LightPosition;
		float4 CameraPosition;
	};

	struct LightBuffer
	{
		D3DXVECTOR4 DiffuseColor;
		D3DXVECTOR4 AmbienceColor;

		float Ka; //ambient coefficient
		float Kd; //diffuse coefficient
		float Ks; //specular coefficient
		float a; //Surface shininess
	};

public:
	DRDirLight();
	DRDirLight(const DRDirLight&);
	~DRDirLight();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, 
		D3DXMATRIX invertedViewProj, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 cameraPosition, D3DXVECTOR3 lightPosition, D3DXVECTOR3 lightDirection, 
		D3DXVECTOR4 lightColor, float lightStrength, D3DXVECTOR4 ambienceColor, MaterialStruct material, D3DXMATRIX lightView, D3DXMATRIX lightProj);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, 
		D3DXMATRIX invertedViewProj, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 cameraPosition, D3DXVECTOR3 lightPosition, D3DXVECTOR3 lightDirection, 
		D3DXVECTOR4 lightColor, float lightStrength, D3DXVECTOR4 ambienceColor, MaterialStruct material, D3DXMATRIX lightView, D3DXMATRIX lightProj);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* samplers[2];

	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* vertexMatrixBuffer;
	ID3D11Buffer* pixelMatrixBuffer;
	ID3D11Buffer* positionalBuffer;
};

#endif