#include "structs.h"

// == RenderTarget2D ==============================================================================

RenderTarget2D::RenderTarget2D() :  Width(0),
	Height(0),
	Format(DXGI_FORMAT_UNKNOWN),
	NumMipLevels(0),
	MultiSamples(0),
	MSQuality(0),
	AutoGenMipMaps(false),
	UAView(NULL),
	ArraySize(1)
{

}

void RenderTarget2D::Initialize(ID3D11Device* device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	UINT numMipLevels,
	UINT multiSamples,
	UINT msQuality,
	BOOL autoGenMipMaps,
	BOOL createUAV,
	UINT arraySize,
	BOOL cubeMap)
{
	Texture = NULL;
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = arraySize;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE|D3D11_BIND_RENDER_TARGET;
	if(createUAV)
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;

	desc.CPUAccessFlags = 0;
	desc.Format = format;
	desc.MipLevels = numMipLevels;
	desc.MiscFlags = (autoGenMipMaps && numMipLevels > 1) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	desc.SampleDesc.Count = multiSamples;
	desc.SampleDesc.Quality = msQuality;
	desc.Usage = D3D11_USAGE_DEFAULT;

	if(cubeMap)
	{
		_ASSERT(arraySize == 6);
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	device->CreateTexture2D(&desc, NULL, &Texture);

	ArraySlices.clear();
	for(UINT i = 0; i < arraySize; ++i)
	{
		ID3D11RenderTargetViewPtr rtView;
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
		rtDesc.Format = format;

		if (arraySize == 1)
		{
			rtDesc.ViewDimension = multiSamples > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
			rtDesc.Texture2D.MipSlice = 0;
		}
		else
		{
			rtDesc.ViewDimension = multiSamples > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtDesc.Texture2DArray.ArraySize = 1;
			rtDesc.Texture2DArray.FirstArraySlice = i;
			rtDesc.Texture2DArray.MipSlice = 0;
		}
		device->CreateRenderTargetView(Texture, &rtDesc, &rtView);

		ArraySlices.push_back(rtView);
	}

	RTView = ArraySlices[0];

	device->CreateShaderResourceView(Texture, NULL, &SRView);

	Width = width;
	Height = height;
	NumMipLevels = numMipLevels;
	MultiSamples = multiSamples;
	Format = format;
	AutoGenMipMaps = autoGenMipMaps;
	ArraySize = arraySize;
	CubeMap = cubeMap;

	if(createUAV)
		device->CreateUnorderedAccessView(Texture, NULL, &UAView);
};

// == DepthStencilBuffer ==========================================================================

DepthStencilBuffer::DepthStencilBuffer() :  Width(0),
	Height(0),
	MultiSamples(0),
	MSQuality(0),
	Format(DXGI_FORMAT_UNKNOWN),
	ArraySize(1)
{

}

void DepthStencilBuffer::Initialize(ID3D11Device* device,
	UINT width,
	UINT height,
	DXGI_FORMAT format,
	BOOL useAsShaderResource,
	UINT multiSamples,
	UINT msQuality,
	UINT arraySize)
{
	UINT bindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (useAsShaderResource)
		bindFlags |= D3D11_BIND_SHADER_RESOURCE;

	DXGI_FORMAT dsTexFormat;
	if (!useAsShaderResource)
		dsTexFormat = format;
	else if (format == DXGI_FORMAT_D16_UNORM)
		dsTexFormat = DXGI_FORMAT_R16_TYPELESS;
	else if(format == DXGI_FORMAT_D24_UNORM_S8_UINT)
		dsTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
	else
		dsTexFormat = DXGI_FORMAT_R32_TYPELESS;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = arraySize;
	desc.BindFlags = bindFlags;
	desc.CPUAccessFlags = 0;
	desc.Format = dsTexFormat;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = multiSamples;
	desc.SampleDesc.Quality = msQuality;
	desc.Usage = D3D11_USAGE_DEFAULT;
	device->CreateTexture2D(&desc, NULL, &Texture);

	ArraySlices.clear();
	for (UINT i = 0; i < arraySize; ++i)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ID3D11DepthStencilViewPtr dsView;
		dsvDesc.Format = format;

		if (arraySize == 1)
		{
			dsvDesc.ViewDimension = multiSamples > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Texture2D.MipSlice = 0;
		}
		else
		{
			dsvDesc.ViewDimension = multiSamples > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			dsvDesc.Texture2DArray.ArraySize = 1;
			dsvDesc.Texture2DArray.FirstArraySlice = i;
			dsvDesc.Texture2DArray.MipSlice = 0;
		}

		dsvDesc.Flags = 0;
		device->CreateDepthStencilView(Texture, &dsvDesc, &dsView);
		ArraySlices.push_back(dsView);

		if (i == 0)
		{
			// Also create a read-only DSV
			dsvDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
			if (format == DXGI_FORMAT_D24_UNORM_S8_UINT || format == DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
				dsvDesc.Flags |= D3D11_DSV_READ_ONLY_STENCIL;
			device->CreateDepthStencilView(Texture, &dsvDesc, &ReadOnlyDSView);
			dsvDesc.Flags = 0;
		}
	}

	DSView = ArraySlices[0];

	if (useAsShaderResource)
	{
		DXGI_FORMAT dsSRVFormat;
		if (format == DXGI_FORMAT_D16_UNORM)
			dsSRVFormat = DXGI_FORMAT_R16_UNORM;
		else if(format == DXGI_FORMAT_D24_UNORM_S8_UINT)
			dsSRVFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS ;
		else
			dsSRVFormat = DXGI_FORMAT_R32_FLOAT;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = dsSRVFormat;

		if (arraySize == 1)
		{
			srvDesc.ViewDimension = multiSamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			srvDesc.Texture2D.MostDetailedMip = 0;
		}
		else
		{
			srvDesc.ViewDimension = multiSamples > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = arraySize;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.MipLevels = 1;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
		}

		device->CreateShaderResourceView(Texture, &srvDesc, &SRView);
	}
	else
		SRView = NULL;

	Width = width;
	Height = height;
	MultiSamples = multiSamples;
	Format = format;
	ArraySize = arraySize;
}

// == RWBuffer ====================================================================================

RWBuffer::RWBuffer() : Size(0), Stride(0), NumElements(0), Format(DXGI_FORMAT_UNKNOWN), RawBuffer(FALSE)
{

}

void RWBuffer::Initialize(ID3D11Device* device, DXGI_FORMAT format, UINT stride, UINT numElements, BOOL rawBuffer)
{
	Format = format;
	Size = stride * numElements;
	Stride = stride;
	NumElements = numElements;
	RawBuffer = rawBuffer;

	if(rawBuffer)
	{
		_ASSERT(stride == 4);
		Format = DXGI_FORMAT_R32_TYPELESS;
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = stride * numElements;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = rawBuffer ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
	bufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&bufferDesc, NULL, &Buffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = Format;

	if(rawBuffer)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.NumElements = numElements;
		srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	}
	else
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.ElementOffset = 0;
		srvDesc.Buffer.ElementWidth = numElements;
	}

	device->CreateShaderResourceView(Buffer, &srvDesc, &SRView);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.Flags = rawBuffer ? D3D11_BUFFER_UAV_FLAG_RAW : 0;

	device->CreateUnorderedAccessView(Buffer, &uavDesc, &UAView);
}

