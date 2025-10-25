#include "Duck.h"
#include "ShaderUtils.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const int vWidth = 650;
const int vHeight = 500;

// camera and state
struct DuckPosition
{
  float x = -6.0f;
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

// rotation direction
const int ROT_DIR = -1;

// angles and speeds
float duckSpinDeg = 0.0f;
const float moveSpeed = 0.06f;
const float spinSpeed = 4.0f;

// turn geometry
float turnRadius = 0.0f;
float turnPivotX = 0.0f;
float turnPivotY = 0.0f;

CubeMesh *cubeMesh = nullptr;
QuadMesh *groundMesh = nullptr;
QuadMesh *panelMesh = nullptr;
int meshSize = 16;

// camera parameters
float cameraZoom = 22.0f;
float cameraYaw = 0.0f;
float cameraPitch = 0.0f;

// limits
const float CAMERA_YAW_MIN = -90.0f;   // leftmost view
const float CAMERA_YAW_MAX = 90.0f;    // rightmost view
const float CAMERA_PITCH_MIN = -10.0f; // how far down you can look
const float CAMERA_PITCH_MAX = 10.0f;  // how far up you can look
const float CAMERA_ZOOM_MIN = 10.0f;   // closest zoom
const float CAMERA_ZOOM_MAX = 30.0f;   // farthest zoom

bool leftMouseDown = false;
bool rightMouseDown = false;
// visibility toggle
bool showBase = true; // booth base visible by default

int lastMouseX = 0;
int lastMouseY = 0;

// flip animation
bool isFlipping = false;      // currently flipping?
bool isFlipped = false;       // fully flipped down?
float flipAngle = 0.0f;       // current rotation around x-axis
const float flipSpeed = 5.0f; // degrees per frame

// global scene parameters
WaveParams gWave;
BoothLayout gBooth;

static ShaderProgram gGroundProg;

// mouse button handler for camera orbit and zoom
void mouseButton(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON)
    leftMouseDown = (state == GLUT_DOWN), lastMouseX = x, lastMouseY = y;
  else if (button == GLUT_RIGHT_BUTTON)
    rightMouseDown = (state == GLUT_DOWN), lastMouseY = y;
  else if ((button == 3 || button == 4) && state == GLUT_DOWN) // scroll wheel
  {
    cameraZoom += (button == 3 ? -1.0f : 1.0f);
    if (cameraZoom < CAMERA_ZOOM_MIN)
      cameraZoom = CAMERA_ZOOM_MIN;
    if (cameraZoom > CAMERA_ZOOM_MAX)
      cameraZoom = CAMERA_ZOOM_MAX;
    glutPostRedisplay();
  }
}

// mouse motion handler for camera orbit and zoom
void mouseMotion(int x, int y)
{
  if (leftMouseDown)
  {
    // adjust yaw (left/right)
    cameraYaw += (x - lastMouseX) * 0.5f;
    // Clamp to 180° total range
    if (cameraYaw > CAMERA_YAW_MAX)
      cameraYaw = CAMERA_YAW_MAX;
    if (cameraYaw < CAMERA_YAW_MIN)
      cameraYaw = CAMERA_YAW_MIN;

    // sdjust pitch (up/down)
    cameraPitch += (y - lastMouseY) * 0.5f;
    // clamp vertical look range
    if (cameraPitch > CAMERA_PITCH_MAX)
      cameraPitch = CAMERA_PITCH_MAX;
    if (cameraPitch < CAMERA_PITCH_MIN)
      cameraPitch = CAMERA_PITCH_MIN;

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
  }
  else if (rightMouseDown)
  {
    // smooth zoom
    cameraZoom += (y - lastMouseY) * 0.1f;
    if (cameraZoom < CAMERA_ZOOM_MIN)
      cameraZoom = CAMERA_ZOOM_MIN;
    if (cameraZoom > CAMERA_ZOOM_MAX)
      cameraZoom = CAMERA_ZOOM_MAX;
    lastMouseY = y;
    glutPostRedisplay();
  }
}

