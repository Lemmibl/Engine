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
#include <memory>
#include <vector>

//Utility
#include "SettingsDependent.h"
#include "Utility.h"

//Managers
class CameraClass;
class D3DManager;
class InputClass;

//Systems
class DayNightCycle;
class TextureHandler;
class TextureCreator;
class ProceduralTextureHandler;
class MaterialHandler;
class MeshHandler;

//Objects
class OBJModel;
class IndexedMesh;
class MarchingCubeChunk;
class ModelClass;
class Skysphere;
struct DirLight;
struct RenderableBundle;

//Debugging/GUI stuff
class DebugOverlayHUD;
struct DebugWindowHandle;
class DebugWindowClass;

//Shaders
class DRDirLightShader;
class DRGBuffer;
class DRWaterClass;
class DRObjModelShader;
class DRDirLightShader;
class DRPointLightShader;
class GeometryShaderGrass;
class MCGBufferTerrainShader;

class UnderwaterFilterShader;
class SSAOShader;
class SSAOBlur;
class DRCompose;
class GaussianBlur;
class TextureShaderClass;
class VertexShaderOnly;
class DepthOnlyShader;

//Shader inputs
#include "ShaderInputStructs.h"

class GameRenderer : public SettingsDependent
{
public:
	GameRenderer();
	~GameRenderer();

	void Shutdown();

	bool Initialize(HWND hwnd, std::shared_ptr<InputClass> inputManager, std::shared_ptr<D3DManager> d3D, std::shared_ptr<DebugOverlayHUD> debugHUD, 
		UINT screenWidth, UINT screenHeight, UINT shadowmapWidth, UINT shadowmapHeight, float screenFar, float screenNear);

	bool Update(HWND hwnd, int fps, int cpuPercentage, float millisecondDeltaTime, float secondDeltaTime, XMFLOAT3* windDirection, std::shared_ptr<CameraClass> camera);
	bool Render(HWND hwnd, RenderableBundle* renderableBundle, std::shared_ptr<CameraClass> camera);

	TextureHandler* GetTextureHandler() { return textureHandler.get(); }
	MaterialHandler* GetMaterialHandler() { return materialHandler.get(); }

	virtual void OnSettingsReload(Config* cfg);

	std::shared_ptr<MeshHandler> GetMeshHandler() { return meshHandler; }

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
	bool RenderDirectionalLight(ShaderInputStructs::DirectionalLightInput* input);
	bool RenderPointLight(XMMATRIX* view, XMMATRIX* invertedView, XMMATRIX* invertedProjection, XMMATRIX* viewProjection);

	//Post processing
	bool RenderUnderwaterFilter(ShaderInputStructs::WaterFilterInput* input);
	bool RenderSSAO(ShaderInputStructs::SSAOShaderInput* input);
	bool RenderComposedScene(ShaderInputStructs::ComposeShaderInput* input);
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
	std::shared_ptr<MeshHandler> meshHandler;

	std::shared_ptr<TextureCreator> textureCreator;
	std::shared_ptr<TextureHandler> textureHandler;
	std::shared_ptr<MaterialHandler> materialHandler;
	std::shared_ptr<ProceduralTextureHandler> proceduralTextureHandler;	
	std::shared_ptr<DayNightCycle> dayNightCycle;
	std::shared_ptr<NoiseClass> noise;
	std::shared_ptr<Utility> utility;

	/************************************************************************/
	/* Render targets and necessary rendering states and objects            */
	/************************************************************************/
	// render target for storing color. R8G8B8A8. Stores a... lerp value in alpha channel, used in case I want to lerp between two materials in lighting stage.
	std::unique_ptr<RenderTarget2D> colorRT;
	
	//has enough channels to hold 2 material IDs... and a lerp between them?
	//DXGI_FORMAT_R10G10B10A2_UINT
	std::unique_ptr<RenderTarget2D> materialRT;

	// render target for storing depth. it's a R32
	std::unique_ptr<RenderTarget2D> depthRT;
	
	//render target for storing normals. R8G8B8A8. 
	// I use a compression algorithm for my normals, so only the .xy channels are used when storing normals. the .zw channels are used for material IDs
	std::unique_ptr<RenderTarget2D> normalRT; 

	//light rendertarget. R8G8B8A8
	std::unique_ptr<RenderTarget2D> lightRT;
	
	//Shadow map. it's a R16 G16 because we use variance shadowmapping 
	std::unique_ptr<RenderTarget2D> shadowRT; 

	//R32, pretty much identical to depth RT in every way
	std::unique_ptr<RenderTarget2D> ssaoRT; 

	//Used for bouncing shadow map.
	std::unique_ptr<RenderTarget2D> R16G16PingPongRT; 

