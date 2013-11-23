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
	mesh.SetVertexStride(vertexStride);
}

TerrainChunk::~TerrainChunk()
{

}

void TerrainChunk::Render(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = mesh.GetVertexStride(); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.r
	deviceContext->IASetVertexBuffers(0, 1, mesh.GetVertexBufferPP(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(mesh.GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// Set the tangent type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}