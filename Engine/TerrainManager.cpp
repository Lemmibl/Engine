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
static const XMFLOAT3 stepCount(60.0f, 60.0f, 60.0f);

TerrainManager::TerrainManager()
{
}


TerrainManager::~TerrainManager()
{
}

void TerrainManager::CreateChunk( int startPosX, int startPosZ )
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
	std::unordered_map<std::pair<int,int>, MarchingCubeChunk*>::const_iterator mappedChunk = map.find(key);

	//If they key doesn't exist, we go about creating the chunk
	if(mappedChunk == map.end())
	{
		MarchingCubeChunk newChunk
		(
			XMFLOAT3(startPosX, 0, startPosZ), 
			XMFLOAT3(startPosX + stepSize.x*stepCount.x, 0 + stepSize.y*stepCount.y, startPosZ + stepSize.z*stepCount.z), 
			stepSize, 
			stepCount
		);

		std::unordered_map<std::pair<int,int>, MarchingCubeChunk*>::const_iterator neighBourchunk = map.find(AddPairs(key, edgePairs[NORTH]));

		if(neighBourchunk != map.end())
		{
			int index1, index2;

			//These is the two different Z indexes for the two different chunks we'll be merging between
			int z1, z2;

			//In this case, we want northern chunk's southern edge values 
			//transported into this chunk's northern edge values

			//Since north is Z+1
			z1 = 60; //Our max value
			z2 = 0; //The min value for the northern chunk

			for(int x = 0; x < stepCount.x; ++x)
			{
				for(int y = 0; y < stepCount.y; ++y)
				{
					index1 = x + y*stepCount.y + z1 * stepCount.y * stepCount.z;
					index2 = x + y*stepCount.y + z2 * stepCount.y * stepCount.z;

					vector<MarchingCubeVoxel>* newChunkVoxels = newChunk.GetVoxelField();
					vector<MarchingCubeVoxel>* neighbourVoxels = neighBourchunk->second->GetVoxelField();

					newChunkVoxels[index1] = neighbourVoxels[index2];
				}
			}
		}
	}
}

//The reason I implement this function in this way is that if we would do a "if(map.find(key)) { return map[key] }"
//We'd have a potential worst case of.... well, going through the whole hash table twice because 
//first it looks for the key with find(), then it does it again through the [] operator. I assume.
bool TerrainManager::GetChunk(int x, int z, MarchingCubeChunk* inOutChunk)
{
	//Make a key out of the values
	std::pair<int,int> key(x, z);

	//We use map.find(), which returns an iterator to the value we're looking for IF it exists. 
	// If it doesn't exist, we get an iterator that points to map.end()
	//http://www.cplusplus.com/reference/unordered_map/unordered_map/end/
	std::unordered_map<std::pair<int,int>, MarchingCubeChunk*>::const_iterator mappedChunk = map.find(key);

	//If the object DOES exist at this key and it's the right object we've retrieved
	if(mappedChunk != map.end())
	{
		if(mappedChunk->first != key)
		{
			MessageBox(NULL , L"TerrainManager::GetChunk has somehow retrieved the wrong chunk from the hash table. Look into that.", L"Error", MB_OK);

			return false;
		}

		//We return the value that the key points to.
		inOutChunk = mappedChunk->second;

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
			//If this chunk is valid
			if(GetChunk(x + (edgePairs[i].first), z + (edgePairs[i].second), tempChunk))
			{
				//Add ptr to activechunk
				activeChunks.push_back(tempChunk);
			}
		}

		lastUsedKey = key;
	}

	return &activeChunks;
}