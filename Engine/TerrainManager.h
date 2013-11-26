#pragma once
#include <map>

class TerrainManager
{
public:
	TerrainManager(void);
	~TerrainManager(void);

private:
	
//What this class is supposed to do is handle the creation and lifetime of chunks. Potentially save/load them to hdd as well.
//Also: Culling, interaction with higher-up-in-hierarchy classes, ...rendering? idk
// 
// I think I've finally figured it out. Just use a sorted map that uses a pair<int,int> as key and a chunk as value. Or maybe like a shared_ptr<chunk> as value
//http://en.cppreference.com/w/cpp/utility/hash
//
};

