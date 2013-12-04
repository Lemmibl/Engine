#include "VegetationManager.h"

VegetationManager::VegetationManager()
{
	vegetationShader = 0;
}

VegetationManager::VegetationManager( const VegetationManager& )
{

}

VegetationManager::~VegetationManager()
{

}

bool VegetationManager::Initialize(ID3D11Device* device, HWND hwnd)
{
	HRESULT result;

	vegetationShader = unique_ptr<VegetationShader>(new VegetationShader());
	if(!vegetationShader)
	{
		return false;
	}

	result = vegetationShader->Initialize(device, hwnd);
	if(!result)
	{
		return false;
	}

	return true;
}

bool VegetationManager::SetupQuads( ID3D11Device* device, std::vector<InstanceType>* positions )
{
	if(!BuildVertexAndIndexBuffers(device))
	{
		return false;
	}

	if(!BuildInstanceBuffer(device, positions))
	{
		return false;
	}

	return true;
}

void VegetationManager::Shutdown()
{
	//// Release the instance buffer.
	//if(instanceBuffer)
	//{
	//	instanceBuffer->Release();
	//	instanceBuffer = 0;
	//}

	//// Release the vertex buffer.
	//if(vertexBuffer)
	//{
	//	vertexBuffer->Release();
	//	vertexBuffer = 0;
	//}

	if(vegetationShader)
	{
		vegetationShader->Shutdown();
		vegetationShader = 0;
	}
}

bool VegetationManager::Render(ID3D11DeviceContext* deviceContext, XMMATRIX* worldViewProjection, XMMATRIX* worldView, XMMATRIX* world, ID3D11ShaderResourceView** textures)
{
	RenderBuffers(deviceContext);

	if(!vegetationShader->Render(deviceContext, mesh.GetIndexCount(), mesh.GetVertexCount(), mesh.GetInstanceCount(), worldViewProjection, worldView, world, textures))
	{
		return false;
	}

	return true;
}

void VegetationManager::RenderBuffers( ID3D11DeviceContext* deviceContext)
{
	mesh.Render(deviceContext);
}

