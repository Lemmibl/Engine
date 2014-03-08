#include "GameRenderer.h"

GameRenderer::GameRenderer() : SettingsDependent(),
	noise()
{
	xPos = yPos = 0.0f;
	timeOfDay = 0.0f;
	timer = 10.0f;
	textureOffsetDeltaTime = 0.0f;

	toggleTextureShader = false;
	returning = false;
	toggleOtherPointLights = false;
	drawWireFrame = false;

	//Load settings
	InitializeSettings(this);
}


GameRenderer::~GameRenderer()
{
}


bool GameRenderer::Initialize(HWND hwnd, std::shared_ptr<CameraClass> camera, std::shared_ptr<InputClass> inputManager, std::shared_ptr<D3DManager> d3D, 
	MeshHandler* extMeshHandler, DebugOverlayHUD* extDebugHUD, UINT screenWidth, UINT screenHeight, UINT shadowmapWidth, UINT shadowmapHeight, float screenFar, float screenNear)
{
	bool result;

	this->toggleDebugInfo = false;
	this->inputManager = inputManager;
	this->shadowMapWidth = shadowmapWidth;
	this->shadowMapHeight = shadowmapHeight;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->farClip = screenFar;
	this->nearClip = screenNear;
	this->d3D = d3D;
	this->camera = camera;
	debugHUD = extDebugHUD;
	meshHandler = extMeshHandler;

	toggleSSAO = 0;
	toggleColorMode = 1;
	fogMinimum = 1.0f;

	srand((unsigned int)time(NULL));

	XMStoreFloat4x4(&baseViewMatrix, camera->GetView());

	result = InitializeLights(hwnd);
	if(!result)
	{
		return false;
	}

	result = InitializeEverythingElse(hwnd);
	if(!result)
	{
		return false;
	}

	result = InitializeModels(hwnd);
	if(!result)
	{
		return false;
	}

	result = InitializeShaders(hwnd);
	if(!result)
	{
		return false;
	}

	InitializeRenderingSpecifics();
	InitializeDebugText();

	return true;
}


bool GameRenderer::InitializeShaders( HWND hwnd )
{
	bool result;

	result = skySphere.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		return false;
	}

	result = pointLightShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Pointlight shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = vertexOnlyShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Vertex only shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = depthOnlyShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Depth only shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = gbufferShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"GBuffer shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = composeShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Compose shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = gaussianBlurShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Gaussian blur shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = textureShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Texture shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = mcubeShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Marching cubes gbuffer shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = geometryShaderGrass.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Geometry shader grass shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = waterShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Water shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	result = objModelShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(NULL, L"OBJ Model shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	return true;
}


bool GameRenderer::InitializeLights( HWND hwnd )
{
	bool result;

#pragma region Point light initialization
	float x, y, z, red, green, blue;
	x = 2.0f;
	z = 2.0f;
	y = 15.0f;

	for(int i = 0; i < 100; i++)
	{
		red = utility.RandomFloat();
		green = utility.RandomFloat();
		blue = utility.RandomFloat();
		// pointLights[i].Position = XMFLOAT3(2.0f+(x * 176.0f), 15.0f, 2.0f + (y * 176.0f));

		PointLight pointLight;
		pointLights.push_back(pointLight);

		pointLights[i].Color = XMFLOAT3(red, green, blue);
		pointLights[i].Position = XMFLOAT3(x, y, z);
		pointLights[i].Radius = 4.0f + (2.0f*utility.RandomFloat()); //Used to both scale the actual point light model and is a factor in the attenuation
		pointLights[i].Intensity = (10.0f*utility.RandomFloat()); //Is used to control the attenuation

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
	dirLight = DirLight();

	dirLightShader = DRDirLight();


	result = dirLightShader.Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Dir light shader couldn't be initialized.", L"Error", MB_OK);
		return false;
	}

	XMVECTOR lookAt = XMLoadFloat3(&camera->GetPosition());//LookAt for dir light. We always want this to be (0,0,0), because it's the easiest to visualize.
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	// Initialize the directional light.
	dirLight.Color = XMFLOAT4(0.6f, 0.4f, 0.4f, 1.0f);
	dirLight.Intensity = 1.5f;
	XMStoreFloat3(&dirLight.Position, (lookAt + XMVectorSet(50.0f, 150.0f, 0.0f, 0.0f)));

	XMVECTOR direction = XMVector3Normalize(XMVectorSubtract(lookAt, XMLoadFloat3(&dirLight.Position)));
	XMStoreFloat3(&dirLight.Direction, direction);

	//XMStoreFloat4x4(&dirLight.Projection, XMMatrixPerspectiveFovLH(XM_PIDIV4, 1.0f, 5.0f, 500.0f));	//Generate PERSPECTIVE light projection matrix and store it as float4x4
	XMStoreFloat4x4(&dirLight.Projection, XMMatrixOrthographicLH(300.0f, 300.0f, 5.0f, 400.0f));					//Generate ORTHOGONAL light projection matrix and store it as float4x4
	XMStoreFloat4x4(&dirLight.View, XMMatrixLookAtLH(XMLoadFloat3(&dirLight.Position), lookAt, up));				//Generate light view matrix and store it as float4x4.
#pragma endregion

	return true;
}


