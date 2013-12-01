#include "Mesh.h"

Mesh::Mesh() : RenderableInterface()
{	
}

Mesh::~Mesh()
{
}

void Mesh::Render( ID3D11DeviceContext* deviceContext )
{
	// Set vertex buffer stride and offset.
	unsigned int stride = vertexStride;
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.r
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer.p, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer.p, DXGI_FORMAT_R32_UINT, 0);

	// Set the tangent type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
