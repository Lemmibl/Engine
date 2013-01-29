////=================================================================================================
////
////  Partially based off of Light Indexed Deferred Sample
////  by MJP
////  http://mynameismjp.wordpress.com/
////
////  All code and content licensed under Microsoft Public License (Ms-PL)
////
////=================================================================================================
//
//#include "deferredrenderer.h"
//
//DeferredRenderer::DeferredRenderer()
//{
//	//deviceManager.SetBackBufferWidth(WindowWidth);
//	//deviceManager.SetBackBufferHeight(WindowHeight);
//	//deviceManager.SetMinFeatureLevel(D3D_FEATURE_LEVEL_11_0);
//
//	//window.SetClientArea(WindowWidth, WindowHeight);
//}
//
//DeferredRenderer::~DeferredRenderer()
//{
//
//}
//
//void DeferredRenderer::LoadContent()
//{
//	ID3D11DevicePtr device = deviceManager.Device();
//	ID3D11DeviceContextPtr deviceContext = deviceManager.ImmediateContext();
//
//	csConstants.Initialize(device);
//
//	//Init lights, add meshes, recieve device manager and device manager context
//
//}
//
//// Creates all required render targets
//void DeferredRenderer::CreateRenderTargets()
//{
//	ID3D11Device* device = deviceManager.Device();
//	UINT width = deviceManager.BackBufferWidth();
//	UINT height = deviceManager.BackBufferHeight();
//
//	const UINT32 NumLights = numberOfLights;
//
//	colorTarget.Initialize(device, width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, 0);
//
//	UINT dtWidth = width;
//	UINT dtHeight = height;
//	deferredOutputTarget.Initialize(device, dtWidth, dtHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, 1, 0, false, true);
//
//	depthBuffer.Initialize(device, width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, true, 0, 0);
//
//	normalsTarget.Initialize(device, width, height, DXGI_FORMAT_R10G10B10A2_UNORM, 1, 0, 0);
//	colorTarget.Initialize(device, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 1, 0, 0);
//	lightingTarget.Initialize(device, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 1, 0, 0);
//	shadowTarget.Initialize(device, width, height, DXGI_FORMAT_D24_UNORM_S8_UINT, 1, 0, 0);
//
//
//	DXGI_FORMAT format = DXGI_FORMAT_R8_UINT;
//	UINT32 stride = sizeof(UINT8);
//	if(NumLights >= 255)
//	{
//		format = DXGI_FORMAT_R16_UINT;
//		stride = sizeof(UINT16);
//	}
//
//	//lightIndicesBuffer.Initialize(device, format, stride, numElements, FALSE);
//
//	RenderLightAndShadowMaps();
//}
//
//static float RandFloat()
//{
//	return rand() / static_cast<float>(RAND_MAX);
//}
//
//void DeferredRenderer::RenderLightAndShadowMaps()
//{
//	//ID3D11DevicePtr device = deviceManager.Device();
//
//	srand(1);
//
//	const UINT32 NumLights = numberOfLights;
//
//	std::vector<PointLight> lights;
//	lights.resize(NumLights);
//
//	for(UINT32 i = 0; i < NumLights; ++i)
//	{
//		PointLight& light = lights[i];
//		light.Position.x = RandFloat() * 15.0f - 7.5f;
//		light.Position.y = RandFloat() * 20.0f;
//		light.Position.z = RandFloat() * 20.0f - 10.0f;
//		light.Color.x = RandFloat() * 30.0f;
//		light.Color.y = RandFloat() * 30.0f;
//		light.Color.z = RandFloat() * 30.0f;
//		light.Falloff = RandFloat() * 5.0f + 1.5f;
//	}
//	
//	//lightsBuffer.Initialize(device, sizeof(Light), NumLights, false, false, false, &lights[0]);
//}
//
//void DeferredRenderer::Update(const Timer& timer)
//{
//	//MouseState mouseState = MouseState::GetMouseState(window);
//	//KeyboardState kbState = KeyboardState::GetKeyboardState(window);
//
//	//if (kbState.IsKeyDown(KeyboardState::Escape))
//	//	window.Destroy();
//
//	//float CamMoveSpeed = 5.0f * timer.DeltaSecondsF();
//	//const float CamRotSpeed = 0.180f * timer.DeltaSecondsF();
//	//const float MeshRotSpeed = 0.180f * timer.DeltaSecondsF();
//
//	//// Move the camera with keyboard input
//	//if (kbState.IsKeyDown(KeyboardState::LeftShift))
//	//	CamMoveSpeed *= 0.25f;
//
//	//XMVECTOR camPos = camera.Position();
//	//if (kbState.IsKeyDown(KeyboardState::W))
//	//	camPos += camera.Forward() * CamMoveSpeed;
//	//else if (kbState.IsKeyDown(KeyboardState::S))
//	//	camPos += camera.Back() * CamMoveSpeed;
//	//if (kbState.IsKeyDown(KeyboardState::A))
//	//	camPos += camera.Left() * CamMoveSpeed;
//	//else if (kbState.IsKeyDown(KeyboardState::D))
//	//	camPos += camera.Right() * CamMoveSpeed;
//	//if (kbState.IsKeyDown(KeyboardState::Q))
//	//	camPos += camera.Up() * CamMoveSpeed;
//	//else if (kbState.IsKeyDown(KeyboardState::E))
//	//	camPos += camera.Down() * CamMoveSpeed;
//	//camera.SetPosition(camPos);
//
//	//// Rotate the camera with the mouse
//	//if(mouseState.RButton.Pressed && mouseState.IsOverWindow)
//	//{
//	//	float xRot = camera.XRotation();
//	//	float yRot = camera.YRotation();
//	//	xRot += mouseState.DY * CamRotSpeed;
//	//	yRot += mouseState.DX * CamRotSpeed;
//	//	camera.SetXRotation(xRot);
//	//	camera.SetYRotation(yRot);
//	//}
//
//	//// Toggle VSYNC
//	//if(kbState.RisingEdge(KeyboardState::V))
//	//	deviceManager.SetVSYNCEnabled(!deviceManager.VSYNCEnabled());
//
//	//AppSettings::Update(kbState, mouseState);
//
//	//// Re-create render targets if MSAA changed or if the number of lights changes
//	//if(depthBuffer.MultiSamples != AppSettings::MSAAMode.NumSamples()
//	//	|| lightsBuffer.NumElements != AppSettings::NumLights.NumLights())
//	//	CreateRenderTargets();
//}
//
//void DeferredRenderer::Render(const Timer& timer)
//{
//	ID3D11DeviceContext* context;
//
//	RenderScene();
//
//	//if(AppSettings::MSAAEnabled())
//	//{
//	//	if(AppSettings::RenderingMode == RenderingModeGUI::IndexedDeferred)
//			context->ResolveSubresource(colorTarget.Texture, 0, colorTargetMSAA.Texture, 0, colorTargetMSAA.Format);
//	//	else
//	//		postProcessor.Downscale(context, deferredOutputTarget.SRView, colorTarget.RTView);
//	//}
//	//else if(AppSettings::RenderingMode == RenderingModeGUI::TiledDeferred)
//	//	context->CopyResource(colorTarget.Texture, deferredOutputTarget.Texture);
//
//
//	//// Kick off post-processing
//	//D3DPERF_BeginEvent(0xFFFFFFFF, L"Post Processing");
//	//PostProcessor::Constants constants;
//	//constants.BloomThreshold = AppSettings::BloomThreshold;
//	//constants.BloomMagnitude = AppSettings::BloomMagnitude;
//	//constants.BloomBlurSigma = AppSettings::BloomBlurSigma;
//	//constants.Tau = AppSettings::AdaptationRate;
//	//constants.KeyValue = AppSettings::KeyValue;
//	//constants.TimeDelta = timer.DeltaSecondsF();
//
//	//postProcessor.SetConstants(constants);
//	//postProcessor.Render(context, colorTarget.SRView, deviceManager.BackBuffer());
//	//D3DPERF_EndEvent();
//
//	ID3D11RenderTargetView* renderTargets[1] = { deviceManager.BackBuffer() };
//	context->OMSetRenderTargets(1, renderTargets, NULL);
//
//	D3D11_VIEWPORT vp;
//	vp.Width = static_cast<float>(deviceManager.BackBufferWidth());
//	vp.Height = static_cast<float>(deviceManager.BackBufferHeight());
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	vp.MinDepth = 0;
//	vp.MaxDepth = 1;
//	context->RSSetViewports(1, &vp);
//}
//
//void DeferredRenderer::ComputeLightTiles()
//{
//	//Compute lightmap + shadowmap
//}
//
//void DeferredRenderer::RenderScene()
//{
//
//	ID3D11DeviceContextPtr context = deviceManager.ImmediateContext();
//
//	ID3D11RenderTargetView* renderTargets[4] = { NULL };
//	ID3D11DepthStencilView* ds = depthBuffer.DSView;
//
//	renderTargets[0] = normalsTarget.RTView;
//	renderTargets[1] = diffuseAlbedoTarget.RTView;
//	renderTargets[2] = specularAlbedoTarget.RTView;
//	renderTargets[3] = lightingTarget.RTView;
//	for(UINT i = 0; i < 4; ++i)
//		context->ClearRenderTargetView(renderTargets[i], reinterpret_cast<float*>(&XMFLOAT4(0, 0, 0, 0)));
//
//	ID3D11RenderTargetView* colorRT = colorTarget.RTView;
//	context->ClearRenderTargetView(colorRT, D3DXVECTOR4(0, 0, 0, 0));
//
//	context->OMSetRenderTargets(4, renderTargets, ds);
//
//	context->ClearDepthStencilView(ds, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
//
//
//	meshRenderer.RenderGBuffer(context, camera, ModelWorldMatrix);
//
//	renderTargets[0] = lightingTarget.RTView;
//	renderTargets[1] = renderTargets[2] = renderTargets[3] = NULL;
//	context->OMSetRenderTargets(4, renderTargets, depthBuffer.DSView);
//
//
//	for(UINT i = 0; i < 4; ++i)
//		renderTargets[i] = NULL;
//	context->OMSetRenderTargets(4, renderTargets, NULL);
//
//	//ComputeLightTiles(); //Compute lightmap + shadowmap
//
//	//Finalize everything and add it together
//}
//
///* MESHRENDERER.H
//
////=================================================================================================
////
////  Light Indexed Deferred Sample
////  by MJP
////  http://mynameismjp.wordpress.com/
////
////  All code and content licensed under Microsoft Public License (Ms-PL)
////
////=================================================================================================
//
//#pragma once
//
//#include "SampleFramework11/PCH.h"
//
//#include "SampleFramework11/Model.h"
//#include "SampleFramework11/GraphicsTypes.h"
//#include "SampleFramework11/DeviceStates.h"
//#include "SampleFramework11/Camera.h"
//#include "SampleFramework11/SH.h"
//
//#include "AppSettings.h"
//
//using namespace SampleFramework11;
//
//// Represents a bounding sphere for a MeshPart
//struct Sphere
//{
//XMFLOAT3 Center;
//float Radius;
//};
//
//// Represents the 6 planes of a frustum
//Float4Align struct Frustum
//{
//XMVECTOR Planes[6];
//};
//
//class MeshRenderer
//{
//
//public:
//
//MeshRenderer();
//
//void Initialize(ID3D11Device* device, ID3D11DeviceContext* context, Model* model, const XMFLOAT3& lightDir,
//const XMFLOAT3& lightColor, const XMMATRIX& world);
//void RenderDepth(ID3D11DeviceContext* context, const Camera& camera, const XMMATRIX& world);
//void RenderGBuffer(ID3D11DeviceContext* context, const Camera& camera, const XMMATRIX& world);
//void Render(ID3D11DeviceContext* context, const Camera& camera, const XMMATRIX& world,
//ID3D11ShaderResourceView* lightsBuffer, ID3D11ShaderResourceView* lightIndicesBuffer,
//UINT32 numTilesX, UINT32 numTilesY);
//
//protected:
//
//void ComputeBoundingSpheres(ID3D11DeviceContext* context, const XMMATRIX& world);
//void DoFrustumTests(const Camera& camera);
//
//static const UINT NumCascades = 4;
//
//ID3D11DevicePtr device;
//
//BlendStates blendStates;
//RasterizerStates rasterizerStates;
//DepthStencilStates depthStencilStates;
//SamplerStates samplerStates;
//
//Model* model;
//XMFLOAT3 lightDir;
//
//std::vector<Sphere> boundingSpheres;
//std::vector<UINT> frustumTests;
//UINT numSuccessfulTests;
//
//DepthStencilBuffer shadowMap;
//
//UINT_PTR currModel;
//
//std::vector<ID3D11InputLayoutPtr> meshInputLayouts;
//ID3D10BlobPtr compiledMeshVS;
//ID3D11VertexShaderPtr meshVS[NumLightsGUI::NumValues];
//ID3D11PixelShaderPtr meshPS[NumLightsGUI::NumValues];
//
//std::vector<ID3D11InputLayoutPtr> meshDepthInputLayouts;
//ID3D11VertexShaderPtr meshDepthVS;
//ID3D10BlobPtr compiledMeshDepthVS;
//
//std::vector<ID3D11InputLayoutPtr> meshGBufferInputLayouts;
//ID3D11VertexShaderPtr meshGBufferVS;
//ID3D11PixelShaderPtr meshGBufferPS;
//ID3D10BlobPtr compiledMeshGBufferVS;
//
//// Constant buffers
//struct MeshVSConstants
//{
//Float4Align XMMATRIX World;
//Float4Align XMMATRIX View;
//Float4Align XMMATRIX WorldViewProjection;
//};
//
//struct MeshPSConstants
//{
//Float4Align XMFLOAT3 LightDirWS;
//Float4Align XMFLOAT3 LightColor;
//Float4Align XMFLOAT3 CameraPosWS;
//Float4Align XMFLOAT3 DiffuseAlbedo;
//Float4Align XMFLOAT3 SpecularAlbedo;
//float Balance;
//float Roughness;
//UINT32 NumTilesX;
//UINT32 NumTilesY;
//UINT32 EnableNormalMapping;
//};
//
//struct SHConstants
//{
//SH9Color AmbientDiffuse;
//};
//
//ConstantBuffer<MeshVSConstants> meshVSConstants;
//ConstantBuffer<MeshPSConstants> meshPSConstants;
//ConstantBuffer<SHConstants> shConstants;
//};
//*/
//
///* MESHRENDERER.CPP
//
////=================================================================================================
////
////  Light Indexed Deferred Sample
////  by MJP
////  http://mynameismjp.wordpress.com/
////
////  All code and content licensed under Microsoft Public License (Ms-PL)
////
////=================================================================================================
//
//#include "PCH.h"
//
//#include "MeshRenderer.h"
//#include "AppSettings.h"
//
//#include "SampleFramework11/Exceptions.h"
//#include "SampleFramework11/Utility.h"
//#include "SampleFramework11/ShaderCompilation.h"
//#include "SampleFramework11/Profiler.h"
//
//// Constants
//static const UINT ShadowMapSize = 2048;
//static const float ShadowDist = 1.0f;
//static const float Backup = 20.0f;
//static const float NearClip = 1.0f;
//static const float CascadeSplits[4] = { 0.125f, 0.25f, 0.5f, 1.0f };
//static const float Bias = 0.005f;
//
//// Finds the approximate smallest enclosing bounding sphere for a set of points. Based on
//// "An Efficient Bounding Sphere", by Jack Ritter.
//static Sphere ComputeBoundingSphereFromPoints(const XMFLOAT3* points, UINT numPoints, UINT stride)
//{
//Sphere sphere;
//
//XMASSERT(numPoints > 0);
//XMASSERT(points);
//
//// Find the points with minimum and maximum x, y, and z
//XMVECTOR MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
//
//MinX = MaxX = MinY = MaxY = MinZ = MaxZ = XMLoadFloat3(points);
//
//for(UINT i = 1; i < numPoints; i++)
//{
//XMVECTOR Point = XMLoadFloat3((XMFLOAT3*)((BYTE*)points + i * stride));
//
//float px = XMVectorGetX(Point);
//float py = XMVectorGetY(Point);
//float pz = XMVectorGetZ(Point);
//
//if(px < XMVectorGetX(MinX))
//MinX = Point;
//
//if(px > XMVectorGetX(MaxX))
//MaxX = Point;
//
//if(py < XMVectorGetY(MinY))
//MinY = Point;
//
//if(py > XMVectorGetY(MaxY))
//MaxY = Point;
//
//if(pz < XMVectorGetZ(MinZ))
//MinZ = Point;
//
//if(pz > XMVectorGetZ(MaxZ))
//MaxZ = Point;
//}
//
//// Use the min/max pair that are farthest apart to form the initial sphere.
//XMVECTOR DeltaX = MaxX - MinX;
//XMVECTOR DistX = XMVector3Length(DeltaX);
//
//XMVECTOR DeltaY = MaxY - MinY;
//XMVECTOR DistY = XMVector3Length(DeltaY);
//
//XMVECTOR DeltaZ = MaxZ - MinZ;
//XMVECTOR DistZ = XMVector3Length(DeltaZ);
//
//XMVECTOR Center;
//XMVECTOR Radius;
//
//if(XMVector3Greater(DistX, DistY))
//{
//if(XMVector3Greater(DistX, DistZ))
//{
//// Use min/max x.
//Center = (MaxX + MinX) * 0.5f;
//Radius = DistX * 0.5f;
//}
//else
//{
//// Use min/max z.
//Center = (MaxZ + MinZ) * 0.5f;
//Radius = DistZ * 0.5f;
//}
//}
//else // Y >= X
//{
//if(XMVector3Greater(DistY, DistZ))
//{
//// Use min/max y.
//Center = (MaxY + MinY) * 0.5f;
//Radius = DistY * 0.5f;
//}
//else
//{
//// Use min/max z.
//Center = (MaxZ + MinZ) * 0.5f;
//Radius = DistZ * 0.5f;
//}
//}
//
//// Add any points not inside the sphere.
//for(UINT i = 0; i < numPoints; i++)
//{
//XMVECTOR Point = XMLoadFloat3((XMFLOAT3*)((BYTE*)points + i * stride));
//
//XMVECTOR Delta = Point - Center;
//
//XMVECTOR Dist = XMVector3Length(Delta);
//
//if(XMVector3Greater(Dist, Radius))
//{
//// Adjust sphere to include the new point.
//Radius = (Radius + Dist) * 0.5f;
//Center += (XMVectorReplicate(1.0f) - Radius * XMVectorReciprocal(Dist)) * Delta;
//}
//}
//
//XMStoreFloat3(&sphere.Center, Center);
//XMStoreFloat(&sphere.Radius, Radius);
//
//return sphere;
//}
//
//// Calculates the furstum planes given a view * projection matrix
//static void ComputeFrustum(const XMMATRIX& viewProj, Frustum& frustum)
//{
//XMVECTOR det;
//XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);
//
//// Corners in homogeneous clip space
//XMVECTOR corners[8] =
//{                                               //                         7--------6
//XMVectorSet( 1.0f, -1.0f, 0.0f, 1.0f),      //                        /|       /|
//XMVectorSet(-1.0f, -1.0f, 0.0f, 1.0f),      //     Y ^               / |      / |
//XMVectorSet( 1.0f,  1.0f, 0.0f, 1.0f),      //     | _              3--------2  |
//XMVectorSet(-1.0f,  1.0f, 0.0f, 1.0f),      //     | /' Z           |  |     |  |
//XMVectorSet( 1.0f, -1.0f, 1.0f, 1.0f),      //     |/               |  5-----|--4
//XMVectorSet(-1.0f, -1.0f, 1.0f, 1.0f),      //     + ---> X         | /      | /
//XMVectorSet( 1.0f,  1.0f, 1.0f, 1.0f),      //                      |/       |/
//XMVectorSet(-1.0f,  1.0f, 1.0f, 1.0f),      //                      1--------0
//};
//
//// Convert to world space
//for(UINT i = 0; i < 8; ++i)
//corners[i] = XMVector3TransformCoord(corners[i], invViewProj);
//
//// Calculate the 6 planes
//frustum.Planes[0] = XMPlaneFromPoints(corners[0], corners[4], corners[2]);
//frustum.Planes[1] = XMPlaneFromPoints(corners[1], corners[3], corners[5]);
//frustum.Planes[2] = XMPlaneFromPoints(corners[3], corners[2], corners[7]);
//frustum.Planes[3] = XMPlaneFromPoints(corners[1], corners[5], corners[0]);
//frustum.Planes[4] = XMPlaneFromPoints(corners[5], corners[7], corners[4]);
//frustum.Planes[5] = XMPlaneFromPoints(corners[1], corners[0], corners[3]);
//}
//
//// Tests a frustum for intersection with a sphere
//static UINT TestFrustumSphere(const Frustum& frustum, const Sphere& sphere)
//{
//XMVECTOR sphereCenter = XMLoadFloat3(&sphere.Center);
//
//UINT result = 1;
//for(UINT i = 0; i < 6; i++) {
//float distance = XMVectorGetX(XMPlaneDotCoord(frustum.Planes[i], sphereCenter));
//
//if (distance < -sphere.Radius)
//return 0;
//else if (distance < sphere.Radius)
//result =  1;
//}
//
//return result;
//}
//
//MeshRenderer::MeshRenderer()
//{
//}
//
//// Loads resources
//void MeshRenderer::Initialize(ID3D11Device* device, ID3D11DeviceContext* context, Model* model,
//const XMFLOAT3& lightDir, const XMFLOAT3& lightColor, const XMMATRIX& world)
//{
//this->device = device;
//this->model = model;
//
//blendStates.Initialize(device);
//rasterizerStates.Initialize(device);
//depthStencilStates.Initialize(device);
//samplerStates.Initialize(device);
//
//meshVSConstants.Initialize(device);
//meshPSConstants.Initialize(device);
//
//meshPSConstants.Data.LightDirWS = lightDir;
//meshPSConstants.Data.LightColor = lightColor;
//this->lightDir = lightDir;
//
//shConstants.Initialize(device);
//
//// Load the mesh shaders
//meshDepthVS.Attach(CompileVSFromFile(device, L"DepthOnly.hlsl", "VS", "vs_5_0", NULL, NULL, &compiledMeshDepthVS));
//
//for(UINT32 lmode = 0; lmode < NumLightsGUI::NumValues; ++lmode)
//{
//CompileOptions opts;
//opts.Add("MaxLights", NumLightsGUI::NumLights(lmode));
//meshVS[lmode].Attach(CompileVSFromFile(device, L"Mesh.hlsl", "VS", "vs_5_0", opts.Defines(), NULL, &compiledMeshVS));
//meshPS[lmode].Attach(CompilePSFromFile(device, L"Mesh.hlsl", "PS", "ps_5_0", opts.Defines()));
//}
//
//meshGBufferVS.Attach(CompileVSFromFile(device, L"GBuffer.hlsl", "VS", "vs_5_0", NULL, NULL, &compiledMeshGBufferVS));
//meshGBufferPS.Attach(CompilePSFromFile(device, L"GBuffer.hlsl", "PS", "ps_5_0"));
//
//for(UINT i = 0; i < model->Meshes().size(); ++i)
//{
//Mesh& mesh = model->Meshes()[i];
//ID3D11InputLayoutPtr inputLayout;
//DXCall(device->CreateInputLayout(mesh.InputElements(), mesh.NumInputElements(),
//compiledMeshVS->GetBufferPointer(), compiledMeshVS->GetBufferSize(), &inputLayout));
//meshInputLayouts.push_back(inputLayout);
//
//DXCall(device->CreateInputLayout(mesh.InputElements(), mesh.NumInputElements(),
//compiledMeshDepthVS->GetBufferPointer(), compiledMeshDepthVS->GetBufferSize(), &inputLayout));
//meshDepthInputLayouts.push_back(inputLayout);
//
//DXCall(device->CreateInputLayout(mesh.InputElements(), mesh.NumInputElements(),
//compiledMeshGBufferVS->GetBufferPointer(), compiledMeshGBufferVS->GetBufferSize(), &inputLayout));
//meshGBufferInputLayouts.push_back(inputLayout);
//}
//
//// Create the shadow map as a texture atlas
//shadowMap.Initialize(device, ShadowMapSize * 2, ShadowMapSize * 2, DXGI_FORMAT_D24_UNORM_S8_UINT, true, 1, 0, 1);
//
//ComputeBoundingSpheres(context, world);
//}
//
//// Calculates the bounding sphere for each MeshPart
//void MeshRenderer::ComputeBoundingSpheres(ID3D11DeviceContext* context, const XMMATRIX& world)
//{
//for(UINT meshIdx = 0; meshIdx < model->Meshes().size(); ++meshIdx)
//{
//Mesh& mesh = model->Meshes()[meshIdx];
//
//// Create staging buffers for copying the vertex/index data to
//ID3D11BufferPtr stagingVB;
//ID3D11BufferPtr stagingIB;
//
//D3D11_BUFFER_DESC bufferDesc;
//bufferDesc.BindFlags = 0;
//bufferDesc.ByteWidth = mesh.NumVertices() * mesh.VertexStride();
//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
//bufferDesc.MiscFlags = 0;
//bufferDesc.StructureByteStride = 0;
//bufferDesc.Usage = D3D11_USAGE_STAGING;
//DXCall(device->CreateBuffer(&bufferDesc, NULL, &stagingVB));
//
//UINT indexSize = mesh.IndexBufferType() == Mesh::Index16Bit ? 2 : 4;
//bufferDesc.ByteWidth = mesh.NumIndices() * indexSize;
//DXCall(device->CreateBuffer(&bufferDesc, NULL, &stagingIB));
//
//context->CopyResource(stagingVB, mesh.VertexBuffer());
//context->CopyResource(stagingIB, mesh.IndexBuffer());
//
//D3D11_MAPPED_SUBRESOURCE mapped;
//context->Map(stagingVB, 0, D3D11_MAP_READ, 0, &mapped);
//const BYTE* verts = reinterpret_cast<const BYTE*>(mapped.pData);
//UINT stride = mesh.VertexStride();
//
//context->Map(stagingIB, 0, D3D11_MAP_READ, 0, &mapped);
//const BYTE* indices = reinterpret_cast<const BYTE*>(mapped.pData);
//const UINT* indices32 = reinterpret_cast<const UINT*>(mapped.pData);
//const WORD* indices16 = reinterpret_cast<const WORD*>(mapped.pData);
//
//for(UINT partIdx = 0; partIdx < mesh.MeshParts().size(); ++partIdx)
//{
//const MeshPart& part = mesh.MeshParts()[partIdx];
//
//std::vector<XMFLOAT3> points;
//for(UINT i = 0; i < part.IndexCount; ++i)
//{
//UINT index = indexSize == 2 ? indices16[part.IndexStart + i] : indices32[part.IndexStart + i];
//XMFLOAT3 point = *reinterpret_cast<const XMFLOAT3*>(verts + (index * stride));
//XMVECTOR position = XMLoadFloat3(&point);
//position = XMVector3TransformCoord(position, world);
//XMStoreFloat3(&point, position);
//points.push_back(point);
//}
//
//Sphere sphere = ComputeBoundingSphereFromPoints(&points[0], static_cast<UINT>(points.size()), sizeof(XMFLOAT3));
//boundingSpheres.push_back(sphere);
//}
//
//context->Unmap(stagingVB, 0);
//context->Unmap(stagingIB, 0);
//}
//}
//
//// Performs frustum/sphere intersection tests for all MeshPart's
//void MeshRenderer::DoFrustumTests(const Camera& camera)
//{
//frustumTests.clear();
//numSuccessfulTests = 0;
//
//Frustum frustum;
//ComputeFrustum(camera.ViewProjectionMatrix(), frustum);
//
//for(UINT i = 0; i < boundingSpheres.size(); ++i)
//{
//const Sphere& sphere = boundingSpheres[i];
//UINT test = TestFrustumSphere(frustum, sphere);
//frustumTests.push_back(test);
//numSuccessfulTests += test;
//}
//}
//
//// Renders all meshes in the model, with shadows
//void MeshRenderer::Render(ID3D11DeviceContext* context, const Camera& camera, const XMMATRIX& world,
//ID3D11ShaderResourceView* lightsBuffer, ID3D11ShaderResourceView* lightIndicesBuffer,
//UINT32 numTilesX, UINT32 numTilesY)
//{
//// RenderShadowMap(context, camera, world);
//
//PIXEvent event(L"Mesh Rendering");
//
//ProfileBlock profileBlock(L"Forward Rendering");
//
//DoFrustumTests(camera);
//if(numSuccessfulTests == 0)
//return;
//
//// Set states
//float blendFactor[4] = {1, 1, 1, 1};
//context->OMSetBlendState(blendStates.BlendDisabled(), blendFactor, 0xFFFFFFFF);
//
//context->OMSetDepthStencilState(depthStencilStates.DepthEnabled(), 0);
//
//context->RSSetState(rasterizerStates.BackFaceCull());
//
//ID3D11SamplerState* sampStates[1] = {
//samplerStates.Anisotropic(),
//};
//context->PSSetSamplers(0, 1, sampStates);
//
//ID3D11ShaderResourceView* srvs[2] = {
//lightsBuffer, lightIndicesBuffer
//};
//context->PSSetShaderResources(2, 2, srvs);
//
//// Set constant buffers
//meshVSConstants.Data.World = XMMatrixTranspose(world);
//meshVSConstants.Data.View = XMMatrixTranspose(camera.ViewMatrix());
//meshVSConstants.Data.WorldViewProjection = XMMatrixTranspose(world * camera.ViewProjectionMatrix());
//meshVSConstants.ApplyChanges(context);
//meshVSConstants.SetVS(context, 0);
//
//float di = AppSettings::DiffuseIntensity;
//float si = AppSettings::SpecularIntensity;
//
//XMStoreFloat3(&meshPSConstants.Data.CameraPosWS, camera.Position());
//meshPSConstants.Data.DiffuseAlbedo = XMFLOAT3(AppSettings::DiffuseR * di, AppSettings::DiffuseG * di, AppSettings::DiffuseB * di);
//meshPSConstants.Data.SpecularAlbedo = XMFLOAT3(AppSettings::SpecularR * si, AppSettings::SpecularG * si, AppSettings::SpecularB * si);
//meshPSConstants.Data.Balance = AppSettings::Balance;
//meshPSConstants.Data.Roughness = AppSettings::Roughness;
//meshPSConstants.Data.NumTilesX = numTilesX;
//meshPSConstants.Data.NumTilesY = numTilesY;
//meshPSConstants.ApplyChanges(context);
//meshPSConstants.SetPS(context, 0);
//
//// Set shaders
//context->DSSetShader(NULL, NULL, 0);
//context->HSSetShader(NULL, NULL, 0);
//context->GSSetShader(NULL, NULL, 0);
//context->VSSetShader(meshVS[AppSettings::NumLights], NULL, 0);
//context->PSSetShader(meshPS[AppSettings::NumLights], NULL, 0);
//
//// Draw all meshes
//UINT partCount = 0;
//for (UINT meshIdx = 0; meshIdx < model->Meshes().size(); ++meshIdx)
//{
//Mesh& mesh = model->Meshes()[meshIdx];
//
//// Set the vertices and indices
//ID3D11Buffer* vertexBuffers[1] = { mesh.VertexBuffer() };
//UINT vertexStrides[1] = { mesh.VertexStride() };
//UINT offsets[1] = { 0 };
//context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, offsets);
//context->IASetIndexBuffer(mesh.IndexBuffer(), mesh.IndexBufferFormat(), 0);
//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//// Set the input layout
//context->IASetInputLayout(meshInputLayouts[meshIdx]);
//
//// Draw all parts
//for (UINT_PTR partIdx = 0; partIdx < mesh.MeshParts().size(); ++partIdx)
//{
//if(frustumTests[partCount++])
//{
//const MeshPart& part = mesh.MeshParts()[partIdx];
//
//ID3D11ShaderResourceView* diffuseMap = NULL;
//ID3D11ShaderResourceView* normalMap = NULL;
//if(part.MaterialIdx < model->Materials().size())
//{
//const MeshMaterial& material = model->Materials()[part.MaterialIdx];
//diffuseMap = material.DiffuseMap;
//normalMap = material.NormalMap;
//}
//
//// Set the textures
//ID3D11ShaderResourceView* psTextures[2] = { diffuseMap, normalMap };
//context->PSSetShaderResources(0, 2, psTextures);
//
//meshPSConstants.Data.EnableNormalMapping = normalMap != NULL;
//meshPSConstants.ApplyChanges(context);
//
//context->DrawIndexed(part.IndexCount, part.IndexStart, 0);
//}
//}
//}
//
//ID3D11ShaderResourceView* nullSRVs[5] = { NULL };
//context->PSSetShaderResources(0, 5, nullSRVs);
//}
//
//void MeshRenderer::RenderGBuffer(ID3D11DeviceContext* context, const Camera& camera, const XMMATRIX& world)
//{
//PIXEvent event(L"GBuffer Rendering");
//
//ProfileBlock profileBlock(L"GBuffer Rendering");
//
//DoFrustumTests(camera);
//if(numSuccessfulTests == 0)
//return;
//
//// Set states
//float blendFactor[4] = {1, 1, 1, 1};
//context->OMSetBlendState(blendStates.BlendDisabled(), blendFactor, 0xFFFFFFFF);
//
//if(AppSettings::EnableZPrepass)
//context->OMSetDepthStencilState(depthStencilStates.DepthEnabled(), 0);
//else
//context->OMSetDepthStencilState(depthStencilStates.DepthWriteEnabled(), 0);
//
//context->RSSetState(rasterizerStates.BackFaceCull());
//
//ID3D11SamplerState* sampStates[1] = {
//samplerStates.Anisotropic(),
//};
//context->PSSetSamplers(0, 1, sampStates);
//
//// Set constant buffers
//meshVSConstants.Data.World = XMMatrixTranspose(world);
//meshVSConstants.Data.View = XMMatrixTranspose(camera.ViewMatrix());
//meshVSConstants.Data.WorldViewProjection = XMMatrixTranspose(world * camera.ViewProjectionMatrix());
//meshVSConstants.ApplyChanges(context);
//meshVSConstants.SetVS(context, 0);
//
//float di = AppSettings::DiffuseIntensity;
//float si = AppSettings::SpecularIntensity;
//
//XMStoreFloat3(&meshPSConstants.Data.CameraPosWS, camera.Position());
//meshPSConstants.Data.DiffuseAlbedo = XMFLOAT3(AppSettings::DiffuseR * di, AppSettings::DiffuseG * di, AppSettings::DiffuseB * di);
//meshPSConstants.Data.SpecularAlbedo = XMFLOAT3(AppSettings::SpecularR * si, AppSettings::SpecularG * si, AppSettings::SpecularB * si);
//meshPSConstants.Data.Balance = AppSettings::Balance;
//meshPSConstants.Data.Roughness = AppSettings::Roughness;
//meshPSConstants.Data.NumTilesX = 0;
//meshPSConstants.Data.NumTilesY = 0;
//meshPSConstants.Data.EnableNormalMapping = true;
//meshPSConstants.ApplyChanges(context);
//meshPSConstants.SetPS(context, 0);
//
//// Set shaders
//context->DSSetShader(NULL, NULL, 0);
//context->HSSetShader(NULL, NULL, 0);
//context->GSSetShader(NULL, NULL, 0);
//context->VSSetShader(meshGBufferVS , NULL, 0);
//context->PSSetShader(meshGBufferPS, NULL, 0);
//
//// Draw all meshes
//UINT partCount = 0;
//for (UINT meshIdx = 0; meshIdx < model->Meshes().size(); ++meshIdx)
//{
//Mesh& mesh = model->Meshes()[meshIdx];
//
//// Set the vertices and indices
//ID3D11Buffer* vertexBuffers[1] = { mesh.VertexBuffer() };
//UINT vertexStrides[1] = { mesh.VertexStride() };
//UINT offsets[1] = { 0 };
//context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, offsets);
//context->IASetIndexBuffer(mesh.IndexBuffer(), mesh.IndexBufferFormat(), 0);
//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//// Set the input layout
//context->IASetInputLayout(meshInputLayouts[meshIdx]);
//
//// Draw all parts
//for (UINT_PTR partIdx = 0; partIdx < mesh.MeshParts().size(); ++partIdx)
//{
//if(frustumTests[partCount++])
//{
//const MeshPart& part = mesh.MeshParts()[partIdx];
//
//ID3D11ShaderResourceView* diffuseMap = NULL;
//ID3D11ShaderResourceView* normalMap = NULL;
//if(part.MaterialIdx < model->Materials().size())
//{
//const MeshMaterial& material = model->Materials()[part.MaterialIdx];
//diffuseMap = material.DiffuseMap;
//normalMap = material.NormalMap;
//}
//
//// Set the textures
//ID3D11ShaderResourceView* psTextures[2] = { diffuseMap, normalMap };
//context->PSSetShaderResources(0, 2, psTextures);
//
//meshPSConstants.Data.EnableNormalMapping = normalMap != NULL;
//meshPSConstants.ApplyChanges(context);
//
//context->DrawIndexed(part.IndexCount, part.IndexStart, 0);
//}
//}
//}
//
//ID3D11ShaderResourceView* nullSRVs[2] = { NULL };
//context->PSSetShaderResources(0, 2, nullSRVs);
//}
//
//// Renders all meshes using depth-only rendering
//void MeshRenderer::RenderDepth(ID3D11DeviceContext* context, const Camera& camera, const XMMATRIX& world)
//{
//PIXEvent event(L"Mesh Depth Rendering");
//
//ProfileBlock profileBlock(L"Depth Rendering");
//
//DoFrustumTests(camera);
//if(numSuccessfulTests == 0)
//return;
//
//// Set states
//float blendFactor[4] = {1, 1, 1, 1};
//context->OMSetBlendState(blendStates.ColorWriteDisabled(), blendFactor, 0xFFFFFFFF);
//context->OMSetDepthStencilState(depthStencilStates.DepthWriteEnabled(), 0);
//context->RSSetState(rasterizerStates.BackFaceCull());
//
//// Set constant buffers
//meshVSConstants.Data.World = XMMatrixTranspose(world);
//meshVSConstants.Data.View = XMMatrixTranspose(camera.ViewMatrix());
//meshVSConstants.Data.WorldViewProjection = XMMatrixTranspose(world * camera.ViewProjectionMatrix());
//meshVSConstants.ApplyChanges(context);
//meshVSConstants.SetVS(context, 0);
//
//// Set shaders
//context->VSSetShader(meshDepthVS , NULL, 0);
//context->PSSetShader(NULL, NULL, 0);
//context->GSSetShader(NULL, NULL, 0);
//context->DSSetShader(NULL, NULL, 0);
//context->HSSetShader(NULL, NULL, 0);
//
//UINT partCount = 0;
//for (UINT_PTR meshIdx = 0; meshIdx < model->Meshes().size(); ++meshIdx)
//{
//Mesh& mesh = model->Meshes()[meshIdx];
//
//// Set the vertices and indices
//ID3D11Buffer* vertexBuffers[1] = { mesh.VertexBuffer() };
//UINT vertexStrides[1] = { mesh.VertexStride() };
//UINT offsets[1] = { 0 };
//context->IASetVertexBuffers(0, 1, vertexBuffers, vertexStrides, offsets);
//context->IASetIndexBuffer(mesh.IndexBuffer(), mesh.IndexBufferFormat(), 0);
//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//// Set the input layout
//context->IASetInputLayout(meshDepthInputLayouts[meshIdx]);
//
//// Draw all parts
//for (UINT_PTR partIdx = 0; partIdx < mesh.MeshParts().size(); ++partIdx)
//{
//if(frustumTests[partCount++])
//{
//const MeshPart& part = mesh.MeshParts()[partIdx];
//context->DrawIndexed(part.IndexCount, part.IndexStart, 0);
//}
//}
//}
//}
//
//*/