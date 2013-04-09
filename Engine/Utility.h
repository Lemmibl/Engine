#pragma once
#include <stdlib.h>
#include <D3D11.h>

class Utility
{
public:
	Utility();
	Utility(const Utility&);
	~Utility();

	inline float random()
	{
		float scale=RAND_MAX+1.0f;
		float base=rand()/scale;
		float fine=rand()/scale;
		return base+fine/scale;
	}


	////--------------------------------------------------------------------------------------
	//// LoadTextureArray loads a texture array and associated view from a series
	//// of textures on disk.
	////--------------------------------------------------------------------------------------
	//inline HRESULT LoadTextureArray( ID3D11Device* pd3dDevice, LPCTSTR* szTextureNames, int iNumTextures,
	//	ID3D11Texture2D** ppTex2D, ID3D11ShaderResourceView** ppSRV )
	//{
	//	HRESULT hr = S_OK;
	//	D3D11_TEXTURE2D_DESC desc;
	//	ZeroMemory( &desc, sizeof( D3D11_TEXTURE2D_DESC ) );

	//	WCHAR str[MAX_PATH];
	//	for( int i = 0; i < iNumTextures; i++ )
	//	{
	//		V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szTextureNames[i] ) );

	//		ID3D11Resource* pRes = NULL;
	//		D3DX11_IMAGE_LOAD_INFO loadInfo;
	//		ZeroMemory( &loadInfo, sizeof( D3DX11_IMAGE_LOAD_INFO ) );
	//		loadInfo.Width = D3DX_FROM_FILE;
	//		loadInfo.Height = D3DX_FROM_FILE;
	//		loadInfo.Depth = D3DX_FROM_FILE;
	//		loadInfo.FirstMipLevel = 0;
	//		loadInfo.MipLevels = 1;
	//		loadInfo.Usage = D3D11_USAGE_STAGING;
	//		loadInfo.BindFlags = 0;
	//		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	//		loadInfo.MiscFlags = 0;
	//		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//MAKE_TYPELESS( DXGI_FORMAT_R8G8B8A8_UNORM );
	//		loadInfo.Filter = D3DX11_FILTER_NONE;
	//		loadInfo.MipFilter = D3DX11_FILTER_NONE;

	//		V_RETURN( D3DX10CreateTextureFromFile( pd3dDevice, str, &loadInfo, NULL, &pRes, NULL ) );
	//		if( pRes )
	//		{
	//			ID3D11Texture2D* pTemp;
	//			pRes->QueryInterface( __uuidof( ID3D11Texture2D ), ( LPVOID* )&pTemp );
	//			pTemp->GetDesc( &desc );

	//			D3D11_MAPPED_TEXTURE2D mappedTex2D;

	//			if( desc.MipLevels > 4 )
	//				desc.MipLevels -= 4;
	//			if( !( *ppTex2D ) )
	//			{
	//				desc.Usage = D3D11_USAGE_DEFAULT;
	//				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//				desc.CPUAccessFlags = 0;
	//				desc.ArraySize = iNumTextures;
	//				V_RETURN( pd3dDevice->CreateTexture2D( &desc, NULL, ppTex2D ) );
	//			}

	//			for( UINT iMip = 0; iMip < desc.MipLevels; iMip++ )
	//			{
	//				pTemp->Map( iMip, D3D11_MAP_READ, 0, &mappedTex2D );

	//				pd3dDevice->UpdateSubresource( ( *ppTex2D ),
	//					D3D11CalcSubresource( iMip, i, desc.MipLevels ),
	//					NULL,
	//					mappedTex2D.pData,
	//					mappedTex2D.RowPitch,
	//					0 );

	//				pTemp->Unmap( iMip );
	//			}

	//			SAFE_RELEASE( pRes );
	//			SAFE_RELEASE( pTemp );
	//		}
	//		else
	//		{
	//			return false;
	//		}
	//	}

	//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	//	ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
	//	SRVDesc.Format = MAKE_SRGB( DXGI_FORMAT_R8G8B8A8_UNORM );
	//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	//	SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
	//	SRVDesc.Texture2DArray.ArraySize = iNumTextures;
	//	V_RETURN( pd3dDevice->CreateShaderResourceView( *ppTex2D, &SRVDesc, ppSRV ) );

	//	return hr;
	//}

	////--------------------------------------------------------------------------------------
	//// This helper function creates a 1D texture full of random vectors.  The shader uses
	//// the current time value to index into this texture to get a random vector.
	////--------------------------------------------------------------------------------------
	//inline HRESULT CreateRandomTexture( ID3D11Device* pd3dDevice )
	//{
	//	HRESULT hr = S_OK;

	//	int iNumRandValues = 1024;
	//	srand( 0 );
	//	//create the data
	//	D3D11_SUBRESOURCE_DATA InitData;
	//	InitData.pSysMem = new float[iNumRandValues * 4];
	//	if( !InitData.pSysMem )
	//		return E_OUTOFMEMORY;
	//	InitData.SysMemPitch = iNumRandValues * 4 * sizeof( float );
	//	InitData.SysMemSlicePitch = iNumRandValues * 4 * sizeof( float );
	//	for( int i = 0; i < iNumRandValues * 4; i++ )
	//	{
	//		( ( float* )InitData.pSysMem )[i] = float( ( rand() % 10000 ) - 5000 );
	//	}

	//	// Create the texture
	//	D3D11_TEXTURE1D_DESC dstex;
	//	dstex.Width = iNumRandValues;
	//	dstex.MipLevels = 1;
	//	dstex.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//	dstex.Usage = D3D11_USAGE_DEFAULT;
	//	dstex.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//	dstex.CPUAccessFlags = 0;
	//	dstex.MiscFlags = 0;
	//	dstex.ArraySize = 1;
	//	V_RETURN( pd3dDevice->CreateTexture1D( &dstex, &InitData, &g_pRandomTexture ) );
	//	SAFE_DELETE_ARRAY( InitData.pSysMem );

	//	// Create the resource view
	//	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	//	ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
	//	SRVDesc.Format = dstex.Format;
	//	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	//	SRVDesc.Texture1D.MipLevels = dstex.MipLevels;
	//	V_RETURN( pd3dDevice->CreateShaderResourceView( g_pRandomTexture, &SRVDesc, &g_pRandomTexRV ) );

	//	return hr;
	//}

};