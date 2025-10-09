#include "Duck.h"
#include <cmath>

const int vWidth = 650;
const int vHeight = 500;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Camera & state
struct DuckPosition
{
  float x = 8.0f;
  float y = 1.0f;
  float z = 0.0f;
};

DuckPosition duckPos;
float duckAngle = 0.0f;

CubeMesh *cubeMesh = nullptr;
QuadMesh *groundMesh = nullptr;
QuadMesh *panelMesh = nullptr;
int meshSize = 16;

float cameraZoom = 22.0f;
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
bool leftMouseDown = false;
bool rightMouseDown = false;
int lastMouseX = 0;
int lastMouseY = 0;

// Global scene params
WaveParams gWave;
BoothLayout gBooth;

// Mouse button handler for camera orbit (left) and zoom (right)
void mouseButton(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON)
    leftMouseDown = (state == GLUT_DOWN), lastMouseX = x, lastMouseY = y;
  else if (button == GLUT_RIGHT_BUTTON)
    rightMouseDown = (state == GLUT_DOWN), lastMouseY = y;
  else if ((button == 3 || button == 4) && state == GLUT_DOWN) // scroll wheel
  {
    cameraZoom += (button == 3 ? -1.0f : 1.0f);
    if (cameraZoom < 5.0f)
      cameraZoom = 5.0f;
    if (cameraZoom > 60.0f)
      cameraZoom = 60.0f;
    glutPostRedisplay();
  }
}

// Mouse motion handler for camera orbit (left) and zoom (right)
void mouseMotion(int x, int y)
{
  if (leftMouseDown)
  {
    cameraYaw += (x - lastMouseX) * 0.5f;
    cameraPitch += (y - lastMouseY) * 0.5f;
    if (cameraPitch > 89.0f)
      cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
      cameraPitch = -89.0f;
    if (cameraYaw >= 360.0f)
      cameraYaw -= 360.0f;
    if (cameraYaw < 0.0f)
      cameraYaw += 360.0f;
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
  }
  else if (rightMouseDown)
  {
    cameraZoom += (y - lastMouseY) * 0.1f;
    if (cameraZoom < 5.0f)
      cameraZoom = 5.0f;
    if (cameraZoom > 60.0f)
      cameraZoom = 60.0f;
    lastMouseY = y;
    glutPostRedisplay();
  }
}

// Compute all booth dimensions + wave parameters once
void setupSceneParams()
{
  // A single, human-readable "scale knob"
  gBooth.duckBodyR = 1.2f;

  // Opening geometry
  gBooth.openW = gBooth.duckBodyR * 16.0f;
  gBooth.openH = gBooth.duckBodyR * 7.0f;

  // Base
  gBooth.baseH = gBooth.duckBodyR * 2.8f;
  gBooth.baseCenterY = -3.25f;
  gBooth.baseTopY = gBooth.baseCenterY + gBooth.baseH * 0.5f;
  gBooth.groundY = gBooth.baseCenterY - gBooth.baseH * 0.5f;

  // Pillars
  gBooth.pillarW = gBooth.duckBodyR * 1.6f;
  const float halfOpenW = gBooth.openW * 0.5f;
  gBooth.pillarX = halfOpenW + gBooth.pillarW * 0.5f;
  gBooth.pillarsOuterHalf = gBooth.pillarX + gBooth.pillarW * 0.5f;

  // Base footprint vs wave clearance
  const float waveSideClear = gBooth.duckBodyR * 0.4f;
  gBooth.baseW = gBooth.openW - 2.0f * waveSideClear;
  gBooth.baseDepth = gBooth.pillarW * 1.4f;

  // Top beam
  gBooth.beamH = gBooth.duckBodyR * 1.8f;
  gBooth.beamOverhang = gBooth.duckBodyR * 1.0f;
  gBooth.beamUndersideY = gBooth.baseTopY + gBooth.openH;
  gBooth.beamTopY = gBooth.beamUndersideY + gBooth.beamH;
  gBooth.pillarH = gBooth.beamTopY - gBooth.groundY - 0.01f;
  gBooth.pillarCenterY = (gBooth.beamTopY + gBooth.groundY) * 0.5f;
  gBooth.beamW = (gBooth.pillarsOuterHalf + gBooth.beamOverhang) * 2.0f;
  gBooth.beamCenterY = gBooth.beamUndersideY + gBooth.beamH * 0.5f;

  // Wave
  gWave.width = gBooth.baseW;
  gWave.waves = 6;
  gWave.amp = gBooth.duckBodyR * 0.72f;
  gWave.lift = gWave.amp + 0.001f;
  gWave.thickZ = gBooth.baseDepth * 0.4f;
  gWave.baseY = -0.001f;
  gWave.x0 = -gWave.width * 0.5f; // left of base
  gWave.x1 = gWave.width * 0.5f;  // right of base
}

