/////////////
// LINKING //
/////////////
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#pragma once

#include <windows.h>
#include <xnamath.h>
#include <DXGI.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <atlcomcli.h>
#include "BlendStateManager.h"
#include "RasterizerStateManager.h"
#include "DepthStencilManager.h"
#include "ShadowManager.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: D3DClass
////////////////////////////////////////////////////////////////////////////////
class D3DManager
{
public:
	D3DManager();
	~D3DManager();

	bool Initialize(HWND hwnd, bool vsync, bool fullscreen, float screenNear, float screenFar, 
		int screenWidth, int screenHeight, UINT shadowMapWidth, UINT shadowMapHeight);
	
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void PresentFrame();

	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetDeviceContext();

	void GetVideoCardInfo(char*, int&);
	//ID3D11RenderTargetView* GetBackBuffer();

	void ResetBackBufferRenderTarget();
	void SetDefaultViewport();

	BlendStateManager*			GetBlendStateManager()			{ return &blendStateManager; }
	RasterizerStateManager*		GetRasterizerStateManager()		{ return &rasterizerStateManager; }
	DepthStencilManager*		GetDepthStencilManager()		{ return &depthStencilManager; }
	ShadowManager*				GetShadowManager()				{ return &shadowManager; }
	HWND GetHwnd() { return hwnd; }

private:
	bool	vsync_enabled;
	int		videoCardMemory;
	char	videoCardDescription[128];

	CComPtr<IDXGISwapChain>				swapChain;
	CComPtr<ID3D11Device>				device;
	CComPtr<ID3D11DeviceContext>		deviceContext;
	CComPtr<ID3D11RenderTargetView>		renderTargetView;
	D3D11_VIEWPORT						viewport;

	HWND hwnd;

	BlendStateManager blendStateManager;
	RasterizerStateManager rasterizerStateManager;
	DepthStencilManager depthStencilManager;
	ShadowManager shadowManager;
};