#include "TreeClass.h"


TreeClass::TreeClass(void)
{
}

TreeClass::TreeClass
	(
	int sizeX,
	int sizeY,
	int sizeZ,
	MarchingCubeVertex *marchingCubeVertices
	)
{

	this->sizeX = sizeX;
	this->sizeY = sizeY;
	this->sizeZ = sizeZ;
	marchingCubeVertices2 = marchingCubeVertices;
}

TreeClass::~TreeClass(void)
{
}



void  TreeClass::LSystemTree()
{


	LSystem();

	//CalculateLine(D3DXVECTOR3(1,-23,1),D3DXVECTOR3(-15,+7,-11), 2.0f);
	CalculateLine(&XMVectorSet(20,20,20, 0.0f), &XMVectorSet(11,-12,11, 0.0f), 2.0f);
	//CalculateLine(D3DXVECTOR3(4,10,-4),D3DXVECTOR3(12,-20,12), 1.5f);
	//CalculateLine(D3DXVECTOR3(-10,-10,-4),D3DXVECTOR3(4,10,-4), 1.5f);
	//CalculateLine(D3DXVECTOR3(10,10,10),D3DXVECTOR3(-10,-10,-4), 1.5f);

}

void TreeClass::CalculateLine(XMVECTOR* a, XMVECTOR* b, float size)
{
	//if (size = 0) {size = 3;}
	int stepsOnTheLine;
	XMVECTOR currentPosOnTheLine = *a;
	XMVECTOR normalizedDirection;
	const XMVECTOR temp = *b - *a;

	normalizedDirection = XMVector3Normalize(temp);

	stepsOnTheLine = (int)(Distance(a,b));


	for(int i = 0; i < stepsOnTheLine; i ++)
	{

		for (z = 1; z < (this->sizeZ - 1); z++)
		{
			for (y = 1; y < (this->sizeY - 1); y++)
			{
				for (x = 1; x < (this->sizeX - 1); x++)
				{
					idx = x + y*this->sizeY + z * this->sizeY * this->sizeZ;

					if(	sqrt((XMVectorGetX(currentPosOnTheLine) - this->marchingCubeVertices2[idx].posX) * (XMVectorGetX(currentPosOnTheLine) - this->marchingCubeVertices2[idx].posX)) < size &&
						sqrt((XMVectorGetY(currentPosOnTheLine) - this->marchingCubeVertices2[idx].posY) * (XMVectorGetY(currentPosOnTheLine) - this->marchingCubeVertices2[idx].posY)) < size &&
						sqrt((XMVectorGetZ(currentPosOnTheLine) - this->marchingCubeVertices2[idx].posZ) * (XMVectorGetZ(currentPosOnTheLine) - this->marchingCubeVertices2[idx].posZ)) < size)
					{

						this->marchingCubeVertices2[idx].density = 0.5f;
					}
					if (this->marchingCubeVertices2[idx].density > 0.02f)
					{
						this->marchingCubeVertices2[idx].inside = true;
					}
					else
					{
						this->marchingCubeVertices2[idx].inside = false;
					}

					this->marchingCubeVertices2[idx].normalX = this->marchingCubeVertices2[idx - 1].density - this->marchingCubeVertices2[idx+1].density;
					this->marchingCubeVertices2[idx].normalY = this->marchingCubeVertices2[idx - this->sizeY].density - this->marchingCubeVertices2[idx + this->sizeY].density;
					this->marchingCubeVertices2[idx].normalZ = this->marchingCubeVertices2[idx - (this->sizeY * this->sizeZ)].density - this->marchingCubeVertices2[idx + (this->sizeY * this->sizeZ)].density; 
				}
			}
		}


		currentPosOnTheLine += normalizedDirection;
	}
}

void TreeClass::LSystem()
{
	XMVECTOR a,b;
	XMVECTOR normalizedDirection;
	float distanceToSplit;
	float distanceFull;

	a = XMVectorSet(0,-40,0,0);
	b = XMVectorSet(0,-1,0,0);

	for(int i=0; i < 0; i++)
	{

		//b = D3DXVECTOR3(0,-10 + 5*i,i*5);

		CalculateLine(&a,&b, 2.0f);
		distanceFull = Distance(&a,&b);
		distanceToSplit = distanceFull * 0.80f;
		
		const XMVECTOR temp = b - a;
		normalizedDirection = XMVector3Normalize(temp);
		a = normalizedDirection * distanceToSplit*0.5f;
		//a = b  + D3DXVECTOR3(15.0f,10.0f,i*10);


		float angle = XM_PI*0.11;
		
		XMMATRIX tempMatrix;
		XMVECTOR tempVector4;
		tempMatrix =	XMMATRIX(	1.0f,	0,				0,			0,
									0,		cos(angle),		sin(angle),	0,
									0,		-sin(angle),	cos(angle),	0,
									0,		0,				0,			1.0f);

		tempVector4 = XMVector3Transform(normalizedDirection,tempMatrix);

		a = b - (XMVector3Transform(normalizedDirection,tempMatrix)*20);

		
		/*normalizedDirection = D3DXVECTOR3(0,0,0);*/



/*
		a = normalizedDirection * distanceToSplit;
		D3DXMATRIX tempMatrix;
		D3DXVECTOR4 tempVector4;
		tempVector4 = D3DXVECTOR4(b,1);
		tempMatrix = D3DXMATRIX(1.0f,0,0,0,
			0,cos(30.0f),sin(30.0f),0,
			0,-sin(30.0f),cos(30.0f),0,
			0,0,0,1.0f);
		D3DXVec3Transform(&tempVector4,&b,&tempMatrix);

		b = D3DXVECTOR3(tempVector4.x,tempVector4.y,tempVector4.z);
*/
	}

}

float TreeClass::Distance(XMVECTOR* vPoint1, XMVECTOR* vPoint2 )
{
	const XMVECTOR Vt = XMVectorSubtract(*vPoint1, *vPoint2);

	float fLength;
	XMStoreFloat(&fLength, XMVector3Length(Vt));

	return fLength;

}