#include "MeshHandler.h"


MeshHandler::MeshHandler(void)
{
}


MeshHandler::~MeshHandler(void)
{
}

bool MeshHandler::LoadIndexedMeshFromFile(ID3D11Device* device, string filepath, IndexedMesh* outMesh )
{
	//////////////////////////////////////////////////////////////////////////
	// Load model information from file
	//////////////////////////////////////////////////////////////////////////

	ifstream fin;
	char input;
	//vector<ModelType> model;
	vector<VertexType> vertices;
	vector<unsigned int> indices;
	unsigned int vertexCount, indexCount;

	// Open the model file.
	fin.open(filepath);

	// If it could not open the file then exit.
	if(fin.fail())
	{
		return false;
	}
	else
	{
		// Read up to the value of vertex count.
		fin.get(input);
		while(input != ':')
		{
			fin.get(input);
		}

		// Read in the vertex count.
		fin >> vertexCount;

		// Set the number of indices to be the same as the vertex count.
		indexCount = vertexCount;

		// Create the model using the vertex count that was read in.
		//model.resize(vertexCount);
		vertices.resize(vertexCount);
		indices.resize(indexCount);

		// Read up to the beginning of the data.
		fin.get(input);
		while(input != ':')
		{
			fin.get(input);
		}
		fin.get(input);
		fin.get(input);

		// Read in the vertex data.
		for(unsigned int i=0; i < vertexCount; i++)
		{
			fin >> vertices[i].position.x >> vertices[i].position.y >> vertices[i].position.z;
			fin >> vertices[i].texcoords.x >> vertices[i].texcoords.y;
			fin >> vertices[i].normal.x >> vertices[i].normal.y >> vertices[i].normal.z;

			indices[i] = i;
		}
	}

	// Close the model file.
	fin.close();

	//vector<VertexType> vertices;
	//vertices.resize(vertexCount);

	//for(int i = 0; i < vertexCount; i++)
	//{
	//	vertices[i].position = XMFLOAT3(model[i].x, model[i].y, model[i].z);
	//	vertices[i].normal = XMFLOAT3(model[i].normalX, model[i].normalY, model[i].normalZ);
	//	vertices[i].texcoords.x = model[i].textureU;
	//	vertices[i].texcoords.y = model[i].textureV;
	//}


	//////////////////////////////////////////////////////////////////////////
	// Now we set up the mesh
	//////////////////////////////////////////////////////////////////////////	
	//////////////////////////////////////////////////////////////////////////	
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//Set relevant values
	outMesh->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	outMesh->SetVertexCount(vertexCount);
	outMesh->SetIndexCount(indexCount);
	outMesh->SetVertexStride(sizeof(VertexType));


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the sub resource texture a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, outMesh->GetVertexBufferPP());
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the sub resource texture a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, outMesh->GetIndexBufferPP());
	if(FAILED(result))
	{
		return false;
	}

	return true;
}
