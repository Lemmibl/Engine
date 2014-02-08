#pragma once
#include <windows.h>
#include <xnamath.h>
#include <string>
#include <d3d11.h>
#include <fstream>
#include <vector>
#include <istream>
#include <atlcomcli.h>
#include <sstream>

#include "IndexedMesh.h"
#include "TextureAndMaterialHandler.h"
#include "DODContainer.h"

class MeshHandler
{
private:
	struct ModelType
	{
		float x, y, z;
		float textureU, textureV;
		float normalX, normalY, normalZ;
		float tangentX, tangentY, tangentZ; //Get rid of?
		float binormalX, binormalY, binormalZ;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoords;
		XMFLOAT3 normal;
	};

public:
	MeshHandler();
	~MeshHandler();

	bool Initialize(TextureAndMaterialHandler* textureAndMaterialHandler);

	//Returns true on success
	bool LoadIndexedMeshFromOBJFile(ID3D11Device* device, std::wstring filepath, IndexedMesh* outMesh);

	//Returns true on success
	bool LoadIndexedMeshFromTXTFile(ID3D11Device* device, std::wstring filepath, IndexedMesh* outMesh);

	private:
	DODContainer<IndexedMesh> meshes;
	DODContainer<TextureAndMaterialHandler::OBJMaterialStruct> materials;
	TextureAndMaterialHandler* texAndMatHandler;
	std::vector<int> meshSubsetIndexStart;
	std::vector<int> meshSubsetTexture;

	std::vector<ID3D11ShaderResourceView*> meshSRV;
	std::vector<std::wstring> textureNameArray;

};

