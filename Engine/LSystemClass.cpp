#include "LSystemClass.h"


LSystemClass::LSystemClass(void)
{
	positionToStart = XMFLOAT2(30.0f,30.0f);
	positionToEnd = XMFLOAT2(30.0f,30.0f);
	length = 5.0f;
	lengthFalloff = 0.5f;
	width = 4.0f;
	widthFalloff = 0.5f;
	maxBranches = 4;
	maxBranchesPerSplit = 3;
	maxDeviation = 0.2f;
	minDeviation = 0.05f;
	SplitPointPercent = 0.7f;
	chanceToSplit = 0.9f;
	angle = 1.0f;

	BranchList = new list<LSystemBranch*>();
}


LSystemClass::~LSystemClass(void)
{
}

void LSystemClass::initialize()
{


	trunk = new LSystemBranch();
	trunk->InitializeFirst(positionToStart,positionToEnd,length,lengthFalloff,width,widthFalloff,maxBranches,maxBranchesPerSplit,maxDeviation,minDeviation,SplitPointPercent,chanceToSplit,angle,BranchList);
	trunk->ID = 0;
	BranchList->push_front(new LSystemBranch());
	


}