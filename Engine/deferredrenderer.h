//////////////////////////////////////////////////////////////////////////////////
//// Filename: deferredrenderer.h
//////////////////////////////////////////////////////////////////////////////////
//#ifndef _DEFERREDRENDERER_H_
//#define _DEFERREDRENDERER_H_
//
////definitions
//#define PIBy2 1.57079633
//
////user made includes
//#include "graphicsclass.h"
//#include "modelclass.h"
//#include "InterfacePointers.h"
//#include "structs.h"
//
////includes
//#include <vector>
//#include <d3dx9math.h>
//#include <D3DX10math.h>
//
//using std::wstring;
//
////#include "StructureDefinitions.h"
//
//
//class DeferredRenderer
//{
//public:
//	DeferredRenderer();
//	DeferredRenderer(DeferredRenderer&);
//	~DeferredRenderer();
//
//private:
//	DepthStencilBuffer depthBuffer; //Depth target
//	RenderTarget2D normalsTarget; //normal target
//	RenderTarget2D colorTarget; //color target
//	RenderTarget2D lightingTarget; //lightmap target
//	RenderTarget2D shadowTarget; //shadowmap target
//
//	RenderTarget2D deferredOutputTarget; //final target
//
//	// Model
//	Model model;
//	MeshRenderer meshRenderer;
//
//	StructuredBuffer lightsBuffer;
//
//	struct CSConstants
//	{
//		D3DXMATRIX View;
//		D3DXMATRIX Projection;
//		D3DXMATRIX InvViewProjection;
//		D3DXVECTOR3 CameraPosWorldSSpace;
//	};
//
//	CSConstants csConstant;
//
//	virtual void LoadContent();
//	virtual void Render();
//	virtual void Update();
//
//	void CreateRenderTargets();
//	void RenderLightAndShadowMaps();
//
//	void ComputeLightTiles();
//	void RenderScene();
//	void RenderHUD();
//
//private:
//	const UINT WindowWidth = 1280;
//	const UINT WindowHeight = 720;
//	const float WindowWidthF = static_cast<float>(WindowWidth);
//	const float WindowHeightF = static_cast<float>(WindowHeight);
//
//	D3DXVECTOR3 SunColor = D3DXVECTOR3(10.0f, 8.0f, 5.0f);
//	D3DXVECTOR3 SunDirection = D3DXVECTOR3(0.2f, 0.977f, -0.4f);
//
//	static const float NearClip = 0.01f;
//	static const float FarClip = 100.0f;
//
//	static const float ModelScale = 0.01f;
//	D3DXMATRIX scaleMatrix = *D3DXMatrixScaling(&scaleMatrix, ModelScale, ModelScale, ModelScale);
//	D3DXMATRIX rotYMatrix = *D3DXMatrixRotationY(&rotYMatrix, PIBy2);
//	static D3DMATRIX ModelWorldMatrix = scaleMatrix * rotYMatrix;
//};
//
//
//#endif