#pragma once
#include <d3d11.h>
#include <windows.h>
#include <xnamath.h>
#include <fstream>
#include "SkysphereShader.h"
#include <atlcomcli.h>
#include <memory>
#include <vector>

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
	void Render(ID3D11DeviceContext* context, XMMATRIX* worldViewProjection, float cameraYPos, XMFLOAT4* fogColor, float time);

	int GetIndexCount();

	//Top of the skysphere.
	XMFLOAT4 GetApexColor();
	XMFLOAT4 GetCenterColor();

	void SetApexColor(XMFLOAT4 val);
	void SetCenterColor(XMFLOAT4 val);

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
};