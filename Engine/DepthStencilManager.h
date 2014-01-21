#pragma once
#include <d3d11.h>
#include <atlcomcli.h>

class DepthStencilManager
{
public:
	DepthStencilManager();
	~DepthStencilManager();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext,  unsigned int screenWidth, unsigned int screenHeight);

	ID3D11DepthStencilView* GetDepthStencilView();

	void SetScreenWidth(unsigned int width) { screenWidth = width; }
	void SetScreenHeight(unsigned int height) { screenHeight = height; }

	void SetDefaultDepthStencilView();
	void SetDepthDisabledStencilState();

	void SetLightStencilMethod1Phase1(); //Man, idk, 
	void SetLightStencilMethod1Phase2();

	void SetLightStencilMethod2Phase1();
	void SetLightStencilMethod2Phase2();

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	unsigned int screenWidth, screenHeight;

	CComPtr<ID3D11Texture2D>			depthStencilBuffer;
	CComPtr<ID3D11DepthStencilState>	depthStencilState;
	CComPtr<ID3D11DepthStencilView>		depthStencilView;

	CComPtr<ID3D11DepthStencilState>	lightDepthStencilState;
	CComPtr<ID3D11DepthStencilState>	depthDisabledStencilState;

	// http://www.altdevblogaday.com/2011/08/08/stencil-buffer-optimisation-for-deferred-lights/
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod1State1;
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod1State2;

	// http://kayru.org/articles/deferred-stencil/
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod2State1;
	CComPtr<ID3D11DepthStencilState>	pointLightStencilMethod2State2;
};

