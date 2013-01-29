////////////////////////////////////////////////////////////////////////////////
// Filename: terrainclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "terrainclass.h"

int TerrainClass::GetVertexCount()
{
	return vertexCount;
}

ID3D11ShaderResourceView* TerrainClass::GetTexture()
{
	return texture->GetTexture();
}

TerrainClass::TerrainClass()
{
	heightMap = 0;
	texture = 0;
	vertices = 0;
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}


bool TerrainClass::Initialize(ID3D11Device* device, int width, int height, float multiplier, char* heightMapFilename, WCHAR* textureFilename)
{
	bool result;

	// Manually set the width and height of the terrain.
	width != 0 ? (terrainWidth = width) : (terrainWidth = 100);
	height != 0 ? (terrainHeight = height) : (terrainHeight = 100);

	multiplier != 0 ? (normalizeMultiplier = multiplier) : (normalizeMultiplier = 15);

	// Load in the height map for the terrain.
	result = LoadHeightMap(heightMapFilename);
	if(!result)
	{
		return false;
	}

	// Essentially flattening the height map by a multiplier (normalizeMultiplier).
	NormalizeHeightMap();

	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Load the texture.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	// Initialize the vertex array that will hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void TerrainClass::Shutdown()
{
	// Release the texture.
	ReleaseTexture();

	// Release the vertex array.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();

	return;
}


void TerrainClass::NormalizeHeightMap()
{
	for(int j=0; j<terrainHeight; j++)
	{
		for(int i=0; i<terrainWidth; i++)
		{
			heightMap[(terrainHeight * j) + i].y /= normalizeMultiplier;
		}
	}

	return;
}

bool TerrainClass::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(terrainHeight-1) * (terrainWidth-1)];
	if(!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for(j=0; j<(terrainHeight-1); j++)
	{
		for(i=0; i<(terrainWidth-1); i++)
		{
			index1 = (j * terrainHeight) + i;
			index2 = (j * terrainHeight) + (i+1);
			index3 = ((j+1) * terrainHeight) + i;

			// Get three vertices from the face.
			vertex1[0] = heightMap[index1].x;
			vertex1[1] = heightMap[index1].y;
			vertex1[2] = heightMap[index1].z;

			vertex2[0] = heightMap[index2].x;
			vertex2[1] = heightMap[index2].y;
			vertex2[2] = heightMap[index2].z;

			vertex3[0] = heightMap[index3].x;
			vertex3[1] = heightMap[index3].y;
			vertex3[2] = heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (terrainHeight-1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for(j=0; j<terrainHeight; j++)
	{
		for(i=0; i<terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if(((i-1) >= 0) && ((j-1) >= 0))
			{
				index = ((j-1) * (terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if((i < (terrainWidth-1)) && ((j-1) >= 0))
			{
				index = ((j-1) * (terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if(((i-1) >= 0) && (j < (terrainHeight-1)))
			{
				index = (j * (terrainHeight-1)) + (i-1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if((i < (terrainWidth-1)) && (j < (terrainHeight-1)))
			{
				index = (j * (terrainHeight-1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			heightMap[index].nx = (sum[0] / length);
			heightMap[index].ny = (sum[1] / length);
			heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete [] normals;
	normals = 0;

	return true;
}

bool TerrainClass::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	int imageSize, i, j, k, index;
	unsigned char* bitmapImage;
	unsigned char height;

	// Open the height map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if(error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	terrainWidth = bitmapInfoHeader.biWidth;
	terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	imageSize = terrainWidth * terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	heightMap = new HeightMapType[terrainWidth * terrainHeight];
	if(!heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k=0;

	// Read the image data into the height map.
	for(j=0; j<terrainHeight; j++)
	{
		for(i=0; i<terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (terrainHeight * j) + i;

			heightMap[index].x = (float)i;
			heightMap[index].y = (float)height;
			heightMap[index].z = (float)j;

			k+=3;
		}
	}


	// Release the bitmap image data.
	delete [] bitmapImage;
	bitmapImage = 0;

	return true;
}

void TerrainClass::ShutdownHeightMap()
{
	if(heightMap)
	{
		delete [] heightMap;
		heightMap = 0;
	}

	return;
}

void TerrainClass::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)terrainWidth;

	// Calculate how many times to repeat the texture.
	incrementCount = terrainWidth / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for(j=0; j<terrainHeight; j++)
	{
		for(i=0; i<terrainWidth; i++)
		{
			// Store the texture coordinate in the height map.
			heightMap[(terrainHeight * j) + i].tu = tuCoordinate;
			heightMap[(terrainHeight * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if(tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if(tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool TerrainClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	texture = new TextureClass;
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

void TerrainClass::ReleaseTexture()
{
	// Release the texture object.
	if(texture)
	{
		texture->Shutdown();
		delete texture;
		texture = 0;
	}

	return;
}

bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	int index, i, j, index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;

	// Create the vertex array.
	vertices = new VertexType[vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for(j=0; j<(terrainHeight-1); j++)
	{
		for(i=0; i<(terrainWidth-1); i++)
		{
			index1 = (terrainHeight * j) + i;          // Bottom left.
			index2 = (terrainHeight * j) + (i+1);      // Bottom right.
			index3 = (terrainHeight * (j+1)) + i;      // Upper left.
			index4 = (terrainHeight * (j+1)) + (i+1);  // Upper right.

			// Upper left.
			tv = heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if(tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(heightMap[index3].x, heightMap[index3].y, heightMap[index3].z);
			vertices[index].texture = D3DXVECTOR2(heightMap[index3].tu, tv);
			vertices[index].normal = D3DXVECTOR3(heightMap[index3].nx, heightMap[index3].ny, heightMap[index3].nz);
			index++;

			// Upper right.
			tu = heightMap[index4].tu;
			tv = heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if(tu == 0.0f) { tu = 1.0f; }
			if(tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(heightMap[index1].tu, heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
			index++;

			// Bottom left.
			vertices[index].position = D3DXVECTOR3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].texture = D3DXVECTOR2(heightMap[index1].tu, heightMap[index1].tv);
			vertices[index].normal = D3DXVECTOR3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
			index++;

			// Upper right.
			tu = heightMap[index4].tu;
			tv = heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if(tu == 0.0f) { tu = 1.0f; }
			if(tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = D3DXVECTOR3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].texture = D3DXVECTOR2(tu, tv);
			vertices[index].normal = D3DXVECTOR3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
			index++;

			// Bottom right.
			tu = heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if(tu == 0.0f) { tu = 1.0f; }

			vertices[index].position = D3DXVECTOR3(heightMap[index2].x, heightMap[index2].y, heightMap[index2].z);
			vertices[index].texture = D3DXVECTOR2(tu, heightMap[index2].tv);
			vertices[index].normal = D3DXVECTOR3(heightMap[index2].nx, heightMap[index2].ny, heightMap[index2].nz);
			index++;
		}
	}

	return true;
}

void TerrainClass::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, vertices, sizeof(VertexType) * vertexCount);
	return;
}


void TerrainClass::ShutdownBuffers()
{
	// Release the vertex array.
	if(vertices)
	{
		delete [] vertices;
		vertices = 0;
	}

	return;
}
