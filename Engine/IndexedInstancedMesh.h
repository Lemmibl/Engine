#pragma once
#include <d3d11.h>
#include <atlcomcli.h>
#include "RenderableInterface.h"

class IndexedInstancedMesh :
	public RenderableInterface
{
public:
	IndexedInstancedMesh();
	~IndexedInstancedMesh();
	bool Render(ID3D11DeviceContext* deviceContext);

public:
	const unsigned int GetVertexCount() const { return vertexCount; }
	const unsigned int GetIndexCount() const { return indexCount; }
	const unsigned int GetInstanceCount() const { return instanceCount; }

	const unsigned int* GetVertexStride() const { return &vertexStride; }
	const unsigned int* GetInstanceStride() const { return &instanceStride; }

	ID3D11Buffer* GetVertexBuffer() { return vertexBuffer.p; }
	ID3D11Buffer* GetIndexBuffer() { return indexBuffer.p; }
	ID3D11Buffer* GetInstanceBuffer() { return instanceBuffer.p; } 

	ID3D11Buffer** GetVertexBufferPP() { return &vertexBuffer.p; }
	ID3D11Buffer** GetIndexBufferPP() { return &indexBuffer.p; }
	ID3D11Buffer** GetInstanceBufferPP() { return &instanceBuffer.p; }

	void SetVertexCount(unsigned int val) { vertexCount = val; }
	void SetIndexCount(unsigned int val) { indexCount = val; }
	void SetInstanceCount(unsigned int val) { instanceCount = val; }

	void SetVertexStride(unsigned int val) { vertexStride = val; }
	void SetInstanceStride(unsigned int val) { instanceStride = val; }

private:
	//These can be set to whatever you want, as long as they match vertexStride, indexCount and vertexCount
	CComPtr<ID3D11Buffer> vertexBuffer, indexBuffer, instanceBuffer;

	unsigned int vertexCount, indexCount, instanceCount;

	//Stride is == sizeof(IndividualVertex). Total size of vertexBuffer is vertexStride * vertexCount
	unsigned int vertexStride, instanceStride; 
};

