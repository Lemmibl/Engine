////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Renderer.h"

Renderer::Renderer()
{
	d3D = 0;
	camera  = 0;
	text = 0;

	frustum = 0;
	textureShader = 0;
	pointLightShader = 0;

	vertexOnlyShader = 0;
	depthOnlyShader = 0;
	composeShader = 0;
	gaussianBlurShader = 0;

	dirLightShader = 0;
	dirLight = 0;

	sphereModel = 0;
	skySphere = 0;
	vegetationManager = 0;
	depthOnlyQuadShader = 0;

	colorRT = 0;
	normalRT = 0;
	depthRT = 0;
	shadowRT = 0;
	lightRT = 0;
	gaussianBlurPingPongRT = 0;

	mcubeShader = 0;
	marchingCubes = 0;
	mcTerrain = 0;

	utility = 0;
	textureAndMaterialHandler = 0;

	timeOfDay = 0.0f;
	timer = 10.0f;
	lodState = 0;
	previousLodState = 0;

	toggleTextureShader = false;
	returning = false;
	toggleOtherPointLights = false;


	xPos = yPos = 0.0f;



	vegetationCount = 150000;
}


Renderer::Renderer(const Renderer& other)
{
}


Renderer::~Renderer()
{
	delete noise;
	noise = 0;
}


bool Renderer::Initialize(HWND hwnd, CameraClass* camera, InputClass* input, D3DClass* d3D, UINT screenWidth, UINT screenHeight,
	UINT shadowmapWidth, UINT shadowmapHeight, float screenFar, float screenNear, bool toggleDebug)
{
	srand((unsigned int)time(NULL));
	bool result;

	this->toggleDebugInfo = toggleDebug;
	this->inputManager = input;
	this->shadowMapWidth = shadowmapWidth;
	this->shadowMapHeight = shadowmapHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->screenFar = screenFar;
	this->screenNear = screenNear;
	this->d3D = d3D;
	this->camera = camera;

	toggleSSAO = 0;
	toggleColorMode = 1;
	fogMinimum = 1.0f;

	utility = new Utility();

	XMStoreFloat4x4(&baseViewMatrix, camera->GetView());

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

	result = InitializeShaders(hwnd);
	if(!result)
	{
		return false;
	}

	return true;
}

