#ifndef DUCK_H
#define DUCK_H

#include <string>
#include <iostream>
#include <vector>
#include <ctime>

// OpenGL & GLM
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

// Project Includes
#include "CubeMesh.h"
#include "QuadMesh.h"
#include "Vectors.h"

// Window / Viewport
extern const int vWidth;
extern const int vHeight;

// Animation State
extern float duckX;
extern float duckAngle;

// Meshes
extern CubeMesh *cubeMesh;
extern QuadMesh *groundMesh;

// Core Functions
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void animationHandler(int value);

// Duck Drawing
void drawDuck();
void drawDuckBody();
void drawDuckHead();
void drawDuckEyes();
void drawDuckBeak();
void drawDuckNeck();
void drawDuckTail();
void drawDuckTarget();
void drawDuckStand();

// Environment
void drawBooth();
void drawWaterWave(float width, int waves, float amp, float depth);

#endif
