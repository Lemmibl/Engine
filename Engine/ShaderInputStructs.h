#pragma once
#include <windows.h>
#include <xnamath.h>

struct ID3D11ShaderResourceView;
struct DirLight;

namespace ShaderInputStructs
{
	struct SSAOShaderInput
	{
		ID3D11ShaderResourceView** rtTextureArray;
		XMMATRIX* worldViewProjection;
		XMMATRIX* view;
		XMMATRIX* projection;
	};

	struct ComposeShaderInput
	{
		XMMATRIX* worldViewProjection; 
		XMMATRIX* worldView; 
		XMMATRIX* view; 
		XMMATRIX* invertedProjection;
		XMMATRIX* invViewProjection; 
		XMFLOAT4* fogColor; 
		ID3D11ShaderResourceView** textureArray; 
		int toggle;
		float fogMinimum; 
		float lightIntensity;
		float cameraHeight;
	};

	struct DirectionalLightInput
	{


		XMMATRIX* worldViewProjection;
		XMMATRIX* worldView;
		XMMATRIX* world; 
		XMMATRIX* view;
		XMMATRIX* invertedView; 
		XMMATRIX* invertedProjection;
		XMMATRIX* lightView;
		XMMATRIX* lightProj;
		XMMATRIX* lightViewProj;
		XMMATRIX* lightViewAndInvertedCameraView; 
		DirLight* dirLight; 
		ID3D11ShaderResourceView** textureArray;
		ID3D11ShaderResourceView** materialTextureArray; 
		XMFLOAT4 ambienceColor;
		XMFLOAT3 cameraPosition; 
	};

	struct WaterFilterInput
	{
		ID3D11ShaderResourceView** textureArray;
		XMMATRIX* WorldViewProjection;
		XMFLOAT4* fogColor;
		float cameraHeight;
	};
}