bool GameRenderer::InitializeModels( HWND hwnd )
{
	bool result;

	// Initialize the model object. It really doesn't matter what textures it has because it's only used for point light volume culling.
	result = sphereModel.Initialize(d3D->GetDevice(), "../Engine/data/skydome.txt", L"../Engine/data/grass.dds", L"../Engine/data/dirt.dds", L"../Engine/data/rock.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	numtrees = 50;

	for(int i = 0; i < numtrees; i++)
	{
		XMFLOAT4X4 temp;

		//Order of operations should be Scaling * Rotation * Translation
		XMStoreFloat4x4(&temp, (XMMatrixRotationY(utility.RandomFloat()*360.0f) * XMMatrixTranslation(utility.RandomFloat()*100.0f, 15.0f, utility.RandomFloat()*100.0f)));

		treeMatrices.push_back(temp);
	}

	return true;
}


bool GameRenderer::InitializeDebugText()
{
	debugHUD->AddNewWindowWithoutHandle("DeltaTime: ", &timer, DataTypeEnumMappings::Float);

	debugHUD->AddNewWindowWithoutHandle("Directional light position: ", &dirLight.Position, DataTypeEnumMappings::Float3);
	debugHUD->AddNewWindowWithoutHandle("Camera position: ", camera->GetPositionPtr(),		DataTypeEnumMappings::Float3);
	debugHUD->AddNewWindowWithoutHandle("Camera rotation: ", camera->GetRotationPtr(),		DataTypeEnumMappings::Float3);

	debugHUD->AddNewWindowWithoutHandle("Point lights are turned on: ", &toggleOtherPointLights,		DataTypeEnumMappings::Bool);
	debugHUD->AddNewWindowWithoutHandle("Point light #1: ", &pointLights[0].Position.y,		DataTypeEnumMappings::Float);

	return true;
}


bool GameRenderer::InitializeEverythingElse( HWND hwnd )
{
	bool result;

	result = textureAndMaterialHandler.Initialize(d3D->GetDevice(), d3D->GetDeviceContext(), &noise, &utility);
	if(!result)
	{
		return false;
	}

	textureAndMaterialHandler.SetupWindtextures(d3D->GetDevice(), d3D->GetDeviceContext(), 0, 0, 1024, 1024, 0.6f);

	result = dayNightCycle.Initialize(DAY); //86400.0f/6 <-- This is realistic day/night cycle. 86400 seconds in a day.
	if(!result)
	{
		return false;
	}

	//dayNightCycle->Update(50.0f, dirLight, skySphere);

	for(int i = 0; i < 7; i++)
	{
		debugWindows[i].Initialize(d3D->GetDevice(), screenWidth, screenHeight, 200, 200);
	}

	fullScreenQuad.Initialize(d3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);

	colorRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	depthRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R32_FLOAT);
	lightRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	shadowRT.Initialize(d3D->GetDevice(), shadowMapWidth, shadowMapHeight, DXGI_FORMAT_R32G32_FLOAT);
	gaussianBlurPingPongRT.Initialize(d3D->GetDevice(), shadowMapWidth, shadowMapHeight, DXGI_FORMAT_R32G32_FLOAT); //Needs to be identical to shadowRT

	//TODO:
	/*

	for(int i = 0; i < 3; i++)
	{
	if(!debugWindows[i].Render(deviceContext, 200+200*i, 0))
	{
	return false;
	}

	if(!textureShader.Render(deviceContext, debugWindows[i].GetIndexCount(), 
	worldBaseViewOrthoProj, gbufferTextures[i]))
	{
	return false;
	}
	}

	if(!debugWindows[4].Render(deviceContext, 800, 0))
	{
	return false;
	}

	if(!textureShader.Render(deviceContext, debugWindows[4].GetIndexCount(), 
	worldBaseViewOrthoProj, shadowRT.SRView))
	{
	return false;
	}

	if(!debugWindows[3].Render(deviceContext, 200, 200))
	{
	return false;
	}

	if(!textureShader.Render(deviceContext, debugWindows[3].GetIndexCount(), 
	worldBaseViewOrthoProj, lightRT.SRView))
	{
	return false;
	}

	if(!debugWindows[5].Render(deviceContext, 400, 200))
	{
	return false;
	}

	if(!textureShader.Render(deviceContext, debugWindows[5].GetIndexCount(), 
	worldBaseViewOrthoProj, *textureAndMaterialHandler.GetWindNormalMap()))
	{
	return false;
	}

	if(!debugWindows[6].Render(deviceContext, 600, 200))
	{
	return false;
	}

	if(!textureShader.Render(deviceContext, debugWindows[6].GetIndexCount(), 
	worldBaseViewOrthoProj, *textureAndMaterialHandler.GetWindTexture()))
	{
	return false;
	}

	*/

	return true;
}