bool Renderer::InitializeShaders( HWND hwnd )
{
	bool result;

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

	gaussianBlurShader = new GaussianBlur();
	if(!gaussianBlurShader)
	{
		return false;
	}

	result = gaussianBlurShader->Initialize(d3D->GetDevice(), hwnd);



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

bool Renderer::InitializeLights( HWND hwnd )
{
	bool result;

#pragma region Point light initialization
	float x, y, z, red, green, blue;
	x = 2.0f;
	z = 2.0f;
	y = 15.0f;

	for(int i = 0; i < 100; i++)
	{
		red = utility->RandomFloat();
		green = utility->RandomFloat();
		blue = utility->RandomFloat();
		// pointLights[i].Position = XMFLOAT3(2.0f+(x * 176.0f), 15.0f, 2.0f + (y * 176.0f));

		PointLight pointLight;
		pointLights.push_back(pointLight);

		pointLights[i].Color = XMFLOAT3(red, green, blue);
		pointLights[i].Position = XMFLOAT3(x, y, z);
		pointLights[i].Radius = 4.0f + (5.0f*utility->RandomFloat()); //Used to both scale the actual point light model and is a factor in the attenuation
		pointLights[i].Intensity = 1.0f;//3.0f*utility->RandomFloat(); //Is used to control the attenuation

		x += 18.0f;

		if(x >= 178.0f) //Every 10th light gets reseted in x and z plane.
		{
			x = 2.0f;
			z += 18.0f;
		}

		if(i != 0 && i % 100 == 0) //Every 100 pointlights we reset and make another layer that is (y+8) higher up.
		{
			x = 2.0f;
			z = 2.0f;
			y += 8.0f;
		}

		XMMATRIX tempScale = XMMatrixScaling(pointLights[0].Radius, pointLights[0].Radius, pointLights[0].Radius);
		XMMATRIX tempTranslation = XMMatrixTranslation(pointLights[i].Position.x, pointLights[i].Position.y, pointLights[i].Position.z);
		XMStoreFloat4x4(&pointLights[i].World, (tempScale * tempTranslation));
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
	dirLight->Color = XMFLOAT4(0.6f, 0.4f, 0.4f, 1.0f);
	dirLight->Intensity = 1.5f;
	dirLight->Position = XMFLOAT3(150.0f, 0.0f, 0.0f);

	XMVECTOR direction = XMVector3Normalize(lookAt - XMLoadFloat3(&dirLight->Position));
	XMStoreFloat3(&dirLight->Direction, direction);

	//XMStoreFloat4x4(&dirLight->Projection, XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.0f, 5.0f, 500.0f));	//Generate PERSPECTIVE light projection matrix and store it as float4x4
	XMStoreFloat4x4(&dirLight->Projection, XMMatrixOrthographicLH(300.0f, 300.0f, 5.0f, 400.0f));					//Generate ORTHOGONAL light projection matrix and store it as float4x4
	XMStoreFloat4x4(&dirLight->View, XMMatrixLookAtLH(XMLoadFloat3(&dirLight->Position), lookAt, up));				//Generate light view matrix and store it as float4x4.
#pragma endregion

	return true;
}

bool Renderer::InitializeModels( HWND hwnd )
{
	bool result;

	noise = new SimplexNoise();
	if(!noise)
	{
		return false;
	}

	metaBalls = new MetaballsClass();
	marchingCubes = new MarchingCubesClass(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(180.0f, 180.0f, 180.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), noise);
	marchingCubes->SetMetaBalls(metaBalls, 0.2f);
	marchingCubes->GetTerrain()->SetTerrainType(7);
	marchingCubes->CalculateMesh(d3D->GetDevice());

	lSystem = new LSystemClass();
	lSystem->initialize();

	vegetationManager = new VegetationManager();
	if(!vegetationManager)
	{
		return false;
	}

	result = vegetationManager->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		return false;
	}

	GenerateVegetation(d3D->GetDevice(), true);

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

	result = textureAndMaterialHandler->Initialize(d3D->GetDevice(), d3D->GetDeviceContext(), noise, utility);
	if(!result)
	{
		return false;
	}

	dayNightCycle = new DayNightCycle();
	if(!dayNightCycle)
	{
		return false;
	}

	result = dayNightCycle->Initialize(500.0f, DAY); //86400.0f/6 <-- This is realistic day/night cycle. 86400 seconds in a day.
	if(!result)
	{
		return false;
	}

	//dayNightCycle->Update(50.0f, dirLight, skySphere);

	// Create the text object.
	text = new TextClass(d3D->GetDevice(), d3D->GetDeviceContext(), hwnd, screenWidth, screenHeight);
	if(!text)
	{
		return false;
	}

	for(int i = 0; i < 7; i++)
	{
		debugWindows[i].Initialize(d3D->GetDevice(), screenWidth, screenHeight, 200, 200);
	}

	fullScreenQuad.Initialize(d3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);

	colorRT = new RenderTarget2D();
	normalRT = new RenderTarget2D();
	depthRT = new RenderTarget2D();
	shadowRT = new RenderTarget2D();
	lightRT = new RenderTarget2D();
	gaussianBlurPingPongRT = new RenderTarget2D();

	colorRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	depthRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R32_FLOAT);
	lightRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	shadowRT->Initialize(d3D->GetDevice(), shadowMapWidth, shadowMapHeight, DXGI_FORMAT_R32G32_FLOAT);
	gaussianBlurPingPongRT->Initialize(d3D->GetDevice(), shadowMapWidth, shadowMapHeight, DXGI_FORMAT_R32G32_FLOAT); //Needs to be identical to shadowRT

	// Create the frustum object.
	frustum = new FrustumClass();
	if(!frustum)
	{
		return false;
	}

	frustum->SetInternals(((float)D3DX_PI/2.0f), 1.0f, 0.5f, 150.0f);
	testBoundingbox = Lemmi2DAABB(XMFLOAT2(0, 0), XMFLOAT2(60, 60));

	return true;
}

