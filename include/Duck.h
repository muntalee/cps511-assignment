#ifndef DUCK_H
#define DUCK_H

#include <string>
#include <iostream>
#include <vector>
#include <ctime>

// opengl & glm
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

// project includes
#include "QuadMesh.h"

// window size
extern const int vWidth;
extern const int vHeight;

// animation
extern float duckAngle;

// meshes
extern QuadMesh *groundMesh;

// scene parameters
// wave parameters used to compute water surface
struct WaveParams
{
  float width = 0.0f;  // wave width
  int waves = 0;       // number of waves
  float amp = 0.0f;    // wave height (amplitude)
  float lift = 0.0f;   // vertical offset for waves
  float thickZ = 0.0f; // water thickness (z dimension)
  float baseY = 0.0f;  // base y for filling water
  float x0 = 0.0f;     // left edge x of water
  float x1 = 0.0f;     // right edge x of water
};

// booth layout parameters that position the duck/stage
struct BoothLayout
{
  float duckBodyR = 0.0f; // duck body radius

  float openW = 0.0f; // opening width of booth
  float openH = 0.0f; // opening height of booth

  float baseH = 0.0f;       // base height of booth
  float baseCenterY = 0.0f; // base center y coordinate
  float baseTopY = 0.0f;    // base top y coordinate
  float groundY = 0.0f;     // ground y level

  float pillarW = 0.0f;          // pillar width
  float pillarX = 0.0f;          // pillar x position
  float pillarsOuterHalf = 0.0f; // half distance to outer pillars

  float baseW = 0.0f;     // base width
  float baseDepth = 0.0f; // base depth (z)

  float beamH = 0.0f;          // beam height
  float beamOverhang = 0.0f;   // beam overhang length
  float beamUndersideY = 0.0f; // underside y of beam
  float beamTopY = 0.0f;       // top y of beam
  float pillarH = 0.0f;        // pillar height
  float pillarCenterY = 0.0f;  // pillar center y
  float beamW = 0.0f;          // beam width
  float beamCenterY = 0.0f;    // beam center y
};

// global instances (defined in Duck3D.cpp)
extern WaveParams gWave;
extern BoothLayout gBooth;

// get wave height at x position
float waveYAt(float x);

// set up scene parameters (booth, wave, sizes)
void setupSceneParams();

// core functions for opengl and app lifecycle
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void animationHandler(int value);

// duck drawing functions (each draws part of the duck)
void drawDuck();
void drawDuckBody();
void drawDuckHead();
void drawDuckEyes();
void drawDuckBeak();
void drawDuckNeck();
void drawDuckTail();
void drawDuckTarget();
void drawDuckStand();

// environment drawing functions
void drawBooth();
void drawWaterWave3D();

#endif
