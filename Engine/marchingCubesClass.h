#include "metaballsClass.h"



class marchingCubesClass
{
	ID3D11Buffer *vertexBuffer, *indexBuffer;
	private:
	int vertexCount, indexCount;

	/* egenskaper for gridden, hvilke x,y,z-koordinater (i objekt-rommet) som gridden skal starte fra */
	double startX;
	double startY;
	double startZ;

	/* egenskaper for gridden, hvilke x,y,z-koordinater (i objekt-rommet) som gridden skal ende på */
	double endX;
	double endY;
	double endZ;

	/* hvor lange steg man skal ta når man beveger seg fra start_* til end_* */
	double stepX;
	double stepY;
	double stepZ;

	/* størrelsen på gridden, regnes ut i konstruktøren */
	int sizeX;
	int sizeY;
	int sizeZ;

	short lookup;

	/* isoverdien for metaballer */
	double metaballsIsoValue;

	/* hvorvidt vi skal tegne ting som en wireframe-struktur */
	bool wireframe;

	/* en peker til et metaball-objekt */
	metaballsClass *mb;

	/* tabellene for oppslag av kanter og triangler */
	const static int edgeTable[256];
	const static int triTable[256][16];
	
	MarchingCubeVectors *things;
	
	/* lagrer punktene som er generert */
	marchingCubeVertex *marchingCubeVertices;

	/* lagrer punktene som genereres fra en enkelt kube */
	marchingCubeVertex verts[12];



public:
	/* konstruktør for objektet, tar initialiseringsverdier som beskrevet lenger ned */
	marchingCubesClass(	double startX, double startY, double startZ, double endX, double endY, double endZ,
			double stepX, double stepY, double stepZ);
	virtual ~marchingCubesClass();

	/* ikke i bruk, tiltenkt for å kunne resize gridden dynamisk */
	void resize();
	/* regner ut hvorvidt punktene er innenfor eller utenfor */
	void computeMetaBalls();
	/* tegner metaballene til opengl */
	void calculateMesh(ID3D11Device* device);

	bool Render(ID3D11DeviceContext* context);

	int GetIndexCount() { return indexCount; }

	/* setter peker til metaballs-objekt og isoverdien man ønsker å benytte */
	void setMetaBalls(metaballsClass *mb, double isoValue)
	{
		this->mb = mb;
		this->mb->setIsoValue(isoValue);
		this->metaballsIsoValue = isoValue;
	}

	void setWireframe(bool s)
	{
		this->wireframe = s;
	}



};