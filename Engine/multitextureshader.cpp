////////////////////////////////////////////////////////////////////////////////
// Filename: multitextureshader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "multitextureshader.h"


MultiTextureShader::MultiTextureShader()
{
	vertexShader = 0;
	pixelShader = 0;
	layout = 0;
	matrixBuffer = 0;
	sampleState = 0;
	multiplierBuffer = 0;
}


MultiTextureShader::MultiTextureShader(const MultiTextureShader& other)
{
}


MultiTextureShader::~MultiTextureShader()
{
}


bool MultiTextureShader::Initialize(ID3D11Device* device, HWND hwnd, float multiplier1, float multiplier2)
{
	bool result;

	textureMultipliers.x = multiplier1;
	textureMultipliers.y = multiplier2;

	// Initialize the vertex and pixel shaders.
	//I use texture.vs because they are identical. Nothing really happens in the vertex shader.
	result = InitializeShader(device, hwnd, L"../Engine/standard.vs", L"../Engine/multitexture.ps");
	if(!result)
	{
		return false;
	}

	return true;
}

void MultiTextureShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool MultiTextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, 
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, textureMultipliers);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool MultiTextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC multiplierBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "StandardVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
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
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "MultiTexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
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

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, 
		&pixelShader);
	if(FAILED(result))
	{
		return false;
	}


	// Create the vertex input layout description.
	// This setup needs to match the VertexType structure in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
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
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

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

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	multiplierBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	multiplierBufferDesc.ByteWidth = sizeof(TextureMultiplierBufferType)*2; //Power of 16 I guess.
	multiplierBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	multiplierBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	multiplierBufferDesc.MiscFlags = 0;
	multiplierBufferDesc.StructureByteStride = 0;

	// Create the matrix constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&multiplierBufferDesc, NULL, &multiplierBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void MultiTextureShader::ShutdownShader()
{
	// Release the sampler state.
	if(sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	if(multiplierBuffer)
	{
		multiplierBuffer->Release();
		multiplierBuffer = 0;
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

	return;
}

void MultiTextureShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool MultiTextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, 
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
	ID3D11ShaderResourceView** textureArray, D3DXVECTOR2 textureMultipliers)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TextureMultiplierBufferType* dataPtr2;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

#pragma region Matrix buffer
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
#pragma endregion

#pragma region Texture multiplier buffer
	// Lock the texture multiplier constant buffer so it can be written to.
	result = deviceContext->Map(multiplierBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (TextureMultiplierBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr2->multipliers = textureMultipliers;

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(multiplierBuffer, 0);
#pragma endregion

	// Set the position of the matrix constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	bufferNumber = 0; //idk

	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &multiplierBuffer);
	deviceContext->PSSetShaderResources(0, 2, textureArray);


	return true;
}

void MultiTextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

//void CShader::SetTexture(char name[])
//{
//	IDirect3DBaseTexture9* texture = NULL;
//	D3DXHANDLE h = m_pConstantTable->GetConstantByName(NULL, name);
//
//	int tex_idx = m_pConstantTable->GetSamplerIndex(h);
//
//	m_pDevice->SetTexture(tex_idx, texture);    
//	m_pDevice->SetSamplerState(tex_idx, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//	m_pDevice->SetSamplerState(tex_idx, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
//	m_pDevice->SetSamplerState(tex_idx, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
//}