// compute booth and wave parameters
void setupSceneParams()
{
  gBooth.duckBodyR = 1.2f;

  gBooth.openW = gBooth.duckBodyR * 16.0f;
  gBooth.openH = gBooth.duckBodyR * 7.0f;

  gBooth.baseH = gBooth.duckBodyR * 2.8f;
  gBooth.baseCenterY = -3.25f;
  gBooth.baseTopY = gBooth.baseCenterY + gBooth.baseH * 0.5f;
  gBooth.groundY = gBooth.baseCenterY - gBooth.baseH * 0.5f;

  gBooth.pillarW = gBooth.duckBodyR * 1.6f;
  const float halfOpenW = gBooth.openW * 0.5f;
  gBooth.pillarX = halfOpenW + gBooth.pillarW * 0.5f;
  gBooth.pillarsOuterHalf = gBooth.pillarX + gBooth.pillarW * 0.5f;

  const float waveSideClear = gBooth.duckBodyR * 0.4f;
  gBooth.baseW = gBooth.openW - 2.0f * waveSideClear;
  gBooth.baseDepth = gBooth.pillarW * 2.0f;

  gBooth.beamH = gBooth.duckBodyR * 1.8f;
  gBooth.beamOverhang = gBooth.duckBodyR * 1.0f;
  gBooth.beamUndersideY = gBooth.baseTopY + gBooth.openH;
  gBooth.beamTopY = gBooth.beamUndersideY + gBooth.beamH;
  gBooth.pillarH = gBooth.beamTopY - gBooth.groundY - 0.01f;
  gBooth.pillarCenterY = (gBooth.beamTopY + gBooth.groundY) * 0.5f;
  gBooth.beamW = (gBooth.pillarsOuterHalf + gBooth.beamOverhang) * 2.0f;
  gBooth.beamCenterY = gBooth.beamUndersideY + gBooth.beamH * 0.5f;

  gWave.width = gBooth.baseW;
  gWave.waves = 6;
  gWave.amp = gBooth.duckBodyR * 0.72f;
  gWave.lift = gWave.amp + 0.001f;
  gWave.thickZ = gBooth.baseDepth * 0.4f;
  gWave.baseY = -0.001f;
  gWave.x0 = -gWave.width * 0.5f;
  gWave.x1 = gWave.width * 0.5f;
}

int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(vWidth, vHeight);
  glutInitWindowPosition(200, 30);
  glutCreateWindow("Duck Target 3D");

  GLenum glewErr = glewInit();
  if (glewErr != GLEW_OK)
  {
    fprintf(stderr, "GLEW init error: %s\n", glewGetErrorString(glewErr));
    return 1;
  }

  initOpenGL(vWidth, vHeight);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutTimerFunc(16, animationHandler, 0);

  glutMainLoop();
  return 0;
}

// initialize OpenGL
void initOpenGL(int w, int h)
{
  GLfloat light_pos[] = {-4.0f, 8.0f, 8.0f, 1.0f};
  GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  GLfloat light_ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};

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

  panelMesh = QuadMesh::MakeUnitPanel();
  setupSceneParams();

  std::string base = "include/shaders/";
  std::string err;
  gGroundProg = MakeGroundProgram(base + "ground.vert", base + "ground.frag", &err);

  if (!gGroundProg.program)
  {
    fprintf(stderr, "Shader program failed: %s\n", err.c_str());
  }
  else
  {
    fprintf(stdout, "Shader compiled and linked successfully.\n");
    groundMesh->CreateMeshVBO(meshSize, gGroundProg.attribPos, gGroundProg.attribNormal);
  }
}

// GLM helpers
static glm::mat4 makeView(float camX, float camY, float camZ)
{
  return glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0, 1, 0));
}
static glm::mat4 makeProj(int w, int h)
{
  float aspect = (float)w / (float)h;
  return glm::perspective(glm::radians(60.0f), aspect, 1.0f, 100.0f);
}

