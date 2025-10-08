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

float duckX = 8.0f;
float duckAngle = 0.0f;

CubeMesh *cubeMesh = nullptr;
QuadMesh *groundMesh = nullptr;
QuadMesh *panelMesh = nullptr;
int meshSize = 16;

// Camera and mouse control variables
float cameraZoom = 22.0f;
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;
bool leftMouseDown = false;
bool rightMouseDown = false;
int lastMouseX = 0;
int lastMouseY = 0;

// Mouse button handler for camera orbit (left) and zoom (right)
void mouseButton(int button, int state, int x, int y)
{
  // left button = orbit, right button = zoom
  if (button == GLUT_LEFT_BUTTON)
    leftMouseDown = (state == GLUT_DOWN), lastMouseX = x, lastMouseY = y;
  else if (button == GLUT_RIGHT_BUTTON)
    rightMouseDown = (state == GLUT_DOWN), lastMouseY = y;
  // scroll wheel = zoom
  else if ((button == 3 || button == 4) && state == GLUT_DOWN)
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

  // Create the reusable unit panel (1x1 in XY, centered at origin)
  panelMesh = QuadMesh::MakeUnitPanel();
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

// DUCK
// glTranslatef(x, y, z): Moves the current matrix by x, y, z units.
// glRotatef(angle, x, y, z): Rotates by 'angle' degrees around the vector (x, y, z).
// glScalef(x, y, z): Scales the current matrix by x, y, z factors.
// glutSolidSphere(radius, slices, stacks): Draws a solid sphere.
// glutSolidCone(base, height, slices, stacks): Draws a solid cone.

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

// Wave
void drawWaterWave3D(float width, int waves, float amp, float lift, float thicknessZ)
{
  const float halfT = thicknessZ * 0.5f;
  const float step = 0.08f;
  const float x0 = -width * 0.5f;
  const float x1 = width * 0.5f;

  // Baseline: exactly 0 (tiny epsilon keeps faces well-ordered)
  const float yBase = -0.001f;

  auto yCurve = [&](float x)
  {
    float t = (x - x0) / width; // 0..1
    return lift + amp * sinf(2.f * (float)M_PI * waves * t);
  };

  // Crest surface
  // colors: #a9e1f8
  glColor3f(0.663f, 0.882f, 0.973f);
  glBegin(GL_TRIANGLE_STRIP);
  for (float x = x0; x <= x1 + 1e-4f; x += step)
  {
    float y = yCurve(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, y, -halfT);
  }
  glEnd();

  // Front face
  glBegin(GL_QUAD_STRIP);
  for (float x = x0; x <= x1 + 1e-4f; x += step)
  {
    float y = yCurve(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, yBase, halfT);
  }
  glEnd();

  // Back face
  glBegin(GL_QUAD_STRIP);
  for (float x = x0; x <= x1 + 1e-4f; x += step)
  {
    float y = yCurve(x);
    glVertex3f(x, y, -halfT);
    glVertex3f(x, yBase, -halfT);
  }
  glEnd();

  // Bottom
  glBegin(GL_QUADS);
  glVertex3f(x0, yBase, halfT);
  glVertex3f(x1, yBase, halfT);
  glVertex3f(x1, yBase, -halfT);
  glVertex3f(x0, yBase, -halfT);
  glEnd();

  // End caps
  float yL = yCurve(x0), yR = yCurve(x1);
  glBegin(GL_QUADS); // left
  glVertex3f(x0, yBase, halfT);
  glVertex3f(x0, yL, halfT);
  glVertex3f(x0, yL, -halfT);
  glVertex3f(x0, yBase, -halfT);
  glEnd();
  glBegin(GL_QUADS); // right
  glVertex3f(x1, yBase, -halfT);
  glVertex3f(x1, yR, -halfT);
  glVertex3f(x1, yR, halfT);
  glVertex3f(x1, yBase, halfT);
  glEnd();
}

void drawBooth()
{
  // duck's radius
  const float DUCK_BODY_R = 1.2f;

  // booth opening
  const float OPEN_W = DUCK_BODY_R * 16.0f; // width between pillars
  const float OPEN_H = DUCK_BODY_R * 7.0f;  // height from base to beam
  const int WAVES = 5;

  // colors
  // pillars/base: #727181
  const float colPillars[3] = {0.447f, 0.443f, 0.506f};
  // top beam: #897777
  const float colBeam[3] = {0.537f, 0.467f, 0.467f};

  // base
  const float baseH = DUCK_BODY_R * 2.8f;
  const float baseCenterY = -3.25f;
  const float baseTopY = baseCenterY + baseH * 0.5f;
  const float groundY = baseCenterY - baseH * 0.5f;

  // pillars
  const float pillarW = DUCK_BODY_R * 1.6f;
  const float halfOpenW = OPEN_W * 0.5f;
  const float pillarX = halfOpenW + pillarW * 0.5f;

  // base width/depth
  const float baseOverhang = DUCK_BODY_R * 1.0f;
  const float pillarsOuterHalf = pillarX + pillarW * 0.5f;
  const float waveSideClear = DUCK_BODY_R * 0.4f;
  const float baseW = OPEN_W - 2.0f * waveSideClear;
  const float baseDepth = pillarW * 1.4f;

  // drawing base
  glColor3fv(colPillars);
  glPushMatrix();
  glTranslatef(0.0f, baseCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, baseW, baseH, baseDepth);
  glPopMatrix();

  // pillar values
  const float beamH = DUCK_BODY_R * 1.8f;
  const float beamOverhang = DUCK_BODY_R * 1.0f;
  const float beamUndersideY = baseTopY + OPEN_H;
  const float beamTopY = beamUndersideY + beamH;
  const float pillarH = beamTopY - groundY - 0.01f; // slight gap to avoid z-fighting
  const float pillarCenterY = (beamTopY + groundY) * 0.5f;

  // draw left pillar
  glColor3fv(colPillars);
  glPushMatrix();
  glTranslatef(-pillarX, pillarCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, pillarW, pillarH, pillarW);
  glPopMatrix();

  // draw right pillar
  glPushMatrix();
  glTranslatef(pillarX, pillarCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, pillarW, pillarH, pillarW);
  glPopMatrix();

  // draw top beam
  const float beamW = (pillarsOuterHalf + beamOverhang) * 2.0f;
  const float beamCenterY = beamUndersideY + beamH * 0.5f;

  glColor3fv(colBeam);
  glPushMatrix();
  glTranslatef(0.0f, beamCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, beamW, beamH, DUCK_BODY_R * 1.8f);
  glPopMatrix();

  // water wave
  const float waveThickZ = baseDepth * 0.4f;
  const float amp = DUCK_BODY_R * 0.85f;
  const float lift = amp + 0.001f;

  glPushMatrix();
  glTranslatef(0.0f, baseTopY, 0.0f); // place wave on top of base
  drawWaterWave3D(baseW, WAVES, amp, lift, waveThickZ);
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

void animationHandler(int value)
{
  duckX -= 0.05f;
  if (duckX < -8.0f)
    duckX = 8.0f;

  glutPostRedisplay();
  glutTimerFunc(16, animationHandler, 0);
}
