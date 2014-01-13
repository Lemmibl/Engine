///////////////////////////////////////////////////////////////////////////////
// Filename: fontclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "fontclass.h"


FontClass::FontClass()
{
	//font = 0;
	//texture = 0;
}


FontClass::FontClass(const FontClass& other)
{
}


FontClass::~FontClass()
{
}


bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result;


	// Load in the text file containing the font data.
	result = LoadFontData(fontFilename);
	if(!result)
	{
		return false;
	}

	// Load the texture that has the font characters on it.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::Shutdown()
{
	// Release the font texture.
	ReleaseTexture();

	// Release the font data.
	ReleaseFontData();

	return;
}


bool FontClass::LoadFontData(char* filename)
{
	std::ifstream fin;
	int i;
	char temp;

	// Create the font spacing buffer.
	font = std::unique_ptr<FontType []>(new FontType[95]);
	if(!font)
	{
		return false;
	}

	// Read in the font size and spacing between chars.
	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	// Read in the 95 used ascii characters for text.
	for(i=0; i<95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> font[i].left;
		fin >> font[i].right;
		fin >> font[i].size;
	}

	// Close the file.
	fin.close();

	return true;
}


void FontClass::ReleaseFontData()
{
	//// Release the font data array.
	//if(font)
	//{
	//	delete [] font;
	//	font = 0;
	//}

	return;
}


bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	// Create the texture object.
	texture = std::unique_ptr<TextureClass>(new TextureClass());
	if(!texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::ReleaseTexture()
{
	//// Release the texture object.
	//if(texture)
	//{
	//	texture->Shutdown();
	//	delete texture;
	//	texture = 0;
	//}

	return;
}


ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return texture->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// Coerce the input vertices into a VertexType structure.
	vertexPtr = (VertexType*)vertices;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(sentence);

	// Initialize the index to the vertex array.
	index = 0;

	// Draw each letter onto a quad.
	for(i=0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if(letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// First triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = XMFLOAT2(font[letter].left, 1.0f);
			index++;

			// Second triangle in quad.
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = XMFLOAT2(font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + font[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = XMFLOAT2(font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + font[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = XMFLOAT2(font[letter].right, 1.0f);
			index++;

			// Update the x location for drawing by the size of the letter and one pixel.
			drawX = drawX + font[letter].size + 1.0f;
		}
	}

	return;
}