	 //For bouncing ssao map.
	std::unique_ptr<RenderTarget2D> R32PingPongRT;

	//For bouncing color map
	std::unique_ptr<RenderTarget2D> ARGB8PingPongRT;

	CComPtr<ID3D11RenderTargetView> gbufferRenderTargets[3]; //render targets for GBuffer pass
	CComPtr<ID3D11RenderTargetView> lightTarget[1];
	CComPtr<ID3D11RenderTargetView> shadowTarget[1];
	CComPtr<ID3D11RenderTargetView> gaussianBlurTarget[1];
	CComPtr<ID3D11RenderTargetView> ssaoTarget[2];
	CComPtr<ID3D11RenderTargetView> waterTarget[1];

	//0 == color, 1 == depth
	CComPtr<ID3D11ShaderResourceView> waterInputTextures[2];
	CComPtr<ID3D11ShaderResourceView> gbufferTextures[3];
	CComPtr<ID3D11ShaderResourceView> dirLightTextures[4];
	CComPtr<ID3D11ShaderResourceView> finalTextures[5];
	CComPtr<ID3D11ShaderResourceView> gaussianBlurTexture[1];
	CComPtr<ID3D11ShaderResourceView> lightMap;
	CComPtr<ID3D11ShaderResourceView> ssaoBlurTextures[3];

	//0 == depth, 1 == normal, 2 == random vectors
	CComPtr<ID3D11ShaderResourceView> ssaoView;
	CComPtr<ID3D11ShaderResourceView> ssaoTextures[3];

	CComPtr<ID3D11DepthStencilView> shadowDepthStencil;
	CComPtr<ID3D11DepthStencilView> depthStencil;

	/************************************************************************/
	/* Shaders                                                              */
	/************************************************************************/

	//GBuffer pass shaders
	std::unique_ptr<DRGBuffer> gbufferShader;
	std::unique_ptr<GeometryShaderGrass> geometryShaderGrass;
	std::unique_ptr<DRWaterClass> waterShader;
	std::unique_ptr<MCGBufferTerrainShader> mcubeShader;
	std::unique_ptr<DRObjModelShader> objModelShader;

	//Light pass shaders
	std::unique_ptr<DRDirLightShader> dirLightShader;
	std::unique_ptr<DRPointLightShader> pointLightShader;

	//Post processing shaders
	std::unique_ptr<UnderwaterFilterShader> underwaterFilterShader;
	std::unique_ptr<SSAOShader> ssaoShader;
	std::unique_ptr<SSAOBlur> ssaoBlurShader;
	std::unique_ptr<DRCompose> composeShader;
	std::unique_ptr<GaussianBlur> gaussianBlurShader;

	//Misc shaders
	std::unique_ptr<TextureShaderClass> textureShader;
	std::unique_ptr<VertexShaderOnly> vertexOnlyShader;
	std::unique_ptr<DepthOnlyShader> depthOnlyShader;


	/************************************************************************/
	/* Shader input objects                                                 */
	/************************************************************************/
	std::shared_ptr<ShaderInputStructs::DirectionalLightInput> dirLightInput;
	std::shared_ptr<ShaderInputStructs::ComposeShaderInput> composeInput;
	std::shared_ptr<ShaderInputStructs::SSAOShaderInput> ssaoInput;
	std::shared_ptr<ShaderInputStructs::WaterFilterInput> waterfilterInput;

	/************************************************************************/
	/* Light and models                                                     */
	/************************************************************************/
	std::unique_ptr<DebugWindowClass> fullScreenQuad;

	std::vector<XMFLOAT4X4> treeMatrices;
	int numtrees;

	std::vector<std::unique_ptr<PointLight>> pointLights;

	std::shared_ptr<DirLight> dirLight;

	std::shared_ptr<ModelClass> sphereModel;
	std::shared_ptr<Skysphere> skySphere;

	/************************************************************************/
	/* Debug data                                                           */
	/************************************************************************/
	std::shared_ptr<DebugOverlayHUD> debugHUD;
	std::vector<std::unique_ptr<DebugWindowClass>> debugWindows;
	std::vector<std::unique_ptr<DebugWindowHandle>> debugWindowHandles;

	/************************************************************************/
	/* Miscellaneous                                                        */
	/************************************************************************/
	XMFLOAT3 camPos, camDir, windDir;
	UINT shadowMapWidth, shadowMapHeight, screenWidth, screenHeight;
	int toggleSSAO, toggleColorMode;
	float xPos, yPos, textureOffsetDeltaTime, fogMinimum, farClip, nearClip, timer, timeOfDay, waterLevel;
	bool returning, toggleDebugInfo, toggleTextureShader, toggleOtherPointLights, drawWireFrame, cameraIsUnderwater;
};