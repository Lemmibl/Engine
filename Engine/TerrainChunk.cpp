#include "TerrainChunk.h"

TerrainChunk::TerrainChunk(XMFLOAT3 startPosition, XMFLOAT3 endPosition, XMFLOAT3 stepSize, XMFLOAT3 stepCount, size_t vertexStride)
:	startPosition(startPosition),
	endPosition(endPosition),
	boundingBox(startPosition, endPosition),
	stepSizeX(stepSize.x),
	stepSizeY(stepSize.y),
	stepSizeZ(stepSize.z),
	stepsX((unsigned int)stepCount.x),
	stepsY((unsigned int)stepCount.y),
	stepsZ((unsigned int)stepCount.z),
	mesh()
{
	vertices.resize(stepsX*stepsY*stepsZ);
	mesh.vertexStride = vertexStride;
}

TerrainChunk::~TerrainChunk()
{

}

void TerrainChunk::Render(ID3D11DeviceContext* deviceContext)
{
	// Set the vertex buffer to active in the input assembler so it can be rendered.r
	deviceContext->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &mesh.vertexStride, 0);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the tangent type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}