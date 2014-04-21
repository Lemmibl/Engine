#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h> 
#include <d3dx11async.h>
#include <fstream>
#include <atlcomcli.h>
#include "SettingsDependent.h"

class GeometryShaderGrass : SettingsDependent
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
		XMMATRIX WorldView;
		XMMATRIX WorldViewProjection;
		float DeltaTime;
		XMFLOAT3 WindDirection;
	};

	struct VariableBuffer
	{
		float vegetationScale;
		float vegetationFalloff;

		float forceScale;
		float waveLength;

		float traversalSpeed;
		float farClip;

		float minimumHeightLevel;
		float maximumHeightLevel;

		float positionalRandomDeviance;
		float angularRandomDeviance;

		float acceptableSlopeAngle;

		float padding;
	};

public:
	GeometryShaderGrass();
	~GeometryShaderGrass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void OnSettingsReload(Config* cfg);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, XMMATRIX* worldViewProjection, 
	ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texIDLookupTexture, ID3D11ShaderResourceView** windForceTexture, int toggleColor, 
	float deltaTime, XMFLOAT3* windDirection);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
		XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texIDLookupTexture, 
		ID3D11ShaderResourceView** windForceTexture, int toggleColor, float deltaTime, XMFLOAT3* windDirection);

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