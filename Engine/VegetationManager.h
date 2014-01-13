#pragma once
#include <vector>
#include <windows.h>
#include <xnamath.h>
#include "VegetationShader.h"
#include "textureclass.h"
#include <atlcomcli.h>
#include <memory>
#include "IndexedInstancedMesh.h"

class VegetationManager
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
	};

public:
	//This actually contains a float3 position and alpha channel contains texture ID for each instance
	struct InstanceType
	{
		XMFLOAT4 position;
		float randomValue;
	};

public:
	VegetationManager();
	VegetationManager(const VegetationManager&);
	~VegetationManager();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool SetupQuads(ID3D11Device* device, std::vector<InstanceType>* positions);
	bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* World, ID3D11ShaderResourceView** textures);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	int GetIndexCount()		{	return mesh.GetIndexCount();		};
	int GetVertexCount()	{	return mesh.GetVertexCount();		};
	int GetInstanceCount()	{	return mesh.GetInstanceCount();		};

	bool BuildInstanceBuffer(ID3D11Device* device, std::vector<InstanceType>* positions);

private:
	bool BuildVertexAndIndexBuffers(ID3D11Device* device);
	XMFLOAT3 CalculateVertexNormals(XMFLOAT3* v1, XMFLOAT3* v2, XMFLOAT3* v3);

private:
	IndexedInstancedMesh mesh;
	std::unique_ptr<VegetationShader> vegetationShader;
};

/*
	Manager for rendering things like grass and bush quads. Possibly long distance tree quads.

	Initialize should take in at least two textures: grass and bushes.... maybe trees/flowers, too.

	You have a SetupQuads function that takes in a vector of float3 positions and generates an array of structs that we turn into a VBO.

	Then a render function where we actually commit the VBO to the geometry shader for rendering.
*/