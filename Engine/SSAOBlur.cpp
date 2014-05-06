#include "SSAOBlur.h"


SSAOBlur::SSAOBlur() : SettingsDependent()
{
}

SSAOBlur::~SSAOBlur()
{
}


bool SSAOBlur::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	InitializeSettings(this);

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/Shaders/standard.vsh", L"../Engine/Shaders/SSAOBlur.psh");
	if(!result)
	{
		return false;
	}

	return true;
}


void SSAOBlur::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool SSAOBlur::RenderBlurX(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection,
	ID3D11ShaderResourceView** texture)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldViewProjection, texture, true);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool SSAOBlur::RenderBlurY(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection,
	ID3D11ShaderResourceView** texture)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldViewProjection, texture, false);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool SSAOBlur::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;
	CComPtr<ID3D10Blob> pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;

	D3D11_BUFFER_DESC vertexMatrixBufferDesc;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "StandardVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
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
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "SSAOBlurPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
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

	//pixelShaderBufferX->Release();
	//pixelShaderBufferX = 0;

	//pixelShaderBufferY->Release();
	//pixelShaderBufferY = 0;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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

	// Setup the description of the matrix constant buffer that is in the vertex shader.
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

	// Setup the description of the constant buffer that is in the pixel shader.
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

void SSAOBlur::ShutdownShader()
{	

	//// Release the camera constant buffer.
	//if(vertexMatrixBuffer)
	//{
	//	vertexMatrixBuffer->Release();
	//	vertexMatrixBuffer = 0;
	//}

	//// Release the sampler state.
	//if(sampler)
	//{
	//	sampler->Release();
	//	sampler = 0;
	//}

	//// Release the layout.
	//if(layout)
	//{
	//	layout->Release();
	//	layout = 0;
	//}

	//// Release the X pixel shader.
	//if(pixelShaderX)
	//{
	//	pixelShaderX->Release();
	//	pixelShaderX = 0;
	//}

	//// Release the Y pixel shader.
	//if(pixelShaderY)
	//{
	//	pixelShaderY->Release();
	//	pixelShaderY = 0;
	//}

	//// Release the vertex shader.
	//if(vertexShader)
	//{
	//	vertexShader->Release();
	//	vertexShader = 0;
	//}

	return;
}


void SSAOBlur::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool SSAOBlur::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** texture, bool blurHorizontal)
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

	dataPtr1->WorldViewProjection = *worldViewProjection;

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

	dataPtr2->horizontalBlur = (blurHorizontal == true) ? 1 : -1;
	dataPtr2->acceptableDepthDifference = acceptableDepthDifference;
	dataPtr2->acceptableNormalDifference = acceptableNormalDifference;
	dataPtr2->averagingOffset = averagingOffset;

	deviceContext->Unmap(pixelBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelBuffer.p);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 3, texture);

	return true;
}

void SSAOBlur::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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

void SSAOBlur::OnSettingsReload( Config* cfg )
{
	const Setting& settings = cfg->getRoot()["shaders"]["ssaoBlur"];

	settings.lookupValue("acceptableNormalDifference", acceptableNormalDifference);
	settings.lookupValue("acceptableDepthDifference", acceptableDepthDifference);
	settings.lookupValue("averagingOffset", averagingOffset);
}
