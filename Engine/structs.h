#pragma once

#include <vector>
#include "interfacepointers.h"
#include <xnamath.h>

struct MaterialStruct
{
	float Ka;
	float Kd;
	float Ks;
	float a;
};

struct PointLight
{
	XMVECTOR Position;
	float Radius;
	XMVECTOR Color;
	float Intensity;
	XMMATRIX World;
};

struct DirLight
{
	XMMATRIX View;
	XMMATRIX Projection;

	XMVECTOR Color;
	XMVECTOR Position;
	XMVECTOR Direction;
	float Intensity;
};

struct RenderTarget2D
{
	ID3D11Texture2DPtr Texture;
	ID3D11RenderTargetViewPtr RTView;
	ID3D11ShaderResourceViewPtr SRView;
	ID3D11UnorderedAccessViewPtr UAView;
	UINT Width;
	UINT Height;
	UINT NumMipLevels;
	UINT MultiSamples;
	UINT MSQuality;
	DXGI_FORMAT Format;
	BOOL AutoGenMipMaps;
	UINT ArraySize;
	BOOL CubeMap;
	std::vector<ID3D11RenderTargetViewPtr> ArraySlices;

	RenderTarget2D();

	void Initialize(ID3D11Device* device,
		UINT width,
		UINT height,
		DXGI_FORMAT format,
		UINT numMipLevels = 1,
		UINT multiSamples = 1,
		UINT msQuality = 0,
		BOOL autoGenMipMaps = false,
		BOOL createUAV = false,
		UINT arraySize = 1,
		BOOL cubeMap = false);
};

struct DepthStencilBuffer
{
	ID3D11Texture2DPtr Texture;
	ID3D11DepthStencilViewPtr DSView;
	ID3D11DepthStencilViewPtr ReadOnlyDSView;
	ID3D11ShaderResourceViewPtr SRView;
	UINT Width;
	UINT Height;
	UINT MultiSamples;
	UINT MSQuality;
	DXGI_FORMAT Format;
	UINT ArraySize;
	std::vector<ID3D11DepthStencilViewPtr> ArraySlices;

	DepthStencilBuffer();

	void Initialize(    ID3D11Device* device,
		UINT width,
		UINT height,
		DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT,
		BOOL useAsShaderResource = false,
		UINT multiSamples = 1,
		UINT msQuality = 0,
		UINT arraySize = 1);
};

struct RWBuffer
{
	ID3D11BufferPtr Buffer;
	ID3D11ShaderResourceViewPtr SRView;
	ID3D11UnorderedAccessViewPtr UAView;
	UINT Size;
	UINT Stride;
	UINT NumElements;
	BOOL RawBuffer;
	DXGI_FORMAT Format;

	RWBuffer();

	void Initialize(ID3D11Device* device, DXGI_FORMAT format, UINT stride, UINT numElements, BOOL rawBuffer = FALSE);
};

struct StructuredBuffer
{
	ID3D11BufferPtr Buffer;
	ID3D11ShaderResourceViewPtr SRView;
	ID3D11UnorderedAccessViewPtr UAView;
	UINT Size;
	UINT Stride;
	UINT NumElements;

	StructuredBuffer();

	void Initialize(ID3D11Device* device, UINT stride, UINT numElements, BOOL useAsUAV = false,
		BOOL appendConsume = false, BOOL useAsDrawIndirect = false, const void* initData = NULL);
};