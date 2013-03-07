////////////////////////////////////////////////////////////////////////////////
// Filename: dirlightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DRDIRLIGHT_H_
#define _DRDIRLIGHT_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
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
		XMFLOAT4X4 LightView;
		XMFLOAT4X4 LightProjection;
		XMFLOAT4X4 InvertedViewProjection;
	};

	struct VertexMatrixBuffer
	{
		XMFLOAT4X4 World;
		XMFLOAT4X4 View;
		XMFLOAT4X4 Projection;
	};

	struct PositionalBuffer
	{
		XMFLOAT3 LightDirection;
		XMFLOAT3 LightPosition;
		XMFLOAT3 CameraPosition;
	};

	struct LightBuffer
	{
		XMFLOAT4 DiffuseColor;
		XMFLOAT4 AmbienceColor;
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
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 projection, 
	XMFLOAT4X4 invertedViewProj, ID3D11ShaderResourceView** textureArray, XMFLOAT3 cameraPosition, XMFLOAT3 lightPosition, 
	XMFLOAT3 lightDirection, XMFLOAT4 lightColor, float lightStrength, XMFLOAT4 ambienceColor, MaterialStruct material, 
	XMFLOAT4X4 lightView, XMFLOAT4X4 lightProj);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 world, XMFLOAT4X4 view, XMFLOAT4X4 projection, 
		XMFLOAT4X4 invertedViewProj, ID3D11ShaderResourceView** textureArray, XMFLOAT3 cameraPosition, XMFLOAT3 lightPosition, XMFLOAT3 lightDirection, 
		XMFLOAT4 lightColor, float lightStrength, XMFLOAT4 ambienceColor, MaterialStruct material, XMFLOAT4X4 lightView, XMFLOAT4X4 lightProj);
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