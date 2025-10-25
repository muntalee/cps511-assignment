#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

// simple vertex: position + normal
struct MeshVertex
{
	glm::vec3 position; // vertex position in 3d
	glm::vec3 normal;		// vertex normal vector
};

// quad made of four pointers to mesh vertices (shared vertices)
struct MeshQuad
{
	MeshVertex *vertices[4]; // pointers to the 4 corner vertices
};

// quad mesh container and rendering helper
class QuadMesh
{
private:
	int maxMeshSize, minMeshSize; // allowed mesh size limits
	float meshDim;								// physical dimension per cell

	int numVertices;			// total allocated vertex count
	MeshVertex *vertices; // contiguous vertex storage

	int numQuads;		 // total allocated quad count
	MeshQuad *quads; // contiguous quad storage

	// cpu-side vbo data buffers (interleaved or separate as floats)
	std::vector<float> verticesVBO;		 // positions for vbo
	std::vector<float> normalsVBO;		 // normals for vbo
	std::vector<unsigned int> indices; // index/ebo data

	int numFacesDrawn; // number of faces currently prepared to draw

	// simple material properties for fixed-function or simple shader use
	GLfloat mat_ambient[4];
	GLfloat mat_specular[4];
	GLfloat mat_diffuse[4];
	GLfloat mat_shininess[1];

	// opengl buffer object ids: 0=pos, 1=norm, 2=ebo
	GLuint vbos[3] = {0, 0, 0};
	bool vboReady = false; // true when vbos are created and populated
	GLint attrPos = -1;		 // attribute location for position
	GLint attrNorm = -1;	 // attribute location for normal

private:
	// allocate cpu-side arrays for vertices/quads
	bool CreateMemory();
	// free cpu-side arrays and reset counters
	void FreeMemory();

public:
	typedef std::pair<int, int> MaxMeshDim;
	// ctor: set max mesh size and default mesh cell dim
	QuadMesh(int maxMeshSize = 40, float meshDim = 1.0f);
	~QuadMesh() { FreeMemory(); }

	// return min,max mesh dimensions allowed
	MaxMeshDim GetMaxMeshDimentions() { return MaxMeshDim(minMeshSize, maxMeshSize); }

	// helpers to append data into the temporary vbo arrays
	void addVertex(float x, float y, float z);																					 // push position into verticesVBO
	void addNormal(float nx, float ny, float nz);																				 // push normal into normalsVBO
	void addIndices(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4); // push quad indices

	// build a mesh of given size at origin using two direction vectors and lengths
	bool InitMesh(int meshSize, glm::vec3 origin, double meshLength, double meshWidth, glm::vec3 dir1, glm::vec3 dir2);
	// draw mesh using legacy immediate mode (slow, for debugging)
	void DrawMesh(int meshSize); // legacy immediate mode

	// create vbos and upload data; provide shader attribute locations
	void CreateMeshVBO(int meshSize, GLint attribVertexPosition, GLint attribVertexNormal);
	// draw using prepared vbos (fast)
	void DrawMeshVBO(int meshSize);

	// set simple material parameters
	void SetMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, double shininess);
	// compute per-vertex normals from quad faces
	void ComputeNormals();

	// create a unit panel mesh helper
	static QuadMesh *MakeUnitPanel();
	// build a box from a panel by extruding and creating 6 faces
	static void DrawBoxFromPanel(QuadMesh *panel, float w, float h, float d);
};
