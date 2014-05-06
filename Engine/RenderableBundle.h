#pragma once;
class IndexedMesh;
class OBJModel;
class MarchingCubeChunk;

struct RenderableBundle
{
	//std::vector<IndexedInstancedMesh> indexedInstancedMeshes;
	std::vector<IndexedMesh*> indexedMeshes;
	std::vector<OBJModel> objModels;
	std::vector<MarchingCubeChunk*> terrainChunks;
};