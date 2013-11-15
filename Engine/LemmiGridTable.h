#pragma once
#include "LemmiEntity.h"
#include <unordered_map>
#include <windows.h>
#include <xnamath.h>
#include <vector>

using namespace std;

class PairHash
{
public:
	size_t operator()(const pair<int, int> &k) const{
		return k.first * 100 + k.second*50;
	}
};


//http://stackoverflow.com/questions/7222143/unordered-map-hash-function-c/7222212#7222212

class LemmiGridTable
{
public:
	LemmiGridTable();
	~LemmiGridTable();

	void Insert(LemmiEntity* entity);
	const vector<LemmiEntity*>& GetCell(XMFLOAT3 position) const;
	const vector<LemmiEntity*>& GetCell(int posX, int posY) const;

private:
	//The pair of ints represents posX and posZ in the world. The world is split into cells that are 100x100 big. First cell is posX==0 to 99 and posZ==0 to 99
	std::unordered_map<std::pair<int, int>, vector<LemmiEntity*>, PairHash> table;

};