// display function
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  float camX = cameraZoom * glm::sin(glm::radians(cameraYaw)) * glm::cos(glm::radians(cameraPitch));
  float camY = cameraZoom * -glm::sin(glm::radians(cameraPitch)) + 3.0f;
  float camZ = cameraZoom * glm::cos(glm::radians(cameraYaw)) * glm::cos(glm::radians(cameraPitch));
  gluLookAt(camX, camY, camZ, 0.0, 3.0, 0.0, 0.0, 1.0, 0.0);

  glutMouseFunc(mouseButton);
  glutMotionFunc(mouseMotion);

  drawBooth();

  glPushMatrix();
  if (duckState == TURN_AT_RIGHT)
  {
    glTranslatef(turnPivotX, turnPivotY, duckPos.z);
    glRotatef(ROT_DIR * duckSpinDeg, 0, 0, 1);
    glTranslatef(0.0f, +turnRadius, 0.0f);
  }
  else if (duckState == TURN_AT_LEFT)
  {
    glTranslatef(turnPivotX, turnPivotY, duckPos.z);
    glRotatef(ROT_DIR * duckSpinDeg, 0, 0, 1);
    glTranslatef(0.0f, -turnRadius, 0.0f);
    glRotatef(180.0f, 0, 0, 1);
  }
  else
  {
    glTranslatef(duckPos.x, duckPos.y, duckPos.z);
    if (duckState == BACKWARD)
      glRotatef(180.0f, 0, 0, 1);
  }

  // flip duck depending on angle
  glRotatef(flipAngle, 1, 0, 0);

  drawDuck();
  glPopMatrix();

  // draw ground with shader
  if (gGroundProg.program)
  {
    glUseProgram(gGroundProg.program);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, -5.f, 0.f));
    glm::mat4 view = makeView(camX, camY, camZ);
    glm::mat4 proj = makeProj(vWidth, vHeight);
    glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(model)));

    glUniformMatrix4fv(gGroundProg.locModel, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(gGroundProg.locView, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(gGroundProg.locProj, 1, GL_FALSE, &proj[0][0]);
    glUniformMatrix3fv(gGroundProg.locNormalMat, 1, GL_FALSE, &normalMat[0][0]);

    glUniform3f(gGroundProg.locLightPos, -4.0f, 8.0f, 8.0f);
    glUniform3f(gGroundProg.locViewPos, camX, camY, camZ);

    glUniform3f(gGroundProg.locMatAmbient, 0.12f, 0.28f, 0.12f);
    glUniform3f(gGroundProg.locMatDiffuse, 0.30f, 0.70f, 0.30f);
    glUniform3f(gGroundProg.locMatSpecular, 0.12f, 0.12f, 0.12f);
    glUniform1f(gGroundProg.locMatShininess, 16.0f);

    groundMesh->DrawMeshVBO(meshSize);
    glUseProgram(0);
  }
  else
  {
    glPushMatrix();
    glTranslatef(0.0, -5.0, 0.0);
    glColor3f(0.3f, 0.7f, 0.3f);
    groundMesh->DrawMesh(meshSize);
    glPopMatrix();
  }

  glutSwapBuffers();
}

// duck drawing
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
  glPushMatrix();
  glTranslatef(0.72f, 1.92f, 0.5f);
  glColor3f(0.0, 0.0, 0.0);
  glutSolidSphere(0.117f, 16, 16);
  glPopMatrix();

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
  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.14f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(4.0, 30, 30);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.26f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 1.0, 1.0);
  glutSolidSphere(3.0, 30, 30);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, -0.18f, 1.38f);
  glScalef(0.22f, 0.22f, 0.06f);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(2.0, 30, 30);
  glPopMatrix();
}

