#include "Duck.h"
#include <cmath>

const int vWidth = 650;
const int vHeight = 500;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// camera & state
struct DuckPosition
{
  float x = 8.0f;
  float y = 1.0f;
  float z = 0.0f;
};
DuckPosition duckPos;

// animation state
enum DuckState
{
  FORWARD,
  TURN_AT_RIGHT,
  BACKWARD,
  TURN_AT_LEFT
};
DuckState duckState = FORWARD;

// positive z-rotation is ccw in opengl; -1 makes it clockwise (your requirement)
const int ROT_DIR = -1;

// angles & speeds
float duckSpinDeg = 0.0f;      // 0..180 during right turn, 180..360 (normalized) on left
const float moveSpeed = 0.05f; // linear travel speed
const float spinSpeed = 3.0f;  // degrees per frame during the turns

// turn geometry (computed on state entry)
float turnRadius = 0.0f; // how wide/tall the loop is
float turnPivotX = 0.0f; // pivot x for the current turn
float turnPivotY = 0.0f; // pivot y for the current turn

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

// global scene params
WaveParams gWave;
BoothLayout gBooth;

// mouse button handler for camera orbit (left) and zoom (right)
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

// mouse motion handler for camera orbit (left) and zoom (right)
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

// compute all booth dimensions + wave parameters once
void setupSceneParams()
{
  // a single, human-readable "scale knob"
  gBooth.duckBodyR = 1.2f;

  // opening geometry
  gBooth.openW = gBooth.duckBodyR * 16.0f;
  gBooth.openH = gBooth.duckBodyR * 7.0f;

  // base
  gBooth.baseH = gBooth.duckBodyR * 2.8f;
  gBooth.baseCenterY = -3.25f;
  gBooth.baseTopY = gBooth.baseCenterY + gBooth.baseH * 0.5f;
  gBooth.groundY = gBooth.baseCenterY - gBooth.baseH * 0.5f;

  // pillars
  gBooth.pillarW = gBooth.duckBodyR * 1.6f;
  const float halfOpenW = gBooth.openW * 0.5f;
  gBooth.pillarX = halfOpenW + gBooth.pillarW * 0.5f;
  gBooth.pillarsOuterHalf = gBooth.pillarX + gBooth.pillarW * 0.5f;

  // base footprint vs wave clearance
  const float waveSideClear = gBooth.duckBodyR * 0.4f;
  gBooth.baseW = gBooth.openW - 2.0f * waveSideClear;
  gBooth.baseDepth = gBooth.pillarW * 1.4f;

  // top beam
  gBooth.beamH = gBooth.duckBodyR * 1.8f;
  gBooth.beamOverhang = gBooth.duckBodyR * 1.0f;
  gBooth.beamUndersideY = gBooth.baseTopY + gBooth.openH;
  gBooth.beamTopY = gBooth.beamUndersideY + gBooth.beamH;
  gBooth.pillarH = gBooth.beamTopY - gBooth.groundY - 0.01f;
  gBooth.pillarCenterY = (gBooth.beamTopY + gBooth.groundY) * 0.5f;
  gBooth.beamW = (gBooth.pillarsOuterHalf + gBooth.beamOverhang) * 2.0f;
  gBooth.beamCenterY = gBooth.beamUndersideY + gBooth.beamH * 0.5f;

  // wave
  gWave.width = gBooth.baseW;
  gWave.waves = 6;
  gWave.amp = gBooth.duckBodyR * 0.72f;
  gWave.lift = gWave.amp + 0.001f;
  gWave.thickZ = gBooth.baseDepth * 0.4f;
  gWave.baseY = -0.001f;
  gWave.x0 = -gWave.width * 0.5f; // left of base
  gWave.x1 = gWave.width * 0.5f;  // right of base
}

// main
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

// init
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

  // ground mesh
  Vector3 origin(-16.0f, 0.0f, 16.0f);
  Vector3 dir1(1.0f, 0.0f, 0.0f);
  Vector3 dir2(0.0f, 0.0f, -1.0f);
  groundMesh = new QuadMesh(meshSize, 32.0f);
  groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1, dir2);

  // reusable unit panel (1x1 in xy, centered at origin)
  panelMesh = QuadMesh::MakeUnitPanel();

  // compute shared scene parameters once
  setupSceneParams();
}

// display
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

  // booth drawing
  drawBooth();

  // draw the duck at its current position and orientation
  glPushMatrix();

  if (duckState == TURN_AT_RIGHT)
  {
    // 180 degree turn under the wave (clockwise)
    glTranslatef(turnPivotX, turnPivotY, duckPos.z);
    glRotatef(ROT_DIR * duckSpinDeg, 0, 0, 1);
    glTranslatef(0.0f, +turnRadius, 0.0f);
    // facing is handled by the orbit
  }
  else if (duckState == TURN_AT_LEFT)
  {
    // 180 degree turn over the duck (clockwise)
    glTranslatef(turnPivotX, turnPivotY, duckPos.z);
    glRotatef(ROT_DIR * duckSpinDeg, 0, 0, 1);
    glTranslatef(0.0f, -turnRadius, 0.0f);
    // extra rotation to face the correct direction
    glRotatef(180.0f, 0, 0, 1);
  }
  else
  {
    // straight segments
    glTranslatef(duckPos.x, duckPos.y, duckPos.z);
    if (duckState == BACKWARD)
      glRotatef(180.0f, 0, 0, 1); // face left when moving back
  }

  drawDuck();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, -5.0, 0.0);
  glColor3f(0.3f, 0.7f, 0.3f);
  groundMesh->DrawMesh(meshSize);
  glPopMatrix();

  glutSwapBuffers();
}

