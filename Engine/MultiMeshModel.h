#pragma once
#include "IndexedMesh.h"
#include <vector>
#include "TextureHandler.h"

class MultiMeshModel
{
public:
	MultiMeshModel();
	~MultiMeshModel();

	std::vector<IndexedMesh*>* GetMeshes() { return &meshes; } 

private:
	std::vector<IndexedMesh*> meshes; //TODO... Handle?
	std::vector<ID3D11ShaderResourceView**> materialTextures; //TODO... Handle?
	std::vector<ID3D11ShaderResourceView**> textures; //TODO... Handle?
};

