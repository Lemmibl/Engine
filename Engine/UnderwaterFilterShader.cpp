////////////////////////////////////////////////////////////////////////////////
// Filename: UnderwaterFilterShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "UnderwaterFilterShader.h"

UnderwaterFilterShader::UnderwaterFilterShader() : SettingsDependent()
{
}


UnderwaterFilterShader::~UnderwaterFilterShader()
{
}


bool UnderwaterFilterShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Load settings from file
	InitializeSettings(this);

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/Shaders/UnderwaterFilterShader.vsh", L"../Engine/Shaders/UnderwaterFilterShader.psh");
	if(!result)
	{
		return false;
	}

	return true;
}


void UnderwaterFilterShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool UnderwaterFilterShader::Render( ID3D11DeviceContext* deviceContext, int indexCount, WaterFilterInput& input)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, input);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool UnderwaterFilterShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;
	CComPtr<ID3D10Blob> pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC vertexMatrixBufferDesc;

	// Initialize the pointers this function will use to null.
	//errorMessage = 0;
	//vertexShaderBuffer = 0;
	//pixelShaderBuffer = 0;

	UINT dwShaderFlags = 0;
	dwShaderFlags |= D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "UnderwaterFilterVertexShader", "vs_5_0", dwShaderFlags, 0, NULL, 
		&vertexShaderBuffer, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage.p, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "UnderwaterFilterPixelShader", "ps_5_0", dwShaderFlags, 0, NULL, 
		&pixelShaderBuffer, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage.p, hwnd, psFilename);
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
	result = device->CreateSamplerState(&samplerDesc, &sampler.p);
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
	vertexMatrixBufferDesc.ByteWidth = sizeof(PixelBuffer);
	vertexMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexMatrixBufferDesc.MiscFlags = 0;
	vertexMatrixBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vertexMatrixBufferDesc, NULL, &pixelBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void UnderwaterFilterShader::ShutdownShader()
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


void UnderwaterFilterShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool UnderwaterFilterShader::SetShaderParameters( ID3D11DeviceContext* deviceContext, WaterFilterInput& input)
{		
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	VertexMatrixBuffer* dataPtr1;
	PixelBuffer* dataPtr2;

	/////////////#1

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(vertexMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr1 = (VertexMatrixBuffer*)mappedResource.pData;

	dataPtr1->WorldViewProjection = *input.WorldViewProjection;
	deviceContext->Unmap(vertexMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &vertexMatrixBuffer.p);


	/////////////#2

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(pixelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (PixelBuffer*)mappedResource.pData;

	dataPtr2->FogColor = XMFLOAT4(input.fogColor->x, input.fogColor->y, input.fogColor->z, 1.0f);
	dataPtr2->cameraHeight = input.cameraHeight;
	dataPtr2->ColorMapDampeningScale = ColorMapDampeningScale;
	dataPtr2->FogColorDampeningScale = FogColorDampeningScale;
	dataPtr2->farClip = farClip;
	dataPtr2->fogEnd = fogEnd;
	dataPtr2->fogStart = fogStart;
	dataPtr2->waterDepthFogScaling = waterDepthFogScaling;
	dataPtr2->waterMinVisionRange = waterMinVisionRange;
	dataPtr2->waterMaxVisionRange = waterMaxVisionRange;
	dataPtr2->waterVisionRange = waterVisionRange;
	dataPtr2->waterLevel = waterLevel;

	deviceContext->Unmap(pixelBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelBuffer.p);

	// Assorted shit

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 2, input.textureArray);

	return true;
}

void UnderwaterFilterShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampler.p);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void UnderwaterFilterShader::OnSettingsReload(Config* cfg)
{
	//waterDepthFogScaling = 0.3;
	//waterVisionRange = 0.04;
	//waterMinVisionRange = 0.2;
	//waterMaxVisionRange = 1.0;

	//ColorMapDampeningScaleX = 0.3;
	//ColorMapDampeningScaleY = 0.3;
	//ColorMapDampeningScaleZ = 0.0;

	//FogColorDampeningScaleX = 0.15;
	//FogColorDampeningScaleY = 0.1;
	//FogColorDampeningScaleZ = 0.0;

	const Setting& settings = cfg->getRoot()["shaders"]["underwaterFilter"];

	settings.lookupValue("FogStart",				fogStart);
	settings.lookupValue("FogEnd",					fogEnd);

	settings.lookupValue("waterDepthFogScaling",	waterDepthFogScaling);
	settings.lookupValue("waterVisionRange",		waterVisionRange);
	settings.lookupValue("waterMinVisionRange",		waterMinVisionRange);
	settings.lookupValue("waterMaxVisionRange",		waterMaxVisionRange);

	settings.lookupValue("ColorMapDampeningScaleX", ColorMapDampeningScale.x);
	settings.lookupValue("ColorMapDampeningScaleY", ColorMapDampeningScale.y);
	settings.lookupValue("ColorMapDampeningScaleZ", ColorMapDampeningScale.z);

	settings.lookupValue("FogColorDampeningScaleX",	FogColorDampeningScale.x);
	settings.lookupValue("FogColorDampeningScaleY", FogColorDampeningScale.y);
	settings.lookupValue("FogColorDampeningScaleZ", FogColorDampeningScale.z);

	const Setting& settings2 = cfg->getRoot()["rendering"];
	settings2.lookupValue("farClip",				farClip);

	const Setting& waterSettings = cfg->getRoot()["shaders"]["waterShader"];
	waterSettings.lookupValue("waterLevels", waterLevel);
}