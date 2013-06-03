////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#pragma once;

//System includes
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <xnamath.h>
#include <ctime>
#include <sstream>
#include <iosfwd>
#include <string>

//Utility
#include "Utility.h"
#include "StructsAndEnums.h"
#include "inputclass.h"
#include "d3dclass.h"
#include "timerclass.h"
#include "controllerclass.h"
#include "fpsmeter.h"
#include "textclass.h"
#include "renderToTextureClass.h"
#include "DayNightCycle.h"
#include "VegetationManager.h"
#include "TextureAndMaterialHandler.h"

//Objects
#include "modelclass.h"
#include "pointlight.h"
#include "cameraclass.h"
#include "frustumclass.h"
#include "debugwindowclass.h"
#include "SkySphere.h"
#include "marchingCubesClass.h"
#include "MCTerrainClass.h"
#include "LSystemClass.h"
#include "SimplexNoise.h"

//Shaders
#include "MCubesGBufferShader.h"
#include "textureshaderclass.h"
#include "VertexShaderOnly.h"
#include "DepthOnlyShader.h"
#include "DRCompose.h"
#include "DRPointLight.h"
#include "DRDirectionalLight.h"
#include "fontshaderclass.h"
#include "DRGBuffer.h"
#include "DepthOnlyQuadShader.h"
#include "GaussianBlur.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class Renderer
{
public:
	Renderer();
	Renderer(const Renderer&);
	~Renderer();

	bool Initialize(HWND hwnd, CameraClass* camera, InputClass* inputManager, D3DClass* d3D, UINT screenWidth, 
		UINT screenHeight, UINT shadowmapWidth, UINT shadowmapHeight, float screenFar, float screenNear, bool toggleDebug);
	bool InitializeShaders(HWND hwnd, ID3D11Device* device);
	bool InitializeLights(HWND hwnd, ID3D11Device* device);
	bool InitializeEverythingElse(HWND hwnd, ID3D11Device* device);
	bool InitializeModels(HWND hwnd, ID3D11Device* device);
	void Shutdown();

	bool Update(HWND hwnd, int, int, float, float seconds);
	bool Render();
	bool RenderToTexture(RenderToTextureClass* const);
	bool RenderScene();

	//And the winner for worst parameter name 2013 goes to.....
	void GenerateVegetation(ID3D11Device* device, bool IfSetupThenTrue_IfUpdateThenFalse);

private:
	D3DClass* d3D;
	CameraClass* camera;
	InputClass* inputManager;
	TextClass* text;
	FrustumClass* frustum;

	DRGBuffer* gbufferShader;
	DRPointLight* pointLightShader;
	vector<PointLight*> pointLights;
	PointLight cameraPointLight;

	VertexShaderOnly* vertexOnlyShader;
	DepthOnlyShader* depthOnlyShader;
	DepthOnlyQuadShader* depthOnlyQuadShader;
	DRCompose* composeShader;
	GaussianBlur* gaussianBlurShader;

	DRDirLight* dirLightShader;
	DirLight* dirLight;

	ModelClass* sphereModel;
	Skysphere* skySphere;
	VegetationManager* vegetationManager;

	RenderTarget2D* colorRT; // render target for storing color. 8R 8G 8B 8A. stores specular intensity in alpha value.
	RenderTarget2D* depthRT; // render target for storing depth. it's a R16 G16 because we use variance shadowmapping
	RenderTarget2D* normalRT; //render target for storing normals. 8R 8G 8B 8A. stores specular power in alpha value.
	RenderTarget2D* lightRT; // light rendertarget
	RenderTarget2D* shadowRT; //Shadow map
	RenderTarget2D* gaussianBlurPingPongRT; //Used for blurring shadow map

	TextureShaderClass* textureShader;
	DebugWindowClass debugWindows[6];
	DebugWindowClass fullScreenQuad;

	XMFLOAT4X4 baseViewMatrix;

	UINT shadowMapWidth, shadowMapHeight, screenWidth, screenHeight;
	float screenFar, screenNear, timer, timeOfDay;

	bool returning, toggleDebugInfo, toggleTextureShader, toggleCameraPointLight;
	float fogMinimum;
	
	MetaballsClass* metaBalls;
	MCubesGBufferShader* mcubeShader;

	DayNightCycle* dayNightCycle;
	MarchingCubesClass* marchingCubes;
	MCTerrainClass* mcTerrain;

	LSystemClass* lSystem;
	SimplexNoise *noise;

	Utility* utility;
	TextureAndMaterialHandler* textureAndMaterialHandler;

	//These should be temporary. Just used for testing my little LOD system.
	vector<XMFLOAT4> LODVector500;
	vector<XMFLOAT4> LODVector2500;
	vector<XMFLOAT4> LODVector5000;
	vector<XMFLOAT4> LODVector10000;
	int lodState, previousLodState, toggleSSAO, toggleColorMode;
	ID3D11ShaderResourceView* lSystemSRV;
	ID3D11ShaderResourceView* ssaoRandomTextureSRV;

	Lemmi2DAABB testBoundingbox;
};

#endif