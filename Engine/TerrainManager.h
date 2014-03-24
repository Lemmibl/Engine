#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include <btBulletDynamicsCommon.h>

#include "Lemmi2DAABB.h"
#include "TerrainNoiseSeeder.h"
#include "MarchingCubeChunk.h"
#include "VegetationManager.h"
#include "marchingCubesClass.h"
#include "d3dmanager.h"
#include "SettingsDependent.h"

#include <unordered_map>
#include <memory> //For shared_ptrs
#include <math.h>

#include "ThreadsafeQueue.h"

using namespace tthread;

class TerrainManager : public SettingsDependent
{
private:

	//http://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
	//http://en.wikipedia.org/wiki/Cantor_pairing_function#Cantor_pairing_function << Apparently only works well with positive values, so not such a good candidate.
	//	return ((v.first + v.second)*(v.first + v.second + 1)/2) + v.second;
	struct int_pair_hash 
	{
		inline std::size_t operator()(const std::pair<int,int>& v) const 
		{
			std::size_t hashOne(std::hash<int>()(v.first*31 + (v.second+33)));

			return hashOne;//((hashOne+hashTwo) * (hashOne+hashTwo + 1) / 2) + hashTwo;
		}
	};

public:
	TerrainManager();
	~TerrainManager();

	void Shutdown();

	bool Initialize(ID3D11Device* device, std::shared_ptr<btDiscreteDynamicsWorld> collisionWorld, HWND hwnd,  XMFLOAT3 cameraPosition);

	////Returns a bool to indicate if we've actually had to change anything. If true, it has changed and we should fetch the new data.
	//bool Update(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT3 currentCameraPosition, float deltaTime);

	//Returns a bool to indicate if we've actually had to change anything. If true, it has changed and we should fetch the new render data.
	bool UpdateAgainstAABB(Lemmi2DAABB* aabb, float deltaTime);

	void ResetTerrain();

	bool DXMultiThreading() { lock_guard<fast_mutex> guard(mutexObject); return multiThreadedMeshCreationEnabled; }

	ThreadsafeQueue<std::pair<int,int>>* GetPreProductionQueue() {  lock_guard<fast_mutex> guard(mutexObject); return &preProductionQueue; }
	ThreadsafeQueue<std::shared_ptr<MarchingCubeChunk>>* GetPostProductionQueue() {  lock_guard<fast_mutex> guard(mutexObject); return &postProductionQueue; }

	std::unordered_map<std::pair<int,int>, std::pair<bool, std::shared_ptr<MarchingCubeChunk>>, int_pair_hash>* GetMap() 
	{ 
		lock_guard<fast_mutex> guard(mutexObject);
		return chunkMap.get(); 
	}

	MarchingCubesClass& GetMarchingCubesClass() 
	{
		lock_guard<fast_mutex> guard(mutexObject);
		return marchingCubes; 
	}

	TerrainNoiseSeeder& GetTerrainNoiser() 
	{ 
		lock_guard<fast_mutex> guard(mutexObject);
		return terrainNoiser; 
	}

	ID3D11Device* GetDevice() { return device; }
	VegetationManager* const GetVegetationManager() { return &vegetationManager; };
	std::vector<RenderableInterface*>* GetTerrainRenderables(int x, int z);
	std::vector<MarchingCubeChunk*>& GetActiveChunks() { return activeChunks; }
	std::vector<RenderableInterface*>& GetActiveRenderables(){ return activeRenderables; }

	unsigned int GetActiveChunkCount() { return activeChunks.size(); }
	unsigned int GetChunkInProductionCount() { return (preProductionQueue.GetSize() + postProductionQueue.GetSize()); }

	void OnSettingsReload(Config* cfg);

	void SetTerrainType(TerrainTypes::Type val) { terrainNoiser.SetTerrainType(val); }

