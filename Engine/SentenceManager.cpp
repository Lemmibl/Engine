#include "SentenceManager.h"


SentenceManager::SentenceManager(ID3D11Device* device, int width, int height)
: screenWidth(width), screenHeight(height), font()
{
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
	//Reset prioQueue
	prioQueue = std::priority_queue<unsigned short, std::vector<unsigned short>, std::greater<unsigned short>>();

	activeSentenceCount = 0;

	for(unsigned int i = 0; i < MaxSentences; i++)
	{
		prioQueue.push(i);
		indices[i].index = i;
		indices[i].generation = 0;
	}
}

SentenceManager::Handle SentenceManager::CreateSentence(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int maxLength, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor)
{
	Handle tempHandle;

	tempHandle.id = MaxSentences+1;
	tempHandle.generation = 0;

	//Bounds check
	if(activeSentenceCount > MaxSentences)
	{
		return tempHandle;
	}

	std::vector<VertexType> vertices;
	std::vector<unsigned long> meshIndices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//We get the next open index slot to write to
	Index& newIndex = indices[GetNextSentenceSlot()];

	//Assign the index to the next free sentence slot.
	newIndex.index = activeSentenceCount++;
	newIndex.generation++;

	const unsigned short vectorSlot = newIndex.index;

	sentences[vectorSlot].id = newIndex.index;

	// Set the maximum length of the sentence.
	sentences[vectorSlot].maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	sentences[vectorSlot].vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	sentences[vectorSlot].indexCount = sentences[vectorSlot].vertexCount;

	// Create the vertex array.
	vertices.resize(sentences[vectorSlot].vertexCount);

	// Create the index array.
	meshIndices.resize(sentences[vectorSlot].indexCount);

	// Initialize the index array.
	for(i=0; i< sentences[vectorSlot].indexCount; i++)
	{
		meshIndices[i] = i;
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
		MessageBox(GetDesktopWindow(), L"Something went wrong when creating sentence vertex buffer. Look in sentenceManager.", L"Error", MB_OK);
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * sentences[vectorSlot].indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = meshIndices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &sentences[vectorSlot].indexBuffer.p);
	if(FAILED(result))
	{
		MessageBox(GetDesktopWindow(), L"Something went wrong when creating sentence index buffer. Look in sentenceManager.", L"Error", MB_OK);
	}

	//Because for some design reason I don't remember (I was drunk), all my external functions recieve a Handle instead of an Index
	tempHandle.id = newIndex.index;
	tempHandle.generation = newIndex.generation;
	
	if(!UpdateSentence(deviceContext, tempHandle, textString, positionX, positionY, rgbColor))
	{
		MessageBox(GetDesktopWindow(), L"Something went updating a sentence. Look in sentenceManager.", L"Error", MB_OK);
	}

	return tempHandle;
}

bool SentenceManager::UpdateSentence(ID3D11DeviceContext* deviceContext, Handle handle, std::string textString, int positionX, int positionY, XMFLOAT3 rgbColor)
{
	Index& idx = indices[handle.id];

	if(idx.generation != handle.generation)
	{
		return false;
	}

	unsigned int id = idx.index;

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

void SentenceManager::ReleaseSentence( int id )
{
	//We remove a sentence. This is how we do that:
	// 1) Swap furthest out sentence with the one we should delete. 
	// 2) Update indices to reflect this.
	// 3) Reduce activeSentences by one. 
	// 4) Re-add the ID to the prio queue so that it can be reused.

	Index idx = indices[id];

	//Get reference to the sentence we're going to remove.
	SentenceType& sentence = sentences[idx.index];
	sentence.id = MaxSentences+1;

	//Swap sentence with furthest out active sentence.
	std::swap(sentence, sentences[activeSentenceCount-1]);

	//Swap indices.
	indices[activeSentenceCount-1].index = idx.index;

	//Decrement active sentences, essentially invalidating the swapped sentence. It's still technically there, just that we're not going to read or access it.
	activeSentenceCount--;

	//Set index to a value that is per design "unreachable" to signify that it's empty.
	idx.index = MaxSentences+1;

	//Add the ID back to the priority queue to let the allocator know that this ID slot is available.
	prioQueue.push(id);
}

bool SentenceManager::IsSentenceValid(Handle handle)
{
	Index& idx = indices[handle.id];

	return (idx.generation == handle.generation && idx.index <= MaxSentences);
}
