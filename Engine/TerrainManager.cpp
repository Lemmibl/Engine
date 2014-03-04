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

static const XMFLOAT3 stepSize(2.0f, 2.0f, 2.0f);

//Needs to be kept at 28, 53, 103 etc, else your position slowly gets out of sync with the culling position. Need to look into this.
static const XMFLOAT3 stepCount(28.0f, 28.0f, 28.0f);

TerrainManager::TerrainManager() 
: SettingsDependent(), marchingCubes((int)stepCount.x, (int)stepCount.y, (int)stepCount.z)
{
}

TerrainManager::~TerrainManager()
{
}


bool TerrainManager::Initialize( ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::shared_ptr<btDiscreteDynamicsWorld> collisionWorld, HWND hwnd, XMFLOAT3 cameraPosition )
{
	//Load settings from file
	InitializeSettings(this);

	timePassed = 0.0f;
	timeThreshold = 10.0f;
	rangeThreshold = 800.0f;

	map = std::make_shared<std::unordered_map<std::pair<int,int>, std::shared_ptr<MarchingCubeChunk>, int_pair_hash>>();
	collisionHandler = collisionWorld;

	noise.ReseedRandom();

	lastUsedKey = std::make_pair<int, int>(99, -99);
	lastMin = std::make_pair<int, int>(-99, -99);
	lastMax = std::make_pair<int, int>(99, 99);

	//Very powerful black magic going on here. Do not disturb.
	stepScaling = (stepSize.x*(stepCount.x-3)) / 5000;

	//I give no shits if this throws a warning, it helps me remember which terraintypes there are and their names.
	//terrainType = TerrainTypes::Plains;//(TerrainNoiseSeeder::TerrainTypes)(1 + rand()%8); //

	terrainNoiser.Initialize((int)stepCount.x, (int)stepCount.y, (int)stepCount.z, &noise, terrainType);

	vegetationManager.Initialize(device, hwnd);

	return true;
}


bool TerrainManager::Update(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT3 currentCameraPosition, float deltaTime)
{
	std::pair<int,int> key(RoundToNearest((currentCameraPosition.x)*stepScaling), RoundToNearest((currentCameraPosition.z)*stepScaling));
	std::pair<int,int> neighbourKey;

	timePassed += deltaTime;

	if(key != lastUsedKey)
	{
		//////////////////////////////////////////////////////////////////////////
		/*
		*	Update active chunks and active renderables.
		*/
		//////////////////////////////////////////////////////////////////////////

		//Temp chunk to hold pointer from each GetChunk call.
		MarchingCubeChunk* tempChunk;

		//Clear active chunks
		activeChunks.clear();
		activeRenderables.clear();

		//Add all new relevant chunks
		for(int i=0; i < 9; i++)
		{
			bool result;
			neighbourKey = AddPairs(key, edgePairs[i]);

			//Fetch chunk from the right grid slot
			result = GetChunk(neighbourKey.first, neighbourKey.second, &tempChunk);

			//If this chunk is valid
			if(result == true)
			{
				//Add ptr to active chunk std::vector
				activeChunks.push_back(tempChunk);
				activeRenderables.push_back(tempChunk->GetTerrainMesh());

				////Add this chunk's instances to the temporary std::vector
				//tempVec.insert(tempVec.end(), tempChunk->GetVegetationInstances()->cbegin(), tempChunk->GetVegetationInstances()->cend());
			}
			else
			{
				CreateChunk(device, deviceContext, neighbourKey.first, neighbourKey.second);
			}
		}

		//Send temporary std::vector to be built into a vegetation instance buffer
		//vegetationManager.BuildInstanceBuffer(device, &tempVec);

		lastUsedKey = key;

		return true;
	}

	return false;
}

void TerrainManager::ResetTerrain()
{
	for(auto it = map->begin(); it != map->end(); it++)
	{
		collisionHandler->removeRigidBody(it->second->GetRigidBody());
	}

	map->clear();
	activeRenderables.clear();
	activeChunks.clear();

	timePassed = 0.0f;
	lastUsedKey = std::make_pair<int, int>(99, -99);
	lastMin = std::make_pair<int, int>(-99, -99);
	lastMax = std::make_pair<int, int>(99, 99);

	noise.ReseedRandom();
}