// duck
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
  // right eye
  glPushMatrix();
  glTranslatef(0.72f, 1.92f, 0.5f);
  glColor3f(0.0, 0.0, 0.0);
  glutSolidSphere(0.117f, 16, 16);
  glPopMatrix();

  // left eye
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
  // base red flattened sphere
  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.14f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(4.0, 30, 30);
  glPopMatrix();

  // white flattened sphere
  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.26f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 1.0, 1.0);
  glutSolidSphere(3.0, 30, 30);
  glPopMatrix();

  // red flattened sphere
  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.38f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(2.0, 30, 30);
  glPopMatrix();
}

// global wave helpers
float waveYAt(float x)
{
  // map x into t∈[0,1] across [x0,x1]
  const float t = (x - gWave.x0) / (gWave.x1 - gWave.x0);
  // sine with 'waves' full cycles across the width
  return gWave.lift + gWave.amp * sinf(2.f * (float)M_PI * gWave.waves * t);
}

// wave geometry
void drawWaterWave3D()
{
  const float halfT = gWave.thickZ * 0.5f;
  const float step = 0.08f;

  // crest surface
  glColor3f(0.663f, 0.882f, 0.973f); // #a9e1f8
  glBegin(GL_TRIANGLE_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, y, -halfT);
  }
  glEnd();

  // front face (filled down to base)
  glBegin(GL_QUAD_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, gWave.baseY, halfT);
  }
  glEnd();

  // back face
  glBegin(GL_QUAD_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, -halfT);
    glVertex3f(x, gWave.baseY, -halfT);
  }
  glEnd();

  // bottom
  glBegin(GL_QUADS);
  glVertex3f(gWave.x0, gWave.baseY, halfT);
  glVertex3f(gWave.x1, gWave.baseY, halfT);
  glVertex3f(gWave.x1, gWave.baseY, -halfT);
  glVertex3f(gWave.x0, gWave.baseY, -halfT);
  glEnd();

  // end caps
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

// booth & wave drawing
void drawBooth()
{
  // colors
  const float colPillars[3] = {0.447f, 0.443f, 0.506f}; // #727181
  const float colBeam[3] = {0.537f, 0.467f, 0.467f};    // #897777

  // base
  glColor3fv(colPillars);
  glPushMatrix();
  glTranslatef(0.0f, gBooth.baseCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.baseW, gBooth.baseH, gBooth.baseDepth);
  glPopMatrix();

  // pillars
  glColor3fv(colPillars);
  glPushMatrix();
  glTranslatef(-gBooth.pillarX, gBooth.pillarCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.pillarW, gBooth.pillarH, gBooth.pillarW);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(gBooth.pillarX, gBooth.pillarCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.pillarW, gBooth.pillarH, gBooth.pillarW);
  glPopMatrix();

  // top beam
  glColor3fv(colBeam);
  glPushMatrix();
  glTranslatef(0.0f, gBooth.beamCenterY, 0.0f);
  QuadMesh::DrawBoxFromPanel(panelMesh, gBooth.beamW, gBooth.beamH, gBooth.duckBodyR * 1.8f);
  glPopMatrix();

  // water wave (sits on base top)
  glPushMatrix();
  glTranslatef(0.0f, gBooth.baseTopY, 0.0f);
  drawWaterWave3D();
  glPopMatrix();
}

// handlers
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

// single source of truth for wave used here (no duplication)
void animationHandler(int)
{
  switch (duckState)
  {
  case FORWARD:
    // follow the sine wave left -> right
    duckPos.x += moveSpeed;
    duckPos.y = waveYAt(duckPos.x) - 0.10f;

    if (duckPos.x >= gWave.x1)
    {
      // clamp and set up a clockwise 180 degrees turn under the wave
      duckPos.x = gWave.x1;
      duckPos.y = waveYAt(gWave.x1) - 0.10f;

      // wider turn so the duck is clearly under the wave
      turnRadius = gWave.amp * 2.4f;
      turnPivotX = duckPos.x;
      turnPivotY = duckPos.y - turnRadius;

      duckSpinDeg = 0.0f;
      duckState = TURN_AT_RIGHT;
    }
    break;

  case TURN_AT_RIGHT:
    // rotate 0 -> 180 degrees clockwise around the below pivot
    duckSpinDeg += spinSpeed;
    if (duckSpinDeg >= 180.0f)
    {
      duckSpinDeg = 180.0f;

      // land at the bottom of the arc and start flat return (right -> left)
      duckPos.x = turnPivotX;
      duckPos.y = turnPivotY - turnRadius;
      duckState = BACKWARD;
    }
    break;

  case BACKWARD:
    // flat horizontal return (no sine)
    duckPos.x -= moveSpeed;
    // keep the same y set at the end of the right turn

    if (duckPos.x <= gWave.x0)
    {
      duckPos.x = gWave.x0;

      // set up a clockwise 180 degrees turn over the duck to bring it back above the wave
      turnRadius = gWave.amp * 2.4f; // match right turn width/height
      turnPivotX = duckPos.x;        // pivot directly above current pos
      turnPivotY = duckPos.y + turnRadius;

      duckSpinDeg = 0.0f;
      duckState = TURN_AT_LEFT;
    }
    break;

  case TURN_AT_LEFT:
    // rotate 0 -> 180 clockwise around the above pivot
    duckSpinDeg += spinSpeed;
    if (duckSpinDeg >= 180.0f)
    {
      duckSpinDeg = 180.0f;

      // land at the top of the arc and resume sine motion forward
      duckPos.x = turnPivotX;
      duckPos.y = turnPivotY + turnRadius;

      // normalize and continue cycle
      duckSpinDeg = 0.0f;
      duckState = FORWARD;
    }
    break;
  }

  glutPostRedisplay();
  glutTimerFunc(16, animationHandler, 0);
}
