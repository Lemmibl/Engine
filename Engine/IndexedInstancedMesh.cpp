#include "IndexedInstancedMesh.h"


IndexedInstancedMesh::IndexedInstancedMesh()
{
}


IndexedInstancedMesh::~IndexedInstancedMesh()
{
}

bool IndexedInstancedMesh::Render( ID3D11DeviceContext* deviceContext )
{
	if(instanceCount > 0 && indexCount > 0)
	{
		unsigned int strides[2];
		unsigned int offsets[2];
		ID3D11Buffer* bufferPointers[2];

		//Set the buffer strides.
		strides[0] = vertexStride; 
		strides[1] = instanceStride; 

		//Set the buffer offsets.
		offsets[0] = 0;
		offsets[1] = 0;

		// Set the array of pointers to the vertex and instance buffers.
		bufferPointers[0] = vertexBuffer.p;	
		bufferPointers[1] = instanceBuffer.p;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

		//Set index buffer
		deviceContext->IASetIndexBuffer(indexBuffer.p, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return true;
	}

	return false;
}
