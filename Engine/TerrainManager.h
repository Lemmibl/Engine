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
//IDEA: Put the .. uh.. gridtree here. Or just like.. a 2D array. Make it simple, stupid(tm).
// Best bet would be a vector<vector>> ? Or a hash-tree ... Bonuses when using a hash-tree is I can use negative values as keys

};