bool Renderer::Update(HWND hwnd, int fps, int cpu, float frameTime, float seconds)
{
	bool result;

	timer += seconds;

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

	//Move all point lights upward
	if(inputManager->IsKeyPressed(DIK_R))
	{
		XMMATRIX tempScale = XMMatrixScaling(pointLights[0].Radius, pointLights[0].Radius, pointLights[0].Radius);

		for(int i = 0; i < (int)(pointLights.size()); i++)
		{
			pointLights[i].Position.y += frameTime*0.01f;

			XMStoreFloat4x4(&pointLights[i].World, (tempScale*XMMatrixTranslation(pointLights[i].Position.x, pointLights[i].Position.y, pointLights[i].Position.z)));
		}
	}

	//Move all point lights downward
	if(inputManager->IsKeyPressed(DIK_F))
	{
		XMMATRIX tempScale = XMMatrixScaling(pointLights[0].Radius, pointLights[0].Radius, pointLights[0].Radius);

		for(int i = 0; i < (int)(pointLights.size()); i++)
		{
			pointLights[i].Position.y -= frameTime*0.01f;

			XMStoreFloat4x4(&pointLights[i].World, (tempScale*XMMatrixTranslation(pointLights[i].Position.x, pointLights[i].Position.y, pointLights[i].Position.z)));
		}
	}

	if(inputManager->WasKeyPressed(DIK_U))
	{
		textureAndMaterialHandler->RebuildRandom2DTexture(d3D->GetDevice(), d3D->GetDeviceContext());
	}

	if(inputManager->WasKeyPressed(DIK_I))
	{
		textureAndMaterialHandler->RebuildMirroredSimplex2DTexture(d3D->GetDevice(), d3D->GetDeviceContext());
	}

	if(inputManager->WasKeyPressed(DIK_O))
	{
		textureAndMaterialHandler->RebuildSimplex2DTexture(d3D->GetDevice(), d3D->GetDeviceContext());
	}

	if(inputManager->WasKeyPressed(DIK_J))
	{
		xPos += 30.0f;

		textureAndMaterialHandler->RebuildTexture(d3D->GetDevice(), d3D->GetDeviceContext(), TEXTURE_GRASS, 1024, 1024, xPos, yPos, true);
	}

	if(inputManager->WasKeyPressed(DIK_K))
	{
		yPos += 30.0f;

		textureAndMaterialHandler->RebuildTexture(d3D->GetDevice(), d3D->GetDeviceContext(), TEXTURE_DIRT, 1024, 1024, xPos, yPos, true);
	}

	if(inputManager->WasKeyPressed(DIK_P))
	{
		//Create and initialize our time... things.
		const time_t timeObject = time(NULL);
		struct tm parts;
		localtime_s(&parts, &timeObject );

		std::ostringstream stringStream;

		//Create the string that will hold the screenshot's name when it gets pooped out into the directory
		stringStream << "SavedTexture_" << (1+parts.tm_mon) << "-" << parts.tm_mday <<  "-" << parts.tm_min << "-" << parts.tm_sec << ".bmp";

		LPCSTR fileName;
		string temp = stringStream.str();
		fileName = (temp).c_str();

		textureAndMaterialHandler->SaveTextureToFile(d3D->GetDeviceContext(), *textureAndMaterialHandler->GetNoiseTexture(), D3DX11_IFF_BMP, fileName);
	}

	if(inputManager->WasKeyPressed(DIK_L))
	{
		//Create and initialize our time... things.
		const time_t timeObject = time(NULL);
		struct tm parts;
		localtime_s(&parts, &timeObject );

		std::ostringstream stringStream;

		//Create the string that will hold the screenshot's name when it gets pooped out into the directory
		stringStream << "SavedTexture_" << (1+parts.tm_mon) << "-" << parts.tm_mday <<  "-" << parts.tm_min << "-" << parts.tm_sec << ".bmp";

		LPCSTR fileName;
		string temp = stringStream.str();
		fileName = (temp).c_str();

		textureAndMaterialHandler->SaveTextureToFile(d3D->GetDeviceContext(), *textureAndMaterialHandler->GetTerrainTexture(), D3DX11_IFF_BMP, fileName);
	}

	if(inputManager->IsKeyPressed(DIK_1))
	{
		seconds = timeOfDay += frameTime;
	}

	if(inputManager->WasKeyPressed(DIK_2))
	{
		toggleColorMode++;

		if(toggleColorMode > 1)
		{
			toggleColorMode = 0;
		}
	}

	if(inputManager->WasKeyPressed(DIK_3))
	{
		fogMinimum -= 0.1f;

		if(fogMinimum < 0.2f)
		{
			fogMinimum = 0.2f;
		}
	}

	if(inputManager->WasKeyPressed(DIK_4))
	{                  
		fogMinimum *= 1.1f;

		if(fogMinimum > 1.0f)
		{
			fogMinimum = 1.0f;
		}
	}

	if(inputManager->WasKeyPressed(DIK_5))
	{
		toggleOtherPointLights = !toggleOtherPointLights;
	}

	if(inputManager->WasKeyPressed(DIK_6))
	{
		toggleSSAO++;

		if(toggleSSAO > 2)
		{
			toggleSSAO = 0;
		}
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD0) || inputManager->WasKeyPressed(DIK_0))
	{
		marchingCubes->GetTerrain()->PulverizeWorldToggle();
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD1) || inputManager->WasKeyPressed(DIK_F1))
	{
		marchingCubes->GetTerrain()->SetTerrainType(1);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD2) || inputManager->WasKeyPressed(DIK_F2))
	{
		marchingCubes->GetTerrain()->SetTerrainType(2);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD3) || inputManager->WasKeyPressed(DIK_F3))
	{
		marchingCubes->GetTerrain()->SetTerrainType(3);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD4) || inputManager->WasKeyPressed(DIK_F4))
	{
		marchingCubes->GetTerrain()->SetTerrainType(4);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD5) || inputManager->WasKeyPressed(DIK_F5))
	{
		marchingCubes->GetTerrain()->SetTerrainType(5);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD6) || inputManager->WasKeyPressed(DIK_F6))
	{
		marchingCubes->GetTerrain()->SetTerrainType(6);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD7) || inputManager->WasKeyPressed(DIK_F7))
	{
		marchingCubes->GetTerrain()->SetTerrainType(7);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD8) || inputManager->WasKeyPressed(DIK_F8))
	{
		marchingCubes->GetTerrain()->SetTerrainType(8);
	}

	if(inputManager->WasKeyPressed(DIK_NUMPAD9) || inputManager->WasKeyPressed(DIK_F9))
	{
		marchingCubes->GetTerrain()->SetTerrainType(9);
	}