// MAIN
int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(vWidth, vHeight);
  glutInitWindowPosition(200, 30);
  glutCreateWindow("Duck Target 3D");

  initOpenGL(vWidth, vHeight);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutTimerFunc(16, animationHandler, 0);

  glutMainLoop();
  return 0;
}

// INIT
void initOpenGL(int w, int h)
{
  GLfloat light_pos[] = {-4.0f, 8.0f, 8.0f, 1.0f};
  GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
  glEnable(GL_NORMALIZE);

  // Ground mesh
  Vector3 origin(-16.0f, 0.0f, 16.0f);
  Vector3 dir1(1.0f, 0.0f, 0.0f);
  Vector3 dir2(0.0f, 0.0f, -1.0f);
  groundMesh = new QuadMesh(meshSize, 32.0f);
  groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1, dir2);

  // Reusable unit panel (1x1 in XY, centered at origin)
  panelMesh = QuadMesh::MakeUnitPanel();

  // Compute shared scene parameters once
  setupSceneParams();
}

// DISPLAY
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  float camX = cameraZoom * sinf(cameraYaw * M_PI / 180.0f) * cosf(cameraPitch * M_PI / 180.0f);
  float camY = cameraZoom * -sinf(cameraPitch * M_PI / 180.0f) + 3.0f;
  float camZ = cameraZoom * cosf(cameraYaw * M_PI / 180.0f) * cosf(cameraPitch * M_PI / 180.0f);

  gluLookAt(camX, camY, camZ,
            0.0, 3.0, 0.0,
            0.0, 1.0, 0.0);

  glutMouseFunc(mouseButton);
  glutMotionFunc(mouseMotion);

  drawBooth();

  glPushMatrix();
  glTranslatef(duckPos.x, duckPos.y, duckPos.z);
  drawDuck();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, -5.0, 0.0);
  glColor3f(0.3f, 0.7f, 0.3f);
  groundMesh->DrawMesh(meshSize);
  glPopMatrix();

  glutSwapBuffers();
}

// DUCK
void drawDuck()
{
  drawDuckBody();
  drawDuckNeck();
  drawDuckHead();
  drawDuckEyes();
  drawDuckBeak();
  drawDuckTail();
  drawDuckTarget();
}

void drawDuckBody()
{
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  glScalef(1.15f, 0.95f, 1.05f);
  glutSolidSphere(1.2f, 30, 30);
  glPopMatrix();
}

void drawDuckNeck()
{
  glPushMatrix();
  glTranslatef(0.36f, 0.36f, 0.0f);
  glRotatef(90.0f, 0, 1, 0);
  glRotatef(-90.0f, 1, 0, 0);
  glColor3f(1.0, 1.0, 0.0);
  glutSolidCone(0.91f, 1.3f, 20, 12);
  glPopMatrix();
}

void drawDuckHead()
{
  glPushMatrix();
  glTranslatef(0.48f, 1.68f, 0.0f);
  glColor3f(1.0, 1.0, 0.0);
  glutSolidSphere(0.65f, 28, 28);
  glPopMatrix();
}

void drawDuckEyes()
{
  // Right eye
  glPushMatrix();
  glTranslatef(0.72f, 1.92f, 0.5f);
  glColor3f(0.0, 0.0, 0.0);
  glutSolidSphere(0.117f, 16, 16);
  glPopMatrix();

  // Left eye
  glPushMatrix();
  glTranslatef(0.72f, 1.92f, -0.5f);
  glColor3f(0.0, 0.0, 0.0);
  glutSolidSphere(0.117f, 16, 16);
  glPopMatrix();
}

void drawDuckBeak()
{
  glPushMatrix();
  glTranslatef(1.08f, 1.68f, 0.0f);
  glRotatef(90, 0, 1, 0);
  glColor3f(1.0, 0.25, 0.0);
  glutSolidCone(0.18f, 0.42f, 20, 12);
  glPopMatrix();
}

void drawDuckTail()
{
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  glTranslatef(-0.96f, 0.6f, 0.0f);
  glRotatef(-90, 0, 1, 0);
  glRotatef(-45, 1, 0, 0);
  glutSolidCone(0.65f, 1.17f, 20, 12);
  glPopMatrix();
}

void drawDuckTarget()
{
  // Base red flattened sphere
  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.14f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(4.0, 30, 30);
  glPopMatrix();

  // White flattened sphere
  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.26f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 1.0, 1.0);
  glutSolidSphere(3.0, 30, 30);
  glPopMatrix();

  // Red flattened sphere
  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.38f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(2.0, 30, 30);
  glPopMatrix();
}

// Global wave helpers
float waveYAt(float x)
{
  // map x into t∈[0,1] across [x0,x1]
  const float t = (x - gWave.x0) / (gWave.x1 - gWave.x0);
  // sine with 'waves' full cycles across the width
  return gWave.lift + gWave.amp * sinf(2.f * (float)M_PI * gWave.waves * t);
}

// Wave geometry
void drawWaterWave3D()
{
  const float halfT = gWave.thickZ * 0.5f;
  const float step = 0.08f;

  // Crest surface
  glColor3f(0.663f, 0.882f, 0.973f); // #a9e1f8
  glBegin(GL_TRIANGLE_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, y, -halfT);
  }
  glEnd();

  // Front face (filled down to base)
  glBegin(GL_QUAD_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, gWave.baseY, halfT);
  }
  glEnd();

  // Back face
  glBegin(GL_QUAD_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, -halfT);
    glVertex3f(x, gWave.baseY, -halfT);
  }
  glEnd();

  // Bottom
  glBegin(GL_QUADS);
  glVertex3f(gWave.x0, gWave.baseY, halfT);
  glVertex3f(gWave.x1, gWave.baseY, halfT);
  glVertex3f(gWave.x1, gWave.baseY, -halfT);
  glVertex3f(gWave.x0, gWave.baseY, -halfT);
  glEnd();

  // End caps
  float yL = waveYAt(gWave.x0), yR = waveYAt(gWave.x1);
  glBegin(GL_QUADS); // left
  glVertex3f(gWave.x0, gWave.baseY, halfT);
  glVertex3f(gWave.x0, yL, halfT);
  glVertex3f(gWave.x0, yL, -halfT);
  glVertex3f(gWave.x0, gWave.baseY, -halfT);
  glEnd();

  glBegin(GL_QUADS); // right
  glVertex3f(gWave.x1, gWave.baseY, -halfT);
  glVertex3f(gWave.x1, yR, -halfT);
  glVertex3f(gWave.x1, yR, halfT);
  glVertex3f(gWave.x1, gWave.baseY, halfT);
  glEnd();
}

// Booth & wave drawing
void drawBooth()
{
  // colors
  const float colPillars[3] = {0.447f, 0.443f, 0.506f}; // #727181
  const float colBeam[3] = {0.537f, 0.467f, 0.467f};    // #897777

  // Base
  glColor3fv(colPillars);
  glPushMatrix();
  glTranslatef(0.0f, gBooth.baseCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.baseW, gBooth.baseH, gBooth.baseDepth);
  glPopMatrix();

  // Pillars
  glColor3fv(colPillars);
  glPushMatrix();
  glTranslatef(-gBooth.pillarX, gBooth.pillarCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.pillarW, gBooth.pillarH, gBooth.pillarW);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(gBooth.pillarX, gBooth.pillarCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.pillarW, gBooth.pillarH, gBooth.pillarW);
  glPopMatrix();

  // Top beam
  glColor3fv(colBeam);
  glPushMatrix();
  glTranslatef(0.0f, gBooth.beamCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.beamW, gBooth.beamH, gBooth.duckBodyR * 1.8f);
  glPopMatrix();

  // Water wave (sits on base top)
  glPushMatrix();
  glTranslatef(0.0f, gBooth.baseTopY, 0.0f);
  drawWaterWave3D();
  glPopMatrix();
}

// HANDLERS
void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
  if (key == 27)
    exit(0);
}

// Single source of truth for wave used here (no duplication)
void animationHandler(int value)
{
  // Move duck left -> right across the full wave span
  duckPos.x -= 0.05f;
  if (duckPos.x < gWave.x0)
    duckPos.x = gWave.x1;

  // Place duck on the crest (slightly sunk for aesthetics)
  duckPos.y = waveYAt(duckPos.x) - 0.10f;

  glutPostRedisplay();
  glutTimerFunc(16, animationHandler, 0);
}
