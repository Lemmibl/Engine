#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <fstream>
#include "SkysphereShader.h"
#include <atlcomcli.h>
#include <memory>
#include <vector>
#include "StructsAndEnums.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Skysphere
////////////////////////////////////////////////////////////////////////////////
class Skysphere
{
private:
	struct ModelType
	{
		//alpha channel contains face ID
		XMFLOAT3 position;
		XMFLOAT3 texcoords;
		XMFLOAT3 normal;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 texcoords;
		XMFLOAT3 normal;
	};

public:
	Skysphere();
	~Skysphere();

	bool Initialize(ID3D11Device* device, HWND hwnd);
	void Shutdown();
	void Render(ID3D11DeviceContext* context, XMMATRIX* world, XMMATRIX* worldViewProjection, float cameraYPos, XMFLOAT4* fogColor, float time, StageOfDay stageOfDay, float lightIntensity);

	int GetIndexCount();

	//Top of the skysphere.
	XMFLOAT4 GetApexColor();
	XMFLOAT4 GetCenterColor();

	void SetApexColor(XMFLOAT4 val);
	void SetCenterColor(XMFLOAT4 val);

	ID3D11ShaderResourceView** GetCloudTexturePP() { return &cloudTexture.p; }
	ID3D11ShaderResourceView** GetStarTexturePP() { return &starTexture.p; }

private:
	bool LoadModel(char*);
	void ReleaseModel();

	bool InitializeBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	std::unique_ptr<ModelType[]> model;
	int vertexCount, indexCount;
	CComPtr<ID3D11Buffer> vertexBuffer;
	CComPtr<ID3D11Buffer> indexBuffer;
	XMFLOAT4 apexColor, centerColor;
	SkysphereShader skysphereShader;

	CComPtr<ID3D11ShaderResourceView> cloudTexture;
	CComPtr<ID3D11ShaderResourceView> starTexture;
};