
#ifndef DUCK_H
#define DUCK_H

#include <string>
#include <iostream>
#include <vector>
#include <ctime>

// ---------- OpenGL & GLM ----------
#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h>
#endif

#define FREEGLUT_STATIC
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

// ---------- Project Includes ----------
#include "CubeMesh.h"
#include "QuadMesh.h"
#include "Vectors.h"

// ---------- Window / Viewport ----------
extern const int vWidth;
extern const int vHeight;

// ---------- Duck Parameters ----------
struct DuckConfig
{
  float bodyRadius;
  float headRadius;
  float beakRadius;
  float beakLength;
  float standHeight;
  float standRadius;
};

// Global duck config
extern DuckConfig duckConfig;

// ---------- Animation State ----------
extern float duckX;     // position across booth
extern float duckAngle; // rotation or dip/flip angle

// ---------- Meshes ----------
extern CubeMesh *cubeMesh;
extern QuadMesh *groundMesh;

// ---------- Core Functions ----------
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void animationHandler(int value);

// ---------- Duck Drawing ----------
void drawDuck();
void drawDuckBody();
void drawDuckHead();
void drawDuckBeak();
void drawDuckNeck();
void drawDuckTail();
void drawDuckTarget();
void drawDuckStand();

// ---------- Environment ----------
void drawBooth();
void drawWaterWave(float width, int waves, float amp, float depth);

#endif
