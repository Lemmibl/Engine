////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DRDirectionalLight.h"

DRDirLight::DRDirLight()
{
	vertexShader = 0;
	pixelShader = 0;
	layout = 0;
	samplers[0] = 0;
	samplers[1] = 0;

	lightBuffer = 0;
	pixelMatrixBuffer = 0; 
	vertexMatrixBuffer = 0;
	positionalBuffer = 0;
}


DRDirLight::DRDirLight(const DRDirLight& other)
{
}


DRDirLight::~DRDirLight()
{
}


bool DRDirLight::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/DRDirectionalLight.vsh", L"../Engine/DRDirectionalLight.psh");
	if(!result)
	{
		return false;
	}

	return true;
}


void DRDirLight::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool DRDirLight::Render( ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, 
	XMMATRIX* invertedViewProj, XMMATRIX* invertedView, ID3D11ShaderResourceView** textureArray, XMFLOAT3 cameraPosition, DirLight* dirLight, 
	XMFLOAT4 ambienceColor, MaterialStruct material, XMMATRIX* lightViewProj )
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, world, view, projection, invertedViewProj, invertedView, textureArray, cameraPosition, dirLight, ambienceColor, material, lightViewProj);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool DRDirLight::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC normalSamplerDesc;
	D3D11_SAMPLER_DESC depthSamplerDesc;

	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC vertexMatrixBufferDesc;
	D3D11_BUFFER_DESC pixelMatrixBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&vertexShaderBuffer, &errorMessage, NULL);
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
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION, 0, NULL, 
		&pixelShaderBuffer, &errorMessage, NULL);
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
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), 
		&layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
	normalSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	normalSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	normalSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	normalSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	normalSamplerDesc.MipLODBias = 0.0f;
	normalSamplerDesc.MaxAnisotropy = 1;
	normalSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	normalSamplerDesc.BorderColor[0] = normalSamplerDesc.BorderColor[1] = normalSamplerDesc.BorderColor[2] = normalSamplerDesc.BorderColor[3] = 0;
	normalSamplerDesc.MinLOD = 0;
	normalSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	

	// Create the texture sampler state.
	result = device->CreateSamplerState(&normalSamplerDesc, &samplers[0]);
	if(FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	depthSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	depthSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	depthSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	depthSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	depthSamplerDesc.MipLODBias = 0.0f;
	depthSamplerDesc.MaxAnisotropy = 1;
	depthSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	depthSamplerDesc.BorderColor[0] = depthSamplerDesc.BorderColor[1] = depthSamplerDesc.BorderColor[2] = depthSamplerDesc.BorderColor[3] = 0;
	depthSamplerDesc.MinLOD = 0;
	depthSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&depthSamplerDesc, &samplers[1]);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	pixelMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelMatrixBufferDesc.ByteWidth = sizeof(PixelMatrixBuffer);
	pixelMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelMatrixBufferDesc.MiscFlags = 0;
	pixelMatrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&pixelMatrixBufferDesc, NULL, &pixelMatrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the pixel shader.
	vertexMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexMatrixBufferDesc.ByteWidth = sizeof(VertexMatrixBuffer);
	vertexMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexMatrixBufferDesc.MiscFlags = 0;
	vertexMatrixBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vertexMatrixBufferDesc, NULL, &vertexMatrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(PositionalBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &positionalBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void DRDirLight::ShutdownShader()
{	
	// Release the light constant buffer.
	if(lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	// Release the camera constant buffer.
	if(vertexMatrixBuffer)
	{
		vertexMatrixBuffer->Release();
		vertexMatrixBuffer = 0;
	}

	if(positionalBuffer)
	{
		positionalBuffer->Release();
		positionalBuffer = 0;
	}

	// Release the matrix constant buffer.
	if(pixelMatrixBuffer)
	{
		pixelMatrixBuffer->Release();
		pixelMatrixBuffer = 0;
	}

	// Release the sampler state.
	if(samplers[0])
	{
		samplers[0]->Release();
		samplers[0] = 0;
	}

	// Release the sampler state.
	if(samplers[1])
	{
		samplers[1]->Release();
		samplers[1] = 0;
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


void DRDirLight::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool DRDirLight::SetShaderParameters( ID3D11DeviceContext* deviceContext, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, 
	XMMATRIX* invertedViewProj, XMMATRIX* invertedView, ID3D11ShaderResourceView** textureArray, XMFLOAT3 cameraPosition, DirLight* dirLight, XMFLOAT4 ambienceColor, MaterialStruct material, XMMATRIX* lightViewProj)
{		
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	VertexMatrixBuffer* dataPtr1;
	PositionalBuffer* dataPtr2;
	PixelMatrixBuffer* dataPtr3;
	LightBuffer* dataPtr4;

	/////////////#1

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(vertexMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr1 = (VertexMatrixBuffer*)mappedResource.pData;

	dataPtr1->World = *world;
	dataPtr1->Projection = *projection;
	dataPtr1->View = *view;
	dataPtr1->CameraPosition = cameraPosition;

	deviceContext->Unmap(vertexMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &vertexMatrixBuffer);

	/////////////#2

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(positionalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (PositionalBuffer*)mappedResource.pData;

	dataPtr2->LightDirection = dirLight->Direction;
	dataPtr2->LightPosition = dirLight->Position;
	dataPtr2->CameraPosition = cameraPosition;

	deviceContext->Unmap(positionalBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &positionalBuffer);

	/////////////#3

	result = deviceContext->Map(pixelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	dataPtr3 = (PixelMatrixBuffer*)mappedResource.pData;

	dataPtr3->InverseView = *invertedView;
	dataPtr3->InvertedViewProjection = *invertedViewProj;
	dataPtr3->LightViewProjection = *lightViewProj;

	deviceContext->Unmap(pixelMatrixBuffer, 0);

	bufferNumber++; 

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelMatrixBuffer);

	/////////////#4

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr4 = (LightBuffer*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr4->DiffuseColor = dirLight->Color;
	dataPtr4->AmbienceColor = ambienceColor;
	dataPtr4->Ka = material.Ka;
	dataPtr4->Kd = material.Kd;
	dataPtr4->Ks = material.Ks;
	dataPtr4->a = material.a;

	// Unlock the constant buffer.
	deviceContext->Unmap(lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber++;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 3, textureArray);

	return true;
}

void DRDirLight::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 2, samplers);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}