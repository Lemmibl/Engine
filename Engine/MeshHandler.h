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
#include "OBJModel.h"

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

	struct CacheEntry
	{
		unsigned int index;
		CacheEntry* next;
	};

public:
	MeshHandler();
	~MeshHandler();

	bool Initialize(TextureAndMaterialHandler* textureAndMaterialHandler);

	//Returns true on success
	bool LoadModelFromOBJFile(ID3D11Device* device, std::wstring filepath, OBJModel* outModel);

	//Returns true on success
	bool LoadIndexedMeshFromTXTFile(ID3D11Device* device, std::wstring filepath, IndexedMesh* outMesh);

	IndexedMesh* GetMesh(unsigned short handle) { return meshes.GetSpecificObject(handle); }

private:
	bool LoadOBJGeometry(ID3D11Device* device, std::wstring filepath, OBJModel* outModel);
	bool LoadOBJMaterials(ID3D11Device* device, std::wstring filepath, OBJModel* outModel);
	unsigned int AddVertex(unsigned int hash, VertexType vertex);
	void DeleteCache();

	bool CheckForDuplicates(std::wstring filepath, OBJModel& potentialReturnHandle);

private:
	DODContainer<IndexedMesh> meshes;
	TextureAndMaterialHandler* texAndMatHandler;
	std::wstring meshMatLib; //String to hold our obj material library filename

	std::vector<CacheEntry*> vertexCache;
	std::vector<std::wstring> textureNameArray;
	std::vector<std::wstring> meshMaterials;

	//Pairings to check for duplicates whenever we add an object.
	//If there is a duplicate (filepath is identical) then we just return the handle to that object instead of loading in exactly the same model.
	std::vector<std::pair<std::wstring, OBJModel>> filePathAndModelPairings;

	std::vector<std::pair<std::wstring, unsigned short>> matNameAndHandlePairings;
};

