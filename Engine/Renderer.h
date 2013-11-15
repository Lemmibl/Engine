////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////

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
#include <cassert>

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

	bool InitializeShaders(HWND hwnd);
	bool InitializeLights(HWND hwnd);
	bool InitializeEverythingElse(HWND hwnd);
	bool InitializeModels(HWND hwnd);
	void Shutdown();

	bool Update(HWND hwnd, int, int, float, float seconds);
	bool Render(HWND hwnd);

	bool RenderShadowmap(ID3D11DeviceContext* deviceContext, XMMATRIX* lightWorldViewProj, XMMATRIX* lightWorldView);
	bool RenderTwoPassGaussianBlur(ID3D11DeviceContext* deviceContext, XMMATRIX* worldBaseViewOrthoProj);
	bool RenderGBuffer(ID3D11DeviceContext* deviceContext, XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix, XMMATRIX* identityWorldViewProj);
	bool RenderDirectionalLight(ID3D11DeviceContext* deviceContext, XMMATRIX* viewMatrix, XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* lightView, XMMATRIX* lightProj, XMMATRIX* invertedProjection);
	bool RenderPointLight(ID3D11DeviceContext* deviceContext, XMMATRIX* view, XMMATRIX* invertedView, XMMATRIX* viewProjection);
	bool RenderComposedScene(ID3D11DeviceContext* deviceContext, XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invertedViewProjection);
	bool RenderDebugInfoAndText(ID3D11DeviceContext* deviceContext, XMMATRIX* worldBaseViewOrthoProj);

	//And the winner for worst parameter name 2013 goes to.....
	void GenerateVegetation(ID3D11Device* device, bool IfSetupThenTrue_IfUpdateThenFalse);

private:
	D3DClass* d3D;
	CameraClass* camera;
	InputClass* inputManager;
	TextClass* text;
	FrustumClass* frustum;

	DRPointLight* pointLightShader;
	vector<PointLight> pointLights;

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

	bool returning, toggleDebugInfo, toggleTextureShader, toggleOtherPointLights;
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
	vector<VegetationManager::InstanceType> LODVector500;
	vector<VegetationManager::InstanceType> LODVector2500;
	vector<VegetationManager::InstanceType> LODVector5000;
	vector<VegetationManager::InstanceType> LODVector10000;
	int lodState, previousLodState, toggleSSAO, toggleColorMode;

	Lemmi2DAABB testBoundingbox;

	/************************************************************************/
	/* Actual rendering related variables                                   */
	/************************************************************************/
	XMFLOAT3 camPos, camDir;
	ID3D11RenderTargetView* gbufferRenderTargets[3]; //render targets for GBuffer pass
	ID3D11RenderTargetView* lightTarget[1];
	ID3D11RenderTargetView* shadowTarget[1];
	ID3D11RenderTargetView* gaussianBlurPingPongRTView[1];

	ID3D11ShaderResourceView* gbufferTextures[3];
	ID3D11ShaderResourceView* dirLightTextures[4];
	ID3D11ShaderResourceView* finalTextures[4];
	ID3D11ShaderResourceView* gaussianBlurTexture[1];
	ID3D11ShaderResourceView* lightMap;

	ID3D11DepthStencilView* shadowDepthStencil;
	ID3D11DepthStencilView* depthStencil; //We set it later on when we need it. Calling d3D->GetDepthStencilView() also calls a reset on DS settings to default, hence we wait with calling it.

	/************************************************************************/
	/* End of rendering related variables									*/
	/************************************************************************/
};