////////////////////////////////////////////////////////////////////////////////
// Filename: DepthOnlyShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DepthOnlyShader.h"

DepthOnlyShader::DepthOnlyShader() : SettingsDependent()
{
}


DepthOnlyShader::~DepthOnlyShader()
{
}


bool DepthOnlyShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	InitializeSettings(this);

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, L"../Engine/Shaders/DepthOnlyShader.vsh", L"../Engine/Shaders/DepthOnlyShader.psh");
	if(!result)
	{
		return false;
	}

	return true;
}


void DepthOnlyShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool DepthOnlyShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX* worldViewProjection, XMMATRIX* worldView)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldViewProjection, worldView);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool DepthOnlyShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	CComPtr<ID3D10Blob> errorMessage;
	CComPtr<ID3D10Blob> vertexShaderBuffer;
	CComPtr<ID3D10Blob> pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "DepthOnlyVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
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

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "DepthOnlyPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
		&pixelShaderBuffer, &errorMessage.p, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have written something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
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

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
		vertexShaderBuffer->GetBufferSize(), &layout);
	if(FAILED(result))
	{
		return false;
	}

	//// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	//vertexShaderBuffer->Release();
	//vertexShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(FarClipBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &farClipBuffer.p);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void DepthOnlyShader::ShutdownShader()
{
	//// Release the matrix constant buffer.
	//if(matrixBuffer)
	//{
	//	matrixBuffer->Release();
	//	matrixBuffer = 0;
	//}

	//// Release the layout.
	//if(layout)
	//{
	//	layout->Release();
	//	layout = 0;
	//}

	//// Release the vertex shader.
	//if(vertexShader)
	//{
	//	vertexShader->Release();
	//	vertexShader = 0;
	//}

	//if(pixelShader)
	//{
	//	pixelShader->Release();
	//	pixelShader = 0;
	//}

	return;
}


void DepthOnlyShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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


bool DepthOnlyShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, XMMATRIX* worldView)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	FarClipBuffer* dataPtr2;
	unsigned int bufferNumber;

	////////////////////////////////////////////////////////////////////////// #1

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->worldViewProjection = *worldViewProjection;
	dataPtr->worldView = *worldView;

	// Unlock the constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer.p);

	////////////////////////////////////////////////////////////////////////// #2

	if(bufferNeedsUpdate)
	{
		// Lock the constant buffer so it can be written to.
		result = deviceContext->Map(farClipBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if(FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		dataPtr2 = (FarClipBuffer*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr2->farClip = farClip;

		// Unlock the constant buffer.
		deviceContext->Unmap(farClipBuffer, 0);

		bufferNeedsUpdate = false;
	}

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &farClipBuffer.p);

	return true;
}


void DepthOnlyShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
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

void DepthOnlyShader::OnSettingsReload( Config* cfg )
{
	bufferNeedsUpdate = true;

	const Setting& settings = cfg->getRoot()["rendering"];

	settings.lookupValue("farClip", farClip);
}
