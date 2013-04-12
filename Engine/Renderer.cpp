////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Renderer.h"

/*
Inför cleana upp kod:
http://gamedev.stackexchange.com/questions/24615/managing-shaders-and-objects-in-directx-11
https://graphics.stanford.edu/wikis/cs448s-11/FrontPage?action=AttachFile&do=get&target=05-GPU_Arch_I.pdf

Inför terrain rendering / många texturer:
http://gamedev.stackexchange.com/questions/14507/loading-a-texture2d-array-in-directx11
http://www.gamedev.net/topic/612977-dynamic-updating-of-structuredbuffer-in-dx11/ < DEN HÄR
http://msdn.microsoft.com/en-us/library/ff476486(VS.85).aspx
http://stackoverflow.com/questions/6347950/programmatically-creating-directx-11-textures-pros-and-cons-of-the-three-differ
http://irrlicht.sourceforge.net/forum/viewtopic.php?t=21236
http://gamedev.stackexchange.com/questions/14873/loading-a-sub-resource-for-a-texture-array

http://rastergrid.com/blog/2010/01/uniform-buffers-vs-texture-buffers/

http://stackoverflow.com/questions/10623787/directx-11-framebuffer-capture-c-no-win32-or-d3dx < SPARA TEXTURER TIL HDD
http://stackoverflow.com/questions/35950/i-dont-understand-stdtr1unordered-map
Spara material ID i normal alpha channel

Inför perlin/simplex noise:
http://stackoverflow.com/questions/4120108/how-to-save-backbuffer-to-file-in-directx-10

Inför SSAO:
http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
http://www.iquilezles.org/www/articles/ssao/ssao.htm

Inför shadow maps:
http://gameengineers.blogspot.se/2013/02/no-title-yet.html
http://lousodrome.net/blog/light/2012/01/23/variance-shadow-maps/
http://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
http://msdn.microsoft.com/en-us/library/windows/desktop/ee416307(v=vs.85).aspx
http://devblog.drheinous.com/2012/10/cascaded-variance-shadow-maps.html
http://developer.download.nvidia.com/SDK/9.5/Samples/samples.html#HLSL_SoftShadows
http://graphics.stanford.edu/~mdfisher/Shadows.html


Inför gräsquads:
http://gamedev.stackexchange.com/questions/5038/shadow-mapping-and-transparent-quads
http://www.gamedev.net/topic/362393-shadow-map-alpha-testing-question-solved/
http://www.gamedev.net/topic/551160-directx-shadow-map-example-has-no-alpha/
http://gamedev.stackexchange.com/questions/22507/what-is-the-alphatocoverage-blend-state-useful-for
http://blogs.msdn.com/b/shawnhar/archive/2009/02/18/depth-sorting-alpha-blended-objects.aspx


http://www.geeks3d.com/20100831/shader-library-noise-and-pseudo-random-number-generator-in-glsl/
http://ogldev.atspace.co.uk/www/tutorial27/tutorial27.html
http://zeuxcg.blogspot.se/2007/09/particle-rendering-revisited.html
http://realtimecollisiondetection.net/blog/?p=91
http://www.flashbang.se/archives/315
http://faculty.ycp.edu/~dbabcock/PastCourses/cs470/labs/lab11.html
http://faculty.ycp.edu/~dbabcock/PastCourses/cs470/labs/lab13.html
http://www.rastertek.com/dx11tut37.html
http://software.intel.com/en-us/articles/rendering-grass-with-instancing-in-directx-10
http://http.developer.nvidia.com/GPUGems/gpugems_ch07.html
http://developer.amd.com/wordpress/media/2012/10/ShaderX_AnimatedGrass.pdf

Directional light lens flare:
http://www.madgamedev.com/post/2010/04/21/Article-Sun-and-Lens-Flare-as-a-Post-Process.aspx
http://stackoverflow.com/questions/14161727/hlsl-drawing-a-centered-circle
if cross product (cameraDirection, lightDirection) == 0 then they're both facing the same way? I think.
*/

Renderer::Renderer()
{
	toggleDebugInfo = true;

	d3D = 0;
	camera  = 0;
	text = 0;

	frustum = 0;
	gbufferShader = 0;
	textureShader = 0;
	pointLightShader = 0;

	vertexOnlyShader = 0;
	depthOnlyShader = 0;
	composeShader = 0;

	dirLightShader = 0;
	dirLight = 0;

	modelList = 0;
	groundModel = 0;
	sphereModel = 0;
	otherModel = 0;
	skySphere = 0;
	vegetationManager = 0;
	depthOnlyQuadShader = 0;

	colorRT = 0;
	normalRT = 0;
	depthRT = 0;
	shadowRT = 0;
	lightRT = 0;

	mcubeShader = 0;
	marchingCubes = 0;
	mcTerrain = 0;

	utility = 0;
	textureAndMaterialHandler = 0;
}


