#pragma once

//TODO: Just delete all of these and #include back the ones that are needed...

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
#include "TextureHandler.h"
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
#include "OBJModel.h"

//Shaders
#include "MCGBufferTerrainShader.h"
#include "textureshaderclass.h"
#include "VertexShaderOnly.h"
#include "DepthOnlyShader.h"
#include "DRCompose.h"
#include "DRPointLight.h"
#include "DRObjModelShader.h"
#include "SSAOShader.h"
#include "DRDirectionalLight.h"
#include "fontshaderclass.h"
#include "DepthOnlyQuadShader.h"
#include "GaussianBlur.h"
#include "GeometryShaderGrass.h"
#include "DRWaterClass.h"
#include "DRGBuffer.h"

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

	TextureHandler* GetTextureHandler() { return &textureHandler; }
	MaterialHandler* GetMaterialHandler() { return &materialHandler; }

	void SetCamera(CameraClass* cam)
	{
		camera.reset(cam);
	}

	void SetMeshHandler(MeshHandler* handler)
	{
		meshHandler = handler;
	}

	virtual void OnSettingsReload(Config* cfg);

private:
	bool InitializeShaders(HWND hwnd);
	bool InitializeLights(HWND hwnd);
	bool InitializeEverythingElse(HWND hwnd);
	bool InitializeModels(HWND hwnd);
	bool InitializeDebugText();
	void InitializeRenderingSpecifics();

	//Scene rendering
	bool RenderShadowmap(XMMATRIX* lightWorldViewProj, XMMATRIX* lightWorldView,  XMMATRIX* lightView,  XMMATRIX* lightProj, RenderableBundle* renderableBundle);
	bool RenderTwoPassGaussianBlur(XMMATRIX* worldBaseViewOrthoProj);
	bool RenderGBuffer(XMMATRIX* viewMatrix, XMMATRIX* baseView, XMMATRIX* projectionMatrix, XMMATRIX* identityWorldViewProj, RenderableBundle* renderableBundle);

	//Light rendering
	bool RenderDirectionalLight(DRDirLight::DirectionalLightInput& input);
	bool RenderPointLight(XMMATRIX* view, XMMATRIX* invertedView, XMMATRIX* invertedProjection, XMMATRIX* viewProjection);

	//Post processing
	bool RenderSSAO(SSAOShader::SSAOShaderInput& input);
	bool RenderComposedScene(DRCompose::ComposeShaderInput& input);
	bool RenderGUI(XMMATRIX* worldBaseViewOrthoProj);

private:
	//Flag to make sure we don't initialize over a bunch of shit
	bool previouslyInitialized;

	/************************************************************************/
	/* Various managers, handlers and system pointers                       */
	/************************************************************************/
	ID3D11DeviceContext* deviceContext;
	ID3D11Device* device;

	std::shared_ptr<D3DManager> d3D;
	std::shared_ptr<CameraClass> camera;
	std::shared_ptr<InputClass> inputManager;
	MeshHandler* meshHandler;

	TextureCreator textureCreator;
	TextureHandler textureHandler;
	MaterialHandler materialHandler;
	ProceduralTextureHandler proceduralTextureHandler;	
	DayNightCycle dayNightCycle;
	NoiseClass noise;
	Utility utility;

	/************************************************************************/
	/* Render targets and necessary rendering states and objects            */
	/************************************************************************/
	// render target for storing color. R8G8B8A8. Stores a... lerp value in alpha channel, used in case I want to lerp between two materials in lighting stage.
	RenderTarget2D colorRT;
	
	// render target for storing depth. it's a R32
	RenderTarget2D depthRT;
	
	//render target for storing normals. R8G8B8A8. 
	// I use a compression algorithm for my normals, so only the .xy channels are used when storing normals. the .zw channels are used for material IDs
	RenderTarget2D normalRT; 

	//light rendertarget. R8G8B8A8
	RenderTarget2D lightRT;
	
	//Shadow map. it's a R16 G16 because we use variance shadowmapping 
	RenderTarget2D shadowRT; 

	//Used for blurring shadow map. this one is also R16G16 because it has to mirror shadowRT
	RenderTarget2D gaussianBlurPingPongRT; 

	//R32, pretty much identical to depth RT in every way
	RenderTarget2D ssaoRT; 

	 //For blurring ssao map. I can't reuse the gaussianblur pingpongRT because that one is R16G16, this one will be R32
	RenderTarget2D ssaoPingPongRT;

	CComPtr<ID3D11RenderTargetView> gbufferRenderTargets[3]; //render targets for GBuffer pass
	CComPtr<ID3D11RenderTargetView> lightTarget[1];
	CComPtr<ID3D11RenderTargetView> shadowTarget[1];
	CComPtr<ID3D11RenderTargetView> gaussianBlurPingPongRTView[1];
	CComPtr<ID3D11RenderTargetView> ssaoRTView[1];

	CComPtr<ID3D11ShaderResourceView> gbufferTextures[3];
	CComPtr<ID3D11ShaderResourceView> dirLightTextures[4];
	CComPtr<ID3D11ShaderResourceView> finalTextures[4];
	CComPtr<ID3D11ShaderResourceView> gaussianBlurTexture[1];
	CComPtr<ID3D11ShaderResourceView> lightMap;

	//0 == depth, 1 == normal, 2 == random vectors
	CComPtr<ID3D11ShaderResourceView> ssaoView;
	CComPtr<ID3D11ShaderResourceView> ssaoTextures[3];

	CComPtr<ID3D11DepthStencilView> shadowDepthStencil;
	CComPtr<ID3D11DepthStencilView> depthStencil;

	/************************************************************************/
	/* Shaders                                                              */
	/************************************************************************/
	VertexShaderOnly vertexOnlyShader;
	DepthOnlyShader depthOnlyShader;
	DRGBuffer gbufferShader;
	DRCompose composeShader;
	GaussianBlur gaussianBlurShader;
	GeometryShaderGrass geometryShaderGrass;
	DRWaterClass waterShader;
	MCGBufferTerrainShader mcubeShader;
	DRObjModelShader objModelShader;
	SSAOShader ssaoShader;

	/************************************************************************/
	/* Shader input objects                                                 */
	/************************************************************************/
	DRDirLight::DirectionalLightInput dirLightInput;
	DRCompose::ComposeShaderInput composeInput;
	SSAOShader::SSAOShaderInput ssaoInput;

	/************************************************************************/
	/* Light and models                                                     */
	/************************************************************************/
	DebugWindowClass fullScreenQuad;

	std::vector<XMFLOAT4X4> treeMatrices;
	int numtrees;

	DRPointLight pointLightShader;
	std::vector<PointLight> pointLights;

	DRDirLight dirLightShader;
	DirLight dirLight;

	ModelClass sphereModel;
	Skysphere skySphere;

	/************************************************************************/
	/* Debug data                                                           */
	/************************************************************************/
	TextureShaderClass textureShader;
	DebugWindowClass debugWindows[7];

	DebugOverlayHUD* debugHUD;
	std::vector<DebugWindowHandle> debugWindowHandles;

	/************************************************************************/
	/* Miscellaneous                                                        */
	/************************************************************************/
	XMFLOAT4X4 baseViewMatrix;
	XMFLOAT3 camPos, camDir, windDir;
	UINT shadowMapWidth, shadowMapHeight, screenWidth, screenHeight;
	int toggleSSAO, toggleColorMode;
	float xPos, yPos, textureOffsetDeltaTime, fogMinimum, farClip, nearClip, timer, timeOfDay;;
	bool returning, toggleDebugInfo, toggleTextureShader, toggleOtherPointLights, drawWireFrame;
};