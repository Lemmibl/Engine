#include "Skysphere.h"

#pragma region Properties
int Skysphere::GetIndexCount()
{
	return indexCount;
}

XMFLOAT4 Skysphere::GetApexColor()
{
	return apexColor;
}

XMFLOAT4 Skysphere::GetCenterColor()
{
	return centerColor;
}

void Skysphere::SetApexColor(XMFLOAT4 val)
{
	apexColor = val;
}

void Skysphere::SetCenterColor(XMFLOAT4 val)
{
	centerColor = val;
}
#pragma endregion Properties

Skysphere::Skysphere()
:	skysphereShader()
{
}

Skysphere::~Skysphere()
{
}

bool Skysphere::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Load in the sky dome model.
	result = LoadModel("../Engine/data/sphere.txt");
	if(!result)
	{
		return false;
	}

	// Load the sky dome into a vertex and index buffer for rendering.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	//http://www.picturetopeople.org/color_converter.html
	//Or you could just use the normal [0,255] number and divide it by 255 to get the [0,1] counterpart.

	//http://www.foszor.com/blog/xna-color-chart/

	//LightSalmon = {R:255 G:160 B:122 A:255}
	//Cornflower Blue = {R:100 G:149 B:237 A:255}
	//Deep Sky Blue = {R:0 G:191 B:255 A:255}
	//Dark Turquoise = {R:0 G:206 B:209 A:255}
	apexColor = XMFLOAT4(0.9f, 160.0f/255, 0.9f, 1.0f);
	centerColor =  XMFLOAT4(0.538f, 0.568f, 0.960f, 1.0f);


	result = skysphereShader.Initialize(device, hwnd);
	if(!result)
	{
		return false;
	}

	return true;
}

void Skysphere::Shutdown()
{
	// Release the vertex and index buffer that were used for rendering the sky dome.
	ReleaseBuffers();

	// Release the sky dome model.
	ReleaseModel();

	//if(skysphereShader)
	//{
	//	skysphereShader->Shutdown();
	//	delete skysphereShader;
	//	skysphereShader = 0;
	//}

	return;
}

void Skysphere::Render(ID3D11DeviceContext* context, XMMATRIX* world, XMMATRIX* worldViewProjection, float cameraYPos, XMFLOAT4* fogColor, float time, StageOfDay stageOfDay, float lightIntensity)
{
	// Render the sky dome.
	RenderBuffers(context);

	skysphereShader.Render(context, GetIndexCount(), world, worldViewProjection, cameraYPos, apexColor, centerColor, *fogColor, time, &cloudTexture.p, &starTexture.p, stageOfDay, lightIntensity);

	return;
}

bool Skysphere::LoadModel(char* filename)
{
	std::ifstream fin;
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
	model = std::unique_ptr<ModelType[]>(new ModelType[vertexCount]());
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

	int counter = 0;
	int faceID = 1;

	// Read in the vertex data.
	for(i=0; i<vertexCount; i++)
	{
		if(counter > 5)
		{
			faceID++;
			counter = 0;
		}

		fin >> model[i].position.x	>> model[i].position.y	>> model[i].position.z;
		fin >> model[i].texcoords.x >> model[i].texcoords.y;
		fin >> model[i].normal.x	>> model[i].normal.y	>> model[i].normal.z;

		 model[i].texcoords.z = faceID;

		counter++;
	}

	// Close the model file.
	fin.close();

	return true;
}

void Skysphere::ReleaseModel()
{
	//if(model)
	//{
	//	delete [] model;
	//	model = 0;
	//}

	return;
}

bool Skysphere::InitializeBuffers(ID3D11Device* device)
{
	std::unique_ptr<VertexType []> vertices;
	std::unique_ptr<unsigned long []> indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = std::unique_ptr<VertexType[]>(new VertexType[vertexCount]());
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = std::unique_ptr<unsigned long[]>(new unsigned long[indexCount]());
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for(i=0; i<vertexCount; i++)
	{
		vertices[i].position	= model[i].position;
		vertices[i].normal		= model[i].normal;
		vertices[i].texcoords	= model[i].texcoords;
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.get();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.get();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	//delete [] vertices;
	//vertices = 0;

	//delete [] indices;
	//indices = 0;

	return true;
}

void Skysphere::ReleaseBuffers()
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

void Skysphere::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.p, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