// wave helpers
float waveYAt(float x)
{
  const float t = (x - gWave.x0) / (gWave.x1 - gWave.x0);
  return gWave.lift + gWave.amp * glm::sin(2.0f * glm::pi<float>() * gWave.waves * t);
}

// draw wave
void drawWaterWave3D()
{
  const float halfT = gWave.thickZ * 0.5f;
  const float step = 0.08f;

  glColor3f(0.0f, 0.8f, 1.0f);
  glBegin(GL_TRIANGLE_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, y, -halfT);
  }
  glEnd();

  glBegin(GL_QUAD_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, halfT);
    glVertex3f(x, gWave.baseY, halfT);
  }
  glEnd();

  glBegin(GL_QUAD_STRIP);
  for (float x = gWave.x0; x <= gWave.x1 + 1e-4f; x += step)
  {
    float y = waveYAt(x);
    glVertex3f(x, y, -halfT);
    glVertex3f(x, gWave.baseY, -halfT);
  }
  glEnd();

  glBegin(GL_QUADS);
  glVertex3f(gWave.x0, gWave.baseY, halfT);
  glVertex3f(gWave.x1, gWave.baseY, halfT);
  glVertex3f(gWave.x1, gWave.baseY, -halfT);
  glVertex3f(gWave.x0, gWave.baseY, -halfT);
  glEnd();

  float yL = waveYAt(gWave.x0), yR = waveYAt(gWave.x1);
  glBegin(GL_QUADS);
  glVertex3f(gWave.x0, gWave.baseY, halfT);
  glVertex3f(gWave.x0, yL, halfT);
  glVertex3f(gWave.x0, yL, -halfT);
  glVertex3f(gWave.x0, gWave.baseY, -halfT);
  glEnd();

  glBegin(GL_QUADS);
  glVertex3f(gWave.x1, gWave.baseY, -halfT);
  glVertex3f(gWave.x1, yR, -halfT);
  glVertex3f(gWave.x1, yR, halfT);
  glVertex3f(gWave.x1, gWave.baseY, halfT);
  glEnd();
}

// draw box
void drawBox(float w, float h, float d)
{
  float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;

  glBegin(GL_QUADS);
  // Front
  glNormal3f(0, 0, 1);
  glVertex3f(-hw, -hh, hd);
  glVertex3f(hw, -hh, hd);
  glVertex3f(hw, hh, hd);
  glVertex3f(-hw, hh, hd);

  // Back
  glNormal3f(0, 0, -1);
  glVertex3f(-hw, -hh, -hd);
  glVertex3f(-hw, hh, -hd);
  glVertex3f(hw, hh, -hd);
  glVertex3f(hw, -hh, -hd);

  // Left
  glNormal3f(-1, 0, 0);
  glVertex3f(-hw, -hh, -hd);
  glVertex3f(-hw, -hh, hd);
  glVertex3f(-hw, hh, hd);
  glVertex3f(-hw, hh, -hd);

  // Right
  glNormal3f(1, 0, 0);
  glVertex3f(hw, -hh, -hd);
  glVertex3f(hw, hh, -hd);
  glVertex3f(hw, hh, hd);
  glVertex3f(hw, -hh, hd);

  // Top
  glNormal3f(0, 1, 0);
  glVertex3f(-hw, hh, -hd);
  glVertex3f(-hw, hh, hd);
  glVertex3f(hw, hh, hd);
  glVertex3f(hw, hh, -hd);

  // Bottom
  glNormal3f(0, -1, 0);
  glVertex3f(-hw, -hh, -hd);
  glVertex3f(hw, -hh, -hd);
  glVertex3f(hw, -hh, hd);
  glVertex3f(-hw, -hh, hd);
  glEnd();
}