void TerrainManager::CreateChunk(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int startPosX, int startPosZ)
{
	//Make a key out of the values
	std::pair<int,int> key(startPosX, startPosZ);

	//See if the key that we're using already exists
	std::unordered_map<std::pair<int,int>, std::shared_ptr<MarchingCubeChunk>>::const_iterator mappedChunk = map->find(key);

	//If they key doesn't exist, we go about creating the chunk
	if(mappedChunk == map->end())
	{
		float actualPosX, actualPosZ;

		actualPosX = (float)(startPosX * (stepSize.x*(stepCount.x-3)));
		actualPosZ = (float)(startPosZ * (stepSize.x*(stepCount.x-3)));

		std::shared_ptr<MarchingCubeChunk> newChunk = std::make_shared<MarchingCubeChunk>
		(
			XMFLOAT3(actualPosX, 0, actualPosZ), 
			XMFLOAT3(actualPosX + stepSize.x*stepCount.x, 0 + stepSize.y*stepCount.y, actualPosZ + stepSize.z*stepCount.z), 
			stepSize, 
			stepCount
		);

		//Noise the chunk
		terrainNoiser.SetCurrentVoxelField(newChunk->GetVoxelField());
		terrainNoiser.Noise3D(1, 1, 1, (int)stepCount.x, (int)stepCount.y, (int)stepCount.z);

		//Create the mesh for the chunk with marching cubes algorithm
		marchingCubes.CalculateMesh(device, newChunk.get(), newChunk->GetTriMesh());

		//Setup collision shape with the triMesh that was created inside marching cubes class
		std::shared_ptr<btBvhTriangleMeshShape> triMeshShape = std::make_shared<btBvhTriangleMeshShape>(newChunk->GetTriMesh(), true);

		//Assign it to this chunk
		newChunk->SetCollisionShape(triMeshShape);

		//Set up some rigid body stuff
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, NULL, newChunk->GetCollisionShape(), btVector3(0,0,0));
		std::shared_ptr<btRigidBody> rigidBody = std::make_shared<btRigidBody>(groundRigidBodyCI);
		rigidBody->setFriction(1);
		rigidBody->setRollingFriction(1);
		rigidBody->setRestitution(0.5f);

		//Assign it to this chunk
		newChunk->SetRigidBody(rigidBody);

		//Add it to the world
		collisionHandler->addRigidBody(newChunk->GetRigidBody());

		//...Generate and place vegetation based on data from the chunk.
		//GenerateVegetation(device, false, newChunk.get());

		//Clean up and remove the HUGE index and vertex std::vectors because they are no longer needed.
		newChunk->GetIndices()->clear();
		newChunk->GetIndices()->swap(*(newChunk->GetIndices()));

		newChunk->GetVoxelField()->clear();
		newChunk->GetVoxelField()->swap(*(newChunk->GetVoxelField()));

		//Create a new slot with key and insert the new chunk.
		map->emplace(std::make_pair<std::pair<int,int>, std::shared_ptr<MarchingCubeChunk>>(key, newChunk));
	}
}

