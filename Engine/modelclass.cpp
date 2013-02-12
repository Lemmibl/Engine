////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

int ModelClass::GetIndexCount()
{
	return indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return texture->GetTexture();
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return textureArray->GetTextureArray();
}

UINT ModelClass::GetSizeOfVertexType()
{
	return sizeof(VertexType);
}

ID3D11Buffer* ModelClass::GetVertexBuffer()
{
	return vertexBuffer;
}

ID3D11Buffer* ModelClass::GetIndexBuffer()
{
	return indexBuffer;
}

ModelClass::ModelClass()
{
	vertexBuffer = 0;
	indexBuffer = 0;
	model = 0;
	texture = 0;
	textureArray = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename1, WCHAR* textureFilename2, WCHAR* textureFilename3)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if(!result)
	{
		return false;
	}

	// Calculate the normal, tangent, and binormal vectors for the model.
	CalculateModelVectors();

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the textures for this model.
	result = LoadTexture(device, textureFilename1);
	if(!result)
	{
		return false;
	}

	// Load the textures for this model.
	result = LoadTextures(device, textureFilename1, textureFilename2, textureFilename3);
	if(!result)
	{
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Release the model texture(s).
	ReleaseTexture();
	ReleaseTextures();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	
	#pragma region old quad code
	//// Set the number of vertices in the vertex array.
	//vertexCount = 4;

	//// Set the number of indices in the index array.
	//indexCount = 6;

	//// Load the vertex array with data.
	//vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	//vertices[0].textexture = D3DXVECTOR2(0.0f, 1.0f);
	//vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);  // Top left.
	//vertices[1].textexture = D3DXVECTOR2(0.0f, 0.0f);
	//vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);  // Top right.
	//vertices[2].textexture = D3DXVECTOR2(1.0f, 0.0f);
	//vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[3].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	//vertices[3].textexture = D3DXVECTOR2(1.0f, 1.0f);
	//vertices[3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//// Load the index array with data.
	//// First triangle
	//indices[0] = 0;	// Bottom left.
	//indices[1] = 1;	// Top left.
	//indices[2] = 3;	// Bottom right.

	//// Second triangle
	//indices[3] = 1;	// Top left.
	//indices[4] = 2;	// Top right.
	//indices[5] = 3;	// Bottom right.

	///* 
	// CW:  T1: 0,1,3  T2: 1,2,3
	// CCW: T1: 0,3,1  T2: 1,3,2
	//*/
	#pragma endregion

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for(int i=0; i < vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(model[i].x, model[i].y, model[i].z);
		vertices[i].texture = D3DXVECTOR2(model[i].textureU, model[i].textureV);
		vertices[i].normal = D3DXVECTOR3(model[i].normalX, model[i].normalY, model[i].normalZ);
		vertices[i].tangent = D3DXVECTOR3(model[i].tangentX, model[i].tangentY, model[i].tangentZ);
		vertices[i].binormal = D3DXVECTOR3(model[i].binormalX, model[i].binormalY, model[i].binormalZ);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the sub resource texture a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
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

	// Give the sub resource texture a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

	// Set the tangentYpe of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	texture = new TextureClass();
	if(!texture)
	{
		return false;
	}

	result = texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture array object.
	if(texture)
	{
		texture->Shutdown();
		delete texture;
		texture = 0;
	}

	return;
}

bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* texture, WCHAR* normal, WCHAR* specular)
{
	bool result;

	// Create the texture array object.
	textureArray = new TextureArray;
	if(!textureArray)
	{
		return false;
	}

	// Initialize the texture array object.
	result = textureArray->Initialize(device, texture, normal, specular);
	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTextures()
{
	// Release the texture array object.
	if(textureArray)
	{
		textureArray->Shutdown();
		delete textureArray;
		textureArray = 0;
	}

	return;
}


bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;

	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if(fin.fail())
	{
		return false;
	}

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
	model = new ModelType[vertexCount];
	if(!model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for(i=0; i<vertexCount; i++)
	{
		fin >> model[i].x >> model[i].y >> model[i].z;
		fin >> model[i].textureU >> model[i].textureV;
		fin >> model[i].normalX >> model[i].normalY >> model[i].normalZ;
	}

	// Close the model file.
	fin.close();

	return true;
}

void ModelClass::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for(i=0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.x = model[index].x;
		vertex1.y = model[index].y;
		vertex1.z = model[index].z;
		vertex1.textureU = model[index].textureU;
		vertex1.textureV = model[index].textureV;
		vertex1.normalX = model[index].normalX;
		vertex1.normalY = model[index].normalY;
		vertex1.normalZ = model[index].normalZ;
		index++;

		vertex2.x = model[index].x;
		vertex2.y = model[index].y;
		vertex2.z = model[index].z;
		vertex2.textureU = model[index].textureU;
		vertex2.textureV = model[index].textureV;
		vertex2.normalX = model[index].normalX;
		vertex2.normalY = model[index].normalY;
		vertex2.normalZ = model[index].normalZ;
		index++;

		vertex3.x = model[index].x;
		vertex3.y = model[index].y;
		vertex3.z = model[index].z;
		vertex3.textureU = model[index].textureU;
		vertex3.textureV = model[index].textureV;
		vertex3.normalX = model[index].normalX;
		vertex3.normalY = model[index].normalY;
		vertex3.normalZ = model[index].normalZ;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		model[index-1].normalX = normal.x;
		model[index-1].normalY = normal.y;
		model[index-1].tangentX = tangent.x;
		model[index-1].tangentY = tangent.y;
		model[index-1].tangentZ = tangent.z;
		model[index-1].binormalX = binormal.x;
		model[index-1].binormalY = binormal.y;
		model[index-1].binormalZ = binormal.z;

		model[index-2].normalX = normal.x;
		model[index-2].normalY = normal.y;
		model[index-2].normalZ = normal.z;
		model[index-2].tangentX = tangent.x;
		model[index-2].tangentY = tangent.y;
		model[index-2].tangentZ = tangent.z;
		model[index-2].binormalX = binormal.x;
		model[index-2].binormalY = binormal.y;
		model[index-2].binormalZ = binormal.z;

		model[index-3].normalX = normal.x;
		model[index-3].normalY = normal.y;
		model[index-3].normalZ = normal.z;
		model[index-3].tangentX = tangent.x;
		model[index-3].tangentY = tangent.y;
		model[index-3].tangentZ = tangent.z;
		model[index-3].binormalX = binormal.x;
		model[index-3].binormalY = binormal.y;
		model[index-3].binormalZ = binormal.z;
	}

	return;
}

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float textureUVector[2], textureVVector[2];
	float denominator;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the textureU and textureV texture space vectors.
	textureUVector[0] = vertex2.textureU - vertex1.textureU;
	textureVVector[0] = vertex2.textureV - vertex1.textureV;

	textureUVector[1] = vertex3.textureU - vertex1.textureU;
	textureVVector[1] = vertex3.textureV - vertex1.textureV;

	// Calculate the denominator of the tangent/binormal equation.
	denominator = 1.0f / (textureUVector[0] * textureVVector[1] - textureUVector[1] * textureVVector[0]);

	// Calculate the cross products and multiply binormalY the coefficient to get the tangent and binormal.
	tangent.x = (textureVVector[1] * vector1[0] - textureVVector[0] * vector2[0]) * denominator;
	tangent.y = (textureVVector[1] * vector1[1] - textureVVector[0] * vector2[1]) * denominator;
	tangent.z = (textureVVector[1] * vector1[2] - textureVVector[0] * vector2[2]) * denominator;

	binormal.x = (textureUVector[0] * vector2[0] - textureUVector[1] * vector1[0]) * denominator;
	binormal.y = (textureUVector[0] * vector2[1] - textureUVector[1] * vector1[1]) * denominator;
	binormal.z = (textureUVector[0] * vector2[2] - textureUVector[1] * vector1[2]) * denominator;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}

void ModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;


	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}


void ModelClass::ReleaseModel()
{
	if(model)
	{
		delete [] model;
		model = 0;
	}

	return;
}
