#pragma once;
#include "metaballsClass.h"
#include "TreeClass.h"
#include "MCTerrainClass.h"
#include "types.h"
#include <windows.h>
#include "Utility.h"


class MarchingCubesClass
{
public:
	MarchingCubesClass(	XMFLOAT3 startPosition, XMFLOAT3 endPosition,
		XMFLOAT3 stepSize, SimplexNoise* simplexNoise);

	~MarchingCubesClass();

	//Calculates whether the points are inside or outside.
	void ComputeMetaBalls();

	//Extract a cube from the .. voxel field...
	void ExtractCube(MarchingCubeVertex** cube);

	//Calculate the lookup value we'll be using to index into the fields
	void CalculateLookupValue(unsigned int* lookup, unsigned int index, MarchingCubeVertex** cube);

	//Triangulate the cube that was extracted before, with the help of our lookup value
	void ProcessCube(unsigned int lookupValue, MarchingCubeVertex* verts, MarchingCubeVertex** cube, vector<unsigned long>& indices, 
		vector<MarchingCubeVectors>& vertices, unsigned int& indexCounter, unsigned int& vertexCounter);

	void SetupBuffers(ID3D11Device* device, vector<unsigned long>& indices, vector<MarchingCubeVectors>& vertices, unsigned int indexCount, unsigned int vertexCount);

	//Converts the metaballs into a mesh for directx to use
	void CalculateMesh(ID3D11Device* device);

	//Render the calculated mesh
	bool Render(ID3D11DeviceContext* context);

	//Reset the mesh
	void Reset(SimplexNoise* simplexNoise);
	void LSystemTree(int vertexID);
	void LSystemTree();
	void Ground(int vertexID);

	float GetHeightOfXZpos();
	int GetIndexCount() { return indexCount; }
	int GetSizeX() { return sizeX; }
	int GetSizeY() { return sizeY; }
	int GetSizeZ() { return sizeZ; }

	TreeClass* GetTree(){return tree; }
	MCTerrainClass* GetTerrain(){ return terrain; }

	/* Setter for metaball pointer and isovalue */
	void SetMetaBalls(MetaballsClass *mb, float isoValue)
	{
		this->mb = mb;
		this->mb->SetIsoValue(isoValue);
		this->metaballsIsoValue = isoValue;
	}

	void SetWireframe(bool s)
	{
		this->wireframe = s;
	}

	private:
	// Returns a point that is interpolated with ten other points for both normals and possition
	inline MarchingCubeVertex Interpolate(MarchingCubeVertex v1, MarchingCubeVertex v2)
	{
		MarchingCubeVertex v;
		float diff;

		diff = (this->metaballsIsoValue - v1.density) / (v2.density - v1.density);
		
		//Interpolate density as well.
		//We don't actually use density after this point, so there's no need to calculate it, but just in case we expand on this in the future ...
		v.density = v1.density + (v2.density - v1.density) * diff;

		// Finds where on the line our point is positioned
		v.position.x = v1.position.x + (v2.position.x - v1.position.x) * diff;
		v.position.y = v1.position.y + (v2.position.y - v1.position.y) * diff;
		v.position.z = v1.position.z + (v2.position.z - v1.position.z) * diff;

		// calculates the average normal for each point
		v.normal.x = v1.normal.x + (v2.normal.x - v1.normal.x) * diff;
		v.normal.y = v1.normal.y + (v2.normal.y - v1.normal.y) * diff;
		v.normal.z = v1.normal.z + (v2.normal.z - v1.normal.z) * diff;

		//Normalize the normal
		float vectorLength = sqrt((v.normal.x*v.normal.x) + (v.normal.y*v.normal.y) + (v.normal.z*v.normal.z));

		v.normal.x /= vectorLength;
		v.normal.y /= vectorLength;
		v.normal.z /= vectorLength;

		return v;
	}

private:
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	int vertexCount, indexCount;
	int x,y,z;
	int index;

	// Properties for the grid, what x,y,z coordinates in object space the grid will start at.
	float startX;
	float startY;
	float startZ;

	// Properties for the grid, what x,y,z coordinates in object space the grid will end at.
	float endX;
	float endY;
	float endZ;

	// How long each step will be between start(x,y,z) and end(x,y,z).
	float stepX;
	float stepY;
	float stepZ;

	// Size of the grid. Is calculated in the constructor.
	int sizeX;
	int sizeY;
	int sizeZ;

	float metaballsIsoValue;

	// Whether we want to draw with wireframe or not.
	bool wireframe;

	// Pointer to a metaball object 
	MetaballsClass *mb;
	TreeClass *tree;
	MCTerrainClass *terrain;

	// Tables for edge cases and triangle lookup 
	const static int edgeTable[256];
	const static int triTable[256][16];
	
	// Stores the points that we generate
	MarchingCubeVertex *marchingCubeVertices;

	// Stores the points from a simple cube 
	MarchingCubeVertex verts[12];
};