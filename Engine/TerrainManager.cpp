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
static const XMFLOAT3 stepCount(50.0f, 50.0f, 50.0f);

volatile static bool IsRunning = true;

//Entry point for each thread
void JobThreadEntryPoint(void* terrainManagerPointer)
{
	TerrainManager* terrainManager = (TerrainManager*)terrainManagerPointer;

	std::pair<int,int> chunkKey;

	while(IsRunning)
	{
		//If the work queue is > 0, pop next chunk
		//Create the meshes for it and add them to relevant systems
		//Flag it as ready

		if(terrainManager->GetPreProductionQueue()->Try_pop(chunkKey))
		{
			float actualPosX, actualPosZ;

			//Calculate start position
			actualPosX = ((float)chunkKey.first * (stepSize.x*(stepCount.x - 3)));
			actualPosZ = ((float)chunkKey.second * (stepSize.x*(stepCount.x - 3)));

			//Create shared ptr to chunk
			std::shared_ptr<MarchingCubeChunk> chunk = std::make_shared<MarchingCubeChunk>
				(
				XMFLOAT3(actualPosX, 0, actualPosZ), 
				XMFLOAT3(actualPosX + stepSize.x*stepCount.x, 0 + stepSize.y*stepCount.y, actualPosZ + stepSize.z*stepCount.z), 
				stepSize, 
				stepCount
				);

			chunk->SetKey(chunkKey.first, chunkKey.second);

			std::vector<MarchingCubeVoxel> voxels;
			voxels.resize(((unsigned int)stepCount.x+1) * ((unsigned int)stepCount.y+1) * ((unsigned int)stepCount.z+1));

			// Set default values for each voxel in the field
			for(unsigned int z = 0; z <= stepCount.z+1; ++z)
			{
				for(unsigned int y = 0; y <= stepCount.y+1; ++y)
				{
					for(unsigned int x = 0; x <= stepCount.x+1; ++x)
					{
						//Calculate index for this voxel
						unsigned int index = x + (y*(unsigned int)stepCount.y) + (z * (unsigned int)stepCount.y * (unsigned int)stepCount.z);

						//Set default values for this voxel
						voxels[index].position.x = actualPosX	+ stepSize.x * x;
						voxels[index].position.y = 0.0f			+ stepSize.y * y;
						voxels[index].position.z = actualPosZ	+ stepSize.z * z;
						voxels[index].density = 0.0f;
						voxels[index].inside = false;
						voxels[index].normal.x = 0.5f;
						voxels[index].normal.y = 0.5f;
						voxels[index].normal.z = 0.5f;
					}
				}
			}

			//Noise the voxel field
			terrainManager->GetTerrainNoiser().Noise3D(1, 1, 1, (int)stepCount.x, (int)stepCount.y, (int)stepCount.z, &voxels);

			//Create the mesh for the chunk with marching cubes algorithm
			terrainManager->GetMarchingCubesClass().CalculateMesh(terrainManager->GetDevice(), chunk, &voxels);

			if(terrainManager->DXMultiThreading())
			{
				terrainManager->CreateWaterMesh(terrainManager->GetDevice(), chunk);
				terrainManager->CreateMesh(terrainManager->GetDevice(), chunk);
			}

			//Setup collision shape with the triMesh that was created inside marching cubes class
			std::shared_ptr<btBvhTriangleMeshShape> triMeshShape = std::make_shared<btBvhTriangleMeshShape>(chunk->GetTriMesh(), true);

			//Assign it to this chunk
			chunk->SetCollisionShape(triMeshShape);

			//Set up some rigid body stuff
			btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, NULL, chunk->GetCollisionShape().get(), btVector3(0,0,0));
			std::shared_ptr<btRigidBody> rigidBody = std::make_shared<btRigidBody>(groundRigidBodyCI);
			rigidBody->setFriction(1);
			rigidBody->setRollingFriction(1);
			rigidBody->setRestitution(0.5f);

			//Assign it to this chunk
			chunk->SetRigidBody(rigidBody);

			float randPosX, randPosZ;
			int indexX, indexZ;

			float resultHeight = 0.0f;

			unsigned int tries = 0;

			//Count of vegetation objects to be rendered on this chunk
			unsigned int vegetationCount = 1 + rand()%4;

			std::vector<XMFLOAT4X4> tempTransforms;

			for(unsigned int i = 0; i < vegetationCount;)
			{
				indexX = rand()%(int)stepCount.x;
				indexZ = rand()%(int)stepCount.z;

				randPosX = actualPosX + (indexX*stepSize.x);
				randPosZ = actualPosZ + (indexZ*stepSize.z);

				//Get the height position of this coordinate
				if(terrainManager->GetTerrainNoiser().GetHighestPositionOfCoordinate(indexX, indexZ, chunk.get(), &voxels, &resultHeight))
				{
					//We scale the result height with stepping size (since the placement algorithm just returns the Y index of the voxel that we'll be placing the object on)
					resultHeight *= stepSize.y;

					//Scale for the object with some randomization spice
					float randScale = 3.0f + (float)(rand()%2);

					XMFLOAT4X4 tempTransform;

					//If we've found a proper position, create a scaled, rotated and translated world matrix to this point
					XMStoreFloat4x4(	
					&tempTransform, 
					
										(		
											XMMatrixScaling(randScale, randScale, randScale) * 
											XMMatrixRotationY((float)(rand()%360)) * 
											XMMatrixTranslation(randPosX, resultHeight, randPosZ)
										)
					);

					//And insert it into our chunk
					tempTransforms.push_back(tempTransform);

					//If we found a proper position, increment loop
					++i;
				}
				else
				{
					//If we didn't find a proper position, instead increment a counter
					++tries;

					//If we've tried to find a position 3 times and still haven't found one...
					if(tries >= 4)
					{
						//Increment and skip
						++i;

						//And reset tries variable for next time
						tries = 0;
					}
				}
			}

			*chunk->GetVegetationTransforms() = tempTransforms;

			terrainManager->GetPostProductionQueue()->Push(chunk);
		}
	}
}

