#pragma once
#include "TerrainManager.h"

//All the  different edge pairings potentially used for triangle extraction
static const std::pair<int, int> edgePairs[9] = 
{
	//North
	std::make_pair<int, int>(0,		1),

	//South
	std::make_pair<int, int>(0,		-1),

	//East
	std::make_pair<int, int>(1,		0),

	//West
	std::make_pair<int, int>(-1,	0),

	std::make_pair<int, int>(-1,	1),
	std::make_pair<int, int>(1,		1),
	std::make_pair<int, int>(0,		0),
	std::make_pair<int, int>(-1,	-1),
	std::make_pair<int, int>(1,		-1),
};

enum Direction
{
	NORTH = 0,
	SOUTH = 1,
	EAST = 2,
	WEST = 3
};

static const XMFLOAT3 stepSize(1.0f, 1.0f, 1.0f);
static const XMFLOAT3 stepCount(100.0f, 100.0f, 100.0f);

TerrainManager::TerrainManager(ID3D11Device* device, SimplexNoise* externalNoise, HWND hwnd)
{
	vegetationCount = 15000;

	mcTerrain.Initialize(100, 100, 100, externalNoise);
	mcTerrain.SetTerrainType(MCTerrainClass::Hills);

	assert(vegetationManager.Initialize(device, hwnd));
	noise = externalNoise;

	int startGridX, startGridZ;

	startGridX = 1;
	startGridZ = 1;

	CreateChunk(device, startGridX, startGridZ-1);
	CreateChunk(device, startGridX, startGridZ+0);
	CreateChunk(device, startGridX, startGridZ+1);

	CreateChunk(device, startGridX+1, startGridZ-1);
	CreateChunk(device, startGridX+1, startGridZ+0);
	CreateChunk(device, startGridX+1, startGridZ+1);

	CreateChunk(device, startGridX-1, startGridZ-1);
	CreateChunk(device, startGridX-1, startGridZ+0);
	CreateChunk(device, startGridX-1, startGridZ+1);

	CreateChunk(device, startGridX+2, startGridZ-1);
	CreateChunk(device, startGridX+2, startGridZ+0);
	CreateChunk(device, startGridX+2, startGridZ+1);

	CreateChunk(device, startGridX-2, startGridZ-1);
	CreateChunk(device, startGridX-2, startGridZ+0);
	CreateChunk(device, startGridX-2, startGridZ+1);

	//Fetch the active chunks
	auto chunks = GetActiveChunks(startGridX, startGridZ);

	//Temp vector to hold vegetation instances
	vector<VegetationManager::InstanceType> tempVec;

	//For all the active chunks...
	for(auto it = chunks->cbegin(); it != chunks->cend(); ++it)
	{
		tempVec.insert(tempVec.end(), (*it)->GetVegetationInstances()->begin(), (*it)->GetVegetationInstances()->end());
	}

	//Send the first vegetation instances while setting up.
	vegetationManager.SetupQuads(device, &tempVec);
}

TerrainManager::~TerrainManager()
{
}

bool TerrainManager::Update(ID3D11Device* device, XMFLOAT3 currentCameraPosition)
{
	std::pair<int,int> key((int)((currentCameraPosition.x)*0.01f + 0.2f), (int)((currentCameraPosition.z)*0.01f + 0.2f));

	if(key != lastUsedKey)
	{
		//////////////////////////////////////////////////////////////////////////
		/*
		*	Update active chunks and active renderables.
		*/
		//////////////////////////////////////////////////////////////////////////

		//Temp chunk to hold pointer from each GetChunk call.
		MarchingCubeChunk* tempChunk;
		vector<VegetationManager::InstanceType> tempVec;

		//Clear active chunks
		activeChunks.clear();
		activeRenderables.clear();

		//Add all new relevant chunks
		for(int i=0; i < 9; i++)
		{
			bool result;

			//Fetch chunk from the right grid slot
			result = GetChunk(key.first + (edgePairs[i].first), key.second + (edgePairs[i].second), &tempChunk);

			//If this chunk is valid
			if(result == true)
			{
				//Add ptr to active chunk vector
				activeChunks.push_back(tempChunk);
				activeRenderables.push_back(tempChunk->GetMesh());

				//Add this chunk's instances to the temporary vector
				tempVec.insert(tempVec.end(), tempChunk->GetVegetationInstances()->cbegin(), tempChunk->GetVegetationInstances()->cend());
			}
		}

		//Send temporary vector to be built into a vegetation instance buffer
		vegetationManager.BuildInstanceBuffer(device, &tempVec);

		return true;
	}

	return false;
}

