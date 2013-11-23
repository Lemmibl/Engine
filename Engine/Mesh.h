#pragma once
#include <d3d11.h>
#include <atlcomcli.h>

//////////////////////////////////////////////////////////////////////////
// So, in order for this to be a functioning mesh, you need to use 
// d3d11device->CreateBuffer()  on the indexBuffer and vertexBuffer object
// Then you also need to fill out vertexCount, indexCount and vertexStride
// 
//////////////////////////////////////////////////////////////////////////
struct Mesh
{
public:
	unsigned int GetVertexCount() const { return vertexCount; }
	unsigned int GetIndexCount() const { return indexCount; }
	size_t GetVertexStride() const { return vertexStride; }

	ID3D11Buffer* GetVertexBuffer() { return vertexBuffer.p; }
	ID3D11Buffer* GetIndexBuffer() { return indexBuffer.p; }

	ID3D11Buffer** GetVertexBufferPP() { return &vertexBuffer.p; }
	ID3D11Buffer** GetIndexBufferPP() { return &indexBuffer.p; }

	void SetVertexCount(unsigned int val) { vertexCount = val; }
	void SetIndexCount(unsigned int val) { indexCount = val; }
	void SetVertexStride(size_t val) { vertexStride = val; }


private:
	//These can be set to whatever you want, as long as they match vertexStride, indexCount and vertexCount
	CComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;

	unsigned int vertexCount, indexCount;

	//Stride is == sizeof(IndividualVertex). Total size of vertexBuffer is vertexStride * vertexCount
	size_t vertexStride; 
};