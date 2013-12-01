#pragma once
#include <map>
#include "MarchingCubeChunk.h"
#include <unordered_map>
#include "MCTerrainClass.h"

class TerrainManager
{
private:

	//http://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
	//http://en.wikipedia.org/wiki/Cantor_pairing_function#Cantor_pairing_function
	//	return ((p.first + p.second)*(p.first + p.second + 1)/2) + p.second;
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
	TerrainManager();
	~TerrainManager();

	void CreateChunk(int startPosX, int startPosZ);
	bool GetChunk(int x, int z, MarchingCubeChunk* inOutChunk);
	vector<MarchingCubeChunk*>* GetActiveChunks(int x, int z);
	vector<RenderableInterface*>* GetTerrainRenderables();

private:
	std::pair<int,int> AddPairs(std::pair<int,int> pairOne, std::pair<int,int> pairTwo)
	{
		return std::make_pair<int, int>(pairOne.first+pairTwo.first, pairOne.second+pairTwo.second);
	}

private:
	std::unordered_map<std::pair<int,int>, MarchingCubeChunk*, int_pair_hash> map;
	vector<MarchingCubeChunk*> activeChunks;
	vector<RenderableInterface*> activeRenderables; //We add each chunk's mesh to this list and only change it when our activechunks change

	std::pair<int,int> lastUsedKey;

	XMFLOAT3 stepSize, stepCount;

	MCTerrainClass mcTerrain;

	//What this class is supposed to do is handle the creation and lifetime of chunks. Potentially save/load them to hdd as well.
	//Also: Culling, interaction with higher-up-in-hierarchy classes, ...rendering? idk
	// 
	// I think I've finally figured it out. Just use a sorted map that uses a pair<int,int> as key and a chunk as value. Or maybe like a shared_ptr<chunk> as value
	//http://en.cppreference.com/w/cpp/utility/hash
	//http://stackoverflow.com/questions/15160889/how-to-make-unordered-set-of-pairs-of-integers-in-c << this

};

