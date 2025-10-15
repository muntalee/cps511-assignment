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

#include "Vectors.h"
#include "QuadMesh.h"

QuadMesh::QuadMesh(int maxMeshSize, float meshDim)
{
	minMeshSize = 1;
	numVertices = 0;
	vertices = NULL;
	numQuads = 0;
	quads = NULL;
	numFacesDrawn = 0;

	this->maxMeshSize = maxMeshSize < minMeshSize ? minMeshSize : maxMeshSize;
	this->meshDim = meshDim;
	CreateMemory();
}

// create memory for mesh
bool QuadMesh::CreateMemory()
{
	vertices = new MeshVertex[(maxMeshSize + 1) * (maxMeshSize + 1)];
	if (!vertices)
		return false;

	quads = new MeshQuad[maxMeshSize * maxMeshSize];
	if (!quads)
		return false;

	return true;
}

// initialize mesh
bool QuadMesh::InitMesh(int meshSize, Vector3 origin, double meshLength, double meshWidth, Vector3 dir1, Vector3 dir2)
{
	Vector3 o;
	int currentVertex = 0;
	double sf1, sf2;

	Vector3 v1, v2;

	v1 = dir1 * (meshLength / meshSize);
	v2 = dir2 * (meshWidth / meshSize);

	Vector3 meshpt;

	numVertices = (meshSize + 1) * (meshSize + 1);
	o.set(origin.x, origin.y, origin.z);

	std::vector<float>().swap(verticesVBO);
	std::vector<float>().swap(normalsVBO);
	std::vector<unsigned int>().swap(indices);

	for (int i = 0; i < meshSize + 1; i++)
	{
		for (int j = 0; j < meshSize + 1; j++)
		{
			meshpt = o + v1 * (float)j;
			vertices[currentVertex].position.set(meshpt.x, meshpt.y, meshpt.z);
			addVertex(meshpt.x, meshpt.y, meshpt.z);
			currentVertex++;
		}
		o += v2;
	}

	numQuads = (meshSize) * (meshSize);
	int currentQuad = 0;

	for (int j = 0; j < meshSize; j++)
	{
		for (int k = 0; k < meshSize; k++)
		{
			quads[currentQuad].vertices[0] = &vertices[j * (meshSize + 1) + k];
			quads[currentQuad].vertices[1] = &vertices[j * (meshSize + 1) + k + 1];
			quads[currentQuad].vertices[2] = &vertices[(j + 1) * (meshSize + 1) + k + 1];
			quads[currentQuad].vertices[3] = &vertices[(j + 1) * (meshSize + 1) + k];
			currentQuad++;
			addIndices(j * (meshSize + 1) + k, j * (meshSize + 1) + k + 1,
								 (j + 1) * (meshSize + 1) + k + 1, (j + 1) * (meshSize + 1) + k);
		}
	}

	this->ComputeNormals();
	for (int j = 0; j < currentVertex; j++)
	{
		addNormal(vertices[j].normal.x, vertices[j].normal.y, vertices[j].normal.z);
	}
	return true;
}

// draw mesh using immediate mode
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

// add one vertex to array
void QuadMesh::addVertex(float x, float y, float z)
{
	verticesVBO.push_back(x);
	verticesVBO.push_back(y);
	verticesVBO.push_back(z);
}

// add one normal to array
void QuadMesh::addNormal(float nx, float ny, float nz)
{
	normalsVBO.push_back(nx);
	normalsVBO.push_back(ny);
	normalsVBO.push_back(nz);
}

// add one quad indices
void QuadMesh::addIndices(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
	indices.push_back(i4);
}

// compute normals for mesh
void QuadMesh::ComputeNormals()
{
	int currentQuad = 0;
	for (int j = 0; j < this->maxMeshSize; j++)
	{
		for (int k = 0; k < this->maxMeshSize; k++)
		{
			Vector3 n0, n1, n2, n3, e0, e1, e2, e3;

			quads[currentQuad].vertices[0]->normal.set(0, 0, 0);
			quads[currentQuad].vertices[1]->normal.set(0, 0, 0);
			quads[currentQuad].vertices[2]->normal.set(0, 0, 0);
			quads[currentQuad].vertices[3]->normal.set(0, 0, 0);

			e0 = quads[currentQuad].vertices[1]->position - quads[currentQuad].vertices[0]->position;
			e1 = quads[currentQuad].vertices[2]->position - quads[currentQuad].vertices[1]->position;
			e2 = quads[currentQuad].vertices[3]->position - quads[currentQuad].vertices[2]->position;
			e3 = quads[currentQuad].vertices[0]->position - quads[currentQuad].vertices[3]->position;

			e0.normalize();
			e1.normalize();
			e2.normalize();
			e3.normalize();

			n0 = e0.cross(-e3);
			n0.normalize();
			quads[currentQuad].vertices[0]->normal += n0;

			n1 = e1.cross(-e0);
			n1.normalize();
			quads[currentQuad].vertices[1]->normal += n1;

			n2 = e2.cross(-e1);
			n2.normalize();
			quads[currentQuad].vertices[2]->normal += n2;

			n3 = e3.cross(-e2);
			n3.normalize();
			quads[currentQuad].vertices[3]->normal += n3;

			quads[currentQuad].vertices[0]->normal.normalize();
			quads[currentQuad].vertices[1]->normal.normalize();
			quads[currentQuad].vertices[2]->normal.normalize();
			quads[currentQuad].vertices[3]->normal.normalize();

			currentQuad++;
		}
	}
}

// create mesh VBOs
void QuadMesh::CreateMeshVBO(int /*meshSize*/, GLint attribVertexPosition, GLint attribVertexNormal)
{
	if (vboReady)
		return;
	if (verticesVBO.empty() || normalsVBO.empty() || indices.empty())
		return;

	attrPos = attribVertexPosition;
	attrNorm = attribVertexNormal;

	glGenBuffers(3, vbos);

	// positions
	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesVBO.size(), verticesVBO.data(), GL_STATIC_DRAW);

	// normals
	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalsVBO.size(), normalsVBO.data(), GL_STATIC_DRAW);

	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	vboReady = true;
}

// draw mesh using VBOs
void QuadMesh::DrawMeshVBO(int /*meshSize*/)
{
	if (!vboReady)
	{
		DrawMesh(maxMeshSize);
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
	glEnableVertexAttribArray((GLuint)attrPos);
	glVertexAttribPointer((GLuint)attrPos, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
	glEnableVertexAttribArray((GLuint)attrNorm);
	glVertexAttribPointer((GLuint)attrNorm, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[2]);
	glDrawElements(GL_QUADS, (GLsizei)indices.size(), GL_UNSIGNED_INT, (void *)0);

	glDisableVertexAttribArray((GLuint)attrPos);
	glDisableVertexAttribArray((GLuint)attrNorm);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// make a unit panel mesh
QuadMesh *QuadMesh::MakeUnitPanel()
{
	auto *m = new QuadMesh(1, 1.0f);
	m->InitMesh(1, Vector3(-0.5f, -0.5f, 0.0f), 1.0, 1.0, Vector3(1, 0, 0), Vector3(0, 1, 0));
	return m;
}

// draw a box using a panel mesh
void QuadMesh::DrawBoxFromPanel(QuadMesh *panel, float w, float h, float d)
{
	if (!panel)
		return;

	// front
	glPushMatrix();
	glTranslatef(0, 0, d * 0.5f);
	glScalef(w, h, 1);
	panel->DrawMesh(1);
	glPopMatrix();
	// back
	glPushMatrix();
	glTranslatef(0, 0, -d * 0.5f);
	glRotatef(180, 0, 1, 0);
	glScalef(w, h, 1);
	panel->DrawMesh(1);
	glPopMatrix();
	// right
	glPushMatrix();
	glTranslatef(w * 0.5f, 0, 0);
	glRotatef(90, 0, 1, 0);
	glScalef(d, h, 1);
	panel->DrawMesh(1);
	glPopMatrix();
	// left
	glPushMatrix();
	glTranslatef(-w * 0.5f, 0, 0);
	glRotatef(-90, 0, 1, 0);
	glScalef(d, h, 1);
	panel->DrawMesh(1);
	glPopMatrix();
	// top
	glPushMatrix();
	glTranslatef(0, h * 0.5f, 0);
	glRotatef(-90, 1, 0, 0);
	glScalef(w, d, 1);
	panel->DrawMesh(1);
	glPopMatrix();
	// bottom
	glPushMatrix();
	glTranslatef(0, -h * 0.5f, 0);
	glRotatef(90, 1, 0, 0);
	glScalef(w, d, 1);
	panel->DrawMesh(1);
	glPopMatrix();
}