#pragma region LOD stuff
	//Distance between camera and middle of mcube chunk. We'll have to do this for each chunk, and keep an individual lodState for each chunk.
	if(timer >= 0.2f)
	{
		int distance = (int)utility->VectorDistance(camera->GetPosition(), XMFLOAT3(90.0f, 40.0f, 90.0f));

		//if(distance <= 100)
		//{
		//	lodState = 3;
		//}
		//else if(distance <= 150)
		//{
		//	lodState = 2;
		//}
		//else if(distance <= 200)
		//{
		//	lodState = 1;
		//}
		//else
		//{
		//	lodState = 0;
		//}
		lodState = 3;

		timer = 0.0f;
	}
#pragma endregion

#pragma region Generate new marching cubes world
	if(inputManager->WasKeyPressed(DIK_N))
	{
		previousLodState = 0; //We set previous lod state to something != lodState so that it'll trigger an instancebuffer rebuild
		lodState = 3;

		marchingCubes->Reset(noise);
		marchingCubes->CalculateMesh(d3D->GetDevice());

		GenerateVegetation(d3D->GetDevice(), false);
	}

	//If the lod state has changed since last update, switch and rebuild vegetation instance buffers
	if(lodState != previousLodState)
	{
		switch (lodState)
		{
		case 0:
			assert(vegetationManager->BuildInstanceBuffer(d3D->GetDevice(), &LODVector500));
			break;

		case 1:
			assert(vegetationManager->BuildInstanceBuffer(d3D->GetDevice(), &LODVector2500));
			break;

		case 2:
			assert(vegetationManager->BuildInstanceBuffer(d3D->GetDevice(), &LODVector5000));
			break;

		case 3:
			assert(vegetationManager->BuildInstanceBuffer(d3D->GetDevice(), &LODVector10000));
			break;
		}
	}
#pragma endregion

	timeOfDay = dayNightCycle->Update(seconds, dirLight, skySphere);

	XMVECTOR lookAt = XMLoadFloat3(&camera->GetPosition());//XMVectorSet(30.0f, 20.0f, 30.0f, 1.0f);//XMLoadFloat3(&camera->GetPosition());//XMLoadFloat3(&camera->GetPosition());//XMLoadFloat3(&camera->GetPosition())+(camera->ForwardVector()*30.0f);//XMLoadFloat3(&camera->GetPosition());//
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR currentLightPos = (XMLoadFloat3(&camera->GetPosition())+XMLoadFloat3(&dirLight->Position));//XMLoadFloat3(&camera->GetPosition())-(camera->ForwardVector()*30.0f);

	XMStoreFloat3(&dirLight->Direction, XMVector3Normalize((lookAt - currentLightPos)));//XMLoadFloat3(&dirLight->Position)
	XMStoreFloat4x4(&dirLight->View, XMMatrixLookAtLH(currentLightPos, lookAt, up)); //Generate light view matrix

	previousLodState = lodState;

	return true;
}

//TODO: THIS SHOULD BE INSIDE VEGETATION MANAGER
void Renderer::GenerateVegetation( ID3D11Device* device, bool IfSetupThenTrue_IfUpdateThenFalse)
{
	float x,z,y;
	int textureID, randValue;

	LODVector500.clear();
	LODVector500.resize(vegetationCount/8);

	LODVector2500.clear();
	LODVector2500.resize(vegetationCount/4);
	
	LODVector5000.clear();
	LODVector5000.resize(vegetationCount/2);

	LODVector10000.clear();
	LODVector10000.resize(vegetationCount);

	for(int i = 0; i < vegetationCount; i++)
	{
		x = (2.0f + (utility->RandomFloat() * marchingCubes->GetSizeX()-2.0f));
		z = (2.0f + (utility->RandomFloat() * marchingCubes->GetSizeZ()-2.0f));

		//Extract highest Y at this point
		y = marchingCubes->GetTerrain()->GetHighestPositionOfCoordinate((int)x, (int)z);

		randValue = rand()%100;

		//No vegetation below Y:30
		if(y <= 20.0f)
		{
			continue;
		}
		else if(y >= 45.0)
		{
			//But the grass should be sparse, so there is
			//95% chance that we won't actually add this to the instance list.
			if(randValue > 95)
			{
				textureID = 0;

				//Place texture ID in .w channel
				VegetationManager::InstanceType temp;
				temp.position = XMFLOAT4(x, y, z, (float)textureID);
				temp.randomValue = (utility->RandomFloat()*360.0f);

				//We use i to control how many should be added to each LOD vector
				if(i <= (vegetationCount / 8))
				{
					LODVector500[i] = (temp);
				}

				if(i <= (vegetationCount / 4))
				{
					LODVector2500[i] = (temp);
				}

				if(i <= (vegetationCount / 2))
				{
					LODVector5000[i] = (temp);
				}

				LODVector10000[i] = (temp);
			}
		}
		else
		{
			if(randValue <= 5)
			{
				textureID = 2; //Some kind of leaf branch that I've turned into a plant quad.
			}
			else if(randValue <= 96) //By far biggest chance that we get normal grass
			{
				textureID = 1; //Normal grass.
			}
			else if(randValue <= 98) //If 97-98
			{
				textureID = 4; //Bush.
			}
			else //If 99-100.
			{
				textureID = 3; //Flower.
			}

			VegetationManager::InstanceType temp;

			//Place texture ID in .w channel
			temp.position = XMFLOAT4(x, y, z, (float)textureID);
			temp.randomValue = (utility->RandomFloat()*XM_PI);

			//We use i to control how many should be added to each LOD vector
			if(i <= 500)
			{
				LODVector500.push_back(temp);
			}

			if(i <= 2500)
			{
				LODVector2500.push_back(temp);
			}

			if(i <= 5000)
			{
				LODVector5000.push_back(temp);
			}

			LODVector10000.push_back(temp);
		}
	}

	if(IfSetupThenTrue_IfUpdateThenFalse)
	{
		vegetationManager->SetupQuads(d3D->GetDevice(), &LODVector10000);
	}
	else
	{
		vegetationManager->BuildInstanceBuffer(d3D->GetDevice(), &LODVector10000);
	}
}