TerrainManager::TerrainManager() 
	: SettingsDependent(), marchingCubes((int)stepCount.x, (int)stepCount.y, (int)stepCount.z), fullyLoaded(false)
{
}

TerrainManager::~TerrainManager()
{
	Shutdown();
}


bool TerrainManager::Initialize(ID3D11Device* device, std::shared_ptr<btDiscreteDynamicsWorld> collisionWorld, HWND hwnd, XMFLOAT3 cameraPosition )
{
	//Load settings from file
	InitializeSettings(this);


	IsRunning = true;
	this->device = device;

	timePassed = 0.0f;
	timeThreshold = 10.0f;
	rangeThreshold = 800.0f;

	chunkMap = std::make_shared<std::unordered_map<std::pair<int,int>, std::pair<bool, std::shared_ptr<MarchingCubeChunk>>, int_pair_hash>>();
	collisionHandler = collisionWorld;

	noise.ReseedRandom();

	lastUsedKey = std::make_pair<int, int>(99, -99);
	lastMin = std::make_pair<int, int>(-99, -99);
	lastMax = std::make_pair<int, int>(99, 99);

	//Very powerful black magic going on here. Do not disturb.
	stepScaling = 1.0f / (stepSize.x*(stepCount.x-3));//(stepSize.x*(stepCount.x-3)) / 5000;

	//I give no shits if this throws a warning, it helps me remember which terraintypes there are and their names.
	//terrainType = TerrainTypes::Plains;//(TerrainNoiseSeeder::TerrainTypes)(1 + rand()%8); //

	terrainNoiser.Initialize((int)stepCount.x, (int)stepCount.y, (int)stepCount.z, &noise, terrainType);

	vegetationManager.Initialize(device, hwnd);

	D3D11_FEATURE_DATA_THREADING threadingFeatures;
	multiThreadedMeshCreationEnabled = true;

	if(!FAILED(device->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingFeatures, sizeof(D3D11_FEATURE_DATA_THREADING))))
	{
		//BOOL to bool casting...
		multiThreadedMeshCreationEnabled = (threadingFeatures.DriverConcurrentCreates != 0);
	}

	//Initialize it first...
	numThreads = thread::hardware_concurrency();

	if(workThreads.size() == 0)
	{
		if(numThreads > 1)
		{
			//If we have 4 or more cores, we create two threads.
			if(numThreads >= 4)
			{
				workThreads.push_back(new thread(&JobThreadEntryPoint, ((void *)this)));
				workThreads.push_back(new thread(&JobThreadEntryPoint, ((void *)this)));
			}
			else
			{
				//If we have more than 1 core but less than 4, we create one thread.
				workThreads.push_back(new thread(&JobThreadEntryPoint, ((void *)this)));
			}
		}
	}

	fullyLoaded = true;

	return true;
}

