/*******************************************************************
                   Hierarchical Duck Target Example
********************************************************************/
#include "Duck.h"
#include <cmath>

const int vWidth = 650;
const int vHeight = 500;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DuckConfig duckConfig = {
    1.2f,  // body radius
    0.65f, // head radius
    0.18f, // beak radius
    0.42f, // beak length
    3.8f,  // stand height
    0.18f  // stand radius
};

float duckX = 8.0f;
float duckAngle = 0.0f;

CubeMesh *cubeMesh = nullptr;
QuadMesh *groundMesh = nullptr;
int meshSize = 16;

// mouse controls
float cameraZoom = 22.0f;
float cameraYaw = 0.0f;   // horizontal angle, in degrees
float cameraPitch = 0.0f; // vertical angle, in degrees
bool leftMouseDown = false;
bool rightMouseDown = false;
int lastMouseY = 0;
int lastMouseX = 0;

// Mouse handlers for camera control
void mouseButton(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON)
  {
    leftMouseDown = (state == GLUT_DOWN);
    lastMouseY = y;
  }
  else if (button == GLUT_RIGHT_BUTTON)
  {
    rightMouseDown = (state == GLUT_DOWN);
    lastMouseX = x;
    lastMouseY = y;
  }
}

void mouseMotion(int x, int y)
{
  if (leftMouseDown)
  {
    int dy = y - lastMouseY;
    cameraZoom += dy * 0.1f;
    if (cameraZoom < 5.0f)
      cameraZoom = 5.0f;
    if (cameraZoom > 60.0f)
      cameraZoom = 60.0f;
    lastMouseY = y;
    glutPostRedisplay();
  }
  else if (rightMouseDown)
  {
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;
    cameraYaw += dx * 0.5f;
    cameraPitch += dy * 0.5f;
    // Clamp pitch to avoid flipping
    if (cameraPitch > 89.0f)
      cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
      cameraPitch = -89.0f;
    // Wrap yaw for 360 degrees
    if (cameraYaw > 360.0f)
      cameraYaw -= 360.0f;
    if (cameraYaw < 0.0f)
      cameraYaw += 360.0f;
    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
  }
}

// ================================================================
// MAIN
// ================================================================
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

// ================================================================
// INIT
// ================================================================
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

  Vector3 origin(-16.0f, 0.0f, 16.0f);
  Vector3 dir1(1.0f, 0.0f, 0.0f);
  Vector3 dir2(0.0f, 0.0f, -1.0f);
  groundMesh = new QuadMesh(meshSize, 32.0f);
  groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1, dir2);
}

// ================================================================
// DISPLAY
// ================================================================
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  float camX = cameraZoom * sinf(cameraYaw * M_PI / 180.0f) * cosf(cameraPitch * M_PI / 180.0f);
  float camY = cameraZoom * -sinf(cameraPitch * M_PI / 180.0f) + 3.0f;
  float camZ = cameraZoom * cosf(cameraYaw * M_PI / 180.0f) * cosf(cameraPitch * M_PI / 180.0f);

  gluLookAt(camX, camY, camZ, // Camera position
            0.0, 3.0, 0.0,    // Look at the center of the scene
            0.0, 1.0, 0.0);   // Up vector

  glutMouseFunc(mouseButton);
  glutMotionFunc(mouseMotion);

  drawBooth();

  glPushMatrix();
  glTranslatef(duckX, 1.0, 0.0);
  drawDuck();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, -5.0, 0.0);
  glColor3f(0.3f, 0.7f, 0.3f);
  groundMesh->DrawMesh(meshSize);
  glPopMatrix();

  glutSwapBuffers();
}

// ================================================================
// DUCK
// ================================================================
void drawDuck()
{
  drawDuckBody();

  // ----- Neck -----
  glPushMatrix();
  glTranslatef(duckConfig.bodyRadius * 0.3f, duckConfig.bodyRadius * 0.3f,
               0.0f);
  glRotatef(90.0f, 0, 1, 0);
  glRotatef(-90.0f, 1, 0, 0);
  glColor3f(1.0, 1.0, 0.0);
  glutSolidCone(duckConfig.headRadius * 1.4f, duckConfig.headRadius * 2.0f, 20,
                12);
  glPopMatrix();

  // ----- Head -----
  glPushMatrix();
  glTranslatef(duckConfig.bodyRadius * 0.4f, duckConfig.bodyRadius * 1.4f,
               0.0f);
  glColor3f(1.0, 1.0, 0.0);
  glutSolidSphere(duckConfig.headRadius, 28, 28);
  glPopMatrix();

  // ----- Eye -----
  glPushMatrix();
  glTranslatef(duckConfig.bodyRadius * 0.6f, duckConfig.bodyRadius * 1.6f,
               0.5f);
  glColor3f(0.0, 0.0, 0.0);
  glutSolidSphere(duckConfig.headRadius * 0.18f, 16, 16);
  glPopMatrix();

  // ----- Beak -----
  glPushMatrix();
  glTranslatef(duckConfig.bodyRadius * 0.9f, duckConfig.bodyRadius * 1.4f,
               0.0f);
  glRotatef(90, 0, 1, 0);
  glColor3f(1.0, 0.25, 0.0);
  glutSolidCone(duckConfig.beakRadius, duckConfig.beakLength, 20, 12);
  glPopMatrix();

  // ----- Tail (points backward, opposite of beak) -----
  drawDuckTail();

  // ----- Target -----
  drawDuckTarget();
}

void drawDuckBody()
{
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  glScalef(1.15f, 0.95f, 1.05f);
  glutSolidSphere(duckConfig.bodyRadius, 30, 30);
  glPopMatrix();
}

void drawDuckTail()
{
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  glTranslatef(-duckConfig.bodyRadius * 0.8f, duckConfig.bodyRadius * 0.5f,
               0.0f);
  glRotatef(-90, 0, 1, 0);
  glRotatef(-45, 1, 0, 0);
  glutSolidCone(duckConfig.headRadius * 1.0f, duckConfig.headRadius * 1.8f, 20,
                12);
  glPopMatrix();
}

void drawDuckTarget()
{
  // Base red flattened sphere (largest, back)
  glPushMatrix();
  glTranslatef(0.0f, -duckConfig.bodyRadius * 0.15f,
               duckConfig.bodyRadius * 0.95f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(4.0, 30, 30);
  glPopMatrix();

  // White flattened sphere (smaller, slightly in front)
  glPushMatrix();
  glTranslatef(0.0f, -duckConfig.bodyRadius * 0.15f,
               duckConfig.bodyRadius * 1.05f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 1.0, 1.0);
  glutSolidSphere(3.0, 30, 30);
  glPopMatrix();

  // // Red flattened sphere (smallest, more in front)
  glPushMatrix();
  glTranslatef(0.0f, -duckConfig.bodyRadius * 0.15f,
               duckConfig.bodyRadius * 1.15f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(2.0, 30, 30);
  glPopMatrix();

  glPopMatrix();
}

// ================================================================
// WATER & BOOTH
// ================================================================
void drawWaterWave(float width, int waves, float amp, float depth)
{
  const float step = 0.08f;
  const float yBottom = -depth;
  glColor3f(0.52f, 0.82f, 1.0f);
  glBegin(GL_QUAD_STRIP);
  for (float x = -width * 0.5f; x <= width * 0.5f + 0.0001f; x += step)
  {
    float t = (x + width * 0.5f) / width;
    float y = amp * sinf(2.0f * (float)M_PI * waves * t);
    glVertex3f(x, y, 0.0f);
    glVertex3f(x, yBottom, 0.0f);
  }
  glEnd();
}

void drawBooth()
{
  // pillar 1
  glColor3f(0.36f, 0.37f, 0.52f);
  glPushMatrix();
  glTranslatef(-7.0, 2.0, 0.0);
  glScalef(1.0, 6.0, 1.0);
  glutSolidCube(1.0);
  glPopMatrix();

  // pillar 2
  glPushMatrix();
  glTranslatef(7.0, 2.0, 0.0);
  glScalef(1.0, 6.0, 1.0);
  glutSolidCube(1.0);
  glPopMatrix();

  // top beam
  glPushMatrix();
  glTranslatef(0.0, 4.5, 0.0);
  glScalef(16.0, 1.0, 1.0);
  glutSolidCube(1.0);
  glPopMatrix();

  // base container
  glPushMatrix();
  glTranslatef(0.0, -0.5, 0.0);
  glColor3f(0.24f, 0.25f, 0.36f);
  glScalef(16.0, 2.0, 6.0);
  glutSolidCube(1.0);
  glPopMatrix();

  // water sine wave
  glPushMatrix();
  glTranslatef(0.0, 1.2f, 0.0);
  drawWaterWave(13.0f, 5, 0.6f, 0.9f);
  glPopMatrix();
}

// ================================================================
// HANDLERS
// ================================================================
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

void animationHandler(int value)
{
  duckX -= 0.05f;
  if (duckX < -8.0f)
    duckX = 8.0f;

  glutPostRedisplay();
  glutTimerFunc(16, animationHandler, 0);
}
