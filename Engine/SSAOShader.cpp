#include "SSAOShader.h"

SSAOShader::SSAOShader() : SettingsDependent()
{
	InitializeSettings(this);
	matrixCreated = false;
}

SSAOShader::~SSAOShader()
{
}

bool SSAOShader::Initialize( ID3D11Device* device, HWND hwnd )
{
	bool result;

	//Load settings from file
	InitializeSettings(this);

	BuildSamplingRays();

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/Shaders/SSAOShader.vsh", L"../Engine/Shaders/SSAOShader.psh");
	if(!result)
	{
		return false;
	}

	return true;
}

bool SSAOShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, ShaderInputStructs::SSAOShaderInput* input )
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

bool SSAOShader::InitializeShader( ID3D11Device* device, HWND hwnd, WCHAR* vertexShaderName, WCHAR* pixelShaderName )
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBlob;
	CComPtr<ID3D10Blob> pixelShaderBlob;
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
	result = D3DX11CompileFromFile(vertexShaderName, NULL, NULL, "SSAOVertexShader", "vs_5_0", dwShaderFlags, 0, NULL, 
		&vertexShaderBlob, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vertexShaderName);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vertexShaderName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(pixelShaderName, NULL, NULL, "SSAOPixelShader", "ps_5_0", dwShaderFlags, 0, NULL, 
		&pixelShaderBlob, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, pixelShaderName);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, pixelShaderName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &vertexShader.p);
	if(FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &pixelShader.p);
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
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), 
		&layout);
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
	result = device->CreateSamplerState(&samplerDesc, &sampler.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the pixel shader.
	vertexMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexMatrixBufferDesc.ByteWidth = sizeof(VertexShaderBuffer);
	vertexMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexMatrixBufferDesc.MiscFlags = 0;
	vertexMatrixBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vertexMatrixBufferDesc, NULL, &vertexShaderBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the pixel shader.
	vertexMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexMatrixBufferDesc.ByteWidth = sizeof(PixelShaderBuffer);
	vertexMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vertexMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexMatrixBufferDesc.MiscFlags = 0;
	vertexMatrixBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vertexMatrixBufferDesc, NULL, &pixelShaderBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void SSAOShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

	//// Release the error message.
	//errorMessage->Release();
	//errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

}

bool SSAOShader::SetShaderParameters( ID3D11DeviceContext* deviceContext, ShaderInputStructs::SSAOShaderInput* input )
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	VertexShaderBuffer* dataPtr1;
	PixelShaderBuffer* dataPtr2;

	//if(!matrixCreated)
	//{
	//	XMMATRIX T = XMMatrixIdentity();
	//	T._11 = 0.5f;
	//	T._22 = -0.5f;
	//	T._41 = 0.5f;
	//	T._42 = 0.5f;


	//	XMStoreFloat4x4(&projMatrix, (*(input->projection) * T)); // 

	//	matrixCreated = true;
	//}

	/////////////#1

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(vertexShaderBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr1 = (VertexShaderBuffer*)mappedResource.pData;

	dataPtr1->worldViewProjection = *input->worldViewProjection;
	dataPtr1->thFOV = thFov;
	dataPtr1->aspectRatio = aspectRatio;

	deviceContext->Unmap(vertexShaderBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &vertexShaderBuffer.p);


	/////////////#2

	// Lock the vertex constant buffer so it can be written to.
	result = deviceContext->Map(pixelShaderBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (PixelShaderBuffer*)mappedResource.pData;

	dataPtr2->View = *input->view;
	dataPtr2->InvertedProjection = *input->invertedProjection; //XMLoadFloat4x4(&projMatrix);
	dataPtr2->thFOV = thFov;
	dataPtr2->aspectRatio = aspectRatio;

	//for(int i = 0; i < rayCount; ++i)
	//{
	//	dataPtr2->samplingRays[i] = samplingRays[i];
	//}

	deviceContext->Unmap(pixelShaderBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelShaderBuffer.p);

	///////// Assorted shit

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampler.p);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 3, input->rtTextureArray);

	return true;
}

void SSAOShader::RenderShader( ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void SSAOShader::OnSettingsReload( Config* cfg )
{
	float screenWidth, screenHeight;

	const Setting& cameraSettings = cfg->getRoot()["camera"];
	cameraSettings.lookupValue("fov", thFov);

	thFov = tan((thFov * (XM_PI/180.0f))*0.5f);

	//windowWidth, windowHeight
	const Setting& renderingSettings = cfg->getRoot()["rendering"];
	renderingSettings.lookupValue("windowWidth", screenWidth);
	renderingSettings.lookupValue("windowHeight", screenHeight);

	aspectRatio = (screenWidth / screenHeight);
}

void SSAOShader::BuildSamplingRays()
{
	samplingRays.resize(rayCount);

	/*
	To sample the normal/depth map and test for occlusion, we need to sample the hemisphere above the rendered pixel position.  
	To generate these sampling vectors, we will generate a set of vectors uniformly distributed over a sphere, and then in our shader, 
	reflect the downward facing (with respect to the pixel normal) vectors so that all of the vectors point towards the hemisphere.  
	Starting with a uniform distribution on a sphere ensures that when we randomize the offsets, 
	we’ll get a resulting random distribution that is somewhat uniform; 
	*/

	// cube corners
	samplingRays[0] = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	samplingRays[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f);
	samplingRays[2] = XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f);
	samplingRays[3] = XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f);
	samplingRays[4] = XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f);
	samplingRays[5] = XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f);
	samplingRays[6] = XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f);
	samplingRays[7] = XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f);

	// cube face centers
	samplingRays[8] =	XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
	samplingRays[9] =	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	samplingRays[10] =	XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	samplingRays[11] =	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	samplingRays[12] =	XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
	samplingRays[13] =	XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	/*
	since we are not taking a huge number of samples, we want to try to make sure that our samples are not too clumped up.  
	We’ll give these offset vectors a random length, so that we sample points at varying distances from our screen pixel.
	*/
	for(int i = 0; i < rayCount; ++i) 
	{
		float distribution = RandomFloat(0.25f, 1.0f);

		XMVECTOR tempVector = XMLoadFloat4(&samplingRays[i]);

		tempVector = XMVector4Normalize(tempVector) * distribution;

		XMStoreFloat4(&samplingRays[i], tempVector);
	}
}
