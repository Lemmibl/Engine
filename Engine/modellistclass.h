#pragma once

//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <xnamath.h>
#include <stdlib.h>
#include <time.h>
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Class name: ModelListClass
///////////////////////////////////////////////////////////////////////////////
class ModelListClass
{
private:
	struct ModelInfoType
	{
		XMFLOAT4 color;
		float positionX, positionY, positionZ;
	};

public:
	ModelListClass();
	ModelListClass(const ModelListClass&);
	~ModelListClass();

	bool Initialize(int numModels);
	void Shutdown();

	int GetModelCount();
	void GetData(int, float&, float&, float&, XMFLOAT4&);

private:
	int modelCount;
	std::unique_ptr<ModelInfoType []> modelInfoList;
};