////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

/////////////
// GLOBALS //
/////////////
const bool SHOW_CURSOR = true;
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_FAR = 100.0f;
const float SCREEN_NEAR = 0.5f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#pragma once;

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "structs.h"

#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "colorshaderclass.h"
#include "timerclass.h"
#include "controllerclass.h"
#include "fpsmeter.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "renderToTextureClass.h"
#include "terrainshaderclass.h"
#include "DRGBuffer.h"
#include "frustumclass.h"
#include "quadtreeclass.h"
#include "debugwindowclass.h"
#include "textureshaderclass.h"
#include "VertexShaderOnly.h"
#include "DepthOnlyShader.h"
#include "DRFinalComposition.h"
#include "DRPointLight.h"
#include "DRDirLight.h"

#include "pointlight.h"
#include "modellistclass.h"
#include "modelclass.h"
#include "marchingCubesClass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();

	bool Frame(int, int, float, bool, bool, bool);
	bool Render();
	bool RenderToTexture(RenderToTextureClass* const);
	bool RenderScene();

	CameraClass* GetCamera();

private:
	D3DClass* d3D;
	CameraClass* camera;
	//TerrainClass* terrain;
	ColorShaderClass* colorShader;
	//TerrainShaderClass* terrainShader;
	TextClass* text;

	FrustumClass* frustum;
	//QuadTreeClass* quadTree;

	DRGBuffer* gbufferShader;
	DRPointLight* pointLightShader;
	vector<PointLight*> pointLights;

	VertexShaderOnly* vertexOnlyShader;
	DepthOnlyShader* depthOnlyShader;
	DRFinalComposition* composeShader;

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

	TextureShaderClass* textureShader;
	DebugWindowClass debugWindows[5];
	DebugWindowClass fullScreenQuad;

	bool toggleDebugInfo;
	bool toggleTextureShader;
	D3DXMATRIX baseViewMatrix, scale, translation;
	D3DXVECTOR4 ambientLight;

	UINT shadowMapWidth, shadowMapHeight;


	marchingCubesClass* marchingCubes;
	metaballsClass* metaBalls;
};

#endif