bool GameRenderer::Update( HWND hwnd, int fps, int cpuPercentage, float millisecondDeltaTime, float secondDeltaTime, XMFLOAT3* windDirection )
{
	timer += secondDeltaTime;
	textureOffsetDeltaTime += secondDeltaTime;
	windDir = *windDirection;

	//Reset every once in a while.
	if(textureOffsetDeltaTime >= 30.0f)
	{
		textureOffsetDeltaTime = -textureOffsetDeltaTime;
	}

	if(inputManager->WasKeyPressed(DIK_E))
	{
		drawWireFrame = !drawWireFrame;
	}

	if(inputManager->WasKeyPressed(DIK_Q))
	{
		toggleDebugInfo = !toggleDebugInfo;

		debugHUD->SetHUDVisibility(toggleDebugInfo);
	}

	if(toggleDebugInfo)
	{
		debugHUD->Update();
	}

	//Move all point lights upward
	if(inputManager->IsKeyPressed(DIK_R))
	{
		XMMATRIX tempScale = XMMatrixScaling(pointLights[0].Radius, pointLights[0].Radius, pointLights[0].Radius);

		for(int i = 0; i < (int)(pointLights.size()); i++)
		{
			pointLights[i].Position.y += millisecondDeltaTime*0.01f;

			XMStoreFloat4x4(&pointLights[i].World, (tempScale*XMMatrixTranslation(pointLights[i].Position.x, pointLights[i].Position.y, pointLights[i].Position.z)));
		}
	}

	//Move all point lights downward
	if(inputManager->IsKeyPressed(DIK_F))
	{
		XMMATRIX tempScale = XMMatrixScaling(pointLights[0].Radius, pointLights[0].Radius, pointLights[0].Radius);

		for(int i = 0; i < (int)(pointLights.size()); i++)
		{
			pointLights[i].Position.y -= millisecondDeltaTime*0.01f;

			XMStoreFloat4x4(&pointLights[i].World, (tempScale*XMMatrixTranslation(pointLights[i].Position.x, pointLights[i].Position.y, pointLights[i].Position.z)));
		}
	}

	if(inputManager->IsKeyPressed(DIK_Y))
	{
		for(int i = 0; i < numtrees; i++)
		{
			XMStoreFloat4x4(&treeMatrices[i], XMMatrixRotationRollPitchYaw(0.0f, secondDeltaTime, 0.0f) * XMLoadFloat4x4(&treeMatrices[i]));
		}
	}

	//Speed up the change of day
	if(inputManager->IsKeyPressed(DIK_1))
	{
		secondDeltaTime += (millisecondDeltaTime);
	}

	//Toggle the coloring mode of materials
	if(inputManager->WasKeyPressed(DIK_2))
	{
		toggleColorMode++;

		if(toggleColorMode > 1)
		{
			toggleColorMode = 0;
		}
	}

	//TODO: Wtf? Change these.. remove..?
	if(inputManager->WasKeyPressed(DIK_3))
	{
		fogMinimum -= 0.1f;

		if(fogMinimum < 0.0f)
		{
			fogMinimum = 0.0f;
		}
	}

	if(inputManager->WasKeyPressed(DIK_4))
	{                  
		fogMinimum += 0.1f;

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

	timeOfDay = dayNightCycle.Update(secondDeltaTime, &dirLight, &skySphere, &camera->GetPosition());

	XMVECTOR lookAt = XMLoadFloat3(&camera->GetPosition());//XMVectorSet(30.0f, 20.0f, 30.0f, 1.0f);//XMLoadFloat3(&camera->GetPosition());//XMLoadFloat3(&camera->GetPosition());//XMLoadFloat3(&camera->GetPosition())+(camera->ForwardVector()*30.0f);//XMLoadFloat3(&camera->GetPosition());//
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR currentLightPos = XMLoadFloat3(&dirLight.Position);//XMLoadFloat3(&camera->GetPosition())-(camera->ForwardVector()*30.0f);

	XMStoreFloat3(&dirLight.Direction, XMVector3Normalize(XMVectorSubtract(lookAt, currentLightPos)));//XMLoadFloat3(&dirLight.Position)
	XMStoreFloat4x4(&dirLight.View, XMMatrixLookAtLH(currentLightPos, lookAt, up)); //Generate light view matrix

	return true;
}


void GameRenderer::InitializeRenderingSpecifics()
{
	deviceContext = d3D->GetDeviceContext();
	device = d3D->GetDevice();

	shadowDepthStencil = d3D->GetShadowManager()->GetShadowmapDSV();
	depthStencil = d3D->GetDepthStencilManager()->GetDepthStencilView();

	//For shadow pre-gbuffer pass
	shadowTarget[0] = shadowRT.RTView;

	//Name should be pretty self-explanatory
	gaussianBlurPingPongRTView[0] = gaussianBlurPingPongRT.RTView;

	//For gbuffer pass
	gbufferRenderTargets[0] = colorRT.RTView;
	gbufferRenderTargets[1] = normalRT.RTView;
	gbufferRenderTargets[2] = depthRT.RTView;

	//For lighting pass
	lightTarget[0] = lightRT.RTView; 

	//For GBuffer pass
	gbufferTextures[0] = colorRT.SRView; 
	gbufferTextures[1] = normalRT.SRView;
	gbufferTextures[2] = depthRT.SRView;

	//For directional light pass
	dirLightTextures[0] = normalRT.SRView;
	dirLightTextures[1] = depthRT.SRView;
	dirLightTextures[2] = shadowRT.SRView;
	dirLightTextures[3] = colorRT.SRView;

	//For the the final composition pass
	finalTextures[0] = colorRT.SRView;
	finalTextures[1] = lightRT.SRView;
	finalTextures[2] = depthRT.SRView;
	finalTextures[3] = normalRT.SRView;

	gaussianBlurTexture[0] = gaussianBlurPingPongRT.SRView;
}


bool GameRenderer::Render(HWND hwnd, RenderableBundle* renderableBundle)
{
	// Clear the scene.
	d3D->BeginScene(0.1f, 0.1f, 0.45f, 0.0f);

#pragma region Other Preparation
	// Generate the view matrix based on the camera's position.
	camPos = camera->GetPosition();
	XMStoreFloat3(&camDir, XMVector3Normalize(camera->ForwardVector()));
#pragma endregion

#pragma region Matrix preparations
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, viewProjection, invertedViewProjection, invertedView, 
		lightView, lightProj, baseView, worldBaseViewOrthoProj, identityWorldViewProj, lightWorldViewProj, 
		invertedProjection, lightWorldView, invertedWorldView, worldBaseView;

	//XMMATRIX shadowScaleBiasMatrix = new XMMATRIX
	//	(
	//	0.5f, 0.0f, 0.0f, 0.5f,
	//	0.0f, 0.5f, 0.0f, 0.5f,
	//	0.0f, 0.0f, 0.5f, 0.5f,
	//	0.0f, 0.0f, 0.0f, 1.0f
	//	);

	worldMatrix = XMMatrixIdentity(); //XMLoadFloat4x4(camera->GetWorldMatrix());//
	camera->GetOrthographicProjection(orthoMatrix);
	camera->GetViewMatrix(viewMatrix);
	camera->GetProjectionMatrix(projectionMatrix);
	lightView = XMLoadFloat4x4(&dirLight.View);
	lightProj = XMLoadFloat4x4(&dirLight.Projection);

	//XMVECTOR nullVec = ;
	viewProjection = XMMatrixMultiply(viewMatrix, projectionMatrix);

	invertedView = XMMatrixInverse(&XMVectorSplatOne(), viewMatrix);
	invertedProjection = XMMatrixInverse(&XMVectorSplatOne(), projectionMatrix);
	invertedViewProjection = XMMatrixInverse(&XMVectorSplatOne(), viewProjection);
	invertedWorldView = XMMatrixInverse(&XMVectorSplatOne(), worldMatrix*viewMatrix);

	lightWorldView = worldMatrix * lightView;
	lightWorldViewProj = worldMatrix * lightView * lightProj;

	identityWorldViewProj = worldMatrix * viewMatrix * projectionMatrix;
	worldBaseView = worldMatrix * XMLoadFloat4x4(&baseViewMatrix);


	lightWorldView =			XMMatrixTranspose(lightWorldView);
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

	worldBaseView =				XMMatrixTranspose(worldBaseView);
	//viewProjection =			XMMatrixTranspose(viewProjection);
	identityWorldViewProj =		XMMatrixTranspose(identityWorldViewProj);

	//Post-transpose matrix multiplications for the calculations that contain orthogonal projections
	worldBaseViewOrthoProj =	(worldMatrix * baseView * orthoMatrix);

	baseView = XMMatrixTranspose(baseView);
#pragma endregion

	if(!RenderShadowmap(&lightWorldViewProj, &lightWorldView, renderableBundle))
	{
		return false;
	}

	if(!RenderTwoPassGaussianBlur(&worldBaseViewOrthoProj))
	{
		return false;
	}

	if(!RenderGBuffer(&viewMatrix, &projectionMatrix, &identityWorldViewProj, renderableBundle))
	{
		return false;
	}

	if(!RenderPointLight(&viewMatrix, &invertedView, &invertedProjection, &viewProjection))
	{
		return false;
	}

	if(!RenderDirectionalLight(&viewMatrix, &worldBaseViewOrthoProj, &lightView, &lightProj, &invertedProjection))
	{
		return false;
	}

	if(!RenderComposedScene(&worldBaseViewOrthoProj, &worldBaseView, &baseView, &invertedProjection, &invertedViewProjection))
	{
		return false;
	}

	if(!RenderGUI(&worldBaseViewOrthoProj))
	{
		return false;
	}

	//This function has for the moment been moved to screenManager
	//// Present the rendered scene to the screen.
	//d3D->PresentFrame();

	return true;
}


bool GameRenderer::RenderShadowmap(XMMATRIX* lightWorldViewProj, XMMATRIX* lightWorldView, RenderableBundle* renderableBundle)
{
	//Early depth pass for shadowmap
	d3D->GetShadowManager()->SetShadowViewport();

	deviceContext->OMSetRenderTargets(1, &shadowTarget[0].p, shadowDepthStencil.p);
	deviceContext->ClearDepthStencilView(shadowDepthStencil.p, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->ClearRenderTargetView(shadowTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

	// Turn off alpha blending after rendering the text->
	d3D->GetBlendStateManager()->TurnOnDefaultBlendState();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	d3D->GetDepthStencilManager()->SetDefaultDepthStencilView();

	d3D->GetRasterizerStateManager()->SetNoCullRasterizer();

	for(unsigned int i = 0; i < renderableBundle->terrainChunks.size(); i++)
	{	
		renderableBundle->terrainChunks[i]->GetTerrainMesh()->Render(deviceContext);

		if(!depthOnlyShader.Render(deviceContext, renderableBundle->terrainChunks[i]->GetTerrainMesh()->GetIndexCount(), lightWorldViewProj, lightWorldView))
		{
			return false;
		}
	}

	return true;
}

//TODO: change this function to just take two generic render targets and a depth stencil, as to make it possible to blur anything
bool GameRenderer::RenderTwoPassGaussianBlur(XMMATRIX* worldBaseViewOrthoProj )
{
	//Shadow map blur stage
	//Change render target to prepare for ping-ponging
	deviceContext->OMSetRenderTargets(1, &gaussianBlurPingPongRTView[0].p, shadowDepthStencil.p);
	deviceContext->ClearRenderTargetView(gaussianBlurPingPongRTView[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
	deviceContext->ClearDepthStencilView(shadowDepthStencil.p, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Blur shadow map texture horizontally
	if(!fullScreenQuad.Render(deviceContext, 0, 0))
	{
		return false;
	}

	if(!gaussianBlurShader.RenderBlurX(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, &dirLightTextures[2].p))
	{
		return false;
	}

	//Change render target back to our shadow map to render the second blur and get the final result
	deviceContext->OMSetRenderTargets(1, &shadowTarget[0].p, shadowDepthStencil.p);
	deviceContext->ClearDepthStencilView(shadowDepthStencil.p, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Blur shadow map texture vertically
	if(!fullScreenQuad.Render(deviceContext, 0, 0))
	{
		return false;
	}

	if(!gaussianBlurShader.RenderBlurY(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, &gaussianBlurTexture[0].p))
	{
		return false;
	}

	return true;
}


bool GameRenderer::RenderGBuffer(XMMATRIX* viewMatrix, XMMATRIX* projectionMatrix, XMMATRIX* identityWorldViewProj, RenderableBundle* renderableBundle)
{
	XMMATRIX worldViewProjMatrix, worldMatrix, worldView, view, proj;

	//GBuffer building stage
	d3D->SetDefaultViewport();

	//depthStencil.p = d3D->GetDepthStencilManager()->GetDepthStencilView();
	deviceContext->OMSetRenderTargets(3, &gbufferRenderTargets[0].p, depthStencil);
	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->ClearRenderTargetView(gbufferRenderTargets[0], D3DXVECTOR4(0.0f, 0.125f, 0.3f, 1.0f));
	deviceContext->ClearRenderTargetView(gbufferRenderTargets[1], D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.5f));
	deviceContext->ClearRenderTargetView(gbufferRenderTargets[2], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));

	d3D->GetRasterizerStateManager()->SetNoCullRasterizer();
	d3D->GetDepthStencilManager()->SetDepthDisabledStencilState();

	worldMatrix = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);

	//Scale skysphere by 3.0f because camera nearClip is 2.0f. Nearclip is 2.0f because else I get precision issues when rendering water.
	worldViewProjMatrix = (XMMatrixScaling(3.0f, 3.0f, 3.0f) * worldMatrix) * ((*viewMatrix) * (*projectionMatrix));
	worldViewProjMatrix = XMMatrixTranspose(worldViewProjMatrix);

	skySphere.Render(deviceContext, &worldViewProjMatrix, camPos.y, &dayNightCycle.GetAmbientLightColor(), timeOfDay);

	d3D->GetDepthStencilManager()->SetDefaultDepthStencilView();
	d3D->GetRasterizerStateManager()->SetBackFaceCullingRasterizer();
	d3D->GetBlendStateManager()->TurnOnDefaultBlendState();

	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//worldMatrix = (XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMLoadFloat4x4((&renderableBundle->testSphere.world)));
	//worldMatrix = XMMatrixTranspose(worldMatrix);
	//view = XMMatrixTranspose(*viewMatrix);
	//proj = XMMatrixTranspose(*projectionMatrix);

	//renderableBundle->testSphere.mesh.Render(deviceContext);

	//gbufferShader.Render(deviceContext, renderableBundle->testSphere.mesh.GetIndexCount(), &worldMatrix, &view, &proj, sphereModel.GetTexture(), farClip);

	worldMatrix = XMMatrixIdentity();
	worldView = XMMatrixTranspose(XMMatrixMultiply(worldMatrix, (*viewMatrix)));
	worldMatrix = XMMatrixTranspose(worldMatrix);

	if(drawWireFrame)
	{	
		d3D->GetRasterizerStateManager()->SetWireframeRasterizer();
	}

	unsigned int vecSize = renderableBundle->terrainChunks.size();

	const auto& chunks = renderableBundle->terrainChunks;

	for(unsigned int i = 0; i < vecSize; i++)
	{	
		chunks[i]->GetTerrainMesh()->Render(deviceContext);

		if(!mcubeShader.Render(d3D->GetDeviceContext(),  chunks[i]->GetTerrainMesh()->GetIndexCount(), &worldMatrix, &worldView, 
			identityWorldViewProj, textureAndMaterialHandler.GetTerrainTextureArray(), textureAndMaterialHandler.GetMaterialLookupTexture(), toggleColorMode))
		{
			return false;
		}
	}

	d3D->GetBlendStateManager()->TurnOnTransparencyBlending();
	d3D->GetRasterizerStateManager()->SetBackFaceCullingRasterizer();

	for(unsigned int i = 0; i < vecSize; i++)
	{	
		chunks[i]->GetWaterMesh()->Render(deviceContext);

		if(!waterShader.Render(d3D->GetDeviceContext(),  chunks[i]->GetWaterMesh()->GetIndexCount(), &worldMatrix, &worldView, identityWorldViewProj, 
			textureAndMaterialHandler.GetVegetationTextureArray(), textureAndMaterialHandler.GetWindTexture(), textureAndMaterialHandler.GetWindNormalMap(), 
			textureOffsetDeltaTime, &windDir))
		{
			return false;
		}
	}

	d3D->GetBlendStateManager()->TurnOnAlphaBlending();
	d3D->GetRasterizerStateManager()->SetNoCullRasterizer();

	for(unsigned int i = 0; i < vecSize; i++)
	{	
		chunks[i]->GetTerrainMesh()->Render(deviceContext);

		if(!geometryShaderGrass.Render(d3D->GetDeviceContext(),  chunks[i]->GetTerrainMesh()->GetIndexCount(), &worldMatrix, &worldView, 
			identityWorldViewProj, textureAndMaterialHandler.GetVegetationTextureArray(), textureAndMaterialHandler.GetMaterialLookupTexture(), 
			textureAndMaterialHandler.GetWindTexture(), toggleColorMode, textureOffsetDeltaTime, &windDir))
		{
			return false;
		}
	}

	//This is where we'll render trees.
	auto& objModels = renderableBundle->objModels;
	vecSize = renderableBundle->objModels.size();

	//Should be temporary ...
	bool textureNeedsUpdate = true;
	bool materialNeedsUpdate = true;

	XMMATRIX tempView = XMMatrixTranspose(*viewMatrix);
	XMMATRIX tempProj = XMMatrixTranspose(*projectionMatrix);

	for(unsigned int i = 0; i < vecSize; ++i)
	{
		auto& model = renderableBundle->objModels[i];
		IndexedMesh* tempMesh = meshHandler->GetMesh(model.GetMeshHandle());

		//Retarded little hack to see viability of rendering many trees
		for(int k = 0; k < numtrees; ++k)
		{
			worldMatrix = XMMatrixTranspose(XMLoadFloat4x4(&(treeMatrices.at(k))));
			objModelShader.UpdateMatrixBuffer(deviceContext, &worldMatrix, &tempView, &tempProj);

			for(int j = 0; j < model.GetSubsetCount()-1; ++j)
			{
				//See if we need to update texture
				auto tex = textureAndMaterialHandler.GetTexture(model.GetTextureHandles().at(j));

				//See if we need to update material
				auto mat = textureAndMaterialHandler.GetMaterial(model.GetMaterialHandles().at(j));

				if(textureNeedsUpdate)
				{
					objModelShader.SetNewTexture(deviceContext, tex, 1);
				}

				if(materialNeedsUpdate)
				{
					objModelShader.SetNewMaterial(deviceContext, mat);
				}

				tempMesh->Render(deviceContext);

				//This will always work, because when we load in the model we add one last set of index subsets that contains the entire count (the end of the indices, essentially).
				int indexCount = model.GetSubSetIndices().at(j+1) - model.GetSubSetIndices().at(j);

				objModelShader.RenderShader(deviceContext, indexCount, model.GetSubSetIndices().at(j));
			}
		}
	}

	d3D->GetBlendStateManager()->TurnOnDefaultBlendState();
	d3D->GetRasterizerStateManager()->SetBackFaceCullingRasterizer();

	return true;
}


bool GameRenderer::RenderPointLight(XMMATRIX* view, XMMATRIX* invertedView, XMMATRIX* invertedProjection, XMMATRIX* viewProjection )
{
	XMMATRIX world, worldView, worldViewProj;

	//Point light stage
	deviceContext->OMSetRenderTargets(1, &lightTarget[0].p, depthStencil);
	deviceContext->ClearRenderTargetView(lightTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_STENCIL, 1.0f, 0);

	d3D->GetBlendStateManager()->TurnOnLightBlending();

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
			d3D->GetDepthStencilManager()->SetLightStencilMethod1Phase1();
			d3D->GetRasterizerStateManager()->SetNoCullRasterizer();

			sphereModel.Render(deviceContext);

			if(!vertexOnlyShader.Render(deviceContext, sphereModel.GetIndexCount(), &worldViewProj))
			{
				return false;
			}

			//Phase two, draw sphere with light algorithm
			d3D->GetDepthStencilManager()->SetLightStencilMethod1Phase2();
			d3D->GetRasterizerStateManager()->SetFrontFaceCullingRasterizer();

			sphereModel.Render(deviceContext);

			if(!pointLightShader.Render(deviceContext, sphereModel.GetIndexCount(), &worldViewProj, &worldView, &world, invertedView, 
				invertedProjection, &pointLights[i], &gbufferTextures[0].p, textureAndMaterialHandler.GetMaterialTextureArray(), camPos))
			{
				return false;
			}

			//if(!textureShader.Render(deviceContext, sphereModel->GetIndexCount(), &worldViewProj, sphereModel->GetTexture()))
			//{
			//	return false;
			//}

			deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
	}

	return true;
}


bool GameRenderer::RenderDirectionalLight(XMMATRIX* viewMatrix, XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* lightView, XMMATRIX* lightProj, XMMATRIX* invertedProjection )
{
	XMMATRIX worldMatrix, worldView, invertedWorldView, invertedView;

	//Directional light stage
	/*TODO: Create a directional light stencilstate that does a NOTEQUAL==0 stencil check.*/

	d3D->GetDepthStencilManager()->SetDepthDisabledStencilState();
	d3D->GetRasterizerStateManager()->SetNoCullRasterizer();

	deviceContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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

	if(!dirLightShader.Render(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, &worldView, &worldMatrix, viewMatrix, 
		&invertedView, invertedProjection, lightView, lightProj, &dirLightTextures[0].p, textureAndMaterialHandler.GetMaterialTextureArray(), 
		camPos, &dirLight, dayNightCycle.GetAmbientLightColor()))
	{
		return false;
	}

	return true;
}

bool GameRenderer::RenderComposedScene(XMMATRIX* worldBaseViewOrthoProj, XMMATRIX* worldView, XMMATRIX* view, XMMATRIX* invertedProjection, XMMATRIX* invertedViewProjection )
{
	//Render final composed scene that is the sum of all the previous scene
	d3D->ResetBackBufferRenderTarget();

	//d3D->GetDepthStencilManager()->SetDefaultDepthStencilView();
	d3D->GetBlendStateManager()->TurnOnDefaultBlendState();
	deviceContext->ClearDepthStencilView(depthStencil,  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if(!fullScreenQuad.Render(deviceContext, 0, 0))
	{
		return false;
	}

	if(!composeShader.Render(deviceContext, fullScreenQuad.GetIndexCount(), worldBaseViewOrthoProj, worldView, view, invertedProjection, 
		invertedViewProjection, &dayNightCycle.GetAmbientLightColor(), fogMinimum, &finalTextures[0].p, *textureAndMaterialHandler.GetSSAORandomTexture(), toggleSSAO))
	{
		return false;
	}

	return true;
}


bool GameRenderer::RenderGUI(XMMATRIX* worldBaseViewOrthoProj )
{
	XMMATRIX worldMatrix;

	deviceContext->ClearDepthStencilView(depthStencil,  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	worldMatrix = XMMatrixIdentity();
	worldMatrix = XMMatrixTranspose(worldMatrix);

	if(toggleDebugInfo)
	{

		d3D->GetDepthStencilManager()->SetDepthDisabledStencilState();

		d3D->GetBlendStateManager()->TurnOnAlphaBlending();

		for(int i = 0; i < 3; i++)
		{
			if(!debugWindows[i].Render(deviceContext, 200+200*i, 0))
			{
				return false;
			}

			if(!textureShader.Render(deviceContext, debugWindows[i].GetIndexCount(), 
				worldBaseViewOrthoProj, gbufferTextures[i]))
			{
				return false;
			}
		}

		if(!debugWindows[4].Render(deviceContext, 800, 0))
		{
			return false;
		}

		if(!textureShader.Render(deviceContext, debugWindows[4].GetIndexCount(), 
			worldBaseViewOrthoProj, shadowRT.SRView))
		{
			return false;
		}

		if(!debugWindows[3].Render(deviceContext, 200, 200))
		{
			return false;
		}

		if(!textureShader.Render(deviceContext, debugWindows[3].GetIndexCount(), 
			worldBaseViewOrthoProj, lightRT.SRView))
		{
			return false;
		}

		if(!debugWindows[5].Render(deviceContext, 400, 200))
		{
			return false;
		}

		if(!textureShader.Render(deviceContext, debugWindows[5].GetIndexCount(), 
			worldBaseViewOrthoProj, *textureAndMaterialHandler.GetWindNormalMap()))
		{
			return false;
		}

		if(!debugWindows[6].Render(deviceContext, 600, 200))
		{
			return false;
		}

		if(!textureShader.Render(deviceContext, debugWindows[6].GetIndexCount(), 
			worldBaseViewOrthoProj, *textureAndMaterialHandler.GetWindTexture()))
		{
			return false;
		}
	}

	// Turn off alpha blending after rendering the text->
	d3D->GetBlendStateManager()->TurnOnDefaultBlendState();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	d3D->GetDepthStencilManager()->SetDefaultDepthStencilView();

	return true;
}


void GameRenderer::Shutdown()
{
	return;
}

void GameRenderer::OnSettingsReload(Config* cfg)
{
	const Setting& settings = cfg->getRoot()["rendering"];

	//let's NOT do this. take all of this from d3dManager, which in turns gets it from engine where it is decided on startup

	settings.lookupValue("windowWidth", screenWidth);
	settings.lookupValue("windowHeight", screenHeight);

	//result = dayNightCycle.Initialize(500.0f, DAY); //86400.0f/6 <-- This is realistic day/night cycle. 86400 seconds in a day.
	//if(!result)
	//{
	//	return false;
	//}

	////dayNightCycle->Update(50.0f, dirLight, skySphere);

	//for(int i = 0; i < 7; i++)
	//{
	//	debugWindows[i].Initialize(d3D->GetDevice(), screenWidth, screenHeight, 200, 200);
	//}

	//fullScreenQuad.Initialize(d3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);

	//colorRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	//normalRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	//depthRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R32_FLOAT);
	//lightRT.Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	//shadowRT.Initialize(d3D->GetDevice(), shadowMapWidth, shadowMapHeight, DXGI_FORMAT_R32G32_FLOAT);
	//gaussianBlurPingPongRT.Initialize(d3D->GetDevice(), shadowMapWidth, shadowMapHeight, DXGI_FORMAT_R32G32_FLOAT); //Needs to be identical to shadowRT
}

#pragma region Hidden stuff.. for now

//if(inputManager->WasKeyPressed(DIK_U))
//{
//	textureAndMaterialHandler.RebuildRandom2DTexture(d3D->GetDevice(), d3D->GetDeviceContext());
//}
//
//if(inputManager->WasKeyPressed(DIK_I))
//{
//	textureAndMaterialHandler.RebuildMirroredSimplex2DTexture(d3D->GetDevice(), d3D->GetDeviceContext());
//}
//
//if(inputManager->WasKeyPressed(DIK_O))
//{
//	textureAndMaterialHandler.RebuildSeamlessSimplex2DTexture(d3D->GetDevice(), d3D->GetDeviceContext(), 0, 0, 100, 100);
//}
//
//if(inputManager->WasKeyPressed(DIK_J))
//{
//	xPos += 30.0f;
//
//	textureAndMaterialHandler.RebuildTexture(d3D->GetDevice(), d3D->GetDeviceContext(), TEXTURE_GRASS, 1024, 1024, xPos, yPos, true);
//}
//
//if(inputManager->WasKeyPressed(DIK_K))
//{
//	yPos += 30.0f;
//
//	textureAndMaterialHandler.RebuildTexture(d3D->GetDevice(), d3D->GetDeviceContext(), TEXTURE_DIRT, 1024, 1024, xPos, yPos, true);
//}
//
//if(inputManager->WasKeyPressed(DIK_P))
//{
//	//Create and initialize our time... things.
//	const time_t timeObject = time(NULL);
//	struct tm parts;
//	localtime_s(&parts, &timeObject );
//
//	std::ostringstream std::stringStream;
//
//	//Create the std::string that will hold the screenshot's name when it gets pooped out into the directory
//	std::stringStream << "SavedTexture_" << (1+parts.tm_mon) << "-" << parts.tm_mday <<  "-" << parts.tm_min << "-" << parts.tm_sec << ".bmp";
//
//	LPCSTR fileName;
//	std::string temp = std::stringStream.str();
//	fileName = (temp).c_str();
//
//	textureAndMaterialHandler.SaveTextureToFile(d3D->GetDeviceContext(), *textureAndMaterialHandler.GetNoiseTexture(), D3DX11_IFF_BMP, fileName);
//}
//
//if(inputManager->WasKeyPressed(DIK_L))
//{
//	//Create and initialize our time... things.
//	const time_t timeObject = time(NULL);
//	struct tm parts;
//	localtime_s(&parts, &timeObject );
//
//	std::ostringstream std::stringStream;
//
//	//Create the std::string that will hold the screenshot's name when it gets pooped out into the directory
//	std::stringStream << "SavedTexture_" << (1+parts.tm_mon) << "-" << parts.tm_mday <<  "-" << parts.tm_min << "-" << parts.tm_sec << ".bmp";
//
//	LPCSTR fileName;
//	std::string temp = stringStream.str();
//	fileName = (temp).c_str();
//
//	textureAndMaterialHandler.SaveTextureToFile(d3D->GetDeviceContext(), *textureAndMaterialHandler.GetTerrainTexture(), D3DX11_IFF_BMP, fileName);
//}

#pragma endregion