Renderer::Renderer(const Renderer& other)
{
}


Renderer::~Renderer()
{
}


bool Renderer::Initialize(HWND hwnd, CameraClass* camera, InputClass* input, D3DClass* d3D, UINT screenWidth, UINT screenHeight,
	UINT shadowmapWidth, UINT shadowmapHeight, float screenFar, float screenNear)
{
	srand((unsigned int)time(NULL));
	bool result;

	this->inputManager = input;
	this->shadowMapWidth = shadowmapWidth;
	this->shadowMapHeight = shadowmapHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->screenFar = screenFar;
	this->screenNear = screenNear;
	this->d3D = d3D;
	this->camera = camera;
	timeOfDay = timer = 0.0f;
	toggleTextureShader = false;
	returning = false;
	debugRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	utility = new Utility();

	XMStoreFloat4x4(&baseViewMatrix, camera->GetView());

	result = InitializeShaders(hwnd);
	if(!result)
	{
		return false;
	}

	result = InitializeLights(hwnd);
	if(!result)
	{
		return false;
	}

	result = InitializeModels(hwnd);
	if(!result)
	{
		return false;
	}

	result = InitializeEverythingElse(hwnd);
	if(!result)
	{
		return false;
	}

	return true;
}

bool Renderer::InitializeShaders(HWND hwnd)
{
	bool result;

	gbufferShader = new DRGBuffer();
	if(!gbufferShader)
	{
		return false;
	}

	result = gbufferShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"GBuffer shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	pointLightShader = new DRPointLight();
	if(!pointLightShader)
	{
		return false;
	}

	result = pointLightShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Pointlight shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	vertexOnlyShader = new VertexShaderOnly();
	if(!vertexOnlyShader)
	{
		return false;
	}

	result = vertexOnlyShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Vertex only shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	depthOnlyShader = new DepthOnlyShader();
	if(!vertexOnlyShader)
	{
		return false;
	}

	result = depthOnlyShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Depth only shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	depthOnlyQuadShader = new DepthOnlyQuadShader();
	if(!depthOnlyQuadShader)
	{
		return false;
	}

	result = depthOnlyQuadShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Depth only quad shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	composeShader = new DRCompose();
	if(!composeShader)
	{
		return false;
	}

	result = composeShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Compose shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	textureShader = new TextureShaderClass();
	if(!textureShader)
	{
		return false;
	}

	result = textureShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Texture shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	mcubeShader = new MCubesGBufferShader();
	if(!mcubeShader)
	{
		return false;
	}

	result = mcubeShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Marching cubes gbuffer shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Renderer::InitializeLights(HWND hwnd)
{
	bool result;

	ambientLight = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

#pragma region Point light initialization
	float x, y, z;
	x = -10.0f;
	z = -10.0f;
	y = 10.0f;

	for(int i = 0; i < 20; i++)
	{
		pointLights.push_back(new PointLight());
		pointLights[i]->Position = XMFLOAT3(x, y, z);
		pointLights[i]->Color = XMFLOAT3(0.1f+i%4, 0.1f+i%2, 1.0f-i%3);
		pointLights[i]->Radius = 8.0f;
		pointLights[i]->Intensity = 512.0f; //The lower it gets, the more intense it gets

		x += 10.0f;

		if(x >= 30.0f) //Every 10th light gets reseted in x and z plane.
		{
			x = -10.0f;
			z += 10.0f;
		}

		if(i != 0 && i % 100 == 0) //Every 100 pointlights we reset and make another layer that is (y+8) higher up.
		{
			x = -10.0f;
			z = -10.0f;
			y += 8.0f;
		}

		XMMATRIX tempScale = XMMatrixScaling(pointLights[0]->Radius, pointLights[0]->Radius, pointLights[0]->Radius);
		XMMATRIX tempTranslation = XMMatrixTranslation(pointLights[i]->Position.x, pointLights[i]->Position.y, pointLights[i]->Position.z);
		XMStoreFloat4x4(&pointLights[i]->World, XMMatrixTranspose(tempScale * tempTranslation));
	}
#pragma endregion

#pragma region Directional light initialization
	// Create the directional light.
	dirLight = new DirLight();
	if(!dirLight)
	{
		return false;
	}

	dirLightShader = new DRDirLight();
	if(!dirLightShader)
	{
		return false;
	}

	result = dirLightShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Dir light shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);//LookAt for dir light. We always want this to be (0,0,0), because it's the easiest to visualize.
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	// Initialize the directional light.
	dirLight->Color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight->Intensity = 128.0f;
	dirLight->Position = XMFLOAT3(0.0f, 120.0f, 0.0f);

	XMVECTOR direction = XMVector3Normalize(lookAt - XMLoadFloat3(&dirLight->Position));
	XMStoreFloat3(&dirLight->Direction, direction);

	//XMStoreFloat4x4(&dirLight->Projection, XMMatrixPerspectiveFovLH(((float)D3DX_PI/2.0f), 1.0f, 10.0f, 300.0f)); //Generate perspective light projection matrix and store it as float4x4
	XMStoreFloat4x4(&dirLight->Projection, XMMatrixOrthographicLH(140.0f, 140.0f, 10.0f, 300.0f)); //Generate orthogonal light projection matrix and store it as float4x4

	XMStoreFloat4x4(&dirLight->View, XMMatrixLookAtLH(XMLoadFloat3(&dirLight->Position), lookAt, up)); //Generate light view matrix and store it as float4x4.
#pragma endregion

	return true;
}

