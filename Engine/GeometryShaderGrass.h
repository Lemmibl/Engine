#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsManager.h"
#include <libconfig.h++>

using namespace std;

class GeometryShaderGrass
{

private:
	struct MatrixBufferType
	{
		XMMATRIX World;
		XMMATRIX WorldView;
		XMMATRIX WorldViewProjection;
	};

	struct GeometryShaderBuffer
	{
		XMMATRIX World;
		XMMATRIX WorldViewProjection;
		float DeltaTime;
	};

	struct VariableBuffer
	{
		float vegetationScale;
		float vegetationFalloff;
		float forceScale;
		float waveLength;
		float traversalSpeed;
		float farClip;
		XMFLOAT2 padding;
	};

public:
	GeometryShaderGrass();
	GeometryShaderGrass(const GeometryShaderGrass&);
	~GeometryShaderGrass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void OnSettingsReload(Config* cfg);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
		XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texIDLookupTexture, 
		ID3D11ShaderResourceView** windForceTexture, int toggleColor, float cameraFarclip, 	float deltaTime);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
		XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texIDLookupTexture, 
		ID3D11ShaderResourceView** windForceTexture, int toggleColor, float cameraFarclip, 	float deltaTime);

	void RenderShader(ID3D11DeviceContext*, int);

private:
	CComPtr<ID3D11VertexShader> vertexShader;
	CComPtr<ID3D11PixelShader> pixelShader;
	CComPtr<ID3D11GeometryShader> geometryShader;
	CComPtr<ID3D11InputLayout> layout;

	CComPtr<ID3D11Buffer> matrixBuffer;
	CComPtr<ID3D11Buffer> variableBuffer;
	CComPtr<ID3D11Buffer> geometryShaderMatrixBuffer;
	CComPtr<ID3D11SamplerState> sampler;


	VariableBuffer values;
	bool bufferNeedsUpdate;
	//float vegetationScale;
	//float vegetationFalloff;
	//float forceScale;
	//float waveLength;
	//float traversalSpeed;
	//float farClip;
};