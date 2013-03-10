////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_
#pragma once

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
#include <DXGI.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <xnamath.h>
#include <windows.h>
#include <xnamath.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

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

	void ChangeBlendState(ID3D11BlendState* blendState);
	void TurnOnLightBlending();
	void ResetBlendState();
	void TurnOffColorBlending();

	void SetBackFaceCullingRasterizer();
	void SetFrontFaceCullingRasterizer();
	void SetNoCullRasterizer();
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
	IDXGISwapChain*				swapChain;
	ID3D11Device*				device;
	ID3D11DeviceContext*		deviceContext;
	ID3D11RenderTargetView*		renderTargetView;
	D3D11_VIEWPORT				viewport;

	ID3D11Texture2D*			depthStencilBuffer;

	ID3D11DepthStencilState*	depthStencilState;
	ID3D11DepthStencilState*	lightDepthStencilState;
	ID3D11DepthStencilState*	depthDisabledStencilState;

	// http://www.altdevblogaday.com/2011/08/08/stencil-buffer-optimisation-for-deferred-lights/
	ID3D11DepthStencilState*	pointLightStencilMethod1State1;
	ID3D11DepthStencilState*	pointLightStencilMethod1State2;

	// http://kayru.org/articles/deferred-stencil/
	ID3D11DepthStencilState*	pointLightStencilMethod2State1;
	ID3D11DepthStencilState*	pointLightStencilMethod2State2;

	ID3D11DepthStencilView*		depthStencilView;

	ID3D11RasterizerState*		rasterState;
	ID3D11RasterizerState*		backfaceCullingCW;
	ID3D11RasterizerState*		frontfaceCullingCW;
	ID3D11RasterizerState*		noCullingCW;

	XMMATRIX projectionMatrix;
	XMMATRIX worldMatrix;
	XMMATRIX orthoMatrix;

	ID3D11BlendState* alphaEnableBlendingState;
	ID3D11BlendState* alphaDisableBlendingState;
	ID3D11BlendState* lightAddBlendState;
	ID3D11BlendState* colorDisabledBlendState;
	ID3D11BlendState* previousBlendState;

	//Shadowmapping stuff
	ID3D11Texture2D*			shadowmapTexture;
	ID3D11ShaderResourceView*	shadowmapResourceView;
	ID3D11DepthStencilView*		shadowmapDSV;
	D3D11_VIEWPORT				shadowmapViewport;
};

#endif