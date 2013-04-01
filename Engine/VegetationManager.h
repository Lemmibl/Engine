#include <vector>
#include <windows.h>
#include <xnamath.h>

#include "textureclass.h"

class VegetationManager
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	};

public:
	VegetationManager();
	VegetationManager(const VegetationManager&);
	~VegetationManager();

	bool Initialize(ID3D11Device*, char*, WCHAR*, WCHAR*, WCHAR*);
	void Shutdown();

	bool SetupQuads(ID3D11Device* device, std::vector<XMFLOAT3>* positions);

	void Render(ID3D11DeviceContext*);


private:
	void RenderBuffers(ID3D11DeviceContext*);

	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;
	TextureClass* textureArray; //should probably have a bunch of textures
	//ID3D11ShaderResourceView* textures[3];
};

/*
	Manager for rendering things like grass and bush quads. Possibly long distance tree quads.

	Initialize should take in at least two textures: grass and bushes.... maybe trees/flowers, too.

	You have a SetupQuads function that takes in a vector of float3 positions and generates an array of structs that we turn into a VBO.

	Then a render function where we actually commit the VBO to the geometry shader for rendering.
*/