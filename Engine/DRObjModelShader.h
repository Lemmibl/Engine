#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsDependent.h"
#include "TextureAndMaterialHandler.h"

class DRObjModelShader : public SettingsDependent
{

private:

	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;
	};

	/*
	struct OBJMaterialStruct
	{
	//Just an identifier to help in assigning the right material to the right mesh when loading obj files.
	WCHAR name[MAX_PATH];

	XMFLOAT3 Ambient;
	XMFLOAT3 Diffuse;
	XMFLOAT3 Specular;

	float smoothness, alpha;
	bool transparent;
	};
	
	THOUGHT: Just rework entire material textureArray to use the layout that objmaterials use, so that you have RGB diffuse scalar, and RGB ambient scalar etc. 
	Then here you just send in the actual handle. And save that in whatever channel. To this shader you send in like.. 
	Handle 9 or whatever. That gets saved and compressed into color target's w channel.
	Then you read from material texture array slot 9 in light shaders.
	*/

	struct VariableBuffer
	{
		int materialID;
		float farClip;
		float alpha;

		float PADDING;

		//XMFLOAT3 Ambient;
		//float PADDING2;

		//XMFLOAT3 Diffuse;
		//float PADDING3;
	};

public:
	DRObjModelShader();
	~DRObjModelShader();

	bool Initialize(ID3D11Device*, HWND);

	bool SetNewTexture(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** srv, int textureCount);
	bool SetNewMaterial(ID3D11DeviceContext* deviceContext, TextureAndMaterialHandler::OBJMaterialStruct material);
	bool UpdateMatrixBuffer(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix);

	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount, int startIndexLocation);
	
	virtual void OnSettingsReload(Config* cfg);

private:
	bool InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderFilepath, WCHAR* pixelShaderFilepath);
	void OutputShaderErrorMessage(ID3D10Blob* d10blob, HWND hwnd, WCHAR* shaderFilename);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11InputLayout> layout;

	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> variableBuffer;

	CComPtr<ID3D11SamplerState> sampler;

	bool bufferNeedsUpdate;
	VariableBuffer variableContainer;
};