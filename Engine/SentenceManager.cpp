#include "SentenceManager.h"


SentenceManager::SentenceManager(ID3D11Device* device, unsigned int maxSentences, int width, int height)
: screenWidth(width), screenHeight(height), font()
{
	this->maxSentences = maxSentences;

	// Initialize the font object. 
	font.Initialize(device, "../Engine/data/fontdata.txt", L"../Engine/data/font.dds");

	//Initializes sentence vector and priority queue
	Reset();
}

SentenceManager::~SentenceManager()
{
}

void SentenceManager::Reset()
{
	//Reset sentences
	sentences.clear();
	sentences.resize(maxSentences);

	//Reset prioQueue
	prioQueue = std::priority_queue<int>();

	for(unsigned int i = 0; i < maxSentences; i++)
	{
		prioQueue.push(i);
		sentences[i].isActive = false;
	}
}

int SentenceManager::CreateSentence(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int maxLength, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor)
{
	std::vector<VertexType> vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	int vectorSlot = GetNextSentenceSlot();

	//Important to set this to true.
	sentences[vectorSlot].isActive = true;

	// Set the maximum length of the sentence.
	sentences[vectorSlot].maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	sentences[vectorSlot].vertexCount = 6 * maxLength; //NO! 4! BAD!

	// Set the number of indexes in the index array.
	sentences[vectorSlot].indexCount = sentences[vectorSlot].vertexCount;

	// Create the vertex array.
	vertices.resize(sentences[vectorSlot].vertexCount);

	// Create the index array.
	indices.resize(sentences[vectorSlot].indexCount);

	// Initialize the index array.
	for(i=0; i< sentences[vectorSlot].indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * sentences[vectorSlot].vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &sentences[vectorSlot].vertexBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * sentences[vectorSlot].indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &sentences[vectorSlot].indexBuffer.p);
	if(FAILED(result))
	{
		MessageBox(GetDesktopWindow(), L"Something went wrong when creating sentence buffer. Look in sentenceManager.", L"Error", MB_OK);
	}

	if(!UpdateSentence(deviceContext, vectorSlot, textString, positionX, positionY, rgbColor))
	{
		MessageBox(GetDesktopWindow(), L"Something went updating a sentence. Look in sentenceManager.", L"Error", MB_OK);
	}

	return vectorSlot;
}

bool SentenceManager::UpdateSentence(ID3D11DeviceContext* deviceContext, int id, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor)
{
	//Break early if sentence isn't active
	if(!sentences[id].isActive)
	{
		return false;
	}

	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Store the color of the sentence.
	sentences[id].ColorRGBA.x = rgbColor.x;
	sentences[id].ColorRGBA.y = rgbColor.y;
	sentences[id].ColorRGBA.z = rgbColor.z;
	sentences[id].ColorRGBA.w = 1.0f; //Whatever. Isn't used, it's alpha.

	// Get the number of letters in the sentence.
	numLetters = textString.length();

	// Check for possible buffer overflow.
	if(numLetters > sentences[id].maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentences[id].vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentences[id].vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((screenWidth / 2) * -1) + positionX);
	drawY = (float)((screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	font.BuildVertexArray((void*)vertices, textString.c_str(), drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentences[id].vertexBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentences[id].vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentences[id].vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}
