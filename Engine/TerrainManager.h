#pragma once

#define BT_NO_SIMD_OPERATOR_OVERLOADS //Needed to fix clash between bullet libraries and xnamath. https://code.google.com/p/bullet/issues/detail?id=710

#include <btBulletDynamicsCommon.h>
#include "Lemmi2DAABB.h"
#include "MarchingCubeChunk.h"
#include <unordered_map>
#include "MCTerrainClass.h"
#include "VegetationManager.h"
#include <memory> //For shared_ptrs
#include "marchingCubesClass.h"
#include "d3dmanager.h"
#include <math.h>

class TerrainManager
{
private:

	//http://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
	//http://en.wikipedia.org/wiki/Cantor_pairing_function#Cantor_pairing_function << Apparently only works well with positive values, so not such a good candidate.
	//	return ((v.first + v.second)*(v.first + v.second + 1)/2) + v.second;
	struct int_pair_hash 
	{
		inline std::size_t operator()(const std::pair<int,int> & v) const 
		{
			std::size_t hashOne(std::hash<int>()(v.first*31));
			std::size_t hashTwo(std::hash<int>()(v.second));

			return hashOne+hashTwo;//((hashOne+hashTwo) * (hashOne+hashTwo + 1) / 2) + hashTwo;
		}
	};

public:
	TerrainManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::shared_ptr<btDiscreteDynamicsWorld> collisionWorld, HWND hwnd,  XMFLOAT3 cameraPosition);
	~TerrainManager();

	//Returns a bool to indicate if we've actually had to change anything. If true, it has changed and we should fetch the new data.
	bool Update(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT3 currentCameraPosition);
	bool UpdateAgainstAABB(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Lemmi2DAABB* aabb);

	void ResetTerrain(int currentPosX, int currrentPosZ);
	void CreateChunk(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int startPosX, int startPosZ);
	void MergeWithNeighbourChunks(MarchingCubeChunk* chunk,  int idX, int idZ);

	VegetationManager* const GetVegetationManager() { return &vegetationManager; };
	bool GetChunk(int x, int z, MarchingCubeChunk** outChunk);
	vector<RenderableInterface*>* GetTerrainRenderables(int x, int z);

	vector<MarchingCubeChunk*>& GetActiveChunks() { return activeChunks; }
	vector<RenderableInterface*>& GetActiveRenderables(){ return activeRenderables; }

	void GenerateVegetation(ID3D11Device* device, bool UpdateInstanceBuffer, MarchingCubeChunk* chunk);
	void RenderVegetation();

	void SetTerrainType(MCTerrainClass::TerrainTypes val) { mcTerrain.SetTerrainType(val); }

private:
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
	std::shared_ptr<btDiscreteDynamicsWorld> collisionHandler;

	MarchingCubesClass marchingCubes;
	MCTerrainClass mcTerrain;
	VegetationManager vegetationManager;
	NoiseClass noise;
	unsigned int vegetationCount;
	float stepScaling;


	std::shared_ptr<std::unordered_map<std::pair<int,int>, std::shared_ptr<MarchingCubeChunk>, int_pair_hash>> map;
	vector<MarchingCubeChunk*> activeChunks;
	vector<RenderableInterface*> activeRenderables; //We add each chunk's mesh to this list and only change it when our activechunks change

	std::pair<int,int> lastUsedKey;
	std::pair<int,int> lastMin;
	std::pair<int,int> lastMax;

	//What this class is supposed to do is handle the creation and lifetime of chunks. Potentially save/load them to hdd as well.
	//Also: Culling, interaction with higher-up-in-hierarchy classes, ...rendering? idk
	// 
	// I think I've finally figured it out. Just use a sorted map that uses a pair<int,int> as key and a chunk as value. Or maybe like a shared_ptr<chunk> as value
	//http://en.cppreference.com/w/cpp/utility/hash
	//http://stackoverflow.com/questions/15160889/how-to-make-unordered-set-of-pairs-of-integers-in-c << this

};

