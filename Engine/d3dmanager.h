////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

/////////////
// LINKING //
/////////////
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

//////////////
// INCLUDES //
//////////////
#pragma once

#include <windows.h>
#include <xnamath.h>
#include <DXGI.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <atlcomcli.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DManager
{
public:
	D3DManager();
	D3DManager(const D3DManager&);
	~D3DManager();

	bool Initialize(HWND hwnd, bool vsync, bool fullscreen, float screenNear, float screenFar, 
		int screenWidth, int screenHeight, UINT shadowMapWidth, UINT shadowMapHeight);
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);
	void GetVideoCardInfo(char*, int&);
	ID3D11DepthStencilView* GetDepthStencilView();
	ID3D11RenderTargetView* GetBackBuffer();
	void SetBackBufferRenderTarget();

	void SetDefaultViewport();
	void SetShadowViewport();

	void TurnZBufferOn();
	void TurnZBufferOff();
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();
	void TurnOnTransparencyBlending();

	void ChangeBlendState(ID3D11BlendState* blendState);
	void TurnOnLightBlending();
	void ResetBlendState();
	void TurnOffColorBlending();
	void TurnOnShadowBlendState();

	void SetBackFaceCullingRasterizer();
	void SetFrontFaceCullingRasterizer();
	void SetNoCullRasterizer();
	void SetWireframeRasterizer();
	void SetDepthBiasRasterizer();
	void ChangeRasterizerState(ID3D11RasterizerState* rasterizerState);
	void ResetRasterizerState();

	void SetLightStencilMethod1Phase1(); //Man, idk, 
	void SetLightStencilMethod1Phase2();

	void SetLightStencilMethod2Phase1();
	void SetLightStencilMethod2Phase2();

	ID3D11ShaderResourceView* GetShadowmapSRV();
	ID3D11DepthStencilView* GetShadowmapDSV();

private:
	bool	vsync_enabled;
	int		videoCardMemory;
	char	videoCardDescription[128];

	//I like doing this for readability.
	CComPtr<IDXGISwapChain>				swapChain;
	CComPtr<ID3D11Device>				device;
	CComPtr<ID3D11DeviceContext>		deviceContext;
	CComPtr<ID3D11RenderTargetView>		renderTargetView;
	D3D11_VIEWPORT						viewport;

	CComPtr<ID3D11Texture2D>			depthStencilBuffer;

	CComPtr<ID3D11DepthStencilState>	depthStencilState;
	CComPtr<ID3D11DepthStencilState>	lightDepthStencilState;
	CComPtr<ID3D11DepthStencilState>	depthDisabledStencilState;

	// http://www.altdevblogaday.com/2011/08/08/stencil-buffer-optimisation-for-deferred-lights/
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod1State1;
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod1State2;

	// http://kayru.org/articles/deferred-stencil/
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod2State1;
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod2State2;

	CComPtr<ID3D11DepthStencilView>		depthStencilView;

	CComPtr<ID3D11RasterizerState>		rasterState;
	CComPtr<ID3D11RasterizerState>		backfaceCullingCW;
	CComPtr<ID3D11RasterizerState>		frontfaceCullingCW;
	CComPtr<ID3D11RasterizerState>		noCullingCW;
	CComPtr<ID3D11RasterizerState>		wireFrameState;
	CComPtr<ID3D11RasterizerState>		depthBiasState;

	XMFLOAT4X4 projectionMatrix, worldMatrix, orthoMatrix;

	CComPtr<ID3D11BlendState> defaultBlendstate;
	CComPtr<ID3D11BlendState> alphaEnableBlendingState;
	CComPtr<ID3D11BlendState> shadowBlendState;
	CComPtr<ID3D11BlendState> alphaDisableBlendingState;
	CComPtr<ID3D11BlendState> lightAddBlendState;
	CComPtr<ID3D11BlendState> colorDisabledBlendState;
	CComPtr<ID3D11BlendState> previousBlendState;
	CComPtr<ID3D11BlendState> transparencyBlendstate;

	//Shadowmapping stuff
	CComPtr<ID3D11Texture2D>			shadowmapTexture;
	CComPtr<ID3D11ShaderResourceView>	shadowmapResourceView;
	CComPtr<ID3D11DepthStencilView>		shadowmapDSV;
	D3D11_VIEWPORT						shadowmapViewport;
};

#endif