	//TODO: BuildMeshes(ID3D11Device* device, ID3D11DeviceContext* deviceContext, MarchingCubeChunk* chunk)
	bool GetChunk(int x, int z, MarchingCubeChunk** outChunk);

	//This here is where we first decide to create a chunk
	void QueueChunkForCreation(int startPosX, int startPosZ);

	//After the work threads have finished everything, we need to do a few final tweaks before the chunk is ready to be released into the world
	void ChunkFinalizing(ID3D11Device* device);

	//Creating the actual chunk. This function will hopefully one day be run on a separate thread or core
	void CreateChunk(ID3D11Device* device, int startPosX, int startPosZ);

	//Create vertex and index buffers from the data that we've created
	void CreateMesh(ID3D11Device* devicePtr, std::shared_ptr<MarchingCubeChunk> chunk);

	//Creates a flat mesh from a min and max pos.
	void CreateWaterMesh(ID3D11Device* devicePtr, std::shared_ptr<MarchingCubeChunk> chunk);

private:
	void MergeWithNeighbourChunks(MarchingCubeChunk* chunk,  int idX, int idZ);
	void GenerateVegetation(ID3D11Device* device, bool UpdateInstanceBuffer, MarchingCubeChunk* chunk);
	void Cleanup(float posX, float posZ);

	inline std::pair<int,int> AddPairs(std::pair<int,int> pairOne, std::pair<int,int> pairTwo)
	{
		return std::make_pair<int, int>(pairOne.first+pairTwo.first, pairOne.second+pairTwo.second);
	}

	inline float RandomFloat()
	{
		float scale=RAND_MAX+1.0f;
		float base=rand()/scale;
		float fine=rand()/scale;
		return base+fine/scale;
	}

	inline int RoundToNearest(float num) 
	{
		return (int)((num > 0.0f) ? (num + 0.5f) : (num - 0.5f));
	}

private:
	ID3D11Device* device;
	std::shared_ptr<btDiscreteDynamicsWorld> collisionHandler;

	MarchingCubesClass marchingCubes;
	TerrainNoiseSeeder terrainNoiser;
	VegetationManager vegetationManager;
	NoiseClass noise;
	unsigned int vegetationCount;
	float stepScaling;
	float timePassed, timeThreshold, rangeThreshold;
	TerrainTypes::Type terrainType;

	mutable std::shared_ptr<std::unordered_map<std::pair<int,int>, std::pair<bool, std::shared_ptr<MarchingCubeChunk>>, int_pair_hash>> chunkMap;
	std::vector<MarchingCubeChunk*> activeChunks;
	std::vector<RenderableInterface*> activeRenderables; //We add each chunk's mesh to this list and only change it when our activechunks change
	
	//Keys to the chunks that are "in construction".

	//This queue is filled by the main thread and consumed by the work threads when they're looking for new work
	ThreadsafeQueue<std::pair<int,int>> preProductionQueue;

	//And this queue is filled by the work threads and consumed by the main thread to do the "sync-critical" parts that can't be performed by the work threads
	ThreadsafeQueue<std::shared_ptr<MarchingCubeChunk>> postProductionQueue;

	std::pair<int,int> lastUsedKey;
	std::pair<int,int> lastMin;
	std::pair<int,int> lastMax;

	bool fullyLoaded;

	std::vector<thread*> workThreads;
	unsigned int numThreads;
	bool multiThreadedMeshCreationEnabled;
	fast_mutex mutexObject;

	//What this class is supposed to do is handle the creation and lifetime of chunks. Potentially save/load them to hdd as well.
	//Also: Culling, interaction with higher-up-in-hierarchy classes, ...rendering? idk
	// 
	// I think I've finally figured it out. Just use a sorted map that uses a pair<int,int> as key and a chunk as value. Or maybe like a shared_ptr<chunk> as value
	//http://en.cppreference.com/w/cpp/utility/hash
	//http://stackoverflow.com/questions/15160889/how-to-make-unordered-set-of-pairs-of-integers-in-c << this

};