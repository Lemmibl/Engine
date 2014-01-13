//#pragma once
//#include <vector>
//#include <windows.h>
//#include <xnamath.h>
//#include <list>
//
//class LSystemBranch
//{
//public:
//	LSystemBranch(void);
//	~LSystemBranch(void);
//
//	void Initialize
//		(
//		LSystemBranch* parrent,
//		unsigned short ID,
//		list<LSystemBranch>* BranchList
//		);
//
//	void InitializeFirst
//		(
//		XMFLOAT2 positionStart,
//		XMFLOAT2 positionEnd,
//		float legth,
//		float lengthFallof,
//		float width,
//		float widthFallof,
//		int maxBranches,
//		int maxBranchesPerSplit,
//		float maxDeviation,
//		float minDeviation,
//		float SplitPointPercent,
//		float chanceToSplit,
//		float angle,
//		list<LSystemBranch>* BranchList
//		);
//
//	void CalculateAngle(float angle);
//
//	void BranchOff(list<LSystemBranch>* BranchList);
//
//
//
//	inline float random()
//	{
//		float scale=RAND_MAX+1.0f;
//		float base=rand()/scale;
//		float fine=rand()/scale;
//		return base+fine/scale;
//	}
//
//public:
//	XMFLOAT2 positionStart, positionEnd;
//	float length;
//	float lengthFallof;
//	float width;
//	float widthFallof;
//	int itterationsDone;
//	int maxBranches;
//	int maxBranchesPerSplit;
//	float maxDeviation;
//	float minDeviation;
//	float SplitPointPercent;
//	float chanceToSplit;
//	float angle;
//	unsigned short ID;
//	LSystemBranch* parrent;
//	LSystemBranch* child;
//	LSystemBranch* child2;
//	LSystemBranch* child3;
//
//	list<LSystemBranch>* BranchList;
//};
//
