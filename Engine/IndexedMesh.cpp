#include "IndexedMesh.h"

IndexedMesh::IndexedMesh() : RenderableInterface(), vertexCount(0), indexCount(0), vertexStride(0)
{	
}

IndexedMesh::~IndexedMesh()
{
}

bool IndexedMesh::Render( ID3D11DeviceContext* deviceContext )
{
	if(indexCount > 0 && vertexCount > 0)
	{
		// Set vertex buffer stride and offset.
		unsigned int stride = vertexStride;
		unsigned int offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.r
		deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.p, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(indexBuffer.p, DXGI_FORMAT_R32_UINT, 0);

		// Set the tangent type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(primitiveTopology);

		return true;
	}

	return false;
}