void TerrainManager::ResetTerrain( int currentPosX, int currrentPosZ )
{
	//... Maybe just empty map and create new chunks at position
}

void TerrainManager::CreateChunk(ID3D11Device* device, int startPosX, int startPosZ)
{
	//////////////////////////////////////////////////////////////////////////
	//
	// Kay, so here's the idea: First we look inside the map if there are any chunks NORTH(index, index+1), EAST(index+1, index), SOUTH(index, index-1) or WEST(index-1, index) of them.
	// If there isn't, just make a chunk. If there is, depending on which case it is, we extract the bordervalues from the border chunk.
	// 
	// Should look something like this:

	//Make a key out of the values
	std::pair<int,int> key(startPosX, startPosZ);

	//See if the key that we're using already exists
	std::unordered_map<std::pair<int,int>, std::shared_ptr<MarchingCubeChunk>>::const_iterator mappedChunk = map.find(key);

	//If they key doesn't exist, we go about creating the chunk
	if(mappedChunk == map.end())
	{
		float actualPosX, actualPosZ;

		actualPosX = (float)(startPosX * 95);
		actualPosZ = (float)(startPosZ * 95);

		shared_ptr<MarchingCubeChunk> newChunk = make_shared<MarchingCubeChunk>
			(
			XMFLOAT3(actualPosX, 0, actualPosZ), 
			XMFLOAT3(actualPosX + stepSize.x*stepCount.x, 0 + stepSize.y*stepCount.y, actualPosZ + stepSize.z*stepCount.z), 
			stepSize, 
			stepCount
			);

		//Noise the chunk
		mcTerrain.SetCurrentVoxelField(newChunk->GetVoxelField());
		mcTerrain.Noise3D(newChunk->GetVoxelField(), 1, 1, 1, 100, 100, 100);

		//Create the mesh for the chunk with marching cubes algorithm
		marchingCubes.CalculateMesh(device, newChunk.get());

		GenerateVegetation(device, false, newChunk.get());

		//Clean up and remove the HUGE index and vertex vectors because they are no longer needed.
		newChunk->GetIndices()->clear();
		newChunk->GetVoxelField()->clear();

		//Create a new slot with key and insert the new chunk.
		map[key] = newChunk;
	}
}

void TerrainManager::MergeWithNeighbourChunks( MarchingCubeChunk* chunk, int idX, int idZ )
{
	//std::unordered_map<std::pair<int,int>, MarchingCubeChunk*>::const_iterator neighBourchunk = map.find(AddPairs(key, edgePairs[NORTH]));

	//if(neighBourchunk != map.end())
	//{
	//	int index1, index2;

	//	//These is the two different Z indexes for the two different chunks we'll be merging between
	//	int z1, z2;

	//	//In this case, we want northern chunk's southern edge values 
	//	//transported into this chunk's northern edge values

	//	//Since north is Z+1
	//	z1 = 60; //Our max value
	//	z2 = 0; //The min value for the northern chunk

	//	for(int x = 0; x < stepCount.x; ++x)
	//	{
	//		for(int y = 0; y < stepCount.y; ++y)
	//		{
	//			index1 = x + y*stepCount.y + z1 * stepCount.y * stepCount.z;
	//			index2 = x + y*stepCount.y + z2 * stepCount.y * stepCount.z;

	//			vector<MarchingCubeVoxel>* newChunkVoxels = newChunk.GetVoxelField();
	//			vector<MarchingCubeVoxel>* neighbourVoxels = neighBourchunk->second->GetVoxelField();

	//			newChunkVoxels[index1] = neighbourVoxels[index2];
	//		}
	//	}
	//}
}

//The reason I implement this function in this way is that if we would do a "if(map.find(key)) { return map[key] }"
//We'd have a potential worst case of.... well, going through the whole hash table twice because 
//first it looks for the key with find(), then it does it again through the [] operator. I assume.
bool TerrainManager::GetChunk(int x, int z, MarchingCubeChunk** outChunk)
{
	//Make a key out of the values
	std::pair<int,int> key(x, z);

	//We use map.find(), which returns an iterator to the value we're looking for IF it exists. 
	// If it doesn't exist, we get an iterator that points to map.end()
	//http://www.cplusplus.com/reference/unordered_map/unordered_map/end/
	std::unordered_map<std::pair<int,int>, std::shared_ptr<MarchingCubeChunk>>::const_iterator mappedChunk = map.find(key);

	//If the object DOES exist at this key and it's the right object we've retrieved
	if(mappedChunk != map.end())
	{
		//Temporary sanity check. Not sure if this can actually happen yet, but if it does happen, we want it to happen loudly.
		if(mappedChunk->first != key)
		{
			MessageBox(NULL , L"TerrainManager::GetChunk has somehow retrieved the wrong chunk from the hash table. Look into that.", L"Error", MB_OK);

			return false;
		}

		//We return the pointer that the iterator points to.
		*outChunk = mappedChunk->second.get();

		return true;
	}

	return false;
}

