#include "MaterialHandler.h"


MaterialHandler::MaterialHandler()
: modelMaterials(32)
{
}


MaterialHandler::~MaterialHandler()
{
}

bool MaterialHandler::Initialize(ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext, TextureCreator* extTexCreator, NoiseClass* extNoise)
{
	device = extDevice;
	deviceContext = extDeviceContext;
	texCreator = extTexCreator;
	noise = extNoise;

	std::vector<ShaderMaterialStruct> materials;

	//The lower the smoothness the wider/more spread out the specular is.
	//Meaning the higher the smoothness is the more focused and intense the specular is.
	ShaderMaterialStruct grass;
	grass.Kambience = 0.6f;
	grass.Kdiffuse = 1.0f;
	grass.Kspecular = 0.01f;
	grass.smoothness = 0.1f;
	grass.shouldBeShadowed = 10.0f;

	ShaderMaterialStruct rock;
	rock.Kambience = 0.6f;
	rock.Kdiffuse = 0.8f;
	rock.Kspecular = 0.6f;
	rock.smoothness = 0.2f;
	rock.shouldBeShadowed = 10.0f;

	ShaderMaterialStruct snow;
	snow.Kambience = 1.0f;
	snow.Kdiffuse = 1.0f;
	snow.Kspecular = 1.0f;
	snow.smoothness = 0.2f;
	snow.shouldBeShadowed = 10.0f;

	ShaderMaterialStruct dirt;
	dirt.Kambience = 0.6f;
	dirt.Kdiffuse = 0.9f;
	dirt.Kspecular = 0.1f;
	dirt.smoothness = 2.0f;
	dirt.shouldBeShadowed = 10.0f;

	ShaderMaterialStruct grassQuads;
	grassQuads.Kambience = 0.8f;
	grassQuads.Kdiffuse = 1.2f;
	grassQuads.Kspecular = 0.0f;
	grassQuads.smoothness = 1.0f;
	grassQuads.shouldBeShadowed = 10.0f;

	ShaderMaterialStruct underGround;
	underGround.Kambience = 0.2f;
	underGround.Kdiffuse = 0.5f;
	underGround.Kspecular = 0.2f;
	underGround.smoothness = 8.0f;
	underGround.shouldBeShadowed = 10.0f;

	ShaderMaterialStruct water;
	water.Kambience = 0.6f;
	water.Kdiffuse = 1.0f;
	water.Kspecular = 2.0f;
	water.smoothness = 256.0f;
	water.shouldBeShadowed = 10.0f;

	ShaderMaterialStruct trees;
	trees.Kambience = 1.0f;
	trees.Kdiffuse = 1.2f;
	trees.Kspecular = 0.0f;
	trees.smoothness = 0.2f;
	trees.shouldBeShadowed = 10.0f;

	materials.push_back(grass);
	materials.push_back(rock);
	materials.push_back(snow);
	materials.push_back(dirt);
	materials.push_back(grassQuads);
	materials.push_back(underGround);
	materials.push_back(water);
	materials.push_back(trees);

	//If this SRV has been initialized before, release it first.
	if(materialTextureArraySRV)
	{
		materialTextureArraySRV.Release();
	}

	texCreator->Build1DMaterialTextureArray(device, deviceContext, materials, materials.size(), 5, &materialTextureArraySRV.p);

	//Brown generally considered to be 150R, 75B and 0G.
	dirtColor.RedMin = 30;
	dirtColor.RedMax = 80;
	dirtColor.GreenMin = 20;
	dirtColor.GreenMax = 40;
	dirtColor.BlueMin = 15;
	dirtColor.BlueMax = 25;

	grassColor.RedMin = 60;
	grassColor.RedMax = 130;
	grassColor.GreenMin = 110;
	grassColor.GreenMax = 180;
	grassColor.BlueMin = 20;
	grassColor.BlueMax = 35;

	//If this SRV has been initialized before, release it first.
	if(terrainMaterialTextureSRV)
	{
		terrainMaterialTextureSRV.Release();
	}

	CreateMaterialTexture(device, deviceContext, 1024, 1024, &terrainMaterialTextureSRV.p, grassColor, 0.0f, 0.0f);

	CreateMaterialLookupTable(device, deviceContext, &materialLookupTableSRV.p, 380);

	return true;
}

	void MaterialHandler::CreateMaterialLookupTable( ID3D11Device* device, ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView** textureSRV, int worldMaxYValue )
	{
		std::vector<PixelData> pixelData;
		pixelData.resize(worldMaxYValue);

		UINT8 texture1, texture2, material1, material2;

		for(size_t i = 0; i < pixelData.size(); i++)
		{
			if(i <= 5)
			{
				//Only dirt
				texture1 = 0;
				texture2 = 0;

				material1 = 3;
				material2 = 3;
			}
			else if(i <= 6)
			{
				//Dirt and grass
				texture1 = 0;
				texture2 = 1;

				material1 = 3;
				material2 = 0;
			}
			else if(i <= 44)
			{
				//Only grass
				texture1 = 1;
				texture2 = 1;

				material1 = 0;
				material2 = 0;
			}
			else if( i <= 45)
			{
				//Grass and rock
				texture1 = 1;
				texture2 = 2;

				material1 = 0;
				material2 = 1;
			}
			else if(i <= 46)
			{
				//Rock and snow
				texture1 = 2;
				texture2 = 4;

				material1 = 1;
				material2 = 2;
			}
			else if(i <= 190)
			{
				//Dirt and grass
				texture1 = 4;
				texture2 = 4;

				material1 = 2;
				material2 = 2;
			}
			else if(i <= 191)
			{
				//Dirt and grass
				texture1 = 4;
				texture2 = 1;

				material1 = 2;
				material2 = 0;
			}
			else
			{
				//Only snow
				texture1 = 1; 
				texture2 = 1;

				material1 = 0;
				material2 = 0;
			}

			pixelData[i].x = texture1;
			pixelData[i].y = texture2;
			pixelData[i].z = material1;
			pixelData[i].w = material2;
		}

		if(texCreator->Build32Bit2DTexture(device, deviceContext, pixelData, worldMaxYValue, 1, textureSRV) != S_OK)
		{
			MessageBox(GetDesktopWindow(), L"Something went wrong when trying to create material lookup table texture. Look in TextureAndMaterialHandler::CreateMaterialLookupTable.", L"Error", MB_OK);
		}
	}


	//Sorry for the vague name. This function is for randomly creating textures for different "materials"... Grass, dirt, rock. 
	//They will hopefully look pretty much like minecraft's default textures.
	void MaterialHandler::CreateMaterialTexture( ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, ID3D11ShaderResourceView** textureSRV, 
		MaterialColorSpectrumUINT8 colorSpectrum, float startPosX=0.0f, float startPosY=0.0f)
	{
		std::vector<PixelData> pixelData;
		pixelData.resize(width * height);

		int i = 0;
		float x, y;

		for(float yCounter = startPosY; yCounter < startPosY+height; yCounter++)
		{
			for(float xCounter = startPosX; xCounter < startPosX+width; xCounter++)
			{
				y = abs(yCounter);
				x = abs(xCounter);

				float firstIteration = noise->SimplexNoise3DZeroToOne(x*0.01f, y*0.01f, 25.0f);
				float secondIteration = noise->SimplexNoise3DZeroToOne(y*0.008f, x*0.008f, 50.0f);

				float multiplier = (secondIteration+firstIteration)*0.5f;

				pixelData[i].x = (UINT8)((float)multiplier * (colorSpectrum.RedMax-colorSpectrum.RedMin)+colorSpectrum.RedMin);		//
				pixelData[i].y = (UINT8)((float)multiplier * (colorSpectrum.GreenMax-colorSpectrum.GreenMin)+colorSpectrum.GreenMin);	//
				pixelData[i].z = (UINT8)((float)multiplier * (colorSpectrum.BlueMax-colorSpectrum.BlueMin)+colorSpectrum.BlueMin);		//
				pixelData[i].w = 1; //Alpha.

				i++;
			}
		}

		noise->ReseedRandom();

		if(FAILED(texCreator->Build32Bit2DTexture(device, deviceContext, pixelData, width, height, textureSRV)))
		{
			MessageBox(GetDesktopWindow(), L"Something went wrong when calling CreateMaterialTexture. Look in TextureAndMaterialHandler::CreateMaterialTexture.", L"Error", MB_OK);
		}
	}

	unsigned short MaterialHandler::AddOBJMaterial( OBJMaterialStruct mat )
	{ 
		unsigned short newHandle = 0; 
		unsigned int hash = GenerateMaterialHash(mat);

		//If there ISNT a duplicate material
		if(!CheckForDuplicateMaterials(hash, newHandle))
		{
			if(modelMaterials.AddNewObject(newHandle))
			{
				modelMaterials[newHandle] = mat;

				//So if EVERYTHING went through, add the hash and related handle so that we can check for duplicates of this material in the future
				materialHashAndKeyPairings.push_back(std::make_pair<unsigned int, unsigned short>(hash, newHandle));
			} 
		}

		return newHandle; 
	};

	bool MaterialHandler::CheckForDuplicateMaterials(unsigned int hash, unsigned short& outHandle )
	{
		for(unsigned int i = 0; i < materialHashAndKeyPairings.size(); i++)
		{
			auto& pair = materialHashAndKeyPairings[i];

			//If this is true, it means we've been trying to add a duplicate material
			if(hash == pair.first)
			{
				//So don't add a duplicate texture, instead just return the handle to the identical texture that is already loaded in
				outHandle = pair.second;

				//Break loop
				return true;
			}
		}

		return false;
	}

	//Worst hash function in the universe, because I'm a fucking retard. Should work for my needs, though.
	unsigned int MaterialHandler::GenerateMaterialHash( OBJMaterialStruct& material )
	{
		float hash = 0;

		hash += material.Ambient.x*10.0f;
		hash += material.Ambient.y*20.0f;
		hash += material.Ambient.z*30.0f;

		hash += material.Diffuse.x*140.0f;
		hash += material.Diffuse.y*250.0f;
		hash += material.Diffuse.z*360.0f;

		hash += material.Specular.x*250.0f;
		hash += material.Specular.y*290.0f;
		hash += material.Specular.z*210.0f;

		hash += material.alpha * 500.0f;
		hash += material.smoothness * 5000.0f;
		hash += (material.transparent ? 10000.0f : 100000.0f);

		return (unsigned int)hash;
	}