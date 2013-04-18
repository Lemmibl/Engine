#pragma once

#include <vector>
#include "interfacepointers.h"
#include <windows.h>
#include <xnamath.h>

struct MaterialStruct
{
	float Kambience;
	float Kdiffuse;
	float Kspecular;
	float smoothness;
};

//We save the pixels in R8G8B8A8. This means that each pixel can have values 1-255.
struct PixelData
{
	UINT8 x, y, z, w;
};

struct Grads
{
	int x;
	int y;
	int z;

	Grads(int extX, int extY, int extZ)
	{
		this->x = extX;
		this->y = extY;
		this->z = extZ;
	}
};

enum StageOfDay
{
	DAWN,
	MORNING, //Transition between dawn and day
	DAY,
	DUSK,
	EVENING, //Transition between dusk and night
	NIGHT
};

struct PointLight
{
	XMFLOAT3 Position;
	float Radius;
	XMFLOAT3 Color;
	float Intensity;
	XMFLOAT4X4 World;
};

struct DirLight
{
	XMFLOAT4X4 View;
	XMFLOAT4X4 Projection;

	XMFLOAT4 Color;
	XMFLOAT3 Position;
	XMFLOAT3 OldPosition; //For day/night cycles
	XMFLOAT3 NextPosition; //For day/night cycles
	XMFLOAT3 Direction;
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