void TerrainManager::Shutdown()
{
	if(fullyLoaded)
	{
		preProductionQueue.Clear();
		preProductionQueue.Shutdown();

		IsRunning = false;

		for(unsigned int i = 0; i < workThreads.size(); i++)
		{
			if(workThreads[i]->joinable())
			{
				workThreads[i]->join();
			}

			delete workThreads[i];
		}

		postProductionQueue.Clear();
		postProductionQueue.Shutdown();

		workThreads.clear();

		if(chunkMap)
		{
			for(auto it = chunkMap->begin(); it != chunkMap->end(); it++)
			{
				//Check against the bool flag first to see if we're trying to delete an object that isn't finished yet.
				if(it->second.first)
				{
					collisionHandler->removeRigidBody(it->second.second->GetRigidBody().get());
					delete it->second.second->GetRigidBody()->getMotionState();
				}
			}
		}
	}

	fullyLoaded = false;
}

void TerrainManager::ResetTerrain()
{
	//Break workthreads
	IsRunning = false;

	//Clear all the queued up work
	preProductionQueue.Clear();
	preProductionQueue.Shutdown();

	postProductionQueue.Clear();
	postProductionQueue.Shutdown();


	//After we've joined all threads and made them stop, we can turn on isRunning again
	IsRunning = true;

	for(auto it = GetMap()->begin(); it != GetMap()->end(); ++it)
	{
		//Check against the bool flag first to see if we're trying to delete an object that isn't finished yet.
		if(it->second.first)
		{
			collisionHandler->removeRigidBody(it->second.second->GetRigidBody().get());
			delete it->second.second->GetRigidBody()->getMotionState();
		}
	}

	chunkMap->clear();
	activeRenderables.clear();
	activeChunks.clear();

	timePassed = 0.0f;
	lastUsedKey = std::make_pair<int, int>(99, -99);
	lastMin = std::make_pair<int, int>(-99, -99);
	lastMax = std::make_pair<int, int>(99, 99);

	noise.ReseedRandom();
}

