#include "LSystemBranch.h"


LSystemBranch::LSystemBranch(void)
{
}


LSystemBranch::~LSystemBranch(void)
{
}

void LSystemBranch::InitializeFirst
	(
	XMFLOAT2 positionStart,
	XMFLOAT2 positionEnd,
	float length,
	float lengthFallof,
	float width,
	float widthFallof,
	int maxBranches,
	int maxBranchesPerSplit,
	float maxDeviation,
	float minDeviation,
	float SplitPointPercent,
	float chanceToSplit,
	float angle,
	list<LSystemBranch*>* BranchList
	)
{
	this->positionStart = positionStart;
	this->positionEnd = positionEnd;
	this->lengthFallof  = lengthFallof;
	this->length = length;
	this->widthFallof = widthFallof;
	this->width = width;

	this->maxBranches = maxBranches;
	this->maxBranchesPerSplit = maxBranchesPerSplit;

	this->maxDeviation = maxDeviation;
	this->minDeviation = minDeviation;
	this->SplitPointPercent = SplitPointPercent;
	this->chanceToSplit = chanceToSplit;
	CalculateAngle(angle);

	this->itterationsDone = 0;
}

void LSystemBranch::Initialize(LSystemBranch* parrent, unsigned short ID)
{
	this->parrent = parrent;
	this->ID = ID;
	positionStart = parrent->positionEnd;

	lengthFallof  = parrent->lengthFallof;
	length = parrent->length * lengthFallof;
	widthFallof = parrent->widthFallof;
	width = parrent->width * widthFallof;


	maxBranches = parrent->maxBranches;
	maxBranchesPerSplit = parrent->maxBranchesPerSplit;


	maxDeviation = parrent->maxDeviation;
	minDeviation = parrent->minDeviation;
	SplitPointPercent = parrent->SplitPointPercent;
	chanceToSplit = parrent->chanceToSplit;

	CalculateAngle(parrent->angle);

	itterationsDone = parrent->itterationsDone +1;
}

void LSystemBranch::CalculateAngle(float angle)
{
	if (ID !=0)
	{
		angle = (minDeviation + (maxDeviation - minDeviation) * random()) * (ID * 2 -3) + parrent->angle;
	}
	else
	{
		angle = parrent->angle;
	}
}

void LSystemBranch::BranchOff()
{
	
	child = new LSystemBranch();
	child->Initialize(this,0);

	if(random() < chanceToSplit)
	{
		child = new LSystemBranch();
		child->Initialize(this,1);
	}
		if(random() < chanceToSplit)
	{
		child2 = new LSystemBranch();
		child2->Initialize(this,2);
	}
}