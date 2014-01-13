////////////////////////////////////////////////////////////////////////////////
// Filename: drcompose.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DRCompose.h"
#include <stdio.h>
#include <d3dcompiler.h>


DRCompose::DRCompose()
{
	//vertexShader = 0;
	//pixelShader = 0;
	//layout = 0;
	//samplers[0] = 0;
	//samplers[1] = 0;

	//vertexMatrixBuffer = 0;
	//pixelMatrixBuffer = 0;

	//Assigning some default values
	variables.sampleRadius = 0.6f; //Controls sampling radius. 0.1f to 1.0f are pretty ok values.
	variables.intensity = 4.0f; //AO intensity. The higher this value is, the darker the occluded parts will be. 1.0f to 10.0f values is pretty ok values.
	variables.scale = 0.8f; //Scales distance between occluders and occludee. Still a little unsure as to what values would be good to use.
	variables.bias = 0.2f; //Cutoff value. The higher this value is, the harsher we are with cutting off low AO values. 0.01f to 0.4f values are pretty ok.
	variables.fogStart = 150.0f;
	variables.fogEnd = 380.0f;

	variables.screenSize = XMFLOAT2(1024.0f, 768.0f);
	variables.randomSize = XMFLOAT2(64.0f, 64.0f);
}


DRCompose::DRCompose(const DRCompose& other)
{
}


DRCompose::~DRCompose()
{
}


bool DRCompose::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Get settings manager instance and add our function to reload event
	SettingsManager& settings = SettingsManager::GetInstance();
	settings.GetEvent()->Add(*this, (&DRCompose::OnSettingsReload));

	//Perhaps slightly hacky, but it saves on rewriting code.
	OnSettingsReload(&settings.GetConfig());

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/Shaders/DRCompose.vsh", L"../Engine/Shaders/DRCompose.hlsl");
	if(!result)
	{
		return false;
	}

	return true;
}


