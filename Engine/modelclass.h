#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <fstream>
#include <atlcomcli.h>
#include <memory>
#include <vector>


#include "textureclass.h"
#include "texturearray.h"

class ModelClass
{
private:
	struct ModelType
	{
		float x, y, z;
		float textureU, textureV;
		float normalX, normalY, normalZ;
		float tangentX, tangentY, tangentZ;
		float binormalX, binormalY, binormalZ;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct TempVertexType
	{
		float x, y, z;
		float textureU, textureV;
		float normalX, normalY, normalZ;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();	
	bool Initialize(ID3D11Device*, char*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView** GetTextureArray();

	UINT GetSizeOfVertexType();
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();


private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool LoadTextures(ID3D11Device*, WCHAR* texture, WCHAR* normal, WCHAR* specular);
	void ReleaseTextures();

	bool LoadModel(char*);
	void ReleaseModel();

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	CComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
	int vertexCount, indexCount;
	std::shared_ptr<TextureClass> texture;
	std::shared_ptr<TextureArray> textureArray;
	std::unique_ptr<ModelType []> model;
};



/*
	 functions for reading in .obj files.

	 bool isDuplicate;
	 indices.clear();
	 for(int i = 0; i < polygonCount; i++)
	 {
	 for(int i2 = 0; i2 < 3; i2++)
	 {
	 indices.push_back(faceDefinitions[i].vertexIndexes[i2]);
	 }
	 }

	 modelData->indices = new unsigned long[indices.size()];
	 modelData->indexCount = indices.size();

	 for(int i = 0; i < vertexCount; i++)
	 {
	 modelData->vertices[i].position = vertexPositions[i];
	 }

	 for(unsigned long i = 0; i < indices.size(); i++)
	 {
	 modelData->indices[i]  = indices[i];
	 }
	 
	 */

/* ******** How to make a cube... in dx9... ************/

 /*
	 // this is the function that puts the 3D models into video RAM
	 void init_graphics(void)
	 {
	 // create the vertices using the CUSTOMVERTEX struct
	 CUSTOMVERTEX vertices[] =
	 {
	 { -3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 0, 255), },
	 { 3.0f, 3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 0), },
	 { -3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(255, 0, 0), },
	 { 3.0f, -3.0f, -3.0f, D3DCOLOR_XRGB(0, 255, 255), },
	 { -3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(0, 0, 255), },
	 { 3.0f, 3.0f, 3.0f, D3DCOLOR_XRGB(255, 0, 0), },
	 { -3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 0), },
	 { 3.0f, -3.0f, 3.0f, D3DCOLOR_XRGB(0, 255, 255), },
	 };

	 // create a vertex buffer interface called v_buffer
	 d3ddev->CreateVertexBuffer(8*sizeof(CUSTOMVERTEX),
	 0,
	 CUSTOMFVF,
	 D3DPOOL_MANAGED,
	 &v_buffer,
	 NULL);

	 VOID* pVoid;    // a void pointer

	 // lock v_buffer and load the vertices into it
	 v_buffer->Lock(0, 0, (void**)&pVoid, 0);
	 memcpy(pVoid, vertices, sizeof(vertices));
	 v_buffer->Unlock();

	 // create the indices using an int array
	 short indices[] =
	 {
	 0, 1, 2,    // side 1
	 2, 1, 3,
	 4, 0, 6,    // side 2
	 6, 0, 2,
	 7, 5, 6,    // side 3
	 6, 5, 4,
	 3, 1, 7,    // side 4
	 7, 1, 5,
	 4, 5, 0,    // side 5
	 0, 5, 1,
	 3, 7, 2,    // side 6
	 2, 7, 6,
	 };

	 // create an index buffer interface called i_buffer
	 d3ddev->CreateIndexBuffer(36*sizeof(short),
	 0,
	 D3DFMT_INDEX16,
	 D3DPOOL_MANAGED,
	 &i_buffer,
	 NULL);

	 // lock i_buffer and load the indices into it
	 i_buffer->Lock(0, 0, (void**)&pVoid, 0);
	 memcpy(pVoid, indices, sizeof(indices));
	 i_buffer->Unlock();
	 }
*/