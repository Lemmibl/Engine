#include "metaballsClass.h"



class MarchingCubesClass
{
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	private:
	int vertexCount, indexCount;

	/* Properties for the grid, what x,y,z coordinates in object space the grid will start at. */
	double startX;
	double startY;
	double startZ;

	/* Properties for the grid, what x,y,z coordinates in object space the grid will end at. */
	double endX;
	double endY;
	double endZ;

	/* How long each step will be between start(x,y,z) and end(x,y,z). */
	double stepX;
	double stepY;
	double stepZ;

	/* Size of the grid. Is calculated in the constructor. */
	int sizeX;
	int sizeY;
	int sizeZ;

	short lookup;
	double metaballsIsoValue;

	/* Whether we want to draw with wireframe or not. */
	bool wireframe;

	/* Pointer to a metaball object */
	MetaballsClass *mb;

	/* Tables for edge cases and triangle lookup */
	const static int edgeTable[256];
	const static int triTable[256][16];
	
	MarchingCubeVectors *things;
	
	/* Stores the points that we generate */
	MarchingCubeVertex *marchingCubeVertices;

	/* Stores the points from a simple cube */
	MarchingCubeVertex verts[12];



public:
	MarchingCubesClass(	double startX, double startY, double startZ, double endX, double endY, double endZ,
			double stepX, double stepY, double stepZ);

	virtual ~MarchingCubesClass();

	/* Calculates whether the points are inside or outside. */
	void ComputeMetaBalls();

	/* Converts the metaballs into a mesh for directx to use */
	void CalculateMesh(ID3D11Device* device);

	bool Render(ID3D11DeviceContext* context);

	int GetIndexCount() { return indexCount; }

	/* Setter for metaball pointer and isovalue */
	void SetMetaBalls(MetaballsClass *mb, double isoValue)
	{
		this->mb = mb;
		this->mb->SetIsoValue(isoValue);
		this->metaballsIsoValue = isoValue;
	}

	void SetWireframe(bool s)
	{
		this->wireframe = s;
	}



};