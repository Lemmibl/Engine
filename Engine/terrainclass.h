////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TERRAINCLASS_H_
#define _TERRAINCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"

///////////////
// CONSTANTS //
///////////////
const int TEXTURE_REPEAT = 4;


////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainClass
////////////////////////////////////////////////////////////////////////////////
class TerrainClass
{
private:
	//struct VertexType
	//{
	//	D3DXVECTOR3 position;
	//	D3DXVECTOR4 color;
	//};
	//struct HeightMapType 
	//{ 
	//	float x, y, z;
	//};
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
	struct HeightMapType 
	{ 
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct VectorType 
	{ 
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	//device, width, height, normalizeMultiplier, heightmap texture filepath, texture filepath
	bool Initialize(ID3D11Device*, int, int, float, char*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	int GetVertexCount();
	void CopyVertexArray(void*);

private:
	bool LoadHeightMap(char*);
	bool CalculateNormals();
	void NormalizeHeightMap();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

private:
	int terrainWidth, terrainHeight, vertexCount;
	float normalizeMultiplier;
	HeightMapType* heightMap;
	TextureClass* texture;
	VertexType* vertices;
};

#endif