void TerrainManager::CreateChunk(ID3D11Device* device, int startPosX, int startPosZ)
{
	//Make a key out of the values
	std::pair<int,int> key(startPosX, startPosZ);

	//See if the key that we're using already exists
	auto mappedChunk = GetMap()->find(key);

	//If they key doesn't exist, we go about creating the chunk
	if(mappedChunk == GetMap()->end())
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

		std::vector<MarchingCubeVoxel> voxels;

		unsigned int index = 0;
		voxels.resize((unsigned int)(stepCount.x+1) * (unsigned int)(stepCount.y+1) * (unsigned int)(stepCount.z+1));


		// Set default values for each voxel in the field
		for(unsigned int z = 0; z <= stepCount.z+1; ++z)
		{
			for(unsigned int y = 0; y <= stepCount.y+1; ++y)
			{
				for(unsigned int x = 0; x <= stepCount.x+1; ++x)
				{
					//Calculate index for this voxel
					index = x + (y*(unsigned int)stepCount.y) + (z * (unsigned int)stepCount.y * (unsigned int)stepCount.z);

					//Set default values for this voxel
					voxels[index].position.x = actualPosX	+ stepSize.x * x;
					voxels[index].position.y = 0			+ stepSize.y * y;
					voxels[index].position.z = actualPosZ	+ stepSize.z * z;
					voxels[index].density = 0.0f;
					voxels[index].inside = false;
					voxels[index].normal.x = 0.5f;
					voxels[index].normal.y = 0.5f;
					voxels[index].normal.z = 0.5f;
				}
			}
		}

		//Noise the voxel field
		terrainNoiser.Noise3D(1, 1, 1, (int)stepCount.x, (int)stepCount.y, (int)stepCount.z, &voxels);

		//Create the mesh for the chunk with marching cubes algorithm
		marchingCubes.CalculateMesh(device, newChunk, &voxels);

		CreateMesh(device, newChunk);
		CreateWaterMesh(device, newChunk);

		//Setup collision shape with the triMesh that was created inside marching cubes class
		std::shared_ptr<btBvhTriangleMeshShape> triMeshShape = std::make_shared<btBvhTriangleMeshShape>(newChunk->GetTriMesh(), true);

		//Assign it to this chunk
		newChunk->SetCollisionShape(triMeshShape);

		//Set up some rigid body stuff
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, NULL, newChunk->GetCollisionShape().get(), btVector3(0,0,0));
		std::shared_ptr<btRigidBody> rigidBody = std::make_shared<btRigidBody>(groundRigidBodyCI);
		rigidBody->setFriction(1);
		rigidBody->setRollingFriction(1);
		rigidBody->setRestitution(0.5f);

		//Assign it to this chunk
		newChunk->SetRigidBody(rigidBody);

		//Add it to the world
		collisionHandler->addRigidBody(newChunk->GetRigidBody().get());

		//...Generate and place vegetation based on data from the chunk.
		//GenerateVegetation(device, false, newChunk.get());

		//Clean up and remove the HUGE index and vertex std::vectors because they are no longer needed.
		newChunk->GetIndices()->clear();
		newChunk->GetIndices()->shrink_to_fit();

		newChunk->GetVertices()->clear();
		newChunk->GetVertices()->shrink_to_fit();

		//Create a new slot with key and insert the new chunk.
		GetMap()->emplace(std::make_pair<std::pair<int,int>, std::pair<bool, std::shared_ptr<MarchingCubeChunk>>>(key, std::make_pair<bool, std::shared_ptr<MarchingCubeChunk>>(true, newChunk)));
	}
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
	auto mappedChunk = chunkMap->find(key);

	//If the object DOES exist at this key and it's the right object we've retrieved...
	if(mappedChunk != chunkMap->end())
	{
		//If it isn't ready to be rendered (it's in production), then we break early and return false
		if(!mappedChunk->second.first)
		{
			return false;
		}

		//Temporary sanity check. Not sure if this can actually happen yet, but if it does happen, we want it to happen loudly.
		if(mappedChunk->first != key)
		{
			MessageBox(NULL , L"TerrainManager::GetChunk has somehow retrieved the wrong chunk from the hash table. Look into that.", L"Error", MB_OK);

			return false;
		}

		//We've passed all checks and return the pointer that the iterator points to.
		*outChunk = mappedChunk->second.second.get();

		return true;
	}

	//There was no chunk located at that key
	return false;
}

