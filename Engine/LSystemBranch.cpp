//#include "LSystemBranch.h"
//
//
//LSystemBranch::LSystemBranch(void)
//{
//}
//
//
//LSystemBranch::~LSystemBranch(void)
//{
//}
//
//void LSystemBranch::InitializeFirst
//	(
//	XMFLOAT2 positionStart,
//	XMFLOAT2 positionEnd,
//	float length,
//	float lengthFallof,
//	float width,
//	float widthFallof,
//	int maxBranches,
//	int maxBranchesPerSplit,
//	float maxDeviation,
//	float minDeviation,
//	float SplitPointPercent,
//	float chanceToSplit,
//	float angle,
//	list<LSystemBranch>* BranchList
//	)
//{
//	this->positionStart = positionStart;
//	this->positionEnd = positionEnd;
//	this->lengthFallof  = lengthFallof;
//	this->length = length;
//	this->widthFallof = widthFallof;
//	this->width = width;
//
//	this->maxBranches = maxBranches;
//	this->maxBranchesPerSplit = maxBranchesPerSplit;
//
//	this->maxDeviation = maxDeviation;
//	this->minDeviation = minDeviation;
//	this->SplitPointPercent = SplitPointPercent;
//	this->chanceToSplit = chanceToSplit;
//	this->angle = angle;
//
//	this->itterationsDone = 0;
//	BranchList = new list<LSystemBranch>;
//	BranchList->push_back(*this);
//	BranchOff(BranchList);
//}
//
//void LSystemBranch::Initialize(LSystemBranch* parrent, unsigned short ID, list<LSystemBranch>* BranchList)
//{
//	this->parrent = parrent;
//	this->BranchList = BranchList;
//	this->ID = BranchList->size();
//	this->positionStart = parrent->positionEnd;
//
//	this->lengthFallof  = parrent->lengthFallof;
//	this->length = parrent->length * lengthFallof;
//	this->widthFallof = parrent->widthFallof;
//	this->width = parrent->width * widthFallof;
//
//
//	this->maxBranches = parrent->maxBranches;
//	this->maxBranchesPerSplit = parrent->maxBranchesPerSplit;
//
//
//	this->maxDeviation = parrent->maxDeviation;
//	this->minDeviation = parrent->minDeviation;
//	this->SplitPointPercent = parrent->SplitPointPercent;
//	this->chanceToSplit = parrent->chanceToSplit;
//
//	this->CalculateAngle(parrent->angle);
//
//
//	this->itterationsDone = parrent->itterationsDone +1;
//}
//
//void LSystemBranch::CalculateAngle(float angle)
//{
//	if (ID != 0)
//	{
//		angle = (minDeviation + (maxDeviation - minDeviation) * random()) * (ID * 2 -3) + parrent->angle;
//	}
//	else
//	{
//		angle = parrent->angle;
//	}
//}
//
//void LSystemBranch::BranchOff(list<LSystemBranch>* BranchList)
//{
//	if(this->itterationsDone <= this->maxBranches)
//	{
//		child = new LSystemBranch();
//		child->Initialize(this,0,BranchList);
//		BranchList->push_back(*child);
//		child->BranchOff(BranchList);
//		if(random() < chanceToSplit)
//		{
//			child2 = new LSystemBranch();
//			child2->Initialize(this,1,BranchList);
//			BranchList->push_back(*child2);
//			child2->BranchOff(BranchList);
//		}
//		if(random() < chanceToSplit)
//		{
//			child3 = new LSystemBranch();
//			child3->Initialize(this,2,BranchList);
//			BranchList->push_back(*child3);
//			child3->BranchOff(BranchList);
//		}
//
//	}
//}