// draw booth and wave
void drawBooth()
{
  const float colPillars[3] = {0.447f, 0.443f, 0.506f};
  const float colBeam[3] = {0.537f, 0.467f, 0.467f};

  if (showBase)
  {
    glColor3fv(colPillars);
    glPushMatrix();
    glTranslatef(0.0f, gBooth.baseCenterY, 0.0f);
    drawBox(gBooth.baseW, gBooth.baseH, gBooth.baseDepth);
    glPopMatrix();
  }

  glColor3fv(colPillars);
  glPushMatrix();
  glTranslatef(-gBooth.pillarX, gBooth.pillarCenterY, 0.0f);
  drawBox(gBooth.pillarW, gBooth.pillarH, gBooth.pillarW * 1.5f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(gBooth.pillarX, gBooth.pillarCenterY, 0.0f);
  drawBox(gBooth.pillarW, gBooth.pillarH, gBooth.pillarW * 1.5f);
  glPopMatrix();

  glColor3fv(colBeam);
  glPushMatrix();
  glTranslatef(0.0f, gBooth.beamCenterY, 0.0f);
  drawBox(gBooth.beamW, gBooth.beamH, gBooth.duckBodyR * 2.6f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, gBooth.baseTopY, 0.0f);
  drawWaterWave3D();
  glPopMatrix();
}

// reshape handler
void reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
}

// keyboard handler
void keyboard(unsigned char key, int x, int y)
{
  if (key == 27) // ESC to quit
    exit(0);

  if ((key == 'f' || key == 'F') && duckState == FORWARD && !isFlipping && !isFlipped)
  {
    isFlipping = true;
  }

  if (key == 32) // SPACE key
  {
    showBase = !showBase; // toggle visibility
    glutPostRedisplay();
  }
}

// animation handler
void animationHandler(int)
{
  switch (duckState)
  {
  case FORWARD:
    duckPos.x += moveSpeed;
    duckPos.y = waveYAt(duckPos.x) - 0.10f;

    if (duckPos.x >= gWave.x1)
    {
      duckPos.x = gWave.x1;
      duckPos.y = waveYAt(gWave.x1) - 0.10f;

      turnRadius = gWave.amp * 2.4f;
      turnPivotX = duckPos.x;
      turnPivotY = duckPos.y - turnRadius;

      duckSpinDeg = 0.0f;
      duckState = TURN_AT_RIGHT;
    }
    break;

  case TURN_AT_RIGHT:
    duckSpinDeg += spinSpeed;
    if (duckSpinDeg >= 180.0f)
    {
      duckSpinDeg = 180.0f;

      duckPos.x = turnPivotX;
      duckPos.y = turnPivotY - turnRadius;
      duckState = BACKWARD;
    }
    break;

  case BACKWARD:
    duckPos.x -= moveSpeed;

    if (duckPos.x <= gWave.x0)
    {
      duckPos.x = gWave.x0;

      turnRadius = gWave.amp * 2.4f;
      turnPivotX = duckPos.x;
      turnPivotY = duckPos.y + turnRadius;

      duckSpinDeg = 0.0f;
      duckState = TURN_AT_LEFT;
    }
    break;

  case TURN_AT_LEFT:
    duckSpinDeg += spinSpeed;
    if (duckSpinDeg >= 180.0f)
    {
      duckSpinDeg = 180.0f;

      duckPos.x = turnPivotX;
      duckPos.y = turnPivotY + turnRadius;

      duckSpinDeg = 0.0f;
      duckState = FORWARD;
    }
    break;
  }

  // flipping animation
  if (isFlipping)
  {
    // rotate downwards
    flipAngle -= flipSpeed;
    if (flipAngle <= -90.0f)
    {
      flipAngle = -90.0f;
      isFlipping = false;
      isFlipped = true;
    }
  }
  else if (isFlipped && duckState == BACKWARD)
  {
    // auto flip back up when moving right -> left
    flipAngle += flipSpeed;
    if (flipAngle >= 0.0f)
    {
      flipAngle = 0.0f;
      isFlipped = false;
    }
  }

  glutPostRedisplay();
  glutTimerFunc(16, animationHandler, 0);
}
