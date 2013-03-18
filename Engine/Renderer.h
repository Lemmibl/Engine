////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#pragma once;

#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <xnamath.h>

#include "structs.h"

#include "d3dclass.h"
#include "cameraclass.h"
#include "timerclass.h"
#include "controllerclass.h"
#include "fpsmeter.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "renderToTextureClass.h"
#include "DRGBuffer.h"
#include "frustumclass.h"
#include "debugwindowclass.h"
#include "textureshaderclass.h"
#include "VertexShaderOnly.h"
#include "DepthOnlyShader.h"
#include "DRCompose.h"
#include "DRPointLight.h"
#include "DRDirLight.h"
#include "inputclass.h"
#include "pointlight.h"
#include "modellistclass.h"
#include "modelclass.h"
#include "MarchingCubeShader.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class Renderer
{
public:
	Renderer();
	Renderer(const Renderer&);
	~Renderer();

	bool Initialize(HWND hwnd, CameraClass* camera, InputClass* inputManager, D3DClass* d3D, UINT screenWidth, UINT screenHeight, UINT shadowmapWidth, UINT shadowmapHeight, float screenFar, float screenNear);
	void Shutdown();

	bool Update(int, int, float);
	bool Render();
	bool RenderToTexture(RenderToTextureClass* const);
	bool RenderScene();

private:
	D3DClass* d3D;
	CameraClass* camera;
	InputClass* inputManager;
	TextClass* text;
	FrustumClass* frustum;

	DRGBuffer* gbufferShader;
	DRPointLight* pointLightShader;
	vector<PointLight*> pointLights;

	VertexShaderOnly* vertexOnlyShader;
	DepthOnlyShader* depthOnlyShader;
	DRCompose* composeShader;

	DRDirLight* dirLightShader;
	DirLight* dirLight;

	ModelListClass* modelList;
	ModelClass* groundModel;
	ModelClass* otherModel;
	ModelClass* sphereModel;

	MaterialStruct defaultModelMaterial;

	//Add a bunch of models and init, destruct and render them.

	RenderTarget2D* colorRT; // render target for storing color. 8R 8G 8B 8A. stores specular intensity in alpha value.
	RenderTarget2D* depthRT; // render target for storing depth. it's a 32bit deep single float
	RenderTarget2D* normalRT; //render target for storing normals. 10R 10G 10B 2A. stores specular power in alpha value.
	RenderTarget2D* lightRT; // light map
	RenderTarget2D* shadowRT;

	TextureShaderClass* textureShader;
	DebugWindowClass debugWindows[5];
	DebugWindowClass fullScreenQuad;

	bool toggleDebugInfo;
	bool toggleTextureShader;
	XMFLOAT4X4 baseViewMatrix;
	XMFLOAT4 ambientLight;

	UINT shadowMapWidth, shadowMapHeight, screenWidth, screenHeight;
	float screenFar, screenNear;
	XMFLOAT3 debutRotation;

	float timer;
	bool returning;

	MarchingCubeShader* mcubeShader;
};

#endif