bool Renderer::Render(HWND hwnd)
{
	// Clear the scene.
	d3D->BeginScene(0.1f, 0.1f, 0.45f, 0.0f);

	ID3D11DeviceContext* deviceContext = d3D->GetDeviceContext();
	ID3D11Device* device = d3D->GetDevice();

#pragma region Preparation
	shadowDepthStencil = d3D->GetShadowmapDSV();

	// Generate the view matrix based on the camera's position.
	camPos = camera->GetPosition();
	XMStoreFloat3(&camDir, XMVector3Normalize(camera->ForwardVector()));

	//For shadow pre-gbuffer pass
	shadowTarget[0] = shadowRT->RTView;

	//Name should be pretty self-explanatory
	gaussianBlurPingPongRTView[0] = gaussianBlurPingPongRT->RTView;

	//For gbuffer pass
	gbufferRenderTargets[0] = colorRT->RTView;
	gbufferRenderTargets[1] = normalRT->RTView;
	gbufferRenderTargets[2] = depthRT->RTView;

	//For lighting pass
	lightTarget[0] = lightRT->RTView; 

	//For GBuffer pass
	gbufferTextures[0] = colorRT->SRView; 
	gbufferTextures[1] = normalRT->SRView;
	gbufferTextures[2] = depthRT->SRView;

	//For directional light pass
	dirLightTextures[0] = normalRT->SRView;
	dirLightTextures[1] = depthRT->SRView;
	dirLightTextures[2] = shadowRT->SRView;
	dirLightTextures[3] = colorRT->SRView;

	//For the the final composition pass
	finalTextures[0] = colorRT->SRView;
	finalTextures[1] = lightRT->SRView;
	finalTextures[2] = depthRT->SRView;
	finalTextures[3] = normalRT->SRView;

	gaussianBlurTexture[0] = gaussianBlurPingPongRT->SRView;
#pragma endregion

#pragma region Matrix preparations
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, scalingMatrix, viewProjection, invertedViewProjection, invertedView, 
		lightView, lightProj, lightViewProj, baseView, worldBaseViewOrthoProj, identityWorldViewProj, lightWorldViewProj, 
		invertedProjection, untransposedViewProj, lightWorldView, lightWorld, invertedWorldView, worldView, worldBaseView;

	//XMMATRIX shadowScaleBiasMatrix = new XMMATRIX
	//	(
	//	0.5f, 0.0f, 0.0f, 0.5f,
	//	0.0f, 0.5f, 0.0f, 0.5f,
	//	0.0f, 0.0f, 0.5f, 0.5f,
	//	0.0f, 0.0f, 0.0f, 1.0f
	//	);

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	d3D->GetWorldMatrix(worldMatrix);
	d3D->GetOrthoMatrix(orthoMatrix);
	camera->GetViewMatrix(viewMatrix);
	camera->GetProjectionMatrix(projectionMatrix);
	lightView = XMLoadFloat4x4(&dirLight->View);
	lightProj = XMLoadFloat4x4(&dirLight->Projection);
	lightWorld = XMMatrixTranslationFromVector(XMLoadFloat3(&dirLight->Position));

	// Construct the frustum.
	frustum->ConstructFrustum(screenFar, &projectionMatrix, &viewMatrix);

	XMVECTOR nullVec = XMVectorSplatOne();
	viewProjection = XMMatrixMultiply(viewMatrix, projectionMatrix);

	invertedView = XMMatrixInverse(&nullVec, viewMatrix);
	invertedProjection = XMMatrixInverse(&nullVec, projectionMatrix);
	invertedViewProjection = XMMatrixInverse(&nullVec, viewProjection);
	invertedWorldView = XMMatrixInverse(&nullVec, worldMatrix*viewMatrix);


	lightViewProj =	XMMatrixMultiply(lightView, lightProj);
	lightWorldView = worldMatrix * lightView;
	lightWorldViewProj = worldMatrix * lightView * lightProj;

	identityWorldViewProj = worldMatrix * viewMatrix * projectionMatrix;
	worldView = worldMatrix * viewMatrix;
	worldBaseView = worldMatrix * baseView;


	lightWorldView =			XMMatrixTranspose(lightWorldView);
	lightViewProj =				XMMatrixTranspose(lightViewProj);
	lightWorldViewProj =		XMMatrixTranspose(lightWorldViewProj);
	lightView =					XMMatrixTranspose(lightView);
	lightProj =					XMMatrixTranspose(lightProj);

	invertedView =				XMMatrixTranspose(invertedView);
	invertedWorldView =			XMMatrixTranspose(invertedWorldView);
	invertedProjection =		XMMatrixTranspose(invertedProjection);
	invertedViewProjection =	XMMatrixTranspose(invertedViewProjection);

	worldMatrix =				XMMatrixTranspose(worldMatrix);
	//viewMatrix =				XMMatrixTranspose(viewMatrix);
	//projectionMatrix =		XMMatrixTranspose(projectionMatrix);
	baseView =					XMMatrixTranspose(XMLoadFloat4x4(&baseViewMatrix));

	worldView =					XMMatrixTranspose(worldView);
	worldBaseView =				XMMatrixTranspose(worldBaseView);
	//viewProjection =			XMMatrixTranspose(viewProjection);
	identityWorldViewProj =		XMMatrixTranspose(identityWorldViewProj);

	//Post-transpose matrix multiplications for the calculations that contain orthogonal projections
	worldBaseViewOrthoProj =	(worldMatrix * baseView * orthoMatrix);

	baseView = XMMatrixTranspose(baseView);
#pragma endregion

	assert(RenderShadowmap(deviceContext, &lightWorldViewProj, &lightWorldView));
	assert(RenderTwoPassGaussianBlur(deviceContext, &worldBaseViewOrthoProj));
	assert(RenderGBuffer(deviceContext, &viewMatrix, &projectionMatrix, &identityWorldViewProj));

	assert(RenderPointLight(deviceContext, &viewMatrix, &invertedView, &viewProjection));
	assert(RenderDirectionalLight(deviceContext, &viewMatrix, &worldBaseViewOrthoProj, &lightView, &lightProj, &invertedProjection));

	assert(RenderComposedScene(deviceContext, &worldBaseViewOrthoProj, &worldBaseView, &baseView, &invertedProjection, &invertedViewProjection));
	assert(RenderDebugInfoAndText(deviceContext, &worldBaseViewOrthoProj));

	// Present the rendered scene to the screen.
	d3D->EndScene();

	return true;
}

