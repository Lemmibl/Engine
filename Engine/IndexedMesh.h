#pragma once
#include <d3d11.h>
#include <atlcomcli.h>
#include "RenderableInterface.h"

//////////////////////////////////////////////////////////////////////////
// So, in order for this to be a functioning mesh, you need to use 
// d3d11device->CreateBuffer()  on the indexBuffer and vertexBuffer object
// Then you also need to fill out vertexCount, indexCount and vertexStride
//////////////////////////////////////////////////////////////////////////
class IndexedMesh : public RenderableInterface
{
public:
	IndexedMesh();
	~IndexedMesh();
	void Render(ID3D11DeviceContext* deviceContext);

public:
	const unsigned int GetVertexCount() const { return vertexCount; }
	const unsigned int GetIndexCount() const { return indexCount; }
	const unsigned int* GetVertexStride() const { return &vertexStride; }

	ID3D11Buffer* GetVertexBuffer() { return vertexBuffer.p; }
	ID3D11Buffer* GetIndexBuffer() { return indexBuffer.p; }

	ID3D11Buffer** GetVertexBufferPP() { return &vertexBuffer.p; }
	ID3D11Buffer** GetIndexBufferPP() { return &indexBuffer.p; }

	void SetVertexCount(unsigned int val) { vertexCount = val; }
	void SetIndexCount(unsigned int val) { indexCount = val; }
	void SetVertexStride(unsigned int val) { vertexStride = val; }

private:
	//These can be set to whatever you want, as long as they match vertexStride, indexCount and vertexCount
	CComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;

	unsigned int vertexCount, indexCount;

	//Stride is == sizeof(IndividualVertex). Total size of vertexBuffer is vertexStride * vertexCount
	unsigned int vertexStride; 
};