#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h> // For wglSwapInterval
#endif

#define FREEGLUT_STATIC
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Vectors.h"
#include "QuadMesh.h"

#define POSITION_ATTRIBUTE 0
#define NORMAL_ATTRIBUTE 2

#define BUFFER_OFFSET(offset) ((void*)(offset))
#define MEMBER_OFFSET(s,m) ((char*)NULL + (offsetof(s,m)))

QuadMesh::QuadMesh(int maxMeshSize, float meshDim)
{
	minMeshSize =1;
	numVertices = 0;
	vertices = NULL;
	numQuads = 0;
	quads = NULL;
	numFacesDrawn = 0;
	
	this->maxMeshSize = maxMeshSize < minMeshSize ? minMeshSize : maxMeshSize;
	this->meshDim = meshDim;
	CreateMemory();

	// setup the material and lights used for the mesh
	mat_ambient[0] = 0.0;
	mat_ambient[1] = 0.0;
	mat_ambient[2] = 0.0;
	mat_ambient[3] = 1.0;
	mat_specular[0] = 0.0;
	mat_specular[1] = 0.0;
	mat_specular[2] = 0.0;
	mat_specular[3] = 1.0;
	mat_diffuse[0] = 0.9;
	mat_diffuse[1] = 0.5;
	mat_diffuse[2] = 0.0;
	mat_diffuse[3] = 1.0;
	mat_shininess[0] = 0.0;
    
}

void QuadMesh::SetMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular, double shininess)
{
	mat_ambient[0] = ambient.x;
	mat_ambient[1] = ambient.y;
	mat_ambient[2] = ambient.z;
	mat_ambient[3] = 1.0;
	mat_specular[0] = specular.x;
	mat_specular[1] = specular.y;
	mat_specular[2] = specular.z;
	mat_specular[3] = 1.0;
	mat_diffuse[0] = diffuse.x;
	mat_diffuse[1] = diffuse.y;
	mat_diffuse[2] = diffuse.z;
	mat_diffuse[3] = 1.0;
	mat_shininess[0] = shininess;
}

bool QuadMesh::CreateMemory()
{
	vertices = new MeshVertex[(maxMeshSize+1)*(maxMeshSize+1)];
	if(!vertices)
	{
		return false;
	}

	quads = new MeshQuad[maxMeshSize*maxMeshSize];
	if(!quads)
	{
		return false;
	}

	
	return true;
}
		
///////////////////////////////////////////////////////////////////////////////
// add single vertex to array
///////////////////////////////////////////////////////////////////////////////
void QuadMesh::addVertex(float x, float y, float z)
{
	verticesVBO.push_back(x);
	verticesVBO.push_back(y);
	verticesVBO.push_back(z);
}



///////////////////////////////////////////////////////////////////////////////
// add single normal to array
///////////////////////////////////////////////////////////////////////////////
void QuadMesh::addNormal(float nx, float ny, float nz)
{
	normalsVBO.push_back(nx);
	normalsVBO.push_back(ny);
	normalsVBO.push_back(nz);
}

void QuadMesh::addIndices(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4)
{
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
	indices.push_back(i4);
}


bool QuadMesh::InitMesh(int meshSize,Vector3 origin,double meshLength,double meshWidth,Vector3 dir1, Vector3 dir2)
{
	Vector3 o;
	int currentVertex = 0; 	  
	double sf1,sf2; 
    
	Vector3 v1,v2;
	
	v1.x = dir1.x;
	v1.y = dir1.y;
	v1.z = dir1.z;

	sf1 = meshLength/meshSize;
	v1 *= sf1;

	v2.x = dir2.x;
	v2.y = dir2.y;
	v2.z = dir2.z;
	sf2 = meshWidth/meshSize;
	v2 *= sf2;
    
	Vector3 meshpt;
	
	// VERTICES
	numVertices=(meshSize+1)*(meshSize+1);
	
	// Starts at front left corner of mesh 
	o.set(origin.x,origin.y,origin.z);

	std::vector<float>().swap(verticesVBO);
	std::vector<float>().swap(normalsVBO);
	std::vector<unsigned int>().swap(indices);
	

	for(int i=0; i< meshSize+1; i++)
	{
		for(int j=0; j< meshSize+1; j++)
		{
			// compute vertex position along mesh row (along x direction)
			meshpt.x = o.x + j * v1.x;
			meshpt.y = o.y + j * v1.y;
			meshpt.z = o.z + j * v1.z;
			vertices[currentVertex].position.set(meshpt.x, meshpt.y, meshpt.z);

			addVertex(meshpt.x, meshpt.y, meshpt.z);
			currentVertex++;
		}
		// go to next row in mesh (negative z direction)
		o += v2;
	}
	
	// Build Quad Polygons
	numQuads=(meshSize)*(meshSize);
	int currentQuad=0;

	for(int j=0; j < meshSize; j++)
	{
		for(int k=0; k < meshSize; k++)
		{
			// Counterclockwise order
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

// Immediate Mode Draw
void QuadMesh::DrawMesh(int meshSize)
{
	int currentQuad=0;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	for(int j=0; j< meshSize; j++)
	{
		for(int k=0; k< meshSize; k++)
		{
			glBegin(GL_QUADS);
			
			glNormal3f(quads[currentQuad].vertices[0]->normal.x,
				       quads[currentQuad].vertices[0]->normal.y,
					   quads[currentQuad].vertices[0]->normal.z);
			glVertex3f(quads[currentQuad].vertices[0]->position.x,
				       quads[currentQuad].vertices[0]->position.y,
					   quads[currentQuad].vertices[0]->position.z);
			
			glNormal3f(quads[currentQuad].vertices[1]->normal.x,
				       quads[currentQuad].vertices[1]->normal.y,
					   quads[currentQuad].vertices[1]->normal.z);
			
			glVertex3f(quads[currentQuad].vertices[1]->position.x,
				       quads[currentQuad].vertices[1]->position.y,
					   quads[currentQuad].vertices[1]->position.z);
			
			glNormal3f(quads[currentQuad].vertices[2]->normal.x,
				       quads[currentQuad].vertices[2]->normal.y,
					   quads[currentQuad].vertices[2]->normal.z);
			
			glVertex3f(quads[currentQuad].vertices[2]->position.x,
				       quads[currentQuad].vertices[2]->position.y,
					   quads[currentQuad].vertices[2]->position.z);
			
			glNormal3f(quads[currentQuad].vertices[3]->normal.x,
				       quads[currentQuad].vertices[3]->normal.y,
					   quads[currentQuad].vertices[3]->normal.z);
			
			glVertex3f(quads[currentQuad].vertices[3]->position.x,
				       quads[currentQuad].vertices[3]->position.y,
					   quads[currentQuad].vertices[3]->position.z);
			glEnd();
			currentQuad++;
		}
	}
}

// VBO Mode Draw
void QuadMesh::DrawMeshVBO(int meshSize)
{

	
}
void QuadMesh::CreateMeshVBO(int meshSize, GLint attribVertexPosition,GLint attribVertexNormal)
{
	
}







void QuadMesh::FreeMemory()
{
	if(vertices)
		delete [] vertices;
	vertices=NULL;
	numVertices=0;

	if(quads)
		delete [] quads;
	quads=NULL;
	numQuads=0;
}

void QuadMesh::ComputeNormals() 
{
	int currentQuad=0;

	for(int j=0; j< this->maxMeshSize; j++)
	{
		for(int k=0; k< this->maxMeshSize; k++)
		{
			Vector3 n0,n1,n2,n3,e0,e1,e2,e3,ne0,ne1,ne2,ne3;
			
			quads[currentQuad].vertices[0]->normal.set(0,0,0);
			quads[currentQuad].vertices[1]->normal.set(0,0,0);
			quads[currentQuad].vertices[2]->normal.set(0,0,0);
			quads[currentQuad].vertices[3]->normal.set(0,0,0);
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