bool VegetationManager::BuildVertexAndIndexBuffers( ID3D11Device* device )
{
	unique_ptr<VertexType[]> vertices;
	unique_ptr<unsigned int []> indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	const XMVECTOR upVector = XMVector3Normalize(XMLoadFloat3(&XMFLOAT3(0.0f, 1.0f, 0.0f)));
	XMFLOAT3 vertexNormal;
	XMVECTOR tempVector;

#pragma region Setting up the vertices

	// Set the number of vertices in the vertex array.
	int vertexCount = 12;
	int indexCount = 18;

	// Create the vertex array.
	vertices = unique_ptr<VertexType[]>(new VertexType[vertexCount]());
	if(!vertices)
	{
		return false;
	}

	indices = unique_ptr<unsigned int []>(new unsigned int[indexCount]());
	if(!indices)
	{
		return false;
	}

	/*
	http://stackoverflow.com/questions/9806630/calculating-the-vertex-normals-of-a-quad

	If you have this -

	v1        v2
	+---------+
	|         | 
	|         |
	+---------+
	v0        v3

	Where v1..v4 are the vertices of your quad then to calculate the normal at v1 you should calculate the vectors along the two edges it is on, and then calculate the cross product of those vertices.

	So, the normal at v1 is

	CrossProduct((v2-v1), (v0-v1))
	You can repeat this for each vertex, although they will all be the same if the quad is "flat"
	*/

	//http://www.crydev.net/viewtopic.php?f=315&t=100319&sid=96ba2555af70cec15decb60e657ee0cc
	//http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter01.html
	//http://www.gamedev.net/topic/645148-dx11-instancing-the-choice-of-method/
	//http://www.braynzarsoft.net/index.php?p=D3D11Instancing
	//-+
	//++
	//--
	//+- 


	XMFLOAT2 quad1Left, quad1Right;
	quad1Left = XMFLOAT2(0.4f, 0.0f);
	quad1Right = XMFLOAT2(-0.6f, -0.2f);

	XMFLOAT2 quad2Left, quad2Right;
	quad2Left = XMFLOAT2(0.0f, -0.7f);
	quad2Right = XMFLOAT2(0.1f, 0.3f);

	XMFLOAT2 quad3Left, quad3Right;
	quad3Left = XMFLOAT2(-0.55f, 0.05f);
	quad3Right = XMFLOAT2(0.25f, -0.7f);

	unsigned int i = 0;
	unsigned int v = 0;

	//Quad #1
	vertices[v+0].position = XMFLOAT3(quad1Left.x, 0.0f, quad1Left.y);  // Bottom left.
	vertices[v+0].texCoord = XMFLOAT2(0.0f, 1.0f);

	vertices[v+1].position = XMFLOAT3(quad1Left.x, 1.0f, quad1Left.y-0.2f);  // Top left.
	vertices[v+1].texCoord = XMFLOAT2(0.0f, 0.0f);

	vertices[v+2].position = XMFLOAT3(quad1Right.x, 1.0f, quad1Right.y-0.2f);  // Top right.
	vertices[v+2].texCoord = XMFLOAT2(1.0f, 0.0f);

	vertices[v+3].position = XMFLOAT3(quad1Right.x, 0.0f, quad1Right.y);  // Bottom right.
	vertices[v+3].texCoord = XMFLOAT2(1.0f, 1.0f);

	indices[i+0] = v+0;
	indices[i+1] = v+1;
	indices[i+2] = v+2;
	indices[i+3] = v+0;
	indices[i+4] = v+2;
	indices[i+5] = v+3;

	//The quad is "straight" so we'll only need to calculate normal once to know all of the normals
	tempVector = XMLoadFloat3(&CalculateVertexNormals(&vertices[1+v].position, &vertices[2+v].position, &vertices[0+v].position));
	tempVector = XMVector3Normalize((tempVector+upVector)/2);

	XMStoreFloat3(&vertexNormal, tempVector);

	vertices[v+0].normal = vertexNormal;
	vertices[v+1].normal = vertexNormal;
	vertices[v+2].normal = vertexNormal;
	vertices[v+3].normal = vertexNormal;

	i += 6;
	v += 4;

	//Quad #2
	vertices[v+0].position = XMFLOAT3(quad2Left.x, 0.0f, quad2Left.y);  // Bottom left.
	vertices[v+0].texCoord = XMFLOAT2(0.0f, 1.0f);

	vertices[v+1].position = XMFLOAT3(quad2Left.x-0.2f, 1.0f, quad2Left.y);  // Top left.
	vertices[v+1].texCoord = XMFLOAT2(0.0f, 0.0f);

	vertices[v+2].position = XMFLOAT3(quad2Right.x-0.2f, 1.0f, quad2Right.y);  // Top right.
	vertices[v+2].texCoord = XMFLOAT2(1.0f, 0.0f);

	vertices[v+3].position = XMFLOAT3(quad2Right.x, 0.0f, quad2Right.y);  // Bottom right.
	vertices[v+3].texCoord = XMFLOAT2(1.0f, 1.0f);

	indices[i+0] = v+0;
	indices[i+1] = v+1;
	indices[i+2] = v+2;
	indices[i+3] = v+0;
	indices[i+4] = v+2;
	indices[i+5] = v+3;

	//The quad is "straight" so we'll only need to calculate normal once to know all of the normals
	tempVector = XMLoadFloat3(&CalculateVertexNormals(&vertices[1+v].position, &vertices[2+v].position, &vertices[0+v].position));
	tempVector = XMVector3Normalize((tempVector+upVector)/2);

	XMStoreFloat3(&vertexNormal, tempVector);

	//I write "0+i" for clarity
	vertices[v+0].normal = vertexNormal;
	vertices[v+1].normal = vertexNormal;
	vertices[v+2].normal = vertexNormal;
	vertices[v+3].normal = vertexNormal;

	i += 6;
	v += 4;
	
	//Quad #3
	vertices[v+0].position = XMFLOAT3(quad3Left.x, 0.0f, quad3Left.y);  // Bottom left.
	vertices[v+0].texCoord = XMFLOAT2(0.0f, 1.0f);

	vertices[v+1].position = XMFLOAT3(quad3Left.x+0.3f, 1.0f, quad3Left.y+0.3f);  // Top left.
	vertices[v+1].texCoord = XMFLOAT2(0.0f, 0.0f);

	vertices[v+2].position = XMFLOAT3(quad3Right.x+0.3f, 1.0f, quad3Right.y+0.3f);  // Top right.
	vertices[v+2].texCoord = XMFLOAT2(1.0f, 0.0f);

	vertices[v+3].position = XMFLOAT3(quad3Right.x, 0.0f, quad3Right.y);  // Bottom right.
	vertices[v+3].texCoord = XMFLOAT2(1.0f, 1.0f);

	indices[i+0] = v+0;
	indices[i+1] = v+1;
	indices[i+2] = v+2;
	indices[i+3] = v+0;
	indices[i+4] = v+2;
	indices[i+5] = v+3;

	//The quad is "straight" so we'll only need to calculate normal once to know all of the normals
	tempVector = XMLoadFloat3(&CalculateVertexNormals(&vertices[1+v].position, &vertices[2+v].position, &vertices[0+v].position));
	tempVector = XMVector3Normalize((tempVector+upVector)/2);

	XMStoreFloat3(&vertexNormal, tempVector);

	vertices[v+0].normal = vertexNormal;
	vertices[v+1].normal = vertexNormal;
	vertices[v+2].normal = vertexNormal;
	vertices[v+3].normal = vertexNormal;

#pragma endregion
	mesh.SetIndexCount(indexCount);
	mesh.SetVertexCount(vertexCount);
	mesh.SetVertexStride(sizeof(VertexType));

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.get();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, mesh.GetVertexBufferPP());
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the sub resource texture a pointer to the index data.
	indexData.pSysMem = indices.get();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, mesh.GetIndexBufferPP());
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

bool VegetationManager::BuildInstanceBuffer( ID3D11Device* device, std::vector<InstanceType>* positions )
{
	D3D11_BUFFER_DESC instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA instanceData;
	HRESULT result;

	// Set the number of instances in the array.
	unsigned int instanceCount = positions->size();

	mesh.SetInstanceCount(instanceCount);

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = positions->data();
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, mesh.GetInstanceBufferPP());
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

XMFLOAT3 VegetationManager::CalculateVertexNormals( XMFLOAT3* topLeft, XMFLOAT3* topRight, XMFLOAT3* bottomLeft)
{
	XMVECTOR v1 = XMVector3Normalize(XMLoadFloat3(topLeft));
	XMVECTOR v2 = XMVector3Normalize(XMLoadFloat3(topRight));
	XMVECTOR v3 = XMVector3Normalize(XMLoadFloat3(bottomLeft));

	XMFLOAT3 result;

	XMStoreFloat3(&result, XMVector3Normalize(XMVector3Cross((v2-v1), (v3-v1))));

	return result;
}