void DRCompose::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool DRCompose::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invViewProjection,
	XMFLOAT4* fogColor, float fogMinimum, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* randomTexture, int toggle)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldViewProjection, worldView, view, invertedProjection, invViewProjection, fogColor, fogMinimum, 
		textureArray, randomTexture, toggle);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool DRCompose::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;
	CComPtr<ID3D10Blob> pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC vertexMatrixBufferDesc;

	//// Initialize the pointers this function will use to null.
	//errorMessage = 0;
	//vertexShaderBuffer = 0;
	//pixelShaderBuffer = 0;

	UINT dwShaderFlags = 0;
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ComposeVertexShader", "vs_5_0", dwShaderFlags, 0, NULL, 
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
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ComposePixelShader", "ps_5_0", dwShaderFlags, 0, NULL, 
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
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &samplers[0].p);
	if(FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &samplers[1].p);
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
	result = device->CreateBuffer(&vertexMatrixBufferDesc, NULL, &vertexMatrixBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the pixel shader.
	vertexMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexMatrixBufferDesc.ByteWidth = sizeof(PixelMatrixBuffer);
	vertexMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexMatrixBufferDesc.MiscFlags = 0;
	vertexMatrixBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vertexMatrixBufferDesc, NULL, &pixelMatrixBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the pixel shader.
	vertexMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexMatrixBufferDesc.ByteWidth = sizeof(VariableBuffer);
	vertexMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexMatrixBufferDesc.MiscFlags = 0;
	vertexMatrixBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &variables;
	data.SysMemPitch = sizeof(VariableBuffer);
	data.SysMemSlicePitch = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vertexMatrixBufferDesc, &data, &variableBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void DRCompose::ShutdownShader()
{	

	//// Release the camera constant buffer.
	//if(vertexMatrixBuffer)
	//{
	//	vertexMatrixBuffer->Release();
	//	vertexMatrixBuffer = 0;
	//}

	//if(pixelMatrixBuffer)
	//{
	//	pixelMatrixBuffer->Release();
	//	pixelMatrixBuffer = 0;
	//}

	//// Release the sampler state.
	//if(samplers[0])
	//{
	//	samplers[0]->Release();
	//	samplers[0] = 0;
	//}

	//if(samplers[1])
	//{
	//	samplers[1]->Release();
	//	samplers[1] = 0;
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


void DRCompose::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool DRCompose::SetShaderParameters( ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invViewProjection,
	XMFLOAT4* fogColor, float fogMinimum, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView* randomTexture, int toggle)
{		
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	VertexMatrixBuffer* dataPtr1;
	PixelMatrixBuffer* dataPtr2;
	VariableBuffer* dataPtr3;

	/////////////#1

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(vertexMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr1 = (VertexMatrixBuffer*)mappedResource.pData;

	dataPtr1->WorldViewProjection = *worldViewProjection;
	dataPtr1->WorldView = *worldView;

	deviceContext->Unmap(vertexMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &vertexMatrixBuffer.p);


	/////////////#2

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(pixelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (PixelMatrixBuffer*)mappedResource.pData;

	dataPtr2->View = *view;
	dataPtr2->InvertedProjection = *invertedProjection;
	dataPtr2->FogColor = XMFLOAT4(fogColor->x, fogColor->y, fogColor->z, fogMinimum);

	deviceContext->Unmap(pixelMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelMatrixBuffer.p);

	if(variables.toggleSSAO != toggle)
	{
		bufferNeedsUpdate = true;
	}

	/////////////#3
	if(bufferNeedsUpdate)
	{
		variables.toggleSSAO = toggle;

		// Lock the vertex constant buffer so it can be written to.
		result = deviceContext->Map(variableBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		dataPtr3 = (VariableBuffer*)mappedResource.pData;

		dataPtr3->bias			=	variables.bias;
		dataPtr3->farClip		=	variables.farClip;
		dataPtr3->fogEnd		=	variables.fogEnd;
		dataPtr3->fogStart		=	variables.fogStart;
		dataPtr3->intensity		=	variables.intensity;
		dataPtr3->sampleRadius	=	variables.sampleRadius;
		dataPtr3->scale			=	variables.scale;
		dataPtr3->toggleSSAO	=	variables.toggleSSAO;
		dataPtr3->randomSize	=	variables.randomSize;
		dataPtr3->screenSize	=	variables.screenSize;

		deviceContext->Unmap(variableBuffer, 0);

		bufferNeedsUpdate = false;
	}

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 1;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &variableBuffer.p);

	///////// Assorted shit

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &randomTexture);
	deviceContext->PSSetShaderResources(1, 4, textureArray);

	return true;
}

void DRCompose::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &samplers[0].p);
	deviceContext->PSSetSamplers(1, 1, &samplers[1].p);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void DRCompose::OnSettingsReload(Config* cfg)
{
	bufferNeedsUpdate = true;

	variables.toggleSSAO = 0;

	const Setting& settings = cfg->getRoot()["shaders"]["composeShader"];

	settings.lookupValue("sampleRadius", variables.sampleRadius);
	settings.lookupValue("intensity", variables.intensity);
	settings.lookupValue("scale", variables.scale);
	settings.lookupValue("bias", variables.bias);

	settings.lookupValue("FogStart", variables.fogStart);
	settings.lookupValue("FogEnd", variables.fogEnd);

	const Setting& settings2 = cfg->getRoot()["rendering"];

	settings2.lookupValue("farClip", variables.farClip);
	settings2.lookupValue("windowWidth", variables.screenSize.x);
	settings2.lookupValue("windowHeight", variables.screenSize.y);

	const Setting& settings3 = cfg->getRoot()["textures"];

	settings3.lookupValue("ssaoNoiseTextureWidth", variables.randomSize.x);
	settings3.lookupValue("ssaoNoiseTextureHeight", variables.randomSize.y);
}