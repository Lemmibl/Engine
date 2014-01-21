#pragma once
#include <atlcomcli.h>
#include <d3d11.h>


class RasterizerStateManager
{
public:
	RasterizerStateManager();
	~RasterizerStateManager();

	bool Initialize(ID3D11Device* extDevice, ID3D11DeviceContext* extDeviceContext);

	void SetBackFaceCullingRasterizer();
	void SetFrontFaceCullingRasterizer();
	void SetNoCullRasterizer();
	void SetWireframeRasterizer();
	void SetDepthBiasRasterizer();

private:
	ID3D11Device* device; 
	ID3D11DeviceContext* deviceContext;

	CComPtr<ID3D11RasterizerState>		backfaceCullingCW;
	CComPtr<ID3D11RasterizerState>		frontfaceCullingCW;
	CComPtr<ID3D11RasterizerState>		noCullingCW;
	CComPtr<ID3D11RasterizerState>		wireFrameState;
	CComPtr<ID3D11RasterizerState>		depthBiasState;
};

