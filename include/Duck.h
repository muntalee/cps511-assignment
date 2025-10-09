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

// Window size
extern const int vWidth;
extern const int vHeight;

// Animation
extern float duckAngle;

// Meshes
extern CubeMesh *cubeMesh;
extern QuadMesh *groundMesh;

// Scene parameters
struct WaveParams
{
  float width = 0.0f;  // wave width
  int waves = 0;       // number of waves
  float amp = 0.0f;    // wave height
  float lift = 0.0f;   // vertical offset
  float thickZ = 0.0f; // water thickness
  float baseY = 0.0f;  // base Y for filling
  float x0 = 0.0f;     // left edge X
  float x1 = 0.0f;     // right edge X
};

struct BoothLayout
{
  float duckBodyR = 0.0f;

  float openW = 0.0f; // opening width
  float openH = 0.0f; // opening height

  float baseH = 0.0f;       // base height
  float baseCenterY = 0.0f; // base center Y
  float baseTopY = 0.0f;    // base top Y
  float groundY = 0.0f;     // ground Y

  float pillarW = 0.0f; // pillar width
  float pillarX = 0.0f; // pillar X position
  float pillarsOuterHalf = 0.0f;

  float baseW = 0.0f;     // base width
  float baseDepth = 0.0f; // base depth

  float beamH = 0.0f;        // beam height
  float beamOverhang = 0.0f; // beam overhang
  float beamUndersideY = 0.0f;
  float beamTopY = 0.0f;
  float pillarH = 0.0f;
  float pillarCenterY = 0.0f;
  float beamW = 0.0f;
  float beamCenterY = 0.0f;
};

// Global instances (defined in Duck3D.cpp)
extern WaveParams gWave;
extern BoothLayout gBooth;

// Get wave height at X
float waveYAt(float x);

// Set up scene parameters
void setupSceneParams();

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
void drawWaterWave3D();

#endif
