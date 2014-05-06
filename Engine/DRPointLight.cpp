////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DRPointLight.h"


DRPointLightShader::DRPointLightShader()
{
	//vertexShader = 0;
	//pixelShader = 0;
	//layout = 0;
	//samplerState = 0;

	//vertexMatrixBuffer = 0;
	//lightBuffer = 0;
	//pixelMatrixBuffer = 0;
}


DRPointLightShader::DRPointLightShader(const DRPointLightShader& other)
{
}


DRPointLightShader::~DRPointLightShader()
{
}


bool DRPointLightShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/Shaders/DRPointLight.vsh", L"../Engine/Shaders/DRPointLight.psh");
	if(!result)
	{
		return false;
	}

	return true;
}


void DRPointLightShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool DRPointLightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* world, XMMATRIX* invertedView, 
	XMMATRIX* invertedProjection, PointLight* pointLight, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialArray, XMFLOAT3 cameraPosition)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldViewProjection, worldView, world, invertedView, invertedProjection, pointLight, textureArray, materialArray, cameraPosition);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool DRPointLightShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;
	CComPtr<ID3D10Blob> pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC vertexMatrixBufferDesc;
	D3D11_BUFFER_DESC pixelMatrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	//// Initialize the pointers this function will use to null.
	//errorMessage = 0;
	//vertexShaderBuffer = 0;
	//pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&vertexShaderBuffer, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&pixelShaderBuffer, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if(FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
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

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
		&layout);
	if(FAILED(result))
	{
		return false;
	}

	//// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	//vertexShaderBuffer->Release();
	//vertexShaderBuffer = 0;

	//pixelShaderBuffer->Release();
	//pixelShaderBuffer = 0;

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
	result = device->CreateSamplerState(&samplerDesc, &samplerState.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	vertexMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexMatrixBufferDesc.ByteWidth = sizeof(VertexMatrixBufferType);
	vertexMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexMatrixBufferDesc.MiscFlags = 0;
	vertexMatrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vertexMatrixBufferDesc, NULL, &vertexMatrixBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	pixelMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelMatrixBufferDesc.ByteWidth = sizeof(PixelMatrixBufferType);
	pixelMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelMatrixBufferDesc.MiscFlags = 0;
	pixelMatrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&pixelMatrixBufferDesc, NULL, &pixelMatrixBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void DRPointLightShader::ShutdownShader()
{	
	//// Release the light constant buffer.
	//if(lightBuffer)
	//{
	//	lightBuffer->Release();
	//	lightBuffer = 0;
	//}

	//// Release the matrix constant buffer.
	//if(vertexMatrixBuffer)
	//{
	//	vertexMatrixBuffer->Release();
	//	vertexMatrixBuffer = 0;
	//}

	//// Release the matrix constant buffer.
	//if(pixelMatrixBuffer)
	//{
	//	pixelMatrixBuffer->Release();
	//	pixelMatrixBuffer = 0;
	//}

	//// Release the sampler state.
	//if(samplerState)
	//{
	//	samplerState->Release();
	//	samplerState = 0;
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

	return;
}


void DRPointLightShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


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

bool DRPointLightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* WorldViewProjection, XMMATRIX* worldView, XMMATRIX* world, XMMATRIX* invertedView, 
	XMMATRIX* invertedProjection, PointLight* pointLight, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** materialArray, XMFLOAT3 cameraPosition)
{		
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	VertexMatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	PixelMatrixBufferType* dataPtr3;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(vertexMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (VertexMatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->WorldViewProjection = *WorldViewProjection;
	dataPtr->WorldView = *worldView;
	dataPtr->World = *world;
	dataPtr->CameraPosition = cameraPosition;

	// Unlock the constant buffer.
	deviceContext->Unmap(vertexMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &vertexMatrixBuffer.p);

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->LightPosition = XMFLOAT4(pointLight->Position.x, pointLight->Position.y, pointLight->Position.z, 1.0f);
	dataPtr2->LightColor =  XMFLOAT4(pointLight->Color.x, pointLight->Color.y, pointLight->Color.z,  pointLight->Radius);
	dataPtr2->CameraPosition = XMFLOAT4(cameraPosition.x, cameraPosition.y, cameraPosition.z, pointLight->Intensity);

	// Unlock the constant buffer.
	deviceContext->Unmap(lightBuffer, 0);

	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer.p);

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(pixelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (PixelMatrixBufferType*)mappedResource.pData;

	//XMFLOAT4 worldPos = XMFLOAT4(pointLight->Position, 1.0f);
	//D3DXVec4Transform(&worldPos,  &worldPos, &world);

	// Copy matrix to buffer
	dataPtr3->InvertedView = *invertedView;
	dataPtr3->InvertedProjection = *invertedProjection;

	// Unlock the constant buffer.
	deviceContext->Unmap(pixelMatrixBuffer, 0);
	
	bufferNumber = 1;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelMatrixBuffer.p);

	// Set the array of textures that the shader needs to sample
	deviceContext->PSSetShaderResources(0, 1, materialArray);
	deviceContext->PSSetShaderResources(1, 3, textureArray);

	return true;
}

void DRPointLightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &samplerState.p);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}