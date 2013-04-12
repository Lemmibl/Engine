#include "LSystemClass.h"


LSystemClass::LSystemClass(void)
{
}


LSystemClass::~LSystemClass(void)
{
}

void LSystemClass::initialize()
{


	trunk = new LSystemBranch();
	trunk->InitializeFirst(positionToStart,positionToEnd,length,lengthFallof,width,widthFallof,maxBranches,maxBranchesPerSplit,maxDeviation,minDeviation,SplitPointPercent,chanceToSplit,angle,&BranchList);

	BranchList.push_back(new LSystemBranch());
	


}