// == StructuredBuffer ============================================================================

StructuredBuffer::StructuredBuffer() : Size(0), Stride(0), NumElements(0)
{
}

void StructuredBuffer::Initialize(ID3D11Device* device, UINT stride, UINT numElements, BOOL useAsUAV,
	BOOL appendConsume, BOOL useAsDrawIndirect, const void* initData)
{
	Size = stride * numElements;
	Stride = stride;
	NumElements = numElements;

	if(appendConsume || useAsDrawIndirect)
		useAsUAV = true;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = stride * numElements;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.BindFlags |= useAsUAV ? D3D11_BIND_UNORDERED_ACCESS : 0;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.MiscFlags |= useAsDrawIndirect ? D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS : 0;
	bufferDesc.StructureByteStride = stride;

	D3D11_SUBRESOURCE_DATA subresourceData;
	subresourceData.pSysMem = initData;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	device->CreateBuffer(&bufferDesc, initData != NULL ? &subresourceData : NULL, &Buffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = numElements;
	device->CreateShaderResourceView(Buffer, &srvDesc, &SRView);

	if(useAsUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = appendConsume ? D3D11_BUFFER_UAV_FLAG_APPEND : 0;
		uavDesc.Buffer.NumElements = numElements;
		device->CreateUnorderedAccessView(Buffer, &uavDesc, &UAView);
	}
}