bool Renderer::RenderShadowmap( ID3D11DeviceContext* deviceContext, XMMATRIX* lightWorldViewProj, XMMATRIX* lightWorldView)
{
	//Early depth pass for shadowmap
	deviceContext->OMSetRenderTargets(1, shadowTarget, shadowDepthStencil);
	deviceContext->ClearDepthStencilView(shadowDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->ClearRenderTargetView(shadowTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	d3D->SetShadowViewport();

	d3D->SetNoCullRasterizer();

	marchingCubes->Render(deviceContext);

	if(!depthOnlyShader->Render(deviceContext, marchingCubes->GetIndexCount(), lightWorldViewProj, lightWorldView))
	{
		return false;
	}

	//Uncomment to enable vegetation quad shadows
	//Just doesn't look good right now. Will have to wait for cascaded shadowmaps or something similar before this is even worth it.
	/*********************************************************************************************/

	//d3D->TurnOnShadowBlendState();
	//vegetationManager->RenderBuffers(deviceContext);

	//depthOnlyQuadShader->Render(deviceContext, vegetationManager->GetVertexCount(), vegetationManager->GetInstanceCount(),
	//	lightWorldViewProj, lightWorldView, textureAndMaterialHandler->GetVegetationTextureArray());

	//d3D->ResetBlendState();

	/*********************************************************************************************/

	return true;
}

bool Renderer::RenderTwoPassGaussianBlur(ID3D11DeviceContext* deviceContext, XMMATRIX* worldBaseViewOrthoProj )
{
	//Shadow map blur stage
	//Change render target to prepare for ping-ponging
	deviceContext->OMSetRenderTargets(1, gaussianBlurPingPongRTView, shadowDepthStencil);
	deviceContext->ClearRenderTargetView(gaussianBlurPingPongRTView[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
	deviceContext->ClearDepthStencilView(shadowDepthStencil, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Blur shadow map texture horizontally
	if(!fullScreenQuad.Render(deviceContext, 0, 0))
	{
		return false;
	}

	if(!gaussianBlurShader->RenderBlurX(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, &dirLightTextures[2]))
	{
		return false;
	}

	//Change render target back to our shadow map to render the second blur and get the final result
	deviceContext->OMSetRenderTargets(1, shadowTarget, shadowDepthStencil);
	deviceContext->ClearDepthStencilView(shadowDepthStencil, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	//Blur shadow map texture vertically
	if(!fullScreenQuad.Render(deviceContext, 0, 0))
	{
		return false;
	}

	if(!gaussianBlurShader->RenderBlurY(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, &gaussianBlurTexture[0]))
	{
		return false;
	}

	return true;
}

bool Renderer::RenderGBuffer(ID3D11DeviceContext* deviceContext, XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix, XMMATRIX* identityWorldViewProj )
{
	XMMATRIX worldViewProjMatrix, worldMatrix, worldView;

	//GBuffer building stage
	d3D->SetDefaultViewport();
	depthStencil = d3D->GetDepthStencilView();
	deviceContext->OMSetRenderTargets(3, gbufferRenderTargets, depthStencil);
	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->ClearRenderTargetView(gbufferRenderTargets[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	deviceContext->ClearRenderTargetView(gbufferRenderTargets[1], D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.5f));
	deviceContext->ClearRenderTargetView(gbufferRenderTargets[2], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

	d3D->SetNoCullRasterizer();
	d3D->TurnZBufferOff();

	worldViewProjMatrix = XMMatrixTranslation(camPos.x, camPos.y, camPos.z) * ((*viewMatrix) * (*projectionMatrix));
	worldViewProjMatrix = XMMatrixTranspose(worldViewProjMatrix);

	skySphere->Render(deviceContext, &worldViewProjMatrix, &dayNightCycle->GetAmbientLightColor(), timeOfDay);

	d3D->SetBackFaceCullingRasterizer();
	d3D->TurnZBufferOn();

	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	worldMatrix = XMMatrixIdentity();
	worldView = XMMatrixTranspose(worldMatrix * (*viewMatrix));
	worldMatrix = XMMatrixTranspose(worldMatrix);

	//if(frustum->Check2DAABB(&testBoundingbox))
	//{
	if(!marchingCubes->Render(deviceContext))
	{
		return false;
	}

	if(!mcubeShader->Render(d3D->GetDeviceContext(), marchingCubes->GetIndexCount(), &worldMatrix, &worldView, 
		identityWorldViewProj, textureAndMaterialHandler->GetTerrainTextureArray(), textureAndMaterialHandler->GetMaterialLookupTexture(), toggleColorMode, screenFar))
	{
		return false;
	}

	d3D->SetNoCullRasterizer();
	d3D->TurnOnAlphaBlending();
	if(!vegetationManager->Render(deviceContext, identityWorldViewProj, &worldView, &worldMatrix, textureAndMaterialHandler->GetVegetationTextureArray()))
	{
		return false;
	}
	d3D->TurnOffAlphaBlending();
	//}

	return true;
}

bool Renderer::RenderPointLight( ID3D11DeviceContext* deviceContext, XMMATRIX* view, XMMATRIX* invertedView, XMMATRIX* viewProjection )
{
	XMMATRIX world, worldView, worldViewProj;

	//Point light stage
	deviceContext->OMSetRenderTargets(1, lightTarget, depthStencil);
	deviceContext->ClearRenderTargetView(lightTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_STENCIL, 1.0f, 0);

	d3D->TurnOnLightBlending();

	if(toggleOtherPointLights)
	{
		for(unsigned int i = 0; i < pointLights.size(); i++)
		{	
			world = XMLoadFloat4x4(&pointLights[i].World);
			worldView = world * (*view);
			worldViewProj =  world * (*viewProjection);

			world = XMMatrixTranspose(world);
			worldView = XMMatrixTranspose(worldView);
			worldViewProj = XMMatrixTranspose(worldViewProj);

			//Phase one, draw sphere with vertex-only shader.
			d3D->SetLightStencilMethod1Phase1();
			d3D->SetNoCullRasterizer();

			sphereModel->Render(deviceContext);

			if(!vertexOnlyShader->Render(deviceContext, sphereModel->GetIndexCount(), &worldViewProj))
			{
				return false;
			}

			//Phase two, draw sphere with light algorithm
			d3D->SetLightStencilMethod1Phase2();
			d3D->SetFrontFaceCullingRasterizer();

			sphereModel->Render(deviceContext);

			if(!pointLightShader->Render(deviceContext, sphereModel->GetIndexCount(), &worldViewProj, &worldView, &world, invertedView, 
				&pointLights[i], gbufferTextures, textureAndMaterialHandler->GetMaterialTextureArray(), camPos))
			{
				return false;
			}

			//if(!textureShader->Render(deviceContext, sphereModel->GetIndexCount(), &worldViewProj, sphereModel->GetTexture()))
			//{
			//	return false;
			//}

			deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	return true;
}


bool Renderer::RenderDirectionalLight( ID3D11DeviceContext* deviceContext, XMMATRIX* viewMatrix, XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* lightView, XMMATRIX* lightProj, XMMATRIX* invertedProjection )
{
	XMMATRIX worldMatrix, worldView, invertedWorldView, invertedView;

	//Directional light stage
	/*TODO: Create a directional light stencilstate that does a NOTEQUAL==0 stencil check.*/
	depthStencil = d3D->GetDepthStencilView();
	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_STENCIL|D3D11_CLEAR_DEPTH, 1.0f, 0);
	d3D->SetBackFaceCullingRasterizer();

	XMVECTOR det = XMVectorSplatOne();
	worldMatrix = XMMatrixIdentity();
	worldView = (worldMatrix * (*viewMatrix));
	invertedView = XMMatrixInverse(&det, (*viewMatrix));
	invertedWorldView = XMMatrixInverse(&det, worldView);

	invertedView = XMMatrixTranspose(invertedView);
	worldMatrix = XMMatrixTranspose(worldMatrix);
	worldView = XMMatrixTranspose(worldView);
	invertedWorldView = XMMatrixTranspose(invertedWorldView);

	if(!fullScreenQuad.Render(deviceContext, 0, 0))
	{
		return false;
	}

	if(!dirLightShader->Render(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, &worldView, &worldMatrix, viewMatrix, 
		&invertedView, invertedProjection, lightView, lightProj, dirLightTextures, textureAndMaterialHandler->GetMaterialTextureArray(), 
		camPos, dirLight, dayNightCycle->GetAmbientLightColor()))
	{
		return false;
	}

	return true;
}

/* worldView or worldBASEView ??? TODO! */

bool Renderer::RenderComposedScene(ID3D11DeviceContext* deviceContext, XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invertedViewProjection )
{
	//Render final composed scene that is the sum of all the previous scene
	d3D->SetBackBufferRenderTarget();
	d3D->ResetBlendState();
	d3D->GetDeviceContext()->ClearDepthStencilView(depthStencil,  D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	fullScreenQuad.Render(deviceContext, 0, 0);

	composeShader->Render(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, worldView, view, invertedProjection, 
		invertedViewProjection, &dayNightCycle->GetAmbientLightColor(), fogMinimum, finalTextures, *textureAndMaterialHandler->GetSSAORandomTexture(), toggleSSAO);

	return true;
}

bool Renderer::RenderDebugInfoAndText(ID3D11DeviceContext* deviceContext, XMMATRIX* worldBaseViewOrthoProj )
{
	XMMATRIX worldMatrix;

	d3D->ResetRasterizerState();
	d3D->ResetBlendState();
	depthStencil = d3D->GetDepthStencilView(); //This also resets the depth stencil state to "default".
	deviceContext->ClearDepthStencilView(depthStencil,  D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	d3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixTranspose(worldMatrix);

	if(toggleDebugInfo)
	{
		for(int i = 0; i < 3; i++)
		{
			if(!debugWindows[i].Render(d3D->GetDeviceContext(), 200+200*i, 0))
			{
				return false;
			}

			if(!textureShader->Render(d3D->GetDeviceContext(), debugWindows[i].GetIndexCount(), 
				worldBaseViewOrthoProj, gbufferTextures[i]))
			{
				return false;
			}
		}

		if(!debugWindows[4].Render(d3D->GetDeviceContext(), 800, 0))
		{
			return false;
		}

		if(!textureShader->Render(d3D->GetDeviceContext(), debugWindows[4].GetIndexCount(), 
			worldBaseViewOrthoProj, shadowRT->SRView))
		{
			return false;
		}

		if(!debugWindows[3].Render(d3D->GetDeviceContext(), 200, 200))
		{
			return false;
		}

		if(!textureShader->Render(d3D->GetDeviceContext(), debugWindows[3].GetIndexCount(), 
			worldBaseViewOrthoProj, lightRT->SRView))
		{
			return false;
		}

		if(!debugWindows[5].Render(d3D->GetDeviceContext(), 400, 200))
		{
			return false;
		}

		if(!textureShader->Render(d3D->GetDeviceContext(), debugWindows[5].GetIndexCount(), 
			worldBaseViewOrthoProj, *textureAndMaterialHandler->GetNoiseTexture()))
		{
			return false;
		}

		if(!debugWindows[6].Render(d3D->GetDeviceContext(), 600, 200))
		{
			return false;
		}

		if(!textureShader->Render(d3D->GetDeviceContext(), debugWindows[6].GetIndexCount(), 
			worldBaseViewOrthoProj, *textureAndMaterialHandler->GetTerrainTexture()))
		{
			return false;
		}

		d3D->TurnZBufferOff();

		d3D->TurnOnAlphaBlending();

		// Render the text user interface elements.
		if(!text->Render(d3D->GetDeviceContext(), worldBaseViewOrthoProj))
		{
			return false;
		}

		// Turn off alpha blending after rendering the text.
		d3D->TurnOffAlphaBlending();

		// Turn the Z buffer back on now that all 2D rendering has completed.
		d3D->TurnZBufferOn();
	}

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

	if(gaussianBlurShader)
	{
		gaussianBlurShader->Shutdown();
		delete gaussianBlurShader;
		gaussianBlurShader = 0;
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

	if(gaussianBlurPingPongRT)
	{
		delete gaussianBlurPingPongRT;
		gaussianBlurPingPongRT  = 0;
	}

	if(sphereModel)
	{
		sphereModel->Shutdown();
		delete sphereModel;
		sphereModel = 0;
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

	if(noise)
	{
		delete noise;
		noise = 0;
	}

	return;
}