void TerrainManager::MergeWithNeighbourChunks( MarchingCubeChunk* chunk, int idX, int idZ )
{
	//std::unordered_map<std::pair<int,int>, MarchingCubeChunk*>::const_iterator neighBourchunk = map->find(AddPairs(key, edgePairs[NORTH]));

	//if(neighBourchunk != map->end())
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

//The reason I implement this function in this way is that if we would do a "if(map->find(key)) { return map[key] }"
//We'd have a potential worst case of.... well, going through the whole hash table twice because 
//first it looks for the key with find(), then it does it again through the [] operator. I assume.
bool TerrainManager::GetChunk(int x, int z, MarchingCubeChunk** outChunk)
{
	//Make a key out of the values
	std::pair<int,int> key(x, z);

	//We use map->find(), which returns an iterator to the value we're looking for IF it exists. 
	// If it doesn't exist, we get an iterator that points to map->end()
	//http://www.cplusplus.com/reference/unordered_map/unordered_map/end/
	std::unordered_map<std::pair<int,int>, std::shared_ptr<MarchingCubeChunk>>::const_iterator mappedChunk = map->find(key);

	//If the object DOES exist at this key and it's the right object we've retrieved
	if(mappedChunk != map->end())
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

//Still keeping this function around in case we'll be placing trees/bushes in the future
void TerrainManager::GenerateVegetation( ID3D11Device* device, bool UpdateInstanceBuffer, MarchingCubeChunk* chunk)
{
	//VegetationManager::InstanceType temp;
	//float x, z, y, startPosX, startPosZ, randValue;
	//int textureID = -1;

	//startPosX = chunk->GetStartPosX();
	//startPosZ = chunk->GetStartPosZ();

	//vector<VegetationManager::InstanceType>* tempVector = chunk->GetVegetationInstances();


	//mcTerrain.SetCurrentVoxelField(chunk->GetVoxelField());

	//for(unsigned int j = 0; j < vegetationCount; j++)
	//{
	//	textureID = -1;

	//	x = (2.0f + (RandomFloat() * stepCount.x-2.0f));
	//	z = (2.0f + (RandomFloat() * stepCount.z-2.0f));

	//	//Extract highest Y at this point
	//	y = mcTerrain.GetHighestPositionOfCoordinate((int)x, (int)z);//(*activeChunks)[i]->GetVoxelField(), 

	//	randValue = (RandomFloat()*360.0f);

	//	//No vegetation below Y:20
	//	if(y >= 20.0f)
	//	{
	//		if(y >= 45.0)
	//		{
	//			//But the grass should be sparse, so there is
	//			//high chance that we won't actually add this to the instance list.
	//			if(randValue > 300.0f)
	//			{
	//				textureID = 0;
	//			}
	//		}
	//		else
	//		{
	//			if(randValue <= 10.0f)
	//			{
	//				textureID = 2; //Some kind of leaf branch that I've turned into a plant quad.
	//			}
	//			else if(randValue <= 355.0f) //By far biggest chance that we get normal grass
	//			{
	//				textureID = 1; //Normal grass.
	//			}
	//			else if(randValue <= 358.0f) //If 97-98
	//			{
	//				textureID = 4; //Bush.
	//			}
	//			else //If 99-100.
	//			{
	//				textureID = 3; //Flowers.
	//			}
	//		}

	//		if(textureID != -1)
	//		{
	//			//Place texture ID in .w channel
	//			temp.position = XMFLOAT4(startPosX+x, y, startPosZ+z, (float)textureID);

	//			//Assign it a random value. This value is used to rotate the instance slightly, as to make all instances look differently.
	//			temp.randomValue = randValue;

	//			//Insert the instance
	//			tempVector->push_back(temp);
	//		}
	//	}
	//}

	//if(tempVector->size() == 0)
	//{
	//	temp.position = XMFLOAT4(10.0f, 0.0f, 10.0f, (float)1.0f);
	//	temp.randomValue = 0.0f;

	//	tempVector->push_back(temp);
	//}
}

std::vector<RenderableInterface*>* TerrainManager::GetTerrainRenderables(int x, int z)
{
	//Make a key out of the values
	std::pair<int,int> key(x, z);

	//The usual, see if we haven't already used this key recently, or if the std::vector is empty.
	//If either of those things are true, we empty the activeRenderables std::vector and insert new, relevant renderables.
	if(key != lastUsedKey || activeRenderables.size() == 0)
	{
		activeRenderables.clear();

		for(auto it = activeChunks.begin(); it != activeChunks.end(); ++it)
		{
			activeRenderables.push_back((*it)->GetTerrainMesh());
		}
	}

	return &activeRenderables;
}

bool TerrainManager::UpdateAgainstAABB( ID3D11Device* device, ID3D11DeviceContext* deviceContext, Lemmi2DAABB* aabb, float deltaTime)
{
	std::pair<int,int> neighbourKey;

	timePassed += deltaTime;

	int startX	=	static_cast<int>(aabb->MinPoint().x);
	int startZ	=	static_cast<int>(aabb->MinPoint().y);
	int endX	=	static_cast<int>(aabb->MaxPoint().x);
	int endZ	=	static_cast<int>(aabb->MaxPoint().y);

	//Define the boundaries. We'll use the boundaries to loop through each relevant grid slot.
	// I calculate the start and ending indices by dividing the positions by a large amount and casting them to int. 
	// The amount I divide with is calculated through step size and step scaling.
	//The *2 in there is ..... because before, the chunks used to be 100x100 units large... ish. Now they're 50x50 large... ish. Hence I needed a magic number offset. Yay!
	startX =	RoundToNearest(startX*(2*stepScaling))-3;
	startZ =	RoundToNearest(startZ*(2*stepScaling))-3;
	endX =		RoundToNearest(endX*(2*stepScaling))+3;
	endZ =		RoundToNearest(endZ*(2*stepScaling))+3;

	//Instead of checking against like...... 25-30 grids we instead first check if the min and max points have changed.
	if(lastMin.first != startX || lastMin.second != startZ || lastMax.first != endX || lastMax.second != endZ)
	{
		//Every few seconds we do a cleanup to remove old chunks
		if(timePassed >= timeThreshold)
		{
			Cleanup(static_cast<float>((startX+endZ)/2), static_cast<float>((startZ+endZ)/2));

			//Reset
			timePassed = 0.0f;
		}

		//Temp chunk to hold pointer from each GetChunk call.
		MarchingCubeChunk* tempChunk;

		//Clear active chunks
		activeChunks.clear();
		activeRenderables.clear();

		/*	So in order to visualize what's actually going on in this function, this is what I do: 
		*
		*		 									endX,endZ
		*		   ____________________________________*
		*		   |X4	  |X4    |etc 	|etc   |X4	  |
		*		   |Z0	  |Z1    |	 	| 	   |Z4	  |
		*		   |______|______|______|______|______|
		*		   |X3	  |X3	 |etc	|etc   |etc   |
		*		   |Z0	  |Z1	 |	 	| 	   |	  |
		*		   |______|______|______|______|______|
		*		   |X2	  |X2    |etc   |etc   |etc   |
		*		   |Z0	  |Z1    |	    | 	   |	  |
		*		   |______|______|______|______|______|
		*		   |X1	  |X1    |etc   |etc   |etc   |
		*		   |Z0	  |Z1    |	    | 	   |	  |
		*		   |______|______|______|______|______|
		*		   |X0	  |X0    |etc 	|etc   |X0	  |
		*		   |Z0	  |Z1    |	 	| 	   |Z4	  |
		*		   |______|______|______|______|______|
		*		  *									
		*	startX,startZ								
		*	
		* Step through each grid by index, and then for each grid do the following:
		* 1) Check in the hash map if this grid already has a chunk assigned to it, in that case fetch it and put it in the active chunks std::vector
		* 2) If there isn't a chunk, create one and insert it into the hash map. Then insert it into the active chunks std::vector.
		* 
		* 
		* Yes. I did spend about 30 minutes making that little ascii... chart... box.
		*/

		//Loop through box. Start at startpositions and end at endX.
		for(int x = startX; x < endX; ++x)
		{
			for(int z = startZ; z < endZ; ++z)
			{
				bool result;
				neighbourKey.first = x;
				neighbourKey.second = z;

				//Fetch chunk from the right grid slot
				result = GetChunk(neighbourKey.first, neighbourKey.second, &tempChunk);

				//If this chunk is valid
				if(result == true)
				{
					//Add ptr to active chunk std::vector
					activeChunks.push_back(tempChunk);
					activeRenderables.push_back(tempChunk->GetTerrainMesh());
				}
				else
				{
					//... create chunk
					CreateChunk(device, deviceContext, neighbourKey.first, neighbourKey.second);

					//Fetch it
					result = GetChunk(neighbourKey.first, neighbourKey.second, &tempChunk);

					//Add ptr to active chunk std::vector
					activeChunks.push_back(tempChunk);
					activeRenderables.push_back(tempChunk->GetTerrainMesh());
				}
			}
		}

		//After we've looped through the entire area, we save the keys that were used. This means that 
		lastMin = std::make_pair<int, int>(startX, startZ);
		lastMax = std::make_pair<int, int>(endX, endZ);

		return true;
	}

	return false;
}

void TerrainManager::Cleanup(float posX, float posZ)
{
	//Iterate through entirety of map
	for(auto it = map->begin(); it != map->end();)
	{
		const XMFLOAT3& position = it->second->GetPosition();

		//If chunk is too far away from position (camera), remove it
		if(abs(position.x - posX) >= rangeThreshold || abs(position.z - posZ) >= rangeThreshold)
		{
			collisionHandler->removeRigidBody(it->second->GetRigidBody());
			it = map->erase(it);
		}
		else
		{
			//If we've erased an item we don't want to increment
			++it;
		}
	}
}

void TerrainManager::OnSettingsReload(Config* cfg)
{
	int type;

	const Setting& settings = cfg->getRoot()["terrain"];
	settings.lookupValue("startingTerrainType", type);

	terrainType = (TerrainTypes::Type)type;
}
