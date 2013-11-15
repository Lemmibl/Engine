////////////////////////////////////////////////////////////////////////////////
// Filename: bitmapclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "bitmapclass.h"

int BitmapClass::GetIndexCount()
{
	return indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return texture->GetTexture();
}

BitmapClass::BitmapClass()
{
}


BitmapClass::BitmapClass(const BitmapClass& other)
{
}


BitmapClass::~BitmapClass()
{
}


bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	// Store the screen size.
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	this->bitmapWidth = bitmapWidth;
	this->bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	previousPosX = -1;
	previousPosY = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if(!result)
	{
		return false;
	}

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	std::vector<VertexType> vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	vertexCount = 4;

	// Set the number of indices in the index array.
	indexCount = 6;

	// Create the index array.
	indices.resize(indexCount);

	// Initialize vertex array to zeros at first.
	vertices.resize(vertexCount);

	// Load the index array with data.
	// First triangle
	indices[0] = 0;	// Bottom left.
	indices[1] = 1;	// Top left.
	indices[2] = 3;	// Bottom right.

	// Second triangle
	indices[3] = 1;	// Top left.
	indices[4] = 2;	// Top right.
	indices[5] = 3;	// Bottom right.

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //Note dynamic instead of default.
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the sub resource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the sub resource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	//// Release the arrays now that the vertex and index buffers have been created and loaded.
	//delete [] vertices;
	//vertices = 0;

	//delete [] indices;
	//indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	//// Release the index buffer.
	//if(indexBuffer)
	//{
	//	indexBuffer->Release();
	//	indexBuffer = 0;
	//}

	//// Release the vertex buffer.
	//if(vertexBuffer)
	//{
	//	vertexBuffer->Release();
	//	vertexBuffer = 0;
	//}

	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* vertices;
	VertexType* verticesPtr;
	HRESULT result;

	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	if((positionX == previousPosX) && (positionY == previousPosY))
	{
		return true;
	}

	// If it has changed then update the position it is being rendered to.
	previousPosX = positionX;
	previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)bitmapHeight;

	screenCenterX = screenWidth/2;
	screenCenterY = screenHeight/2;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	vertices[0].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[2].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[3].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	texture = std::shared_ptr<TextureClass>(new TextureClass());
	if(!texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void BitmapClass::ReleaseTexture()
{
	//// Release the texture object.
	//if(texture)
	//{
	//	texture->Shutdown();
	//	delete texture;
	//	texture = 0;
	//}

	return;
}