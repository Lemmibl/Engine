#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>

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
	
	//http://faculty.ycp.edu/~dbabcock/PastCourses/cs470/labs/lab11.html
	//http://www.gamedev.net/topic/583161-d3d11-creating-a-dynamic-texture/
	//http://gamedev.stackexchange.com/questions/27690/reading-from-a-staging-2d-texture-array-in-directx10
	//http://www.gamedev.net/topic/622350-loading-of-3d-textures-from-a-2d-texture/

	/*
	By using Texture Arrays. When you fill out your D3D11_TEXTURE2D_DESC look at the ArraySize member. 
	This desc struct is the one that gets passed to ID3D11Device::CreateTexture2D. 
	Then in your shader you use a 3rd texcoord sampling index which indicates which 2D texture in the array you are referring to.
	*/

	/*
	void TreeSprites::buildShaderResourceView()
	{
		//
		// Load the texture elements individually from file.  These textures
		// won't be used by the GPU (0 bind flags), they are just used to 
		// load the image data from file.  We use the STAGING usage so the
		// CPU can read the resource.
		//

		std::wstring filenames[4] = 
		{
			L"tree0.dds",
			L"tree1.dds",
			L"tree2.dds",
			L"tree3.dds"
		};


		//TODO: Add Code to load in textures from file

		ID3D10Texture2D* srcTex[4];
		for(UINT i = 0; i < 4; ++i)
		{
			D3DX10_IMAGE_LOAD_INFO loadInfo;

			loadInfo.Width  = D3DX10_FROM_FILE;
			loadInfo.Height = D3DX10_FROM_FILE;
			loadInfo.Depth  = D3DX10_FROM_FILE;
			loadInfo.FirstMipLevel = 0;
			loadInfo.MipLevels = D3DX10_FROM_FILE;
			loadInfo.Usage = D3D10_USAGE_STAGING;
			loadInfo.BindFlags = 0;
			loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
			loadInfo.MiscFlags = 0;
			loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			loadInfo.Filter = D3DX10_FILTER_NONE;
			loadInfo.MipFilter = D3DX10_FILTER_NONE;
			loadInfo.pSrcInfo  = 0;

			HR(D3DX10CreateTextureFromFile(md3dDevice, filenames[i].c_str(),
				&loadInfo, 0, (ID3D10Resource**)&srcTex[i], 0));
		}

		//
		// Create the texture array.  Each element in the texture 
		// array has the same format/dimensions.
		//

		D3D10_TEXTURE2D_DESC texElementDesc;
		srcTex[0]->GetDesc(&texElementDesc);

		D3D10_TEXTURE2D_DESC texArrayDesc;
		texArrayDesc.Width              = texElementDesc.Width;
		texArrayDesc.Height             = texElementDesc.Height;
		texArrayDesc.MipLevels          = texElementDesc.MipLevels;
		texArrayDesc.ArraySize          = 4;
		texArrayDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
		texArrayDesc.SampleDesc.Count   = 1;
		texArrayDesc.SampleDesc.Quality = 0;
		texArrayDesc.Usage              = D3D10_USAGE_DEFAULT;
		texArrayDesc.BindFlags          = D3D10_BIND_SHADER_RESOURCE;
		texArrayDesc.CPUAccessFlags     = 0;
		texArrayDesc.MiscFlags          = 0;


		//TODO: Declare a description for a texture array
		ID3D10Texture2D* texArray = 0;
		HR(md3dDevice->CreateTexture2D( &texArrayDesc, 0, &texArray));

		//
		// Copy individual texture elements into texture array.
		//
		for(UINT i = 0; i < 4; ++i)
		{
			// for each mipmap level...
			for(UINT j = 0; j < texElementDesc.MipLevels; ++j)
			{
				D3D10_MAPPED_TEXTURE2D mappedTex2D;
				srcTex[i]->Map(j, D3D10_MAP_READ, 0, &mappedTex2D);

				md3dDevice->UpdateSubresource(texArray,
					D3D10CalcSubresource(j, i, texElementDesc.MipLevels),
					0, mappedTex2D.pData, mappedTex2D.RowPitch, 0);

				srcTex[i]->Unmap(j);
			}
		}

		//
		// Create a resource view to the texture array.
		//

		D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = 4;

		HR(md3dDevice->CreateShaderResourceView(texArray, &viewDesc, &mTreeMapArrayRV));

		//
		// Cleanup--we only need the resource view.
		//

		ReleaseCOM(texArray);

		for(UINT i = 0; i < 4; ++i)
			ReleaseCOM(srcTex[i]);
	}*/

};