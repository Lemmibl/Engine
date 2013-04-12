#pragma once

#include "LSystemBranch.h"


class LSystemClass
{
public:
	LSystemClass(void);
	~LSystemClass(void);

	
	void initialize();
	
private:
	//list<LSystemBranch> BranchList;
	XMFLOAT2 positionToStart;
	XMFLOAT2 positionToEnd;
	float length;
	float lengthFallof;
	float width;
	float widthFallof;
	int itterationsDone;
	int maxBranches;
	int maxBranchesPerSplit;
	float maxDeviation;
	float minDeviation;
	float SplitPointPercent;
	float chanceToSplit;
	float angle;
	unsigned short ID;
	list<LSystemBranch*> BranchList;

	LSystemBranch* trunk;
	
};

