////////////////////////////////////////////////////////////////////////////////
// Filename: SkySphere.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SKYSPHERE_H_
#define _SKYSPHERE_H_


//////////////
// INCLUDES //
//////////////
#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <fstream>
#include "SkysphereShader.h"
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: Skysphere
////////////////////////////////////////////////////////////////////////////////
class Skysphere
{
private:
	struct ModelType
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoords;
		XMFLOAT3 normal;
	};

	struct VertexType
	{
		XMFLOAT3 position;
	};

public:
	Skysphere();
	Skysphere(const Skysphere&);
	~Skysphere();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	void Render(ID3D11DeviceContext* context, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection, float time);

	int GetIndexCount();

	//Top of the skysphere.
	XMFLOAT4 GetApexColor();

	XMFLOAT4 GetCenterColor();

	//Bottom of the skysphere; opposite of apex.
	XMFLOAT4 GetAntapexColor();

private:
	bool LoadModel(char*);
	void ReleaseModel();

	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ModelType* model;
	int vertexCount, indexCount;
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	XMFLOAT4 apexColor, centerColor, antapexColor;
	SkysphereShader* skysphereShader;
};

#endif