bool Renderer::InitializeModels(HWND hwnd)
{
	bool result;

	metaBalls = new MetaballsClass();
	marchingCubes = new MarchingCubesClass(0.0f, 0.0f, 0.0f, 60.0f, 60.0f, 60.0f, 1.0f, 1.0f, 1.0f);
	marchingCubes->SetMetaBalls(metaBalls, 0.2f);

	marchingCubes->GetTerrain()->Noise3D();
	marchingCubes->CalculateMesh(d3D->GetDevice());

	skySphere = new Skysphere();
	if(!skySphere)
	{
		return false;
	}

	result = skySphere->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		return false;
	}

	vegetationManager = new VegetationManager();
	if(!vegetationManager)
	{
		return false;
	}

	result = vegetationManager->Initialize(d3D->GetDevice(), hwnd, L"../Engine/data/Vegetation/grassQuad.dds", L"../Engine/data/Vegetation/leafbranch.dds");
	if(!result)
	{
		return false;
	}

	std::vector<XMFLOAT4>* tempContainer = new std::vector<XMFLOAT4>();

	for(int i = 0; i < 5000; i++)
	{
		float x,z,y;
		x = (float)((2.0f + (utility->Random() * 56))* 1.0f);
		z = (float)((2.0f + (utility->Random() * 56))* 1.0f);
		int j;
		y = marchingCubes->GetTerrain()->GetHighestPositionOfCoordinate(x,z);

		float k;

		if(y > 45.0f)
		{
			k = 0.0f;
		}
		else if(y < 10.0f)
		{
			k = 1.0f;
		}
		else
		{
			k = 2.0f + utility->Random()*6.0f;
		}

		XMFLOAT4 temp = XMFLOAT4((float)x, y, (float)z, k);
		tempContainer->push_back(temp);
	}

	vegetationManager->SetupQuads(d3D->GetDevice(), tempContainer);

	// Create the model object.
	groundModel = new ModelClass;
	if(!groundModel)
	{
		return false;
	}

	// Initialize the model object.
	result = groundModel->Initialize(d3D->GetDevice(), "../Engine/data/ground.txt", L"../Engine/data/grass.dds", L"../Engine/data/ground_normal.dds", L"../Engine/data/ground_specular.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	sphereModel = new ModelClass();
	if(!sphereModel)
	{
		return false;
	}

	// Initialize the model object. It really doesn't matter what textures it has because it's only used for point light volume culling.
	result = sphereModel->Initialize(d3D->GetDevice(), "../Engine/data/skydome.txt", L"../Engine/data/grass.dds", L"../Engine/data/dirt.dds", L"../Engine/data/rock.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	otherModel = new ModelClass();
	if(!otherModel)
	{
		return false;
	}

	result = otherModel->Initialize(d3D->GetDevice(), "../Engine/data/cube.txt", L"../Engine/data/stone02.dds", L"../Engine/data/bump02.dds", L"../Engine/data/stone_specmap.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	modelList = new ModelListClass();
	if(!modelList)
	{
		return false;
	}

	result = modelList->Initialize(10);
	if(!result)
	{
		return false;
	}

	return true;
}

bool Renderer::InitializeEverythingElse( HWND hwnd )
{
	bool result;

	textureAndMaterialHandler = new TextureAndMaterialHandler();
	if(!textureAndMaterialHandler)
	{
		return false;
	}

	result = textureAndMaterialHandler->Initialize(d3D->GetDevice(), d3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	dayNightCycle = new DayNightCycle();
	if(!dayNightCycle)
	{
		return false;
	}

	result = dayNightCycle->Initialize(3600.0f*5.0f, DUSK);
	if(!result)
	{
		return false;
	}

	dayNightCycle->Update(0.0f, dirLight, skySphere);

	// Create the text object.
	text = new TextClass();
	if(!text)
	{
		return false;
	}

	// Initialize the text object.
	result = text->Initialize(d3D->GetDevice(), d3D->GetDeviceContext(), hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object. Look in graphicsclass.", L"Error", MB_OK);
		return false;
	}

	for(int i = 0; i < 5; i++)
	{
		debugWindows[i].Initialize(d3D->GetDevice(), screenWidth, screenHeight, 200, 200);
	}


	defaultModelMaterial.a = 8.0f;
	defaultModelMaterial.Ka = 0.4f;
	defaultModelMaterial.Kd = 0.8f;
	defaultModelMaterial.Ks = 0.7f;

	fullScreenQuad.Initialize(d3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);

	colorRT = new RenderTarget2D();
	normalRT = new RenderTarget2D();
	depthRT = new RenderTarget2D();
	shadowRT = new RenderTarget2D();
	lightRT = new RenderTarget2D();

	colorRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	depthRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R32_FLOAT);
	shadowRT->Initialize(d3D->GetDevice(), shadowMapWidth, shadowMapHeight, DXGI_FORMAT_R16G16_FLOAT);
	lightRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);


	// Create the frustum object.
	frustum = new FrustumClass;
	if(!frustum)
	{
		return false;
	}

	return true;
}

bool Renderer::Update(int fps, int cpu, float frameTime, float seconds)
{
	bool result;

	XMMATRIX tempScale = XMMatrixScaling(pointLights[0]->Radius, pointLights[0]->Radius, pointLights[0]->Radius);

	if(inputManager->WasKeyPressed(DIK_Q))
	{
		toggleDebugInfo = !toggleDebugInfo;
	}

	result = text->SetFps(fps, d3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	result = text->SetCpu(cpu, d3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	XMFLOAT3 temp = camera->GetPosition();
	result = text->SetCameraPosition((int)temp.x, (int)temp.y, (int)temp.z, d3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	temp = camera->GetRotation();
	result = text->SetCameraRotation((int)temp.x, (int)temp.y, (int)temp.z, d3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	if(inputManager->WasKeyPressed(DIK_T))
	{
		if(defaultModelMaterial.a <= 512.0f)
			defaultModelMaterial.a *= 2.0f;
	}

	if(inputManager->WasKeyPressed(DIK_G))
	{
		if(defaultModelMaterial.a >= 1.0f)
			defaultModelMaterial.a *= 0.5f;
	}

	if(inputManager->IsKeyPressed(DIK_R))
	{
		for(int i = 0; i < (int)pointLights.size(); i++)
		{
			pointLights[i]->Position.y += frameTime*0.006f;

			XMStoreFloat4x4(&pointLights[i]->World, XMMatrixTranspose(tempScale*XMMatrixTranslation(pointLights[i]->Position.x, pointLights[i]->Position.y, pointLights[i]->Position.z)));
		}
	}

	if(inputManager->IsKeyPressed(DIK_F))
	{
		//dirLight->Position.x -= frameTime*0.02f;
		//dirLight->Position.z -= frameTime*0.02f;

		for(int i = 0; i < (int)pointLights.size(); i++)
		{
			pointLights[i]->Position.y -= frameTime*0.006f;

			XMStoreFloat4x4(&pointLights[i]->World, XMMatrixTranspose(tempScale*XMMatrixTranslation(pointLights[i]->Position.x, pointLights[i]->Position.y, pointLights[i]->Position.z)));
		}
	}

	if(inputManager->IsKeyPressed(DIK_C))
	{
		debugRotation.x += frameTime*0.002f;
		debugRotation.y += frameTime*0.002f;
	}

	if(inputManager->IsKeyPressed(DIK_V))
	{
		debugRotation.x -= frameTime*0.002f;
		debugRotation.y -= frameTime*0.002f;		
	}

	if(inputManager->IsKeyPressed(DIK_1))
	{
		seconds = timeOfDay += frameTime*2.0f;
	}


	if(inputManager->WasKeyPressed(DIK_N))
	{

		std::vector<XMFLOAT4>* tempContainer = new std::vector<XMFLOAT4>();

		marchingCubes->Reset();
		marchingCubes->GetTerrain()->Noise3D();
		marchingCubes->CalculateMesh(d3D->GetDevice());

		float x,z,y;
		int j;

		x = (float)((2.0f + (utility->Random() * 56))* 1.0f);
		z = (float)((2.0f + (utility->Random() * 56))* 1.0f);

		y = marchingCubes->GetTerrain()->GetHighestPositionOfCoordinate(x,z);

		float k;

		if(y > 30.0f)
		{
			k = 0.0f;
		}
		else if(y < 10.0f)
		{
			k = 1.0f;
		}
		else
		{
			k = 2.0f + utility->Random()*6.0f;
		}

		XMFLOAT4 temp = XMFLOAT4((float)x, y, (float)z, k);
		tempContainer->push_back(temp);

		vegetationManager->SetupQuads(d3D->GetDevice(), tempContainer);

		delete tempContainer;
		tempContainer = 0;
	}

	timeOfDay = dayNightCycle->Update(seconds, dirLight, skySphere);

	XMVECTOR lookAt = XMLoadFloat3(&camera->GetPosition());//XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);//(camera->ForwardVector()*30.0f)+XMLoadFloat3(&camera->GetPosition());
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR currentLightPos = XMLoadFloat3(&dirLight->Position)+XMLoadFloat3(&camera->GetPosition());

	XMStoreFloat3(&dirLight->Direction, XMVector3Normalize(lookAt - currentLightPos));//XMLoadFloat3(&dirLight->Position)
	XMStoreFloat4x4(&dirLight->View, XMMatrixLookAtLH(currentLightPos, lookAt, up)); //Generate light view matrix

	return true;
}

bool Renderer::Render()
{
	// Clear the scene.
	d3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

#pragma region Preparation
	ID3D11DeviceContext* context;
	context = d3D->GetDeviceContext();

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, scalingMatrix, viewProjection, 
		invertedViewProjection, invertedView, lightView, lightProj, lightViewProj, baseView;
	float positionX, positionY, positionZ, radius;
	XMFLOAT4 color;
	XMFLOAT3 camPos;
	bool result, renderModel;
	int modelCount, renderCount;
	ID3D11RenderTargetView* gbufferRenderTargets[3] = { NULL, NULL, NULL }; //render targets for GBuffer pass
	ID3D11RenderTargetView* lightTarget[1] = { NULL };
	ID3D11RenderTargetView* shadowTarget[1] = { NULL };

	ID3D11ShaderResourceView* gbufferTextures[3] = { NULL, NULL, NULL };
	ID3D11ShaderResourceView* dirLightTextures[3] = { NULL, NULL, NULL };
	ID3D11ShaderResourceView* finalTextures[3] = { NULL, NULL, NULL };

	ID3D11ShaderResourceView* lightMap = NULL;

	ID3D11DepthStencilView* shadowDS = d3D->GetShadowmapDSV();
	ID3D11DepthStencilView* ds;

	// Generate the view matrix based on the camera's position.
	camPos = camera->GetPosition();

	gbufferRenderTargets[0] = colorRT->RTView;
	gbufferRenderTargets[1] = normalRT->RTView;
	gbufferRenderTargets[2] = depthRT->RTView;

	//For lighting pass
	lightTarget[0] = lightRT->RTView; 

	//For shadow pre-gbuffer pass
	shadowTarget[0] = shadowRT->RTView;

	//For GBuffer pass
	gbufferTextures[0] = colorRT->SRView; 
	gbufferTextures[1] = normalRT->SRView;
	gbufferTextures[2] = depthRT->SRView;

	//For directional light pass
	dirLightTextures[0] = normalRT->SRView;
	dirLightTextures[1] = depthRT->SRView;
	dirLightTextures[2] = shadowRT->SRView;

	//For the the final composition pass
	finalTextures[0] = colorRT->SRView;
	finalTextures[1] = lightRT->SRView;
	finalTextures[2] = depthRT->SRView;

	// Get the number of models that will be rendered.
	modelCount = modelList->GetModelCount();

	// Initialize the count of models that have been rendered.
	renderCount = 0;
#pragma endregion

#pragma region Matrix preparations
	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	d3D->GetWorldMatrix(worldMatrix);
	d3D->GetOrthoMatrix(orthoMatrix);
	camera->GetViewMatrix(viewMatrix);
	camera->GetProjectionMatrix(projectionMatrix);
	lightView = XMLoadFloat4x4(&dirLight->View);
	lightProj = XMLoadFloat4x4(&dirLight->Projection);

	// Construct the frustum.
	frustum->ConstructFrustum(screenFar, &projectionMatrix, &viewMatrix);

	XMVECTOR nullVec;
	lightViewProj = XMMatrixMultiply(lightView, lightProj);

	viewProjection = XMMatrixMultiply(viewMatrix, projectionMatrix);

	invertedView = XMMatrixInverse(&nullVec, viewMatrix);
	invertedViewProjection = XMMatrixInverse(&nullVec, viewProjection);

	worldMatrix =				XMMatrixTranspose(worldMatrix);
	viewMatrix =				XMMatrixTranspose(viewMatrix);
	projectionMatrix =			XMMatrixTranspose(projectionMatrix);

	lightView =					XMMatrixTranspose(lightView);
	lightProj =					XMMatrixTranspose(lightProj);

	lightViewProj =				XMMatrixTranspose(lightViewProj);

	viewProjection =			XMMatrixTranspose(viewProjection);
	invertedViewProjection =	XMMatrixTranspose(invertedViewProjection);
	baseView =					XMMatrixTranspose(XMLoadFloat4x4(&baseViewMatrix));
	invertedView =				XMMatrixTranspose(invertedView);
#pragma endregion

#pragma region Early depth pass for shadowmap
	context->OMSetRenderTargets(1, shadowTarget, shadowDS);
	context->ClearDepthStencilView(shadowDS, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->ClearRenderTargetView(shadowTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
	d3D->SetShadowViewport();

	//d3D->SetFrontFaceCullingRasterizer();
	d3D->SetNoCullRasterizer();
	//d3D->SetBackFaceCullingRasterizer();

	// Go through all the models and render them only if they can be seen by the camera view.
	for(int i =0; i < modelCount; i++)
	{
		// Get the position and color of the sphere model at this index.
		modelList->GetData(i, positionX, positionY, positionZ, color);

		// Move the model to the location it should be rendered at.
		worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ);	
		worldMatrix = XMMatrixTranspose(worldMatrix);

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		otherModel->Render(context);

		result = depthOnlyShader->Render(context, otherModel->GetIndexCount(), &worldMatrix, &lightView, &lightProj);
		if(!result)
		{
			return false;
		}
	}

	worldMatrix = XMMatrixIdentity(); 
	worldMatrix = XMMatrixTranspose(worldMatrix);
	marchingCubes->Render(context);

	result = depthOnlyShader->Render(context, marchingCubes->GetIndexCount(), &worldMatrix, &lightView, &lightProj);
	if(!result)
	{
		return false;
	}

	worldMatrix = XMMatrixIdentity(); 
	worldMatrix = XMMatrixTranspose(worldMatrix);
	vegetationManager->RenderBuffers(context);

	depthOnlyQuadShader->Render(context, vegetationManager->GetVertexCount(), vegetationManager->GetInstanceCount(),
		&worldMatrix, &lightView, &lightProj, textureAndMaterialHandler->GetVegetationTextures());
#pragma endregion

#pragma region GBuffer building stage
	d3D->SetDefaultViewport();
	ds = d3D->GetDepthStencilView();
	context->OMSetRenderTargets(3, gbufferRenderTargets, ds);
	context->ClearDepthStencilView(ds, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->ClearRenderTargetView(gbufferRenderTargets[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	context->ClearRenderTargetView(gbufferRenderTargets[1], D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.0f));
	context->ClearRenderTargetView(gbufferRenderTargets[2], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

	d3D->SetNoCullRasterizer();
	d3D->TurnZBufferOff();

	worldMatrix = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
	worldMatrix = XMMatrixTranspose(worldMatrix);

	skySphere->Render(context, &worldMatrix, &viewMatrix, &projectionMatrix, timeOfDay);

	d3D->SetBackFaceCullingRasterizer();
	d3D->TurnZBufferOn();

	context->ClearDepthStencilView(ds, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Move the model to the location it should be rendered at.
	worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	scalingMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(debugRotation.x, debugRotation.y, debugRotation.z);

	worldMatrix = scalingMatrix* rotationMatrix * worldMatrix;
	worldMatrix = XMMatrixTranspose(worldMatrix);

	groundModel->Render(context);

	gbufferShader->Render(context, groundModel->GetIndexCount(), &worldMatrix, &viewMatrix, &projectionMatrix, 
		groundModel->GetTextureArray(), screenFar);

	renderCount++;

	// Go through all the models and render them only if they can be seen by the camera view.
	for(int i =0; i < modelCount; i++)
	{
		// Get the position and color of the sphere model at this index.
		modelList->GetData(i, positionX, positionY, positionZ, color);

		// Set the radius of the sphere to 1.0 since this is already known.
		radius = 1.0f;

		// Check if the sphere model is in the view frustum.
		renderModel = frustum->CheckSphere(positionX, positionY, positionZ, radius);

		// If it can be seen then render it, if not skip this model and check the next sphere.
		if(renderModel)
		{
			// Move the model to the location it should be rendered at.
			worldMatrix = XMMatrixTranslation(positionX, positionY, positionZ); 
			worldMatrix = XMMatrixTranspose(worldMatrix);

			// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
			otherModel->Render(context);

			// Render the model using the gbuffer shader.
			result = gbufferShader->Render(context, otherModel->GetIndexCount(), &worldMatrix, &viewMatrix, 
				&projectionMatrix, otherModel->GetTextureArray(), screenFar);
			if(!result)
			{
				return false;
			}
		}
	}

	worldMatrix = XMMatrixIdentity(); 
	worldMatrix = XMMatrixTranspose(worldMatrix);

	marchingCubes->Render(context);
	result = mcubeShader->Render(d3D->GetDeviceContext(), marchingCubes->GetIndexCount(), 
		&worldMatrix, &viewMatrix, &projectionMatrix, textureAndMaterialHandler->GetTerrainTextures());//TODO

	d3D->TurnOnAlphaBlending();
	d3D->SetNoCullRasterizer();
	vegetationManager->Render(context, &worldMatrix, &viewMatrix, &projectionMatrix, textureAndMaterialHandler->GetVegetationTextures());
	d3D->TurnOffAlphaBlending();

	text->SetRenderCount(renderCount, context);
#pragma endregion

	context->OMSetRenderTargets(1, lightTarget, ds);
	context->ClearRenderTargetView(lightTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	context->ClearDepthStencilView(ds, D3D11_CLEAR_STENCIL, 1.0f, 0);

#pragma region Point light stage
	//Phase one, draw sphere with vertex-only shader.
	d3D->TurnOnLightBlending();

	for(unsigned int i = 0; i < pointLights.size(); i++)
	{	
		d3D->SetLightStencilMethod1Phase1();
		d3D->SetNoCullRasterizer();

		sphereModel->Render(context);

		result = vertexOnlyShader->Render(context, sphereModel->GetIndexCount(), &XMLoadFloat4x4(&pointLights[i]->World), &viewMatrix, 
			&projectionMatrix);

		//Phase two, draw sphere with light algorithm
		d3D->SetLightStencilMethod1Phase2();
		d3D->SetFrontFaceCullingRasterizer();

		sphereModel->Render(context);


		result = pointLightShader->Render(context, sphereModel->GetIndexCount(), &viewMatrix, 
			&projectionMatrix, &invertedViewProjection, pointLights[i], gbufferTextures, camPos);
		if(!result)
		{
			return false;
		}

		context->ClearDepthStencilView(ds, D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
#pragma endregion

#pragma region Directional light stage
	///*TODO: Create a directional light stencilstate that does a NOTEQUAL==0 stencil check.*/
	ds = d3D->GetDepthStencilView();
	context->ClearDepthStencilView(ds, D3D11_CLEAR_STENCIL|D3D11_CLEAR_DEPTH, 1.0f, 0);
	d3D->SetBackFaceCullingRasterizer();
	d3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranspose(worldMatrix);

	fullScreenQuad.Render(context, 0, 0);

	result = dirLightShader->Render(context, fullScreenQuad.GetIndexCount(), &worldMatrix, &baseView, &orthoMatrix, &invertedViewProjection, &invertedView, 
		dirLightTextures, camPos, dirLight, ambientLight, defaultModelMaterial, &lightViewProj);
	if(!result)
	{
		return false;
	}
#pragma endregion

#pragma region Final compose stage
	d3D->SetBackBufferRenderTarget();
	context->ClearDepthStencilView(ds,  D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	fullScreenQuad.Render(context, 0, 0);

	composeShader->Render(context, fullScreenQuad.GetIndexCount(), &worldMatrix, &baseView, 
		&orthoMatrix, finalTextures);
#pragma endregion

#pragma region Debug and text stage
	d3D->ResetRasterizerState();
	d3D->ResetBlendState();
	ds = d3D->GetDepthStencilView(); //This also resets the depth stencil state to "default".
	context->ClearDepthStencilView(ds,  D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	d3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranspose(worldMatrix);

	if(toggleDebugInfo)
	{
		for(int i = 0; i < 3; i++)
		{
			result = debugWindows[i].Render(d3D->GetDeviceContext(), 200+200*i, 0);
			if(!result)
			{
				return false;
			}

			result = textureShader->Render(d3D->GetDeviceContext(), debugWindows[i].GetIndexCount(), 
				&worldMatrix, &baseView, &orthoMatrix, gbufferTextures[i]);
			if(!result)
			{
				return false;
			}
		}

		result = debugWindows[3].Render(d3D->GetDeviceContext(), 200, 200);
		if(!result)
		{
			return false;
		}

		result = textureShader->Render(d3D->GetDeviceContext(), debugWindows[3].GetIndexCount(), 
			&worldMatrix, &baseView, &orthoMatrix, lightRT->SRView);
		if(!result)
		{
			return false;
		}

		result = debugWindows[4].Render(d3D->GetDeviceContext(), 800, 0);
		if(!result)
		{
			return false;
		}

		result = textureShader->Render(d3D->GetDeviceContext(), debugWindows[4].GetIndexCount(), 
			&worldMatrix, &baseView, &orthoMatrix, shadowRT->SRView);
		if(!result)
		{
			return false;
		}


		d3D->TurnZBufferOff();

		d3D->TurnOnAlphaBlending();

		// Render the text user interface elements.
		result = text->Render(d3D->GetDeviceContext(), &worldMatrix, &baseView, &orthoMatrix);
		if(!result)
		{
			return false;
		}

		// Turn off alpha blending after rendering the text.
		d3D->TurnOffAlphaBlending();

		// Turn the Z buffer back on now that all 2D rendering has completed.
		d3D->TurnZBufferOn();
	}
#pragma endregion

	// Present the rendered scene to the screen.
	d3D->EndScene();

	return true;
}

void Renderer::Shutdown()
{
	if(utility)
	{
		delete utility;
		utility = 0;
	}

	if (text)
	{
		text->Shutdown();
		delete text;
		text = 0;
	}

	if(frustum)
	{
		delete frustum;
		frustum = 0;
	}

	if(textureAndMaterialHandler)
	{
		delete textureAndMaterialHandler;
		textureAndMaterialHandler = 0;
	}

	if(gbufferShader)
	{
		gbufferShader->Shutdown();
		delete gbufferShader;
		gbufferShader = 0;
	}

	if(textureShader)
	{
		textureShader->Shutdown();
		delete textureShader;
		textureShader = 0;
	}

	if(pointLightShader)
	{
		pointLightShader->Shutdown();
		delete pointLightShader;
		pointLightShader = 0;
	}

	if(dirLight)
	{
		delete dirLight;
		dirLight = 0;
	}

	if(depthOnlyShader)
	{
		depthOnlyShader->Shutdown();
		delete depthOnlyShader;
		depthOnlyShader = 0;
	}

	if(vertexOnlyShader)
	{
		vertexOnlyShader->Shutdown();
		delete vertexOnlyShader;
		vertexOnlyShader = 0;
	}

	if(dirLightShader)
	{
		dirLightShader->Shutdown();
		delete dirLightShader;
		dirLightShader = 0;
	}

	if(composeShader)
	{
		composeShader->Shutdown();
		delete composeShader;
		composeShader = 0;
	}

	if(colorRT)
	{
		delete colorRT;
		colorRT = 0;
	}

	if(normalRT)
	{
		delete normalRT;
		normalRT = 0;
	}

	if(depthRT)
	{
		delete depthRT;
		depthRT = 0;
	}

	if(lightRT)
	{
		delete lightRT;
		lightRT = 0;
	}

	if(shadowRT)
	{
		delete shadowRT;
		shadowRT = 0;
	}

	if(modelList)
	{
		modelList->Shutdown();
		delete modelList;
		modelList = 0;
	}

	if(groundModel)
	{
		groundModel->Shutdown();
		delete groundModel;
		groundModel = 0;
	}

	if(sphereModel)
	{
		sphereModel->Shutdown();
		delete sphereModel;
		sphereModel = 0;
	}

	if(otherModel)
	{
		otherModel->Shutdown();
		delete otherModel;
		otherModel = 0;
	}

	if(skySphere)
	{
		skySphere->Shutdown();
		delete skySphere;
		skySphere = 0;
	}

	if(vegetationManager)
	{
		vegetationManager->Shutdown();
		delete vegetationManager;
		vegetationManager = 0;
	}

	for(std::vector<PointLight*>::iterator tmp = pointLights.begin(); tmp != pointLights.end(); tmp++) 
	{
		PointLight* light = 0;
		swap(light, *tmp); //Added for extra safety.

		if(light)
		{
			// Release the sentence.
			delete light;
			light = 0;
		}
	}
	pointLights.clear();

	if(marchingCubes)
	{
		delete marchingCubes;
		marchingCubes = 0;
	}

	if(mcubeShader)
	{
		mcubeShader->Shutdown();
		mcubeShader = 0;
	}

	if(dayNightCycle)
	{
		delete dayNightCycle;
		dayNightCycle = 0;
	}

	return;
}


