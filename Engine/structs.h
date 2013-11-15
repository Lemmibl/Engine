#pragma once

#include <vector>
#include "interfacepointers.h"
#include <d3dx9math.h>
#include <d3d11.h>

<<<<<<< master:Engine/structs.h
typedef D3DXVECTOR2 float2;
typedef D3DXVECTOR3 float3;
typedef D3DXVECTOR4 float4;

struct MaterialStruct
{
	float Ka;
	float Kd;
	float Ks;
	float a;
=======
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

//IDs for different things. Potential TODO: Read these in from an XML or smth?
enum MaterialID
{
	MATERIAL_GRASS,
	MATERIAL_ROCK,
	MATERIAL_SNOW,
	MATERIAL_DIRT,
	MATERIAL_GRASSQUADS,
	MATERIAL_UNDERGROUND
};

enum TextureID
{
	TEXTURE_DIRT,
	TEXTURE_GRASS,
	TEXTURE_ROCK,
	TEXTURE_SAND,
	TEXTURE_SNOW,
	TEXTURE_TILEDSTONE
>>>>>>> local:Engine/StructsAndEnums.h
};

struct PointLight
{
	D3DXVECTOR3 Position;
	float Radius;
	D3DXVECTOR3 Color;
	float Intensity;
	D3DXMATRIX World;
};

struct DirLight
{
	D3DXMATRIX View;
	D3DXMATRIX Projection;

	D3DXVECTOR4 Color;
	D3DXVECTOR3 Position;
	D3DXVECTOR3 Direction;
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