bool TerrainManager::UpdateAgainstAABB(Lemmi2DAABB* aabb, float deltaTime)
{
	ChunkFinalizing(device);

	std::pair<int,int> neighbourKey;

	timePassed += deltaTime;

	lastCamX = (aabb->CenterPoint().x*stepScaling);
	lastCamZ = (aabb->CenterPoint().y*stepScaling);

	int startX	=	static_cast<int>(aabb->MinPoint().x);
	int startZ	=	static_cast<int>(aabb->MinPoint().y);
	int endX	=	static_cast<int>(aabb->MaxPoint().x);
	int endZ	=	static_cast<int>(aabb->MaxPoint().y);

	//Define the boundaries. We'll use the boundaries to loop through each relevant grid slot.
	// I calculate the start and ending indices by dividing the positions by a large amount and casting them to int. 
	// The amount I divide with is calculated through step size and step scaling.
	//The *2 in there is ..... because before, the chunks used to be 100x100 units large... ish. Now they're 50x50 large... ish. Hence I needed a magic number offset. Yay!
	startX =	RoundToNearest(startX*(stepScaling))-3;
	startZ =	RoundToNearest(startZ*(stepScaling))-3;
	endX =		RoundToNearest(endX*(stepScaling))+3;
	endZ =		RoundToNearest(endZ*(stepScaling))+3;

	//Instead of checking against like...... 25-30 grids we instead first check if the min and max points have changed.
	if(lastMin.first != startX && lastMin.second != startZ || lastMax.first != endX || lastMax.second != endZ)
	{
		//Every few seconds we do a cleanup to remove old chunks
		if(timePassed >= timeThreshold)
		{
			Cleanup(aabb->CenterPoint().x, aabb->CenterPoint().y);

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
					QueueChunkForCreation(neighbourKey.first, neighbourKey.second, static_cast<int>(lastCamX), static_cast<int>(lastCamZ));
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
	lock_guard<fast_mutex> guard(mutexObject);

	//Iterate through entirety of map
	for(auto it = chunkMap->begin(); it != chunkMap->end();)
	{
		//We only do checks if the chunk has actually been created
		if(it->second.first)
		{
			const XMFLOAT3& position = it->second.second->GetPosition();

			//If chunk is too far away from position (camera), remove it
			if(abs(position.x - posX) >= rangeThreshold || abs(position.z - posZ) >= rangeThreshold)
			{
				collisionHandler->removeRigidBody(it->second.second->GetRigidBody().get());
				delete it->second.second->GetRigidBody()->getMotionState();

				it = chunkMap->erase(it);
			}
			else
			{
				//If we've erased an item we don't want to increment
				++it;
			}
		}
		else
		{
			//If we've erased an item we don't want to increment
			++it;
		}
	}
}

void TerrainManager::QueueChunkForCreation(int startPosX, int startPosZ, int cameraPosX, int cameraPosZ)
{
	//So this is a sanity check to see if we aren't trying to create a chunk that is ... really far away, for whatever reason
	if(abs(startPosX - cameraPosX) < 15 && abs(startPosZ - cameraPosZ) < 15)
	{
		std::pair<int,int> key(startPosX, startPosZ);

		//See if the key that we're using already exists
		unsigned int count = GetMap()->count(key);

		assert(count <= 1);

		//If the key doesn't exist, we go about creating the chunk
		if(count == 0)
		{
			if(numThreads <= 1)
			{
				CreateChunk(device, startPosX, startPosZ);
			}
			else
			{

				//Insert the key and newly created chunk
				GetMap()->emplace(std::make_pair<std::pair<int,int>, std::pair<bool, std::shared_ptr<MarchingCubeChunk>>>(key, std::make_pair<bool, std::shared_ptr<MarchingCubeChunk>>(false, nullptr)));

				//Push key into production line to tell the work threads to begin noising and creating this chunk properly
				preProductionQueue.Push(key);
			}
		}
	}
}

void TerrainManager::ChunkFinalizing(ID3D11Device* device)
{
	std::pair<bool, std::shared_ptr<MarchingCubeChunk>> key;

	//If popping succeeds ...
	if(postProductionQueue.Try_pop(key.second))
	{
		//Do one last check to make sure this isn't a wildly out of bounds chunk
		if(abs(key.second->GetKey().first - lastCamX) < 15 && abs(key.second->GetKey().second - lastCamZ) < 15)
		{
			//If we can't create the meshes in the work threads due to driver limitations, then we do it in the main thread...
			if(!DXMultiThreading())
			{
				CreateMesh(device, key.second);
				CreateWaterMesh(device, key.second);
			}

			//Clear index vector, then shrink to 0 (free up memory)
			key.second->GetIndices()->clear();
			key.second->GetIndices()->shrink_to_fit();

			//Clear vertex vector, then shrink to 0 (free up memory)
			key.second->GetVertices()->clear();
			key.second->GetVertices()->shrink_to_fit();

			//Add it to the world
			collisionHandler->addRigidBody(key.second->GetRigidBody().get());

			//Replace chump value
			(*chunkMap)[key.second->GetKey()].second = key.second;

			//Flag bool as true, this means it's ready to use...
			(*chunkMap)[key.second->GetKey()].first = true;
		}
	}
}

void TerrainManager::OnSettingsReload(Config* cfg)
{
	int type;

	const Setting& settings = cfg->getRoot()["terrain"];
	settings.lookupValue("startingTerrainType", type);

	terrainType = (TerrainTypes::Type)type;

	//Only set terraintype if it's been fully initialized
	if(fullyLoaded)
	{
		terrainNoiser.SetTerrainType(terrainType);
	}
}

void TerrainManager::CreateMesh(ID3D11Device* devicePtr, std::shared_ptr<MarchingCubeChunk> chunk)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//Just ptrs cuz too lazy to rename everything
	auto indices = chunk->GetIndices();
	auto vertices = chunk->GetVertices();

	chunk->GetTerrainMesh()->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	chunk->GetTerrainMesh()->SetVertexCount(vertices->size());
	chunk->GetTerrainMesh()->SetIndexCount(indices->size());
	chunk->GetTerrainMesh()->SetVertexStride(sizeof(MarchingCubeVectors));

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(MarchingCubeVectors) * vertices->size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the sub resource texture a pointer to the vertex data.
	vertexData.pSysMem = vertices->data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = devicePtr->CreateBuffer(&vertexBufferDesc, &vertexData, chunk->GetTerrainMesh()->GetVertexBufferPP());

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indices->size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the sub resource texture a pointer to the index data.
	indexData.pSysMem = indices->data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = devicePtr->CreateBuffer(&indexBufferDesc, &indexData, chunk->GetTerrainMesh()->GetIndexBufferPP());
}

void TerrainManager::CreateWaterMesh(ID3D11Device* devicePtr, std::shared_ptr<MarchingCubeChunk> chunk)
{
	if(chunk->GetWaterLevel() > 0.1f)
	{

		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;

		XMFLOAT2 minPos;
		unsigned int index = 0;

		//We measure the positions of each vertex that is below Y:2.
		//And keep track of each minimum and maximum value, so that when we've processed the entire mesh of this chunk, 
		// we know the MinXY and MaxXY values for when we want to create the water mesh!
		minPos.x = chunk->GetStartPosX();
		minPos.y = chunk->GetStartPosZ();

		auto waterMesh = chunk->GetWaterMesh();

		waterMesh->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//Okay, so. I have already decided that the spacing will be 1.0f between each vertex.
		unsigned int stepsX = (unsigned int)((chunk->GetStepCountX())*chunk->GetStepSizeX());
		unsigned int stepsZ = (unsigned int)((chunk->GetStepCountZ())*chunk->GetStepSizeZ());

		std::vector<XMFLOAT3> vertices;
		std::vector<unsigned int> indices;

		//Rather shitty tutorial with wrong algorithms in some places, but I managed to piece it together anyway.
		//http://www.uniqsoft.co.uk/directx/html/tut3/tut3.htm

		// Create the structure to hold the height map data.
		vertices.resize((stepsX) * (stepsZ));

		// Read the image data into the height map.
		for(float z = 0; z < stepsZ; z++)
		{
			for(float x = 0; x < stepsX; x++)
			{
				index = (int)(x + (z * stepsX));

				//Offset each position with a magic number to make sure there are no seams between the different water meshes.
				vertices[index].x = (minPos.x + x); //0.0475f -(x*0.115f)
				vertices[index].y = chunk->GetWaterLevel();
				vertices[index].z = (minPos.y + z); //0.00908 -(z*0.115f)
			}
		}


		//TODO: this index buffer is the same for every mesh. Maybe create once and copy it.
		int indexOffset = 0;
		indices.resize(((stepsX * 2) + 2) * (stepsZ - 1));

		for(unsigned int z = 0; z < stepsZ-1; z++ )
		{
			for(unsigned int x = 0; x < stepsX; x++ )
			{
				index = x + ( z * stepsX );
				indices[indexOffset] = index;

				indexOffset++;
				index = x +( (z+1) * stepsX );

				indices[indexOffset] = index;
				indexOffset++;
			}

			//Place in copy of previous one
			indices[indexOffset] = index;
			indexOffset++;

			//Place in first one for next row
			index = 0 + ( (z+1) * stepsX );
			indices[indexOffset] = index;
			indexOffset++;
		}

		waterMesh->SetVertexCount(vertices.size());
		waterMesh->SetIndexCount(indices.size());
		waterMesh->SetVertexStride(sizeof(XMFLOAT3));

		// Set up the description of the static vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(XMFLOAT3) * vertices.size();
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the sub resource texture a pointer to the vertex data.
		vertexData.pSysMem = vertices.data();
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
		result = devicePtr->CreateBuffer(&vertexBufferDesc, &vertexData, waterMesh->GetVertexBufferPP());

		// Set up the description of the static index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the sub resource texture a pointer to the index data.
		indexData.pSysMem = indices.data();
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = devicePtr->CreateBuffer(&indexBufferDesc, &indexData, waterMesh->GetIndexBufferPP());
	}
}

#pragma region Shit that Im not currently using but dont want to delete in case it becomes relevant again later
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
			if(abs((*it)->GetKey().first - lastCamX) < 15 && abs((*it)->GetKey().second - lastCamZ) < 15)
			{
				activeRenderables.push_back((*it)->GetTerrainMesh());
			}
		}
	}

	return &activeRenderables;
}

//bool TerrainManager::Update(ID3D11Device* device, XMFLOAT3 currentCameraPosition, float deltaTime)
//{
//	std::pair<int,int> key(RoundToNearest((currentCameraPosition.x)*stepScaling), RoundToNearest((currentCameraPosition.z)*stepScaling));
//	std::pair<int,int> neighbourKey;
//
//	timePassed += deltaTime;
//
//	if(key != lastUsedKey)
//	{
//		//////////////////////////////////////////////////////////////////////////
//		/*
//		*	Update active chunks and active renderables.
//		*/
//		//////////////////////////////////////////////////////////////////////////
//
//		//Temp chunk to hold pointer from each GetChunk call.
//		MarchingCubeChunk* tempChunk;
//
//		//Clear active chunks
//		activeChunks.clear();
//		activeRenderables.clear();
//
//		//Add all new relevant chunks
//		for(int i=0; i < 9; i++)
//		{
//			bool result;
//			neighbourKey = AddPairs(key, edgePairs[i]);
//
//			//Fetch chunk from the right grid slot
//			result = GetChunk(neighbourKey.first, neighbourKey.second, &tempChunk);
//
//			//If this chunk is valid
//			if(result == true)
//			{
//				//Add ptr to active chunk std::vector
//				activeChunks.push_back(tempChunk);
//				activeRenderables.push_back(tempChunk->GetTerrainMesh());
//
//				////Add this chunk's instances to the temporary std::vector
//				//tempVec.insert(tempVec.end(), tempChunk->GetVegetationInstances()->cbegin(), tempChunk->GetVegetationInstances()->cend());
//			}
//			else
//			{
//				CreateChunk(device, neighbourKey.first, neighbourKey.second);
//			}
//		}
//
//		//Send temporary std::vector to be built into a vegetation instance buffer
//		//vegetationManager.BuildInstanceBuffer(device, &tempVec);
//
//		lastUsedKey = key;
//
//		return true;
//	}
//
//	return false;
//}

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
#pragma endregion