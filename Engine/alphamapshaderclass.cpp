////////////////////////////////////////////////////////////////////////////////
// Filename: alphamapshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "alphamapshaderclass.h"


AlphaMapShaderClass::AlphaMapShaderClass()
{
<<<<<<< master:Engine/alphamapshaderclass.cpp
	vertexShader = 0;
	pixelShader = 0;
	layout = 0;
	matrixBuffer = 0;
	sampleState = 0;
=======
	//vertexShader = 0;
	//pixelShader = 0;
	//layout = 0;
	//matrixBuffer = 0;
	//colorTypeBuffer = 0;
	//sampler = 0;
>>>>>>> local:Engine/MCubesGBufferShader.cpp
}


AlphaMapShaderClass::AlphaMapShaderClass(const AlphaMapShaderClass& other)
{
}


AlphaMapShaderClass::~AlphaMapShaderClass()
{
}


bool AlphaMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/standard.vsh", L"../Engine/alphamap.psh");
	if(!result)
	{
		return false;
	}

	return true;
}


void AlphaMapShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

<<<<<<< master:Engine/alphamapshaderclass.cpp

bool AlphaMapShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray)
=======
bool MCubesGBufferShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, XMMATRIX* worldViewProjection, 
	ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texAndMatLookupTable, int toggleColor, float farclip)
>>>>>>> local:Engine/MCubesGBufferShader.cpp
{
	bool result;


	// Set the shader parameters that it will use for rendering.
<<<<<<< master:Engine/alphamapshaderclass.cpp
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray);
=======
	result = SetShaderParameters(deviceContext, worldMatrix, worldViewMatrix, worldViewProjection, textureArray, texAndMatLookupTable, toggleColor, farclip);
>>>>>>> local:Engine/MCubesGBufferShader.cpp
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool AlphaMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
<<<<<<< master:Engine/alphamapshaderclass.cpp
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
=======
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;
	CComPtr<ID3D10Blob> pixelShaderBuffer;

>>>>>>> local:Engine/MCubesGBufferShader.cpp
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

<<<<<<< master:Engine/alphamapshaderclass.cpp

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "StandardVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
=======
	//// Initialize the pointers this function will use to null.
	//errorMessage = 0;
	//vertexShaderBuffer = 0;
	//pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "MCubesGBufferVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &vertexShaderBuffer, &errorMessage.p, NULL);
>>>>>>> local:Engine/MCubesGBufferShader.cpp
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
<<<<<<< master:Engine/alphamapshaderclass.cpp
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "AlphaMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
=======
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "MCubesGBufferPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &pixelShaderBuffer, &errorMessage.p, NULL);
>>>>>>> local:Engine/MCubesGBufferShader.cpp
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, 
		&vertexShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, 
		&pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), &layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	//vertexShaderBuffer->Release();
	//vertexShaderBuffer = 0;

	//pixelShaderBuffer->Release();
	//pixelShaderBuffer = 0;

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

<<<<<<< master:Engine/alphamapshaderclass.cpp
=======
	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ColorTypeBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &colorTypeBuffer);
	if(FAILED(result))
	{
		return false;
	}

>>>>>>> local:Engine/MCubesGBufferShader.cpp
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &sampleState);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void AlphaMapShaderClass::ShutdownShader()
{
<<<<<<< master:Engine/alphamapshaderclass.cpp
	// Release the sampler state.
	if(sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if(matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if(layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the pixel shader.
	if(pixelShader)
	{
		pixelShader->Release();
		pixelShader = 0;
	}

	// Release the vertex shader.
	if(vertexShader)
	{
		vertexShader->Release();
		vertexShader = 0;
	}
=======


	//// Release the sampler state.
	//if(sampler)
	//{
	//	sampler->Release();
	//	sampler = 0;
	//}

	//// Release the matrix constant buffer.
	//if(matrixBuffer)
	//{
	//	matrixBuffer->Release();
	//	matrixBuffer = 0;
	//}

	//if(colorTypeBuffer)
	//{
	//	colorTypeBuffer->Release();
	//	colorTypeBuffer = 0;
	//}

	//// Release the layout.
	//if(layout)
	//{
	//	layout->Release();
	//	layout = 0;
	//}

	//// Release the pixel shader.
	//if(pixelShader)
	//{
	//	pixelShader->Release();
	//	pixelShader = 0;
	//}

	//// Release the vertex shader.
	//if(vertexShader)
	//{
	//	vertexShader->Release();
	//	vertexShader = 0;
	//}
>>>>>>> local:Engine/MCubesGBufferShader.cpp

	return;
}



void AlphaMapShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	//// Release the error message.
	//errorMessage->Release();
	//errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

<<<<<<< master:Engine/alphamapshaderclass.cpp

bool AlphaMapShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
	ID3D11ShaderResourceView** textureArray)
=======
bool MCubesGBufferShader::SetShaderParameters( ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
	XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texAndMatLookupTable, int toggleColor, float farclip)
>>>>>>> local:Engine/MCubesGBufferShader.cpp
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the matrix constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	
	// Unlock the matrix constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the matrix constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer.p);

<<<<<<< master:Engine/alphamapshaderclass.cpp
	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 3, textureArray);
=======
	// Lock the matrix constant buffer so it can be written to.
	result = deviceContext->Map(colorTypeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (ColorTypeBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr2->colorModeInX_FarClipInY_ZWUnused = XMFLOAT4((float)toggleColor, farclip, 0.0f, 0.0f);

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(colorTypeBuffer, 0);

	// Set the position of the matrix constant buffer in the pixel sh§ader.
	bufferNumber = 0;

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &colorTypeBuffer.p);

	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, textureArray);
	deviceContext->PSSetShaderResources(1, 1, texAndMatLookupTable);
>>>>>>> local:Engine/MCubesGBufferShader.cpp

	return true;
}


void AlphaMapShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
<<<<<<< master:Engine/alphamapshaderclass.cpp
	deviceContext->PSSetSamplers(0, 1, &sampleState);
=======
	deviceContext->PSSetSamplers(0, 1, &sampler.p);
>>>>>>> local:Engine/MCubesGBufferShader.cpp

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}