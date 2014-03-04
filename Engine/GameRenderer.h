#pragma once

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
#include "timerclass.h"
#include "controllerclass.h"
#include "fpsmeter.h"
#include "textclass.h"
#include "renderToTextureClass.h"
#include "DayNightCycle.h"
#include "SettingsDependent.h"
#include "DebugOverlayHUD.h"

//Managers
#include "d3dmanager.h"
#include "TerrainManager.h"
#include "TextureAndMaterialHandler.h"
#include "VegetationManager.h"
#include "MeshHandler.h"

//Objects
#include "modelclass.h"
#include "cameraclass.h"
#include "frustumclass.h"
#include "debugwindowclass.h"
#include "SkySphere.h"
#include "marchingCubesClass.h"
#include "TerrainNoiseSeeder.h"
#include "LSystemClass.h"
#include "NoiseClass.h"
#include "IndexedMesh.h"

//Shaders
#include "MCGBufferTerrainShader.h"
#include "textureshaderclass.h"
#include "VertexShaderOnly.h"
#include "DepthOnlyShader.h"
#include "DRCompose.h"
#include "DRPointLight.h"
#include "DRDirectionalLight.h"
#include "fontshaderclass.h"
#include "DepthOnlyQuadShader.h"
#include "GaussianBlur.h"
#include "GeometryShaderGrass.h"
#include "DRWaterClass.h"
#include "DRGBuffer.h"
#include "OBJModel.h"
#include "DRObjModelShader.h"

class GameRenderer : public SettingsDependent
{
public:

	struct TestSphere
	{
		IndexedMesh mesh;
		XMFLOAT4X4 world;
	};

	struct RenderableBundle
	{
		std::vector<IndexedMesh> indexedMeshes;
		//std::vector<IndexedInstancedMesh> indexedInstancedMeshes;
		std::vector<OBJModel> objModels;

		std::vector<MarchingCubeChunk*> terrainChunks;
		TestSphere testSphere; //temp.
	};

public:
	GameRenderer();
	~GameRenderer();

	void Shutdown();

	bool Initialize(HWND hwnd, std::shared_ptr<CameraClass> camera, std::shared_ptr<InputClass> inputManager, std::shared_ptr<D3DManager> d3D, MeshHandler* meshHandler, DebugOverlayHUD* debugHUD, 
		UINT screenWidth, UINT screenHeight, UINT shadowmapWidth, UINT shadowmapHeight, float screenFar, float screenNear);

	bool Update(HWND hwnd, int fps, int cpuPercentage, float millisecondDeltaTime, float secondDeltaTime, XMFLOAT3* windDirection);
	bool Render(HWND hwnd, RenderableBundle* renderableBundle);

	TextureAndMaterialHandler* GetTextureAndMaterialHandler() { return &textureAndMaterialHandler; }

	virtual void OnSettingsReload(Config* cfg);

private:
	bool InitializeShaders(HWND hwnd);
	bool InitializeLights(HWND hwnd);
	bool InitializeEverythingElse(HWND hwnd);
	bool InitializeModels(HWND hwnd);
	bool InitializeDebugText();
	void InitializeRenderingSpecifics();

	bool RenderShadowmap(XMMATRIX* lightWorldViewProj, XMMATRIX* lightWorldView, RenderableBundle* renderableBundle);
	bool RenderTwoPassGaussianBlur(XMMATRIX* worldBaseViewOrthoProj);
	bool RenderGBuffer(XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix, XMMATRIX* identityWorldViewProj, RenderableBundle* renderableBundle);

	//bool RenderTerrain();
	//bool RenderVegetation();

	bool RenderDirectionalLight(XMMATRIX* viewMatrix, XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* lightView, XMMATRIX* lightProj, XMMATRIX* invertedProjection);
	bool RenderPointLight(XMMATRIX* view, XMMATRIX* invertedView, XMMATRIX* invertedProjection, XMMATRIX* viewProjection);
	bool RenderComposedScene(XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invertedViewProjection);
	bool RenderGUI(XMMATRIX* worldBaseViewOrthoProj);

private:
	ID3D11DeviceContext* deviceContext;
	ID3D11Device* device;

	std::shared_ptr<D3DManager> d3D;
	std::shared_ptr<CameraClass> camera;
	std::shared_ptr<InputClass> inputManager;
	MeshHandler* meshHandler;

	std::vector<XMFLOAT4X4> treeMatrices;
	int numtrees;

	DebugOverlayHUD* debugHUD;
	std::vector<DebugWindowHandle> debugWindowHandles;

	DRPointLight pointLightShader;
	std::vector<PointLight> pointLights;

	VertexShaderOnly vertexOnlyShader;
	DepthOnlyShader depthOnlyShader;
	DRGBuffer gbufferShader;
	DRCompose composeShader;
	GaussianBlur gaussianBlurShader;
	GeometryShaderGrass geometryShaderGrass;
	DRWaterClass waterShader;
	MCGBufferTerrainShader mcubeShader;
	DRObjModelShader objModelShader;

	DRDirLight dirLightShader;
	DirLight dirLight;

	ModelClass sphereModel;
	Skysphere skySphere;

	TextureShaderClass textureShader;
	DebugWindowClass debugWindows[7];
	DebugWindowClass fullScreenQuad;

	XMFLOAT4X4 baseViewMatrix;

	UINT shadowMapWidth, shadowMapHeight, screenWidth, screenHeight;
	float farClip, nearClip, timer, timeOfDay;

	bool returning, toggleDebugInfo, toggleTextureShader, toggleOtherPointLights, drawWireFrame;
	float fogMinimum;

	DayNightCycle dayNightCycle;
	NoiseClass noise;

	Utility utility;
	TextureAndMaterialHandler textureAndMaterialHandler;

	int toggleSSAO, toggleColorMode;
	float xPos, yPos, textureOffsetDeltaTime;

	XMFLOAT3 camPos, camDir, windDir;

	RenderTarget2D colorRT; // render target for storing color. 8R 8G 8B 8A. stores specular intensity in alpha value.
	RenderTarget2D depthRT; // render target for storing depth. it's a R16 G16 because we use variance shadowmapping
	RenderTarget2D normalRT; //render target for storing normals. 8R 8G 8B 8A. stores specular power in alpha value.
	RenderTarget2D lightRT; // light rendertarget
	RenderTarget2D shadowRT; //Shadow map
	RenderTarget2D gaussianBlurPingPongRT; //Used for blurring shadow map

	//http://stackoverflow.com/questions/2319224/dynamic-array-of-com-objects
	//http://369o.com/data/books/atl/0321159624/ch03lev1sec4.html

	CComPtr<ID3D11RenderTargetView> gbufferRenderTargets[3]; //render targets for GBuffer pass
	CComPtr<ID3D11RenderTargetView> lightTarget[1];
	CComPtr<ID3D11RenderTargetView> shadowTarget[1];
	CComPtr<ID3D11RenderTargetView> gaussianBlurPingPongRTView[1];

	CComPtr<ID3D11ShaderResourceView> gbufferTextures[3];
	CComPtr<ID3D11ShaderResourceView> dirLightTextures[4];
	CComPtr<ID3D11ShaderResourceView> finalTextures[4];
	CComPtr<ID3D11ShaderResourceView> gaussianBlurTexture[1];
	CComPtr<ID3D11ShaderResourceView> lightMap;

	CComPtr<ID3D11DepthStencilView> shadowDepthStencil;
	CComPtr<ID3D11DepthStencilView> depthStencil;
};