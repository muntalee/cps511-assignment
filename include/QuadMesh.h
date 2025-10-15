#include <GL/glew.h>
#include "Vectors.h"
#include <vector>

struct MeshVertex
{
	Vector3 position;
	Vector3 normal;
};
struct MeshQuad
{
	MeshVertex *vertices[4];
};

class QuadMesh
{
private:
	int maxMeshSize, minMeshSize;
	float meshDim;

	int numVertices;
	MeshVertex *vertices;

	int numQuads;
	MeshQuad *quads;

	std::vector<float> verticesVBO;
	std::vector<float> normalsVBO;
	std::vector<unsigned int> indices;

	int numFacesDrawn;

	GLfloat mat_ambient[4];
	GLfloat mat_specular[4];
	GLfloat mat_diffuse[4];
	GLfloat mat_shininess[1];

	GLuint vbos[3] = {0, 0, 0}; // 0:pos, 1:norm, 2:ebo
	bool vboReady = false;
	GLint attrPos = -1;
	GLint attrNorm = -1;

private:
	bool CreateMemory();
	void FreeMemory();

public:
	typedef std::pair<int, int> MaxMeshDim;
	QuadMesh(int maxMeshSize = 40, float meshDim = 1.0f);
	~QuadMesh() { FreeMemory(); }

	MaxMeshDim GetMaxMeshDimentions() { return MaxMeshDim(minMeshSize, maxMeshSize); }

	void addVertex(float x, float y, float z);
	void addNormal(float nx, float ny, float nz);
	void addIndices(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4);

	bool InitMesh(int meshSize, Vector3 origin, double meshLength, double meshWidth, Vector3 dir1, Vector3 dir2);
	void DrawMesh(int meshSize); // legacy immediate mode

	void CreateMeshVBO(int meshSize, GLint attribVertexPosition, GLint attribVertexNormal);
	void DrawMeshVBO(int meshSize);

	void SetMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular, double shininess);
	void ComputeNormals();

	static QuadMesh *MakeUnitPanel();
	static void DrawBoxFromPanel(QuadMesh *panel, float w, float h, float d);
};
