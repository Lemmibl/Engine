#pragma once
#include <stdlib.h>
#include <D3D11.h>
#include <d3dx9.h>
#include <string>
#include <d3dx11tex.h>
#include "MCTerrainClass.h"
#include <vector>

class Utility
{
public:
	Utility();
	Utility(const Utility&);
	~Utility();

	float Random();
	HRESULT Build2DTextureArray(ID3D11Device* device, ID3D11DeviceContext* deviceContext, 
		WCHAR** filenames, int textureCount, ID3D11ShaderResourceView** textureArray);
	
	//http://faculty.ycp.edu/~dbabcock/PastCourses/cs470/labs/lab11.html
	//http://www.gamedev.net/topic/583161-d3d11-creating-a-dynamic-texture/
	//http://gamedev.stackexchange.com/questions/27690/reading-from-a-staging-2d-texture-array-in-directx10
	//http://www.gamedev.net/topic/622350-loading-of-3d-textures-from-a-2d-texture/

	/*
	By using Texture Arrays. When you fill out your D3D11_TEXTURE2D_DESC look at the ArraySize member. 
	This desc struct is the one that gets passed to ID3D11Device::CreateTexture2D. 
	Then in your shader you use a 3rd texcoord sampling index which indicates which 2D texture in the array you are referring to.
	*/
};