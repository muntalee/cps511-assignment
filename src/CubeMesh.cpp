#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h> // For wglSwapInterval
#endif

#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include "CubeMesh.h"
#include "Vectors.h"

static GLubyte iquads[][4] = {{0, 3, 2, 1},  // back face
                              {2, 3, 7, 6},  // top face
                              {0, 4, 7, 3},  // left face
                              {1, 2, 6, 5},  // right face
                              {4, 5, 6, 7},  // front face
                              {0, 1, 5, 4}}; // bottom face

static GLfloat ivertices[][3] = {
    {-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, {1.0, 1.0, -1.0}, {-1.0, 1.0, -1.0},
    {-1.0, -1.0, 1.0},  {1.0, -1.0, 1.0},  {1.0, 1.0, 1.0},  {-1.0, 1.0, 1.0}};

static GLfloat iquadNormals[][3] = {{0.0, 0.0, -1.0},  // Back Face
                                    {0.0, 1.0, 0.0},   // Top Face
                                    {-1.0, 0.0, 0.0},  // Left Face
                                    {1.0, 0.0, 0.0},   // Right Face
                                    {0.0, 0.0, 1.0},   // Front Face
                                    {0.0, -1.0, 0.0}}; // Bottom Face

CubeMesh::CubeMesh() {
  angle = 0.0f;
  sfx = sfy = sfz = 1.0;
  tx = 0.0;
  ty = 0.0;
  tz = 0.0;

  // Vertex positions of a standard size cube (width 2), centered at the origin
  // of its own Model Coordinate System
  vertices = ivertices;
  quads = iquads;
  quadNormals = iquadNormals;

  mat_ambient[0] = 0.05;
  mat_ambient[1] = 0.0;
  mat_ambient[2] = 0.0;
  mat_ambient[3] = 1.0;
  mat_specular[0] = 0.5;
  mat_specular[1] = 0.5;
  mat_specular[2] = 0.5;
  mat_specular[3] = 1.0;
  mat_diffuse[0] = 0.05;
  mat_diffuse[1] = 0.0;
  mat_diffuse[2] = 0.0;
  mat_diffuse[3] = 1.0;
  mat_shininess[0] = 4.0;
}

void CubeMesh::setMaterial(Vector3 ambient, Vector3 diffuse, Vector3 specular,
                           double shininess) {
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
  mat_shininess[0] = 4.0;
}

void CubeMesh::drawCubeMesh() {
  // Setup the material and lights used for the cube
  // you may want to use the alternate material if this cube is selected
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  // Draw Cube using simple immediate mode rendering
  glBegin(GL_QUADS);
  // Back Face
  glNormal3f(quadNormals[0][0], quadNormals[0][1], quadNormals[0][2]);
  glVertex3f(vertices[quads[0][0]][0], vertices[quads[0][0]][1],
             vertices[quads[0][0]][2]);
  glVertex3f(vertices[quads[0][1]][0], vertices[quads[0][1]][1],
             vertices[quads[0][1]][2]);
  glVertex3f(vertices[quads[0][2]][0], vertices[quads[0][2]][1],
             vertices[quads[0][2]][2]);
  glVertex3f(vertices[quads[0][3]][0], vertices[quads[0][3]][1],
             vertices[quads[0][3]][2]);
  // Top Face
  glNormal3f(quadNormals[1][0], quadNormals[1][1], quadNormals[1][2]);
  glVertex3f(vertices[quads[1][0]][0], vertices[quads[1][0]][1],
             vertices[quads[1][0]][2]);
  glVertex3f(vertices[quads[1][1]][0], vertices[quads[1][1]][1],
             vertices[quads[1][1]][2]);
  glVertex3f(vertices[quads[1][2]][0], vertices[quads[1][2]][1],
             vertices[quads[1][2]][2]);
  glVertex3f(vertices[quads[1][3]][0], vertices[quads[1][3]][1],
             vertices[quads[1][3]][2]);
  // Left Face
  glNormal3f(quadNormals[2][0], quadNormals[2][1], quadNormals[2][2]);
  glVertex3f(vertices[quads[2][0]][0], vertices[quads[2][0]][1],
             vertices[quads[2][0]][2]);
  glVertex3f(vertices[quads[2][1]][0], vertices[quads[2][1]][1],
             vertices[quads[2][1]][2]);
  glVertex3f(vertices[quads[2][2]][0], vertices[quads[2][2]][1],
             vertices[quads[2][2]][2]);
  glVertex3f(vertices[quads[2][3]][0], vertices[quads[2][3]][1],
             vertices[quads[2][3]][2]);
  // Right Face
  glNormal3f(quadNormals[3][0], quadNormals[3][1], quadNormals[3][2]);
  glVertex3f(vertices[quads[3][0]][0], vertices[quads[3][0]][1],
             vertices[quads[3][0]][2]);
  glVertex3f(vertices[quads[3][1]][0], vertices[quads[3][1]][1],
             vertices[quads[3][1]][2]);
  glVertex3f(vertices[quads[3][2]][0], vertices[quads[3][2]][1],
             vertices[quads[3][2]][2]);
  glVertex3f(vertices[quads[3][3]][0], vertices[quads[3][3]][1],
             vertices[quads[3][3]][2]);
  // Front Face
  glNormal3f(quadNormals[4][0], quadNormals[4][1], quadNormals[4][2]);
  glVertex3f(vertices[quads[4][0]][0], vertices[quads[4][0]][1],
             vertices[quads[4][0]][2]);
  glVertex3f(vertices[quads[4][1]][0], vertices[quads[4][1]][1],
             vertices[quads[4][1]][2]);
  glVertex3f(vertices[quads[4][2]][0], vertices[quads[4][2]][1],
             vertices[quads[4][2]][2]);
  glVertex3f(vertices[quads[4][3]][0], vertices[quads[4][3]][1],
             vertices[quads[4][3]][2]);
  // Bottom Face
  glNormal3f(quadNormals[5][0], quadNormals[5][1], quadNormals[5][2]);
  glVertex3f(vertices[quads[5][0]][0], vertices[quads[5][0]][1],
             vertices[quads[5][0]][2]);
  glVertex3f(vertices[quads[5][1]][0], vertices[quads[5][1]][1],
             vertices[quads[5][1]][2]);
  glVertex3f(vertices[quads[5][2]][0], vertices[quads[5][2]][1],
             vertices[quads[5][2]][2]);
  glVertex3f(vertices[quads[5][3]][0], vertices[quads[5][3]][1],
             vertices[quads[5][3]][2]);
  glEnd();
}
