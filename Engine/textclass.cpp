///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass()
{
	font = 0;
	fontShader = 0;

	fpsCount = 0;
	cpuLoad = 0;

	mousePosX = 0;
	mousePosY = 0;

	cameraPosX = 0;
	cameraPosY = 0;
	cameraPosZ = 0;
	cameraRotX = 0;
	cameraRotY = 0;
	cameraRotZ = 0;
	numberOfModelsDrawn = 0;
	numberOfTrianglesRendered = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, 
	XMMATRIX* baseViewMatrix)
{
	bool result;
	int index = 0;

	// Store the screen width and height.
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	// Store the base view matrix.
	this->baseViewMatrix = XMMatrixTranspose(*baseViewMatrix);

	// Create the font object.
	font = new FontClass;
	if(!font)
	{
		return false;
	}

	// Initialize the font object.
	result = font->Initialize(device, "../Engine/data/fontdata.txt", L"../Engine/data/font.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object.
	fontShader = new FontShaderClass;
	if(!fontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = fontShader->Initialize(device, hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}


	// Initialize the first sentence.
	result = InitializeSentence(&mousePosX, 32, device);
	if(!result)
	{
		return false;
	}

	sentences.push_back(mousePosX);

	// Initialize the first sentence.
	result = InitializeSentence(&mousePosY, 32, device);
	if(!result)
	{
		return false;
	}

	sentences.push_back(mousePosY);

	// Initialize the first sentence.
	result = InitializeSentence(&numberOfModelsDrawn, 32, device);
	if(!result)
	{
		return false;
	}

	sentences.push_back(numberOfModelsDrawn);


	// Initialize the first sentence.
	result = InitializeSentence(&fpsCount, 32, device);
	if(!result)
	{
		return false;
	}


	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(fpsCount, "Fps count", 100, 20*index, 1.0f, 1.0f, 1.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(fpsCount);

	// Initialize the first sentence.
	result = InitializeSentence(&cpuLoad, 32, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cpuLoad, "Cpu load ", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(cpuLoad);

	// Initialize the first sentence.
	result = InitializeSentence(&cameraPosX, 32, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraPosX, "cameraPosX", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(cameraPosX);

	// Initialize the first sentence.
	result = InitializeSentence(&cameraPosY, 32, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraPosY, "cameraPosY", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(cameraPosY);

	// Initialize the first sentence.
	result = InitializeSentence(&cameraPosZ, 32, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraPosZ, "cameraPosZ", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(cameraPosZ);

	// Initialize the first sentence.
	result = InitializeSentence(&cameraRotX, 32, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraRotX, "cameraRotX", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(cameraRotX);

	// Initialize the first sentence.
	result = InitializeSentence(&cameraRotY, 32, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraRotY, "cameraRotY", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(cameraRotY);

	// Initialize the first sentence.
	result = InitializeSentence(&cameraRotZ, 32, device);
	if(!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraRotZ, "cameraRotZ", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	index++;
	sentences.push_back(cameraRotZ);

	// Initialize the eleventh sentence.
	result = InitializeSentence(&numberOfTrianglesRendered, 32, device);
	if(!result)
	{
		return false;
	}

	//// Now update the sentence vertex buffer with the new string information.
	//result = UpdateSentence(numberOfTrianglesRendered, "numberOfTrianglesRendered", 100, 20*index, 1.0f, 1.0f, 0.0f, deviceContext);
	//if(!result)
	//{
	//	return false;
	//}

	//sentences.push_back(numberOfTrianglesRendered);

	return true;
}

void TextClass::Shutdown()
{
	ReleaseSentences(sentences);

	// Release the font shader object.
	if(fontShader)
	{
		fontShader->Shutdown();
		delete fontShader;
		fontShader = 0;
	}

	// Release the font object.
	if(font)
	{
		font->Shutdown();
		delete font;
		font = 0;
	}

	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* orthoMatrix)
{
	bool result;

	for(std::vector<SentenceType*>::iterator sentence = sentences.begin(); sentence != sentences.end(); sentence++) 
	{
		result = RenderSentence(*sentence, deviceContext, worldMatrix, orthoMatrix);
		if(!result)
		{
			return false;
		}
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create a new sentence object.
	*sentence = new SentenceType;
	if(!*sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength; //NO! 4! BAD!

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for(i=0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
	ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if(numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((screenWidth / 2) * -1) + positionX);
	drawY = (float)((screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if(*sentence)
	{
		// Release the sentence vertex buffer.
		if((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}

	return;
}

void TextClass::ReleaseSentences(vector<SentenceType*> sentences)
{
	for(std::vector<SentenceType*>::iterator tmp = sentences.begin(); tmp != sentences.end(); tmp++) 
	{
		SentenceType* sentence = 0;
		swap(sentence, *tmp); //Added for extra safety.

		if(sentence)
		{
			// Release the sentence vertex buffer.
			if((sentence)->vertexBuffer)
			{
				(sentence)->vertexBuffer->Release();
				(sentence)->vertexBuffer = 0;
			}

			// Release the sentence index buffer.
			if((sentence)->indexBuffer)
			{
				(sentence)->indexBuffer->Release();
				(sentence)->indexBuffer = 0;
			}

			// Release the sentence.
			delete sentence;
			sentence = 0;
		}
	}

	sentences.clear();

	return;
}


bool TextClass::RenderSentence(SentenceType* sentence, ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* orthoMatrix)
{
	unsigned int stride, offset;
	XMFLOAT4 pixelColor;
	bool result;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = fontShader->Render(deviceContext, sentence->indexCount, worldMatrix, &baseViewMatrix, orthoMatrix, font->GetTexture(), pixelColor);
	if(!result)
	{
		false;
	}

	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;


	// Convert the mouseX integer to string format.
	_itoa_s(mouseX, tempString, 10);

	// Setup the mouseX string.
	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(mousePosX, mouseString, 20, 20, 1.0f, 1.0f, 1.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	// Convert the mouseY integer to string format.
	_itoa_s(mouseY, tempString, 10);

	// Setup the mouseY string.
	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(mousePosY, mouseString, 20, 40, 1.0f, 1.0f, 1.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[32];
	char fpsString[32];
	float red, green, blue;
	bool result;


	// Truncate the fps to below 10,000.
	if(fps > 9999)
	{
		fps = 9999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// If fps is 60 or above set the fps color to green.
	if(fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 60 set the fps color to yellow.
	if(fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 30 set the fps color to red.
	if(fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(fpsCount, fpsString, 10, 20, red, green, blue, deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;
	float red, green, blue;

	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	if(cpu >= 60)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	if(cpu < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if(cpu < 30)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cpuLoad, cpuString, 10, 40, red, green, blue, deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetRendercount(int count, ID3D11DeviceContext* deviceContext)
{
	char tempString[32];
	char countString[32];
	bool result;

	// Convert the cpu integer to string format.
	_itoa_s(count, tempString, 10);

	// Setup the cpu string.
	strcpy_s(countString, "Amount of spheres rendered: ");
	strcat_s(countString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(numberOfModelsDrawn, countString, 10, 40, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetCameraPosition(int X, int Y, int Z, ID3D11DeviceContext* deviceContext)
{
	char xString[32];
	char yString[32];
	char zString[32];
	char finalResultX[32];
	char finalResultY[32];
	char finalResultZ[32];
	bool result;

	// Convert the float to string format.
	_itoa_s(X, xString, 10);
	_itoa_s(Y, yString, 10);
	_itoa_s(Z, zString, 10);

	// Setup the cpu string.
	strcpy_s(finalResultX, "Camera position X: ");
	strcat_s(finalResultX, xString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraPosX, finalResultX, 10, 60, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	strcpy_s(finalResultY, "Camera position Y: ");
	strcat_s(finalResultY, yString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraPosY, finalResultY, 10, 80, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	strcpy_s(finalResultZ, "Camera position Z: ");
	strcat_s(finalResultZ, zString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraPosZ, finalResultZ, 10, 100, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}


	return true;
}

bool TextClass::SetCameraRotation(int X, int Y, int Z, ID3D11DeviceContext* deviceContext)
{
	char xString[32];
	char yString[32];
	char zString[32];
	char finalResultX[32];
	char finalResultY[32];
	char finalResultZ[32];
	bool result;

	// Convert the float to string format.
	_itoa_s(X, xString, 10);
	_itoa_s(Y, yString, 10);
	_itoa_s(Z, zString, 10);

	// Setup the cpu string.
	strcpy_s(finalResultX, "Camera rotation X: ");
	strcat_s(finalResultX, xString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraRotX, finalResultX, 10, 120, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	strcpy_s(finalResultY, "Camera rotation Y: ");
	strcat_s(finalResultY, yString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraRotY, finalResultY, 10, 140, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	strcpy_s(finalResultZ, "Camera rotation Z: ");
	strcat_s(finalResultZ, zString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(cameraRotZ, finalResultZ, 10, 160, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetRenderCount(int count, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char renderString[32];
	bool result;


	// Truncate the render count if it gets to large to prevent a buffer overflow.
	if(count > 999999999)
	{
		count = 999999999;
	}

	// Convert the cpu integer to string format.
	_itoa_s(count, tempString, 10);

	// Setup the cpu string.
	strcpy_s(renderString, "Render Count: ");
	strcat_s(renderString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(numberOfTrianglesRendered, renderString, 10, 180, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	return true;
}
