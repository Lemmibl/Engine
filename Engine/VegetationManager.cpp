#include "VegetationManager.h"

VegetationManager::VegetationManager()
{

}

VegetationManager::VegetationManager( const VegetationManager& )
{

}

VegetationManager::~VegetationManager()
{

}

bool VegetationManager::Initialize(ID3D11Device* device, WCHAR* filename1, WCHAR* filename2)
{
	bool result;


	textureArray[0] = new TextureClass();

	// Initialize the texture object.
	result = textureArray[0]->Initialize(device, filename1);
	if(!result)
	{
		return false;
	}

	textureArray[1] = new TextureClass();

	// Initialize the texture object.
	result = textureArray[1]->Initialize(device, filename1);
	if(!result)
	{
		return false;
	}

	return true;
}

bool VegetationManager::SetupQuads( ID3D11Device* device, std::vector<XMFLOAT3>* positions )
{

	#pragma region old quad code
	//// Set the number of vertices in the vertex array.
	//vertexCount = 4;

	//// Set the number of indices in the index array.
	//indexCount = 6;

	//// Load the vertex array with data.
	//vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	//vertices[0].textexture = XMFLOAT2(0.0f, 1.0f);
	//vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
	//vertices[1].textexture = XMFLOAT2(0.0f, 0.0f);
	//vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
	//vertices[2].textexture = XMFLOAT2(1.0f, 0.0f);
	//vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	//vertices[3].textexture = XMFLOAT2(1.0f, 1.0f);
	//vertices[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	//// Load the index array with data.
	//// First triangle
	//indices[0] = 0;	// Bottom left.
	//indices[1] = 1;	// Top left.
	//indices[2] = 3;	// Bottom right.

	//// Second triangle
	//indices[3] = 1;	// Top left.
	//indices[4] = 2;	// Top right.
	//indices[5] = 3;	// Bottom right.

	/* 
	// CW:  T1: 0,1,3  T2: 1,2,3
	// CCW: T1: 0,3,1  T2: 1,3,2
	*/
	#pragma endregion

	VertexType* vertices;
	InstanceType* instances;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	vertexCount = 8;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	//Quad #1
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texCoord = XMFLOAT2(0.0f, 1.0f);
	vertices[0].texID = 0;

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
	vertices[1].texCoord = XMFLOAT2(0.0f, 0.0f);
	vertices[1].texID = 0;

	vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
	vertices[2].texCoord = XMFLOAT2(1.0f, 0.0f);
	vertices[2].texID = 0;

	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[3].texCoord = XMFLOAT2(1.0f, 1.0f);
	vertices[3].texID = 0;

	//Quad #2
	vertices[4].position = XMFLOAT3(0.0f, -1.0f, -1.0f);  // Bottom left.
	vertices[4].texCoord = XMFLOAT2(0.0f, 1.0f);
	vertices[4].texID = 1;

	vertices[5].position = XMFLOAT3(0.0f, 1.0f, -1.0f);  // Top left.
	vertices[5].texCoord = XMFLOAT2(0.0f, 0.0f);
	vertices[5].texID = 1;

	vertices[6].position = XMFLOAT3(0.0f, 1.0f, 1.0f);  // Top right.
	vertices[6].texCoord = XMFLOAT2(1.0f, 0.0f);
	vertices[6].texID = 1;

	vertices[7].position = XMFLOAT3(0.0f, -1.0f, 1.0f);  // Bottom right.
	vertices[7].texCoord = XMFLOAT2(1.0f, 1.0f);
	vertices[7].texID = 1;


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex array now that the vertex buffer has been created and loaded.
	delete [] vertices;
	vertices = 0;

	// Set the number of instances in the array.
	instanceCount = positions->size();

	// Create the instance array.
	instances = new InstanceType[instanceCount];
	if(!instances)
	{
		return false;
	}

	// Load the instance array with data.
	for(int i = 0; i < positions->size(); i++)
	{
		instances[i].position = positions->at(i);
	}

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the instance array now that the instance buffer has been created and loaded.
	delete [] instances;
	instances = 0;

	return true;
}

void VegetationManager::Shutdown()
{
	// Release the instance buffer.
	if(instanceBuffer)
	{
		instanceBuffer->Release();
		instanceBuffer = 0;
	}

	// Release the vertex buffer.
	if(vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	delete textureArray[0];
	textureArray[0] = 0;

	delete textureArray[1];
	textureArray[1] = 0;
}

void VegetationManager::Render(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	//Set the buffer strides.
	strides[0] = sizeof(VertexType); 
	strides[1] = sizeof(InstanceType); 

	//Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = vertexBuffer;	
	bufferPointers[1] = instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}