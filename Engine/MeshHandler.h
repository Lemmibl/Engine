#pragma once
#include "IndexedMesh.h"
#include <string>
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <fstream>
#include <atlcomcli.h>
#include <memory>
#include <vector>

using namespace std;

class MeshHandler
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
		XMFLOAT2 texcoords;
		XMFLOAT3 normal;
	};

public:
	MeshHandler();
	~MeshHandler();

	//Returns true on success
	bool LoadIndexedMeshFromFile(ID3D11Device* device, string filepath, IndexedMesh* outMesh);
};

