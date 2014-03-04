#pragma once
#include <vector>

class OBJModel
{
public:
	OBJModel();
	~OBJModel();

	const unsigned short GetSubsetCount()		const { return subsetCount; }
	const unsigned short GetTextureCount()		const { return textureCount; }
	const unsigned short GetMaterialCount()		const { return materialCount; } 
	const unsigned short GetMeshHandle()		const { return meshHandle; }

	//.. Well you come up with a better name for it. : |
	const std::vector<unsigned int>& GetSubSetIndices() { return meshSubsetIndices; }
	const std::vector<unsigned short>& GetTextureHandles() { return textureHandles; }
	const std::vector<unsigned short>& GetMaterialHandles() { return materialHandles; }

	void InsertSubsetIndexStart(unsigned int val)		{ meshSubsetIndices.push_back(val);		subsetCount++; }
	void InsertTextureHandle(unsigned short val)		{ textureHandles.push_back(val);		textureCount++; }
	void InsertMaterialHandle(unsigned short val)		{ materialHandles.push_back(val);		materialCount++; }
	void SetMeshHandle(unsigned short mesh)				{ meshHandle = mesh; }

private:
	unsigned short subsetCount, textureCount, materialCount;
	unsigned short meshHandle; //Handle into MeshHandler to get the mesh
	
	//For each submesh there is a range of indices to separate them. Each submesh has a start, and the end is defined by getting the start of the next submesh.
	std::vector<unsigned int> meshSubsetIndices;

	//For each group or submesh, there is a texture and a material handle
	std::vector<unsigned short> textureHandles;
	std::vector<unsigned short> materialHandles;


	//TODO: Transform handles?
};

