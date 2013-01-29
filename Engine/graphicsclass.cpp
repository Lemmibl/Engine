////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

CameraClass* GraphicsClass::GetCamera()
{
	return camera;
}

GraphicsClass::GraphicsClass()
{
	d3D = 0;
	camera  = 0;
	//terrain = 0;
	colorShader = 0;
	text = 0;
	toggleDebugInfo = true;
	//terrainShader = 0;

	frustum = 0;
	//quadTree = 0;
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

	colorRT = 0;
	normalRT = 0;
	depthRT = 0;
	lightRT = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	srand((unsigned int)time(NULL));
	toggleTextureShader = false;
	bool result;

	shadowMapWidth = screenWidth;
	shadowMapHeight = screenHeight;

	// Create the Direct3D object.
	d3D = new D3DClass;
	if(!d3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = d3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR, shadowMapWidth, shadowMapHeight);

	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D. Look in graphicsclass.", L"Error", MB_OK);
		return false;
	}


	// Create the camera object.
	camera = new CameraClass;
	if(!camera)
	{
		MessageBox(hwnd, L"Could not create the camera object. Look in graphicsclass.", L"Error", MB_OK);
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	camera->SetPosition(0.0f, 0.0f, -1.0f);
	camera->Render();
	camera->GetViewMatrix(baseViewMatrix);

	camera->SetPosition(0.0f, 10.0f, -10.0f);
	camera->SetRotation(45.0f, 0.0f, 0.0f);

	camera->SetPerspectiveProjection(screenWidth, screenHeight, (float)D3DX_PI / 4.0f, SCREEN_NEAR, SCREEN_FAR); 


	// Create the model object.
	groundModel = new ModelClass;
	if(!groundModel)
	{
		return false;
	}

	// Initialize the model object.
	result = groundModel->Initialize(d3D->GetDevice(), "../Engine/data/ground.txt", L"../Engine/data/ground_diffuse.dds", L"../Engine/data/ground_normal.dds", L"../Engine/data/ground_specular.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the model object.
	sphereModel = new ModelClass;
	if(!sphereModel)
	{
		return false;
	}

	// Initialize the model object.
	result = sphereModel->Initialize(d3D->GetDevice(), "../Engine/data/sphere2.txt", L"../Engine/data/stone02.dds", L"../Engine/data/bump02.dds", L"../Engine/data/stone_specmap.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	otherModel = new ModelClass;
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

	colorShader = new ColorShaderClass();
	if(!colorShader)
	{
		return false;
	}

	result = colorShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object. Look in graphicsclass.", L"Error", MB_OK);
		return false;
	}

	// Create the text object.
	text = new TextClass;
	if(!text)
	{
		return false;
	}

	// Initialize the text object.
	result = text->Initialize(d3D->GetDevice(), d3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object. Look in graphicsclass.", L"Error", MB_OK);
		return false;
	}

	//terrain = new TerrainClass();
	//if(!terrain)
	//{
	//	return false;
	//}

	//result = terrain->Initialize(d3D->GetDevice(), 100, 100, 5.0f, "../Engine/data/heightmap.bmp", L"../Engine/data/dirt.dds");
	//if(!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the terrain object. Look in graphicsclass.", L"Error", MB_OK);
	//	return false;
	//}

	//// Create the terrain shader object.
	//terrainShader = new TerrainShaderClass;
	//if(!terrainShader)
	//{
	//	return false;
	//}

	//// Initialize the terrain shader object.
	//result = terrainShader->Initialize(d3D->GetDevice(), hwnd);
	//if(!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the terrain shader object.", L"Error", MB_OK);
	//	return false;
	//}

	// Create the frustum object.
	frustum = new FrustumClass;
	if(!frustum)
	{
		return false;
	}

	//// Create the quad tree object.
	//quadTree = new QuadTreeClass;
	//if(!quadTree)
	//{
	//	return false;
	//}

	//// Initialize the quad tree object.
	//result = quadTree->Initialize(terrain, d3D->GetDevice());
	//if(!result)
	//{
	//	MessageBox(hwnd, L"Could not initialize the quad tree object.", L"Error", MB_OK);
	//	return false;
	//}

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

	composeShader = new DRFinalComposition();
	if(!composeShader)
	{
		return false;
	}

	result = composeShader->Initialize(d3D->GetDevice(), hwnd);
	if(!result)
	{
		return false;
	}

	for(int i = 0; i < 5; i++)
	{
		debugWindows[i].Initialize(d3D->GetDevice(), screenWidth, screenHeight, 200, 200);
	}

	#pragma region Point light initialization
	float x, y, z;
	x = -6.0f;
	z = -4.0f;
	y = -2.0f;

	float pointLightRadius = 2.0f;
	D3DXMatrixScaling(&scale, pointLightRadius, pointLightRadius, pointLightRadius);

	for(int i = 0; i < 20; i++)
	{
		pointLights.push_back(new PointLight());
		pointLights[i]->Position = D3DXVECTOR3(x, y, z);
		pointLights[i]->Color = D3DXVECTOR3(0.3f + i%4, 0.7f + i % 2, 0.2f + i%3);
		pointLights[i]->Radius = pointLightRadius;
		pointLights[i]->Intensity = 2.0f;

		x += 4.0f;

		if(x >= 10.0f) //Every 10th light gets reseted in x and z plane.
		{
			x = -6.0f;
			z += 4.0f;
		}

		if(i != 0 && i % 100 == 0) //Every 100 pointlights we reset make another layer that is 5Y higher up.
		{
			x = -10.0f;
			z = -10.0f;
			y += 5.0f;
		}

		D3DXMatrixTranslation(&translation, pointLights[i]->Position.x, pointLights[i]->Position.y, pointLights[i]->Position.z);
		pointLights[i]->World = scale * translation;
	}
	#pragma endregion

	ambientLight = D3DXVECTOR4(1.0f, 0.3f, 0.3f, 1.0f);

	/************************************************************************/
	/* TODO: Look up http://stackoverflow.com/questions/35950/i-dont-understand-stdtr1unordered-map  */
	/************************************************************************/
	
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

	D3DXVECTOR3 lookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f); //LookAt for dir light. We always want this to be (0,0,0), because it's the easiest to visualize.
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	// Initialize the directional light.
	dirLight->Color = D3DXVECTOR4(0.7f, 0.7f, 0.7f, 1.0f);
	dirLight->Intensity = 128.0f;
	dirLight->Position = D3DXVECTOR3(-5.0f, 30.0f, 0.0f);

	D3DXVECTOR3 direction = lookAt - dirLight->Position;
	D3DXVec3Normalize(&direction, &direction);
	dirLight->Direction = direction;

	//dirLight->Projection = *D3DXMatrixPerspectiveFovLH(&dirLight->Projection, D3DX_PI/2.0f, 1.0f, 5.0f, 140.0f);
	D3DXMatrixOrthoLH(&dirLight->Projection, (float)shadowMapWidth, (float)shadowMapHeight, 5.0f, 100.0f);
	D3DXMatrixLookAtLH(&dirLight->View, &dirLight->Position, &lookAt, &up); //Generate light view matrix
	#pragma endregion

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

	defaultModelMaterial.a = 128.0f;
	defaultModelMaterial.Ka = 0.1f;
	defaultModelMaterial.Kd = 1.0f;
	defaultModelMaterial.Ks = 0.3f;

	fullScreenQuad.Initialize(d3D->GetDevice(), screenWidth, screenHeight, screenWidth, screenHeight);

	colorRT = new RenderTarget2D();
	normalRT = new RenderTarget2D();
	depthRT = new RenderTarget2D();

	lightRT = new RenderTarget2D();

	colorRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	normalRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
	depthRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R32_FLOAT);

	lightRT->Initialize(d3D->GetDevice(), screenWidth, screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

	return true;
}

