
#include <GL/glew.h>
#include "Vectors.h"

struct MeshVertex
{
	Vector3	position;
	Vector3 normal;
};



struct MeshQuad
{
	// pointers to vertices of each quad
	MeshVertex *vertices[4];	
};


class QuadMesh
{
private:
	
	int maxMeshSize;
	int minMeshSize;
	float meshDim;

	int numVertices;
	MeshVertex *vertices;

	int numQuads;
	MeshQuad *quads;

	// These structures are filled in for you in InitMesh() and you may make use 
	// of them for drawing the mesh using VBOs (or a VAO and VBOs)
	std::vector<float> verticesVBO;
	std::vector<float> normalsVBO;
	std::vector<unsigned int> indices;

	int numFacesDrawn;
	
	GLfloat mat_ambient[4];
    GLfloat mat_specular[4];
    GLfloat mat_diffuse[4];
	GLfloat mat_shininess[1];

	GLuint vao;
	GLuint vbos[3];
	
	
	

	
private:
	bool CreateMemory();
	void FreeMemory();

public:

	typedef std::pair<int, int> MaxMeshDim;

	QuadMesh(int maxMeshSize = 40, float meshDim = 1.0f);
	
	~QuadMesh()
	{
		FreeMemory();
	}

	MaxMeshDim GetMaxMeshDimentions()
	{
		return MaxMeshDim(minMeshSize, maxMeshSize);
	}
	void addVertex(float x, float y, float z);
	void addNormal(float nx, float ny, float nz);
	void addIndices(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4);
	
	bool InitMesh(int meshSize, Vector3 origin, double meshLength, double meshWidth,Vector3 dir1, Vector3 dir2);
	void DrawMesh(int meshSize); // Draws using Immediate Mode Rendering

	// Draw using VBOs - you need to fill in this code as well as CreateMeshVBO and then in 
	// Robot3D.cpp you need to call DrawMeshVBO() instead of DrawMesh()
	void DrawMeshVBO(int meshSize); 
	void CreateMeshVBO(int meshSize, GLint attribVertexPosition, GLint attribVertexNormal);

	
	
	void SetMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular, double shininess);
	void ComputeNormals();
	
	
};
