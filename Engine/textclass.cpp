///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight)
: fontShader(),
sentenceManager(device, screenWidth, screenHeight)
{
	// Store the screen width and height.
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	// Initialize the font shader object.
	if(!fontShader.Initialize(device, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
	}

	XMFLOAT3 nullVec = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mousePosXTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);
	mousePosYTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);

	numberOfModelsDrawnTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);
	fpsCountTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);
	cpuLoadTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);

	cameraPosXTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);
	cameraPosYTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);
	cameraPosZTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);

	cameraRotXTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);
	cameraRotYTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);
	cameraRotZTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);

	numberOfTrianglesRenderedTextID = sentenceManager.CreateSentence(device, deviceContext, 32, "", 0, 0, nullVec);

	black = XMFLOAT3(0.0f, 0.0f, 0.0f);
	white = XMFLOAT3(1.0f, 1.0f, 1.0f);

	red = XMFLOAT3(1.0f, 0.0f, 0.0f);
	green = XMFLOAT3(0.0f, 1.0f, 0.0f);
	blue = XMFLOAT3(0.0f, 0.0f, 1.0f);

	yellow = XMFLOAT3(1.0f, 1.0f, 0.0f);
	cyan = XMFLOAT3(0.0f, 1.0f, 1.0f);
	magenta = XMFLOAT3(1.0f, 0.0f, 1.0f);
}

//TODO: http://stackoverflow.com/questions/15734465/resizing-dynamic-string-array

TextClass::~TextClass()
{
}

void TextClass::Shutdown()
{
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection)
{
	//auto because I'm lazy scum
	auto tempVec = sentenceManager.GetAllSentences();
	auto fontClass = sentenceManager.GetFont();

	for(unsigned int i = 0; i < sentenceManager.GetActiveSentenceCount(); i++) 
	{
		if(!RenderSentence(&tempVec[i], fontClass, deviceContext, worldViewProjection))
		{
			return false;
		}
	}

	return true;
}

bool TextClass::RenderSentence(const SentenceManager::SentenceType* sentence, FontClass* font, ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection)
{
	unsigned int stride, offset;
	bool result;


	// Set vertex buffer stride and offset.
	stride = sizeof(SentenceManager::VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer.p, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer.p, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the text using the font shader.
	result = fontShader.Render(deviceContext, sentence->indexCount, worldViewProjection, font->GetTexture(), sentence->ColorRGBA);
	if(!result)
	{
		false;
	}

	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	std::string  finalStringX, finalStringY;
	bool result;

	// Setup the mouseX string.
	finalStringX += "Mouse X: " + std::to_string((long double)mouseX);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, mousePosXTextID, finalStringX, 5, 20, white);
	if(!result)
	{
		return false;
	}

	// Convert the mouseY integer to string format.
	finalStringY += "Mouse Y: " + std::to_string((long double)mouseY);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, mousePosYTextID, finalStringX, 5, 40, white);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	std::string  fpsString;
	bool result;


	// Truncate the fps to below 10,000.
	if(fps > 9999)
	{
		fps = 9999;
	}

	fpsString += "Fps: " + std::to_string((long double)fps);

	// If fps is 60 or above set the fps color to green.
	if(fps >= 120)
	{
		result = sentenceManager.UpdateSentence(deviceContext, fpsCountTextID, fpsString, 5, 20, green);
	}

	// If fps is below 60 set the fps color to yellow.
	if(fps < 120)
	{
		result = sentenceManager.UpdateSentence(deviceContext, fpsCountTextID, fpsString, 5, 20, yellow);
	}

	// If fps is below 30 set the fps color to red.
	if(fps < 60)
	{
		result = sentenceManager.UpdateSentence(deviceContext, fpsCountTextID, fpsString, 5, 20, red);
	}

	return result;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	std::string cpuString;
	bool result;

	cpuString += "CPU Load is at: " + std::to_string((long double)cpu) + "%";

	if(cpu >= 70)
	{
		// Update the sentence vertex buffer with the new string information.
		result = sentenceManager.UpdateSentence(deviceContext, cpuLoadTextID, cpuString, 5, 40, green);
	}
	else if(cpu < 70)
	{
		// Update the sentence vertex buffer with the new string information.
		result = sentenceManager.UpdateSentence(deviceContext, cpuLoadTextID, cpuString, 5, 40, yellow);
	}
	else if(cpu < 30)
	{
		// Update the sentence vertex buffer with the new string information.
		result = sentenceManager.UpdateSentence(deviceContext, cpuLoadTextID, cpuString, 5, 40, red);
	}

	return result;
}

bool TextClass::SetRendercount(int count, ID3D11DeviceContext* deviceContext)
{
	std::string  countString;
	bool result;

	countString += "Amount of objects rendered: " + std::to_string((long double)count);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, numberOfModelsDrawnTextID, countString, 5, 180, red);

	return true;
}

bool TextClass::SetCameraPosition(int X, int Y, int Z, ID3D11DeviceContext* deviceContext)
{
	std::string  finalResultX;
	std::string  finalResultY;
	std::string  finalResultZ;
	bool result;

	// Setup the cpu string.
	finalResultX += "Camera position X: " + std::to_string((long double)X);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, cameraPosXTextID, finalResultX, 5, 60, magenta);



	finalResultY += "Camera position Y: " + std::to_string((long double)Y);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, cameraPosYTextID, finalResultY, 5, 80, magenta);



	finalResultZ += "Camera position Z: " + std::to_string((long double)Z);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, cameraPosZTextID, finalResultZ, 5, 100, magenta);

	return result;
}

bool TextClass::SetCameraRotation(int X, int Y, int Z, ID3D11DeviceContext* deviceContext)
{
	std::string  finalX, finalY, finalZ;
	bool result;

	finalX += "Camera rotation X: " + std::to_string((long double)X);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, cameraRotXTextID, finalX, 5, 120, cyan);

	
	
	finalY += "Camera rotation Y: " + std::to_string((long double)Y);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, cameraRotYTextID, finalY, 5, 140, cyan);

	
	
	finalZ += "Camera rotation Z: " + std::to_string((long double)Z);

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, cameraRotZTextID, finalZ, 5, 160, cyan);
	if(!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetLastChar(char text, ID3D11DeviceContext* deviceContext)
{
	std::string  stringThing;
	bool result;

	stringThing += "Last key pressed: " + text;

	// Update the sentence vertex buffer with the new string information.
	result = sentenceManager.UpdateSentence(deviceContext, lastCharPressedTextID, stringThing, 5, 320, yellow);
	if(!result)
	{
		return false;
	}

	return true;
}