bool GraphicsClass::Frame(int fps, int cpu, float frameTime, bool toggle, bool left, bool right)
{
	bool result;

	if(toggle) //We toggle if key has been pressed (toggle == true).
	{
		toggleDebugInfo = false;
	}
	else
	{
		toggleDebugInfo = true;
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

	D3DXVECTOR3 temp = camera->GetPosition();
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


	if(left)
	{
		for(unsigned int i = 0; i < pointLights.size(); i++)
		{
			pointLights[i]->Position.y += frameTime*0.01f;

			D3DXMatrixTranslation(&translation, pointLights[i]->Position.x, pointLights[i]->Position.y, pointLights[i]->Position.z);
			pointLights[i]->World = scale * translation;
		}
	}

	if(right)
	{
		for(unsigned int i = 0; i < pointLights.size(); i++)
		{
			pointLights[i]->Position.y -= frameTime*0.01f;

			D3DXMatrixTranslation(&translation, pointLights[i]->Position.x, pointLights[i]->Position.y, pointLights[i]->Position.z);
			pointLights[i]->World = scale * translation;
		}
	}

	return true;
}

bool GraphicsClass::Render()
{
	// Clear the scene.
	d3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	#pragma region Preparation
	ID3D11DeviceContext* context;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, scalingMatrix, viewProjection, invertedViewProjection, lightView, lightProj;
	float positionX, positionY, positionZ, radius;
	D3DXVECTOR4 color;
	D3DXVECTOR3 camPos;
	bool result, renderModel;
	int modelCount, renderCount;
	ID3D11RenderTargetView* gbufferRenderTargets[3] = { NULL }; //render targets for GBuffer pass
	ID3D11RenderTargetView* lightTarget[1] = { NULL };

	ID3D11ShaderResourceView* gbufferTextures[3] = { NULL, NULL, NULL };
	ID3D11ShaderResourceView* dirLightTextures[3] = { NULL, NULL, NULL };
	ID3D11ShaderResourceView* finalTextures[2] = { NULL, NULL };

	ID3D11ShaderResourceView* lightMap = NULL;

	ID3D11DepthStencilView* shadowDS = d3D->GetShadowmapDSV();
	ID3D11DepthStencilView* ds;

	camPos = camera->GetPosition();

	gbufferRenderTargets[0] = colorRT->RTView;
	gbufferRenderTargets[1] = normalRT->RTView;
	gbufferRenderTargets[2] = depthRT->RTView;

	lightTarget[0] = lightRT->RTView; //For lighting pass

	gbufferTextures[0] = colorRT->SRView; //For GBuffer pass
	gbufferTextures[1] = normalRT->SRView;
	gbufferTextures[2] = depthRT->SRView;

	dirLightTextures[0] = d3D->GetShadowmapSRV(); //For GBuffer pass
	dirLightTextures[1] = normalRT->SRView;
	dirLightTextures[2] = depthRT->SRView;

	finalTextures[0] = colorRT->SRView; //For the the final composition pass
	finalTextures[1] = lightRT->SRView;

	context = d3D->GetDeviceContext();

	// Generate the view matrix based on the camera's position.
	camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	d3D->GetWorldMatrix(worldMatrix);
	d3D->GetOrthoMatrix(orthoMatrix);
	camera->GetViewMatrix(viewMatrix);
	camera->GetProjectionMatrix(projectionMatrix);
	lightView = dirLight->View;
	lightProj = dirLight->Projection;

	//D3DXMatrixMultiply(&lightViewProj, &dirLight->View, &dirLight->Projection);
	D3DXMatrixMultiply(&viewProjection, &viewMatrix, &projectionMatrix);
	D3DXMatrixInverse(&invertedViewProjection, NULL, &viewProjection);

	// Construct the frustum.
	frustum->ConstructFrustum(SCREEN_FAR, projectionMatrix, viewMatrix);

	// Get the number of models that will be rendered.
	modelCount = modelList->GetModelCount();

	// Initialize the count of models that have been rendered.
	renderCount = 0;
	#pragma endregion

	#pragma region Early depth pass for shadowmap
	context->OMSetRenderTargets(0, 0, shadowDS);
	d3D->SetShadowViewport();
	d3D->SetBackFaceCullingRasterizer();
	context->ClearDepthStencilView(shadowDS, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Move the model to the location it should be rendered at.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, -10.0f, 0.0f);
	D3DXMatrixScaling(&scalingMatrix, 0.2f, 0.2f, 0.2f);

	worldMatrix = scalingMatrix * worldMatrix;

	groundModel->Render(context);
	result = depthOnlyShader->Render(context, groundModel->GetIndexCount(), worldMatrix, lightView, lightProj);
	if(!result)
	{
		return false;
	}

	// Go through all the models and render them only if they can be seen by the camera view.
	for(int i =0; i < modelCount; i++)
	{
		// Get the position and color of the sphere model at this index.
		modelList->GetData(i, positionX, positionY, positionZ, color);

		// Move the model to the location it should be rendered at.
		D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ); 

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		otherModel->Render(context);

		// Render the model using the gbuffer shader.
		result = depthOnlyShader->Render(context, otherModel->GetIndexCount(), worldMatrix, lightView, lightProj);
		if(!result)
		{
			return false;
		}
	}
	#pragma endregion

	#pragma region GBuffer building stage
	d3D->SetDefaultViewport();
	ds = d3D->GetDepthStencilView();
	context->OMSetRenderTargets(3, gbufferRenderTargets, ds);
	context->ClearDepthStencilView(ds, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->ClearRenderTargetView(gbufferRenderTargets[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	context->ClearRenderTargetView(gbufferRenderTargets[1], D3DXVECTOR4(0.5f, 0.5f, 0.5f, 0.0f)); //Will result in an average grey color, which is kind of the default state of normals.
	context->ClearRenderTargetView(gbufferRenderTargets[2], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

	// Move the model to the location it should be rendered at.
	D3DXMatrixTranslation(&worldMatrix, 0.0f, -10.0f, 0.0f);
	D3DXMatrixScaling(&scalingMatrix, 0.2f, 0.2f, 0.2f);

	worldMatrix *= scalingMatrix;

	groundModel->Render(context);
	gbufferShader->Render(context, groundModel->GetIndexCount(), worldMatrix, camera->GetView(), projectionMatrix, groundModel->GetTextureArray());

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
			D3DXMatrixTranslation(&worldMatrix, positionX, positionY, positionZ); 

			// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
			otherModel->Render(context);

			// Render the model using the gbuffer shader.
			result = gbufferShader->Render(context, otherModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, otherModel->GetTextureArray());
			if(!result)
			{
				return false;
			}

			// Since this model was rendered then increase the count for this frame.
			renderCount++;
		}
	}

	text->SetRenderCount(renderCount, context);
	#pragma endregion

	#pragma region Point light stage
	context->OMSetRenderTargets(1, lightTarget, ds);
	context->ClearRenderTargetView(lightTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));
	context->ClearDepthStencilView(ds, D3D11_CLEAR_STENCIL, 1.0f, 0);

	#pragma region Old pointlight code
	////Phase one, draw sphere with vertex-only shader.
	//d3D->SetLightStencilMethod2Phase1();
	//d3D->SetBackFaceCullingRasterizer();
	//d3D->TurnOffColorBlending();

	//for(unsigned int i = 0; i < pointLights.size(); i++)
	//{	
	//	sphereModel->Render(context);

	//	result = vertexOnlyShader->Render(context, sphereModel->GetIndexCount(), pointLights[i]->World, viewMatrix, 
	//		projectionMatrix);
	//	if(!result)
	//	{
	//		return false;
	//	}
	//}

	//context->OMSetRenderTargets(1, lightTarget, ds);
	//context->ClearRenderTargetView(lightTarget[0], D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f));

	////Phase two, draw sphere with light algorithm
	//d3D->SetLightStencilMethod2Phase2();
	//d3D->SetFrontFaceCullingRasterizer();
	//d3D->TurnOnLightBlending();

	//for(unsigned int i = 0; i < pointLights.size(); i++)
	//{	
	//	sphereModel->Render(context);

	//	if(!toggleDebugInfo)
	//	{
	//		result = textureShader->Render(context, sphereModel->GetIndexCount(), pointLights[i]->World, viewMatrix, 
	//			projectionMatrix, sphereModel->GetTexture());
	//	}
	//	else
	//	{
	//		result = pointLightShader->Render(context, sphereModel->GetIndexCount(), pointLights[i]->World, viewMatrix, 
	//			projectionMatrix, invertedViewProjection, pointLights[i], gbufferTextures, camera->GetPosition());
	//	}

	//	if(!result)
	//	{
	//		return false;
	//	}
	//}
	#pragma endregion

	//Phase one, draw sphere with vertex-only shader.
	d3D->TurnOnLightBlending();

	for(unsigned int i = 0; i < pointLights.size(); i++)
	{	
		d3D->SetLightStencilMethod1Phase1();
		d3D->SetNoCullRasterizer();

		sphereModel->Render(context);

		result = vertexOnlyShader->Render(context, sphereModel->GetIndexCount(), pointLights[i]->World, viewMatrix, 
			projectionMatrix);

		//Phase two, draw sphere with light algorithm
		d3D->SetLightStencilMethod1Phase2();
		d3D->SetFrontFaceCullingRasterizer();

		sphereModel->Render(context);

		if(!toggleDebugInfo)
		{
			result = textureShader->Render(context, sphereModel->GetIndexCount(), pointLights[i]->World, viewMatrix, 
				projectionMatrix, sphereModel->GetTexture());
		}
		else
		{
			result = pointLightShader->Render(context, sphereModel->GetIndexCount(), pointLights[i]->World, viewMatrix, 
				projectionMatrix, invertedViewProjection, pointLights[i], gbufferTextures, camera->GetPosition());
		}

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
	d3D->TurnOnLightBlending();
	d3D->SetBackFaceCullingRasterizer();
	d3D->GetWorldMatrix(worldMatrix);

	fullScreenQuad.Render(context, 0, 0);

	result = dirLightShader->Render(context, fullScreenQuad.GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, invertedViewProjection, 
		dirLightTextures, camPos, dirLight->Position, dirLight->Direction, dirLight->Color, dirLight->Intensity, ambientLight, defaultModelMaterial, lightView, lightProj);
	if(!result)
	{
		return false;
	}
	#pragma endregion

	#pragma region Final compose stage
	d3D->SetBackBufferRenderTarget();
	context->ClearDepthStencilView(ds,  D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	fullScreenQuad.Render(context, 0, 0);

	composeShader->Render(context, fullScreenQuad.GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix, finalTextures);
	#pragma endregion

	#pragma region Debug and text stage
	d3D->ResetRasterizerState();
	d3D->ResetBlendState();
	ds = d3D->GetDepthStencilView(); //This also resets the depth stencil state to "default".
	context->ClearDepthStencilView(ds,  D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	d3D->GetWorldMatrix(worldMatrix);

	//for(int i = 0; i < 3; i++)
	//{
	//	result = debugWindows[i].Render(d3D->GetDeviceContext(), 200+200*i, 0);
	//	if(!result)
	//	{
	//		return false;
	//	}

	//	result = textureShader->Render(d3D->GetDeviceContext(), debugWindows[i].GetIndexCount(), 
	//		worldMatrix, baseViewMatrix, orthoMatrix, gbufferTextures[i]);
	//	if(!result)
	//	{
	//		return false;
	//	}
	//}

	//result = debugWindows[3].Render(d3D->GetDeviceContext(), 200, 200);
	//if(!result)
	//{
	//	return false;
	//}

	//result = textureShader->Render(d3D->GetDeviceContext(), debugWindows[3].GetIndexCount(), 
	//	worldMatrix, baseViewMatrix, orthoMatrix, lightRT->SRView);
	//if(!result)
	//{
	//	return false;
	//}

	result = debugWindows[4].Render(d3D->GetDeviceContext(), 800, 0);
	if(!result)
	{
		return false;
	}

	result = textureShader->Render(d3D->GetDeviceContext(), debugWindows[4].GetIndexCount(), 
		worldMatrix, baseViewMatrix, orthoMatrix, dirLightTextures[0]);
	if(!result)
	{
		return false;
	}


	d3D->TurnZBufferOff();

	d3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	result = text->Render(d3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	d3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	d3D->TurnZBufferOn();
	#pragma endregion

	// Present the rendered scene to the screen.
	d3D->EndScene();

	return true;
}

void GraphicsClass::Shutdown()
{
	if(d3D)
	{
		d3D->Shutdown();
		delete d3D;
		d3D = 0;
	}

	if(camera)
	{
		delete camera;
		camera = 0;
	}

	if(colorShader)
	{
		colorShader->Shutdown();
		delete colorShader;
		colorShader = 0;
	}

	if (text)
	{
		text->Shutdown();
		delete text;
		text = 0;
	}

	//if(terrain)
	//{
	//	terrain->Shutdown();
	//	delete terrain;
	//	terrain = 0;
	//}

	//// Release the terrain shader object.
	//if(terrainShader)
	//{
	//	terrainShader->Shutdown();
	//	delete terrainShader;
	//	terrainShader = 0;
	//}

	//// Release the quad tree object.
	//if(quadTree)
	//{
	//	quadTree->Shutdown();
	//	delete quadTree;
	//	quadTree = 0;
	//}

	// Release the frustum object.
	if(frustum)
	{
		delete frustum;
		frustum = 0;
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


	// Release the light object.
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

	return;
}

