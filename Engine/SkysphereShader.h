////////////////////////////////////////////////////////////////////////////////
// Filename: skysphereshader.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SKYSPHERESHADER_H_
#define _SKYSPHERESHADER_H_


//////////////
// INCLUDES //
//////////////
#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: SkysphereShader
////////////////////////////////////////////////////////////////////////////////
class SkysphereShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX WorldViewProjection;
	};

	struct GradientBufferType
	{
		XMFLOAT4 ApexColor;
		XMFLOAT4 CenterColor;
		XMFLOAT4 FogColor;
	};

	struct TimeBufferType
	{
		float Time;
		XMFLOAT3 padding;
	};

public:
	SkysphereShader();
	SkysphereShader(const SkysphereShader&);
	~SkysphereShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* context, int indexCount, XMMATRIX* worldViewProjection, 
	XMFLOAT4 ApexColor, XMFLOAT4 CenterColor, XMFLOAT4 FogColor, float time);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* context, XMMATRIX* worldViewProjection, 
		XMFLOAT4 ApexColor, XMFLOAT4 CenterColor, XMFLOAT4 FogColor, float time);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* layout;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* gradientBuffer;
	ID3D11Buffer* timeBuffer;
};

#endif