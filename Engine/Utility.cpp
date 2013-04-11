#include "Utility.h"

Utility::Utility()
{

}

Utility::Utility( const Utility& )
{

}

Utility::~Utility()
{

}

float Utility::Random()
{
	float scale=RAND_MAX+1.0f;
	float base=rand()/scale;
	float fine=rand()/scale;
	return base+fine/scale;
}

HRESULT Utility::Build2DTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
	WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArraySRV)
{
	HRESULT hResult;

	//Create a vector to temporarily store the textures that we load from the HDD
	vector<ID3D11Texture2D*> srcTex;
	srcTex.resize(textureCount);

	//For some reason if we try to access the last texture in the vector, we get weird errors, 
	//so we simply add an extra slot at the end of the vector that won't be used.
	for(int i = 0; i < textureCount; i++) 
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.Width = 1024;		
		loadInfo.Height = 1024;
		loadInfo.Depth  = 0;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 1;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX11_FILTER_NONE;
		loadInfo.MipFilter = D3DX11_FILTER_NONE;
		loadInfo.pSrcInfo  = 0;


		hResult = D3DX11CreateTextureFromFile(device, filenames[i],
				&loadInfo, 0, (ID3D11Resource**)&srcTex[i], &hResult);
		if(FAILED(hResult))
		{
			return false;
		}
	}

	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = textureCount;
	texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	/*
	You don't want to copy in the array slices one by one after you've created the texture. 
	What you want to do is have all of the data for all slices ready in CPU memory, 
	then provide that data as the initialization data when you create your texture array. 
	One way to do this is to load all of the array slices as STAGING resources, 
	map them to get the pointer to their data, and then use those pointers when initializing the IMMUTABLE Texture2D 
	resource for your texture array.
	*/

	//TODO: Declare a description for a texture array
	ID3D11Texture2D* texArray = 0;
	hResult = device->CreateTexture2D( &texArrayDesc, 0, &texArray);
	if(FAILED(hResult))
	{
		return false;
	}

	// Copy individual texture elements into texture array.
	for(int i = 0; i < textureCount; i++)
	{
		// for each mipmap level...
		for(UINT j = 0; j < texElementDesc.MipLevels; j++)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;

			//Read info from srcTex[i] and put it in mappedTex2D
			hResult = deviceContext->Map(srcTex[i], j, D3D11_MAP_READ, 0, &mappedTex2D);
			if(FAILED(hResult))
			{
				return false;
			}

			//Use mappedTex2D to move the data from srcTex[i] ===> texArray
			deviceContext->UpdateSubresource
			(
				texArray,
				D3D11CalcSubresource(j, i, texElementDesc.MipLevels),
				0, 
				mappedTex2D.pData, //Data from srcTex[i]
				mappedTex2D.RowPitch, //Size of one row (width*texelSize). In this case, our texels are A8R8G8B8 
				mappedTex2D.DepthPitch //Size of one texture (height*width*texelSize)
			);

			deviceContext->Unmap(srcTex[i], j);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = textureCount;

	hResult = device->CreateShaderResourceView(texArray, &viewDesc, textureArraySRV);
	if(FAILED(hResult))
	{
		return false;
	}


	// Cleanup--we only need the resource view.
	texArray->Release();
	texArray = 0;

	for(int i = 0; i < textureCount; ++i)
	{
		srcTex[i]->Release();
		srcTex[i] = 0;
	}


	return S_OK;
}

/*
    //
	//---------------------------------------------------------------------------------------
	//	Load all textures into Texture2D array with D3DX11CreateTextureFromFile
	//---------------------------------------------------------------------------------------
	UINT iNumOfMaterials = g_sFileNames.size();
	
	vector<ID3D11Texture2D*> pTexture2Ds;
	pTexture2Ds.resize(iNumOfMaterials);;
	
	for (int i=0; i<iNumOfMaterials; i++)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = resolution;		
		loadInfo.Height = resolution;
		loadInfo.Depth  = 0;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 0;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX11_FILTER_NONE;
		loadInfo.MipFilter = D3DX11_FILTER_NONE;
		loadInfo.pSrcInfo  = 0;

		hr = D3DX11CreateTextureFromFile(pd3dDevice, g_sFileNames[i].c_str(), 		
			//&loadInfo, 0, (ID3D11Resource**)&pTexture2Ds[i], 0);
			&loadInfo, 0, &pTexture2Ds[i], 0);
	}

	//---------------------------------------------------------------------------------------
	//	Create the texture array.  Each element in the texture 
	//		array has the same format/dimensions.
	//---------------------------------------------------------------------------------------
	D3D11_TEXTURE2D_DESC texElementDesc;
	pTexture2Ds[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width              = texElementDesc.Width;
	texArrayDesc.Height             = texElementDesc.Height;
	texArrayDesc.MipLevels          = texElementDesc.MipLevels;
	texArrayDesc.ArraySize          = iNumOfMaterials;
	texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count   = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage              = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags     = 0;
	texArrayDesc.MiscFlags          = 0;

	ID3D11Texture2D* texArray = 0;
	hr = pd3dDevice->CreateTexture2D( &texArrayDesc, 0, &texArray);
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	//	Copy individual texture elements into texture array.
	//---------------------------------------------------------------------------------------
	// for each texture element...
	ID3D11DeviceContext* pd3dImmediateContext;
	pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	for(UINT i = 0; i < g_sFileNames.size(); ++i)
	{
		// for each mipmap level...
		for(UINT j = 0; j < texElementDesc.MipLevels; ++j)
		{			
			D3D11_MAPPED_SUBRESOURCE MappedResource;
			hr = pd3dImmediateContext->Map( pTexture2Ds[i], j, D3D11_MAP_READ, 0, &MappedResource ) ;			

			pd3dImmediateContext->UpdateSubresource(texArray, D3D11CalcSubresource(j, i, texElementDesc.MipLevels), 0, MappedResource.pData, MappedResource.RowPitch, 0);			

			pd3dImmediateContext->Unmap(pTexture2Ds[i], j);
		}
	}	
	//---------------------------------------------------------------------------------------


	//---------------------------------------------------------------------------------------
	//					Create a resource view to the texture array.
	//---------------------------------------------------------------------------------------
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = iNumOfMaterials;

	hr = pd3dDevice->CreateShaderResourceView(texArray, &viewDesc, &g_pTextureArray);
	//---------------------------------------------------------------------------------------

	SAFE_RELEASE(texArray);
	for (int i=0; i<pTexture2Ds.size(); i++)
		SAFE_RELEASE(pTexture2Ds[i]); 
*/


















/*
TextureArray2D Renderer::createTexArray2D(	DXGI_FORMAT _format, 
uint32 _w, 
uint32 _h, 
uint8 _l,
void* _data,
bool _allocateMipmap,
bool _dynamic)
{
// Create wrapper
TextureArray2D texObject;
texObject.m_texture			= NULL;
texObject.m_srv				= NULL;
texObject.m_resolution.x	= _w;
texObject.m_resolution.y	= _h;
texObject.m_levels			= _allocateMipmap?std::max(MipmapLevels(_w),MipmapLevels(_h)):1;
texObject.m_layers			= _l;
texObject.m_format			= _format;

// Create texture
DXGI_SAMPLE_DESC sampDesc;
sampDesc.Count				= 1;
sampDesc.Quality			= 0;

D3D11_TEXTURE2D_DESC texDesc;
texDesc.Width				= _w;
texDesc.Height				= _h;
texDesc.MipLevels			= _allocateMipmap?0:1; // 0 Create the entire mipmap chain
texDesc.ArraySize			= _l;
texDesc.Format				= _format;
texDesc.SampleDesc			= sampDesc;
texDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
texDesc.MiscFlags			= 0;
texDesc.CPUAccessFlags		= 0;
texDesc.Usage				= _dynamic?D3D11_USAGE_DEFAULT:D3D11_USAGE_IMMUTABLE;

ID3D11Texture2D* tex		= NULL;
if(_data == NULL)
{
CheckError(m_device->CreateTexture2D(&texDesc,NULL,&tex));
}
else
{
if(_allocateMipmap)
{
dxf::Error("Mipmap on texture array is not implemented yet");
}
else
{
D3D11_SUBRESOURCE_DATA subData;
subData.pSysMem			= _data;
subData.SysMemPitch		= _w * ToByte(_format);
subData.SysMemSlicePitch= _w * _h * ToByte(_format);

CheckError(m_device->CreateTexture2D(&texDesc,&subData,&tex));
}
}
texObject.m_texture.Attach(tex);

// Create default shader view
D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
srvDesc.Format							= _format;
srvDesc.ViewDimension					= D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
srvDesc.Texture2DArray.FirstArraySlice	= 0;
srvDesc.Texture2DArray.ArraySize		= _l;
srvDesc.Texture2DArray.MipLevels		= texObject.m_levels;
srvDesc.Texture2DArray.MostDetailedMip	= 0;

ID3D11ShaderResourceView*srv= NULL;
m_device->CreateShaderResourceView(tex,&srvDesc,&srv);
texObject.m_srv.Attach(srv);

return texObject;
}
*/