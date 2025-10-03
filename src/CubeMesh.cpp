#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "CubeMesh.h"
#include "Vectors.h"

static GLubyte iquads[][4] = {{0, 3, 2, 1}, {2, 3, 7, 6}, {0, 4, 7, 3}, {1, 2, 6, 5}, {4, 5, 6, 7}, {0, 1, 5, 4}};

static GLfloat ivertices[][3] = {
    {-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0}, {-1.0, -1.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 1.0, 1.0}, {-1.0, 1.0, 1.0}};

static GLfloat iquadNormals[][3] = {{0.0, 0.0, -1.0}, {0.0, 1.0, 0.0}, {-1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, -1.0, 0.0}};

CubeMesh::CubeMesh()
{
  angle = 0.0f;
  sfx = sfy = sfz = 1.0;
  tx = ty = tz = 0.0;

  vertices = ivertices;
  quads = iquads;
  quadNormals = iquadNormals;
}

void CubeMesh::drawCubeMesh()
{
  // No materials here — just rely on glColor3f() before calling
  glBegin(GL_QUADS);
  for (int i = 0; i < 6; i++)
  {
    glNormal3f(quadNormals[i][0], quadNormals[i][1], quadNormals[i][2]);
    for (int j = 0; j < 4; j++)
    {
      glVertex3f(vertices[quads[i][j]][0], vertices[quads[i][j]][1],
                 vertices[quads[i][j]][2]);
    }
  }
  glEnd();
}
