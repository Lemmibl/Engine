#include <vector>
#include <windows.h>
#include <xnamath.h>
#include "VegetationShader.h"
#include "textureclass.h"

class VegetationManager
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texCoord;
	};

	//This actually contains a float3 position and alpha channel contains texture ID for each instance
	struct InstanceType
	{
		XMFLOAT4 position; 
	};

public:
	VegetationManager();
	VegetationManager(const VegetationManager&);
	~VegetationManager();

	bool Initialize(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void Shutdown();
	bool SetupQuads(ID3D11Device* device, std::vector<XMFLOAT4>* positions);
	bool Render(ID3D11DeviceContext* deviceContext, XMMATRIX* world, XMMATRIX* view, XMMATRIX* projection);
	void RenderBuffers(ID3D11DeviceContext*);

	inline int GetVertexCount()		{	return vertexCount;		}
	inline int GetInstanceCount()	{	return instanceCount;	}
	inline ID3D11ShaderResourceView** GetTextureArray() {	return textureArray; }

private:
	ID3D11Buffer *vertexBuffer, *instanceBuffer;
	int vertexCount, instanceCount;
	ID3D11ShaderResourceView* textureArray[2];
	VegetationShader* vegetationShader;
};

/*
	Manager for rendering things like grass and bush quads. Possibly long distance tree quads.

	Initialize should take in at least two textures: grass and bushes.... maybe trees/flowers, too.

	You have a SetupQuads function that takes in a vector of float3 positions and generates an array of structs that we turn into a VBO.

	Then a render function where we actually commit the VBO to the geometry shader for rendering.
*/