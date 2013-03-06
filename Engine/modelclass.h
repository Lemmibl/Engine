////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <Xnamath.h>
#include <fstream>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "texturearray.h"



////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
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

	//This struct is for quads.

	//struct VertexType
	//{
	//	XMFLOAT3 position;
	//	XMFLOAT4 color;
	//};


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

	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;
	TextureClass* texture;
	ModelType* model;
	TextureArray* textureArray;
};

#endif