vector<MarchingCubeChunk*>* TerrainManager::GetActiveChunks( int x, int z )
{	
	//Make a key out of the values
	std::pair<int,int> key(x, z);

	//If activeChunks.size()==0, it means that this is the first time this function has been called.
	//Thus we add all 9 chunks around this point to activechunks.
	//We also check to see if this key has already been used, which means that the player is still in the same area, 
	//hence we don't need to change anything, and we just return the list of active chunks.
	if(activeChunks.size() == 0 || key != lastUsedKey)
	{
		//Temp chunk to hold pointer from each GetChunk call.
		MarchingCubeChunk* tempChunk;

		//Clear active chunks
		activeChunks.clear();

		//Add all new relevant chunks
		for(int i=0; i < 9; i++)
		{
			bool result;
			result = GetChunk(x + (edgePairs[i].first), z + (edgePairs[i].second), &tempChunk);

			//If this chunk is valid
			if(result == true)
			{
				//Add ptr to active chunk vector
				activeChunks.push_back(tempChunk);
			}
		}

		//Save the key
		lastUsedKey = key;
	}

	return &activeChunks;
}

void TerrainManager::GenerateVegetation( ID3D11Device* device, bool UpdateInstanceBuffer, MarchingCubeChunk* chunk)
{
	VegetationManager::InstanceType temp;
	float x,z,y, randValue;
	int textureID = -1;
	unsigned int stepCountX, stepCountZ;
	stepCountX = chunk->GetStepCountX();
	stepCountZ = chunk->GetStepCountZ();

	vector<VegetationManager::InstanceType>* tempVector = chunk->GetVegetationInstances();


		mcTerrain.SetCurrentVoxelField(chunk->GetVoxelField());

		for(unsigned int j = 0; j < vegetationCount; j++)
		{
			textureID = -1;

			x = (2.0f + (RandomFloat() * stepCountX-2.0f));
			z = (2.0f + (RandomFloat() * stepCountZ-2.0f));

			//Extract highest Y at this point
			y = mcTerrain.GetHighestPositionOfCoordinate((int)x, (int)z);//(*activeChunks)[i]->GetVoxelField(), 

			randValue = (RandomFloat()*360.0f);

			//No vegetation below Y:20
			if(y >= 20.0f)
			{
				if(y >= 45.0)
				{
					//But the grass should be sparse, so there is
					//high chance that we won't actually add this to the instance list.
					if(randValue > 300.0f)
					{
						textureID = 0;
					}
				}
				else
				{
					if(randValue <= 40.0f)
					{
						textureID = 2; //Some kind of leaf branch that I've turned into a plant quad.
					}
					else if(randValue <= 330.0f) //By far biggest chance that we get normal grass
					{
						textureID = 1; //Normal grass.
					}
					else if(randValue <= 350.0f) //If 97-98
					{
						textureID = 4; //Bush.
						textureID = 1;
					}
					else //If 99-100.
					{
						textureID = 3; //Flowers.
						textureID = 1;
					}
				}

				if(textureID != -1)
				{
					//Place texture ID in .w channel
					temp.position = XMFLOAT4(chunk->GetStartPosX()+x, y, chunk->GetStartPosZ()+z, (float)textureID);

					//Assign it a random value. This value is used to rotate the instance slightly, as to make all instances look differently.
					temp.randomValue = randValue;

					//Insert the instance
					tempVector->push_back(temp);
				}
			}
	}

	if(tempVector->size() == 0)
	{
		temp.position = XMFLOAT4(10.0f, 0.0f, 10.0f, (float)1.0f);
		temp.randomValue = 0.0f;

		tempVector->push_back(temp);
	}
}

vector<RenderableInterface*>* TerrainManager::GetTerrainRenderables(int x, int z)
{
	//Make a key out of the values
	std::pair<int,int> key(x, z);

	if(key != lastUsedKey || activeRenderables.size() == 0)
	{
		activeRenderables.clear();

		for(auto it = activeChunks.begin(); it != activeChunks.end(); ++it)
		{
			activeRenderables.push_back((*it)->GetMesh());
		}
	}

	return &activeRenderables;
}
