#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "QuadMesh.h"

// constructor: allocate basic state and memory for max mesh size
QuadMesh::QuadMesh(int maxMeshSize, float meshDim)
{
	// set minimum allowed mesh size and clear pointers/counters
	minMeshSize = 1;
	numVertices = 0;
	vertices = NULL;
	numQuads = 0;
	quads = NULL;
	numFacesDrawn = 0;

	// store provided sizes (clamp to min)
	this->maxMeshSize = maxMeshSize < minMeshSize ? minMeshSize : maxMeshSize;
	this->meshDim = meshDim;
	CreateMemory();
}

// create memory for vertex and quad arrays, return false if allocation failed
bool QuadMesh::CreateMemory()
{
	// allocate vertex array for a (maxMeshSize+1)^2 grid
	vertices = new MeshVertex[(maxMeshSize + 1) * (maxMeshSize + 1)];
	if (!vertices)
		return false;

	// allocate quad array for maxMeshSize^2 quads
	quads = new MeshQuad[maxMeshSize * maxMeshSize];
	if (!quads)
		return false;

	return true;
}

// free allocated memory and reset vbo flag
void QuadMesh::FreeMemory()
{
	if (vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}
	if (quads)
	{
		delete[] quads;
		quads = nullptr;
	}
	vboReady = false;
}

// initialize mesh geometry and cpu-side vbo arrays
// meshSize: number of quads per side
// origin: starting corner position
// meshLength/meshWidth: extents along dir1 and dir2
// dir1/dir2: directions spanning the mesh plane
bool QuadMesh::InitMesh(int meshSize, glm::vec3 origin, double meshLength, double meshWidth, glm::vec3 dir1, glm::vec3 dir2)
{
	glm::vec3 o;
	int currentVertex = 0;

	// step vectors for grid spacing
	glm::vec3 v1 = dir1 * (float)(meshLength / meshSize);
	glm::vec3 v2 = dir2 * (float)(meshWidth / meshSize);

	glm::vec3 meshpt;
	numVertices = (meshSize + 1) * (meshSize + 1);
	o = origin;

	// clear any existing vbo arrays
	std::vector<float>().swap(verticesVBO);
	std::vector<float>().swap(normalsVBO);
	std::vector<unsigned int>().swap(indices);

	// create vertex positions row by row and fill cpu position array
	for (int i = 0; i < meshSize + 1; i++)
	{
		for (int j = 0; j < meshSize + 1; j++)
		{
			meshpt = o + v1 * (float)j;
			vertices[currentVertex].position = meshpt;
			addVertex(meshpt.x, meshpt.y, meshpt.z); // also push to verticesVBO
			currentVertex++;
		}
		o += v2; // move to next row
	}

	// create quads and build index list for each quad
	numQuads = (meshSize) * (meshSize);
	int currentQuad = 0;

	for (int j = 0; j < meshSize; j++)
	{
		for (int k = 0; k < meshSize; k++)
		{
			// assign quad vertex pointers into vertex array
			quads[currentQuad].vertices[0] = &vertices[j * (meshSize + 1) + k];
			quads[currentQuad].vertices[1] = &vertices[j * (meshSize + 1) + k + 1];
			quads[currentQuad].vertices[2] = &vertices[(j + 1) * (meshSize + 1) + k + 1];
			quads[currentQuad].vertices[3] = &vertices[(j + 1) * (meshSize + 1) + k];
			currentQuad++;

			// add quad indices in winding order (for element array)
			addIndices(j * (meshSize + 1) + k, j * (meshSize + 1) + k + 1,
								 (j + 1) * (meshSize + 1) + k + 1, (j + 1) * (meshSize + 1) + k);
		}
	}

	// compute smooth vertex normals and fill normalsVBO
	this->ComputeNormals();
	for (int j = 0; j < currentVertex; j++)
	{
		addNormal(vertices[j].normal.x, vertices[j].normal.y, vertices[j].normal.z);
	}
	return true;
}

// draw mesh using immediate mode (glBegin/glEnd)
// meshSize provided to know how many quads to draw
void QuadMesh::DrawMesh(int meshSize)
{
	int currentQuad = 0;

	for (int j = 0; j < meshSize; j++)
	{
		for (int k = 0; k < meshSize; k++)
		{
			glBegin(GL_QUADS);
			for (int v = 0; v < 4; v++)
			{
				// set normal then vertex for each corner
				glNormal3f(quads[currentQuad].vertices[v]->normal.x,
									 quads[currentQuad].vertices[v]->normal.y,
									 quads[currentQuad].vertices[v]->normal.z);
				glVertex3f(quads[currentQuad].vertices[v]->position.x,
									 quads[currentQuad].vertices[v]->position.y,
									 quads[currentQuad].vertices[v]->position.z);
			}
			glEnd();
			currentQuad++;
		}
	}
}

// add one vertex to the cpu-side position array used for vbo upload
void QuadMesh::addVertex(float x, float y, float z)
{
	verticesVBO.push_back(x);
	verticesVBO.push_back(y);
	verticesVBO.push_back(z);
}

// add one normal to the cpu-side normal array used for vbo upload
void QuadMesh::addNormal(float nx, float ny, float nz)
{
	normalsVBO.push_back(nx);
	normalsVBO.push_back(ny);
	normalsVBO.push_back(nz);
}

// add four indices for a quad to the index list (element array)
void QuadMesh::addIndices(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
	indices.push_back(i4);
}

// compute vertex normals by averaging corner normals of each quad
// this fills per-vertex normal in the vertex array
void QuadMesh::ComputeNormals()
{
	int currentQuad = 0;
	for (int j = 0; j < this->maxMeshSize; j++)
	{
		for (int k = 0; k < this->maxMeshSize; k++)
		{
			glm::vec3 n0, n1, n2, n3, e0, e1, e2, e3;

			// reset normals for this quad's vertices (accumulate then normalize)
			quads[currentQuad].vertices[0]->normal = glm::vec3(0.0f);
			quads[currentQuad].vertices[1]->normal = glm::vec3(0.0f);
			quads[currentQuad].vertices[2]->normal = glm::vec3(0.0f);
			quads[currentQuad].vertices[3]->normal = glm::vec3(0.0f);

			// compute edge directions around the quad
			e0 = quads[currentQuad].vertices[1]->position - quads[currentQuad].vertices[0]->position;
			e1 = quads[currentQuad].vertices[2]->position - quads[currentQuad].vertices[1]->position;
			e2 = quads[currentQuad].vertices[3]->position - quads[currentQuad].vertices[2]->position;
			e3 = quads[currentQuad].vertices[0]->position - quads[currentQuad].vertices[3]->position;

			e0 = glm::normalize(e0);
			e1 = glm::normalize(e1);
			e2 = glm::normalize(e2);
			e3 = glm::normalize(e3);

			// compute corner normals using adjacent edges and add to vertex normal
			n0 = glm::normalize(glm::cross(e0, -e3));
			n1 = glm::normalize(glm::cross(e1, -e0));
			n2 = glm::normalize(glm::cross(e2, -e1));
			n3 = glm::normalize(glm::cross(e3, -e2));

			quads[currentQuad].vertices[0]->normal += n0;
			quads[currentQuad].vertices[1]->normal += n1;
			quads[currentQuad].vertices[2]->normal += n2;
			quads[currentQuad].vertices[3]->normal += n3;

			// normalize the accumulated normals
			quads[currentQuad].vertices[0]->normal = glm::normalize(quads[currentQuad].vertices[0]->normal);
			quads[currentQuad].vertices[1]->normal = glm::normalize(quads[currentQuad].vertices[1]->normal);
			quads[currentQuad].vertices[2]->normal = glm::normalize(quads[currentQuad].vertices[2]->normal);
			quads[currentQuad].vertices[3]->normal = glm::normalize(quads[currentQuad].vertices[3]->normal);

			currentQuad++;
		}
	}
}

// create gpu vbos from cpu-side vectors (positions, normals, indices)
// attribVertexPosition and attribVertexNormal specify shader attribute locations
void QuadMesh::CreateMeshVBO(int /*meshSize*/, GLint attribVertexPosition, GLint attribVertexNormal)
{
	if (vboReady)
		return;
	if (verticesVBO.empty() || normalsVBO.empty() || indices.empty())
		return;

	attrPos = attribVertexPosition;
	attrNorm = attribVertexNormal;

	glGenBuffers(3, vbos);

	// positions buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesVBO.size(), verticesVBO.data(), GL_STATIC_DRAW);

	// normals buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalsVBO.size(), normalsVBO.data(), GL_STATIC_DRAW);

	// element/index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// unbind to leave clean state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	vboReady = true;
}

// draw mesh using vbos (vertex attribs must be enabled by shader)
// falls back to immediate mode if vbos not ready
void QuadMesh::DrawMeshVBO(int /*meshSize*/)
{
	if (!vboReady)
	{
		// fallback to immediate mode drawing if vbos not ready
		DrawMesh(maxMeshSize);
		return;
	}

	// bind position buffer and set attribute pointer
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glEnableVertexAttribArray((GLuint)attrPos);
	glVertexAttribPointer((GLuint)attrPos, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	// bind normal buffer and set attribute pointer
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glEnableVertexAttribArray((GLuint)attrNorm);
	glVertexAttribPointer((GLuint)attrNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	// bind index buffer and draw quads
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
	glDrawElements(GL_QUADS, (GLsizei)indices.size(), GL_UNSIGNED_INT, (void *)0);

	// disable and unbind
	glDisableVertexAttribArray((GLuint)attrPos);
	glDisableVertexAttribArray((GLuint)attrNorm);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// convenience: create a single quad unit panel centered at origin
QuadMesh *QuadMesh::MakeUnitPanel()
{
	auto *m = new QuadMesh(1, 1.0f);
	m->InitMesh(1, glm::vec3(-0.5f, -0.5f, 0.0f), 1.0, 1.0, glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
	return m;
}
