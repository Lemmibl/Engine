////////////////////////////////////////////////////////////////////////////////
// Filename: dirlightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DRDIRLIGHT_H_
#define _DRDIRLIGHT_H_


//////////////
// INCLUDES //
//////////////
#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
#include "StructsAndEnums.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: DRDirLight
////////////////////////////////////////////////////////////////////////////////
class DRDirLight
{
private:
	struct PixelMatrixBuffer
	{
		XMMATRIX InvertedViewProjection;
		XMMATRIX LightViewProjection;
	};

	struct VertexMatrixBuffer
	{
		XMMATRIX WorldViewProjection;
	};

	struct PositionalBuffer
	{
		XMFLOAT4 LightDirection;
		XMFLOAT4 LightPosition;
		XMFLOAT4 CameraPosition;
	};

	struct LightBuffer
	{
		XMFLOAT4 DiffuseColor;
		XMFLOAT4 AmbienceColor;
	};

public:
	DRDirLight();
	DRDirLight(const DRDirLight&);
	~DRDirLight();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, XMMATRIX* invertedViewProj, 
		ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialTextureArray, XMFLOAT3 cameraPosition, 
		DirLight* dirLight, XMFLOAT4 ambienceColor, XMMATRIX* lightViewProj);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, XMMATRIX* invertedViewProj, 
		ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialTextureArray, XMFLOAT3 cameraPosition, 
		DirLight* dirLight, XMFLOAT4 ambienceColor, XMMATRIX* lightViewProj);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* samplers[3];

	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* vertexMatrixBuffer;
	ID3D11Buffer* pixelMatrixBuffer;
	ID3D11Buffer* positionalBuffer;
};

#endif