#include "GeometryShaderGrass.h"

GeometryShaderGrass::GeometryShaderGrass()
{
	//vertexShader = 0;
	//pixelShader = 0;
	//layout = 0;
	//matrixBuffer = 0;
	//colorTypeBuffer = 0;
	//sampler = 0;
}


GeometryShaderGrass::GeometryShaderGrass(const GeometryShaderGrass& other)
{
}


GeometryShaderGrass::~GeometryShaderGrass()
{
}

bool GeometryShaderGrass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/Shaders/GeometryShaderGrass.vsh", L"../Engine/Shaders/GeometryShaderGrass.gsh", L"../Engine/Shaders/GeometryShaderGrass.psh");
	if(!result)
	{
		return false;
	}

	return true;
}

void GeometryShaderGrass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool GeometryShaderGrass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, XMMATRIX* worldViewProjection, 
	ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texAndMatLookupTable, ID3D11ShaderResourceView** windForceTexture, int toggleColor, float farclip, float deltaTime)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, worldViewMatrix, worldViewProjection, textureArray, texAndMatLookupTable, windForceTexture, toggleColor, farclip, deltaTime);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool GeometryShaderGrass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename)
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;
	CComPtr<ID3D10Blob> geometryShaderBuffer;
	CComPtr<ID3D10Blob> pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "GrassVS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &vertexShaderBuffer, &errorMessage.p, NULL);
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
	result = D3DX11CompileFromFile(gsFilename, NULL, NULL, "GrassGS", "gs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &geometryShaderBuffer, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, gsFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, gsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "GrassPS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
		0, NULL, &pixelShaderBuffer, &errorMessage.p, NULL);
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

	// semantic name, semantic index, start component, component count, output slot
	//D3D11_SO_DECLARATION_ENTRY pDecl[3] =
	//{
	//	{ 0, "SV_POSITION", 0, 0, 4, 0 },   // output all components of position
	//	{ 0, "NORMAL", 0, 0, 3, 0 },     // output the first 3 of the normal
	//	{ 0, "TEXCOORD", 0, 0, 2, 0 }    // output the first 2 texture coordinates
	//};

	//result = device->CreateGeometryShaderWithStreamOutput(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), pDecl, 
	//	sizeof(pDecl), NULL, 0, 0, NULL, &geometryShader.p);
	//if(FAILED(result))
	//{
	//	return false;
	//}

	result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, 
		&geometryShader);
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
	// This setup needs to match the VertexType structure in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(GeometryShaderBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &geometryShaderMatrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ColorTypeBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, NULL, &colorTypeBuffer);
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
	result = device->CreateSamplerState(&samplerDesc, &sampler);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void GeometryShaderGrass::ShutdownShader()
{
	return;
}

void GeometryShaderGrass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool GeometryShaderGrass::SetShaderParameters( ID3D11DeviceContext* deviceContext, XMMATRIX* worldMatrix, XMMATRIX* worldViewMatrix, 
	XMMATRIX* worldViewProjection, ID3D11ShaderResourceView** textureArray, ID3D11ShaderResourceView** texAndMatLookupTable, ID3D11ShaderResourceView** windForceTexture, 
	int toggleColor, float farclip, float deltaTime)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	ColorTypeBuffer* dataPtr2;
	GeometryShaderBuffer* dataPtr3;
	unsigned int bufferNumber;

	//////////////////////////////////////////////////////////////////////////
	//#1

	// Lock the matrix constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->World = *worldMatrix;
	dataPtr->WorldView = *worldViewMatrix;
	dataPtr->WorldViewProjection = *worldViewProjection;

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the matrix constant buffer in the vertex sh§ader.
	bufferNumber = 0;

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer.p);



	//////////////////////////////////////////////////////////////////////////
	//#2
	
	// Lock the matrix constant buffer so it can be written to.
	result = deviceContext->Map(geometryShaderMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (GeometryShaderBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr3->World = *worldMatrix;
	dataPtr3->WorldViewProjection = *worldViewProjection;
	dataPtr3->DeltaTime = deltaTime;

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(geometryShaderMatrixBuffer.p, 0);

	// Set the position of the matrix constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->GSSetConstantBuffers(bufferNumber, 1, &geometryShaderMatrixBuffer.p);



	//////////////////////////////////////////////////////////////////////////
	//#3

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

	//////////////////////////////////////////////////////////////////////////
	// Assorted shit

	// Set shader texture resource for geometry shader
	deviceContext->GSSetShaderResources(0, 1, windForceTexture);

	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, textureArray);
	//deviceContext->PSSetShaderResources(1, 1, texAndMatLookupTable);

	return true;
}

void GeometryShaderGrass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->GSSetShader(geometryShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampler.p);
	deviceContext->GSSetSamplers(0, 1, &sampler.p);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	//Set GS shader back to null after we've drawn. Else it'll do work on everything else we draw. :)
	deviceContext->GSSetShader(NULL, NULL, 0);

	return;
}