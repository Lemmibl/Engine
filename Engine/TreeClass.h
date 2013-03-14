#pragma once
#include <windows.h>
#include <xnamath.h>
#include <math.h>
#include "customStructs.h"



class TreeClass
{
private:
	int sizeX;
	int sizeY;
	int sizeZ;
	int x,y,z, idx;
	MarchingCubeVertex *marchingCubeVertices2;
public:
	TreeClass(void);

	TreeClass
		(
		int sizeX,
		int sizeY,
		int sizeZ,
		MarchingCubeVertex *marchingCubeVertices
		);

	~TreeClass(void);

	void CalculateLine(XMVECTOR* a, XMVECTOR* b, float size);

	void LSystem();

	float Distance(XMVECTOR* vPoint1, XMVECTOR* vPoint2);


	void LSystemTree();
};