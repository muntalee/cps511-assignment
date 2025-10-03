/*******************************************************************
                   Hierarchical Duck Target Example
********************************************************************/
#include "Duck.h"

// ---------------- GLOBALS ---------------- //
const int vWidth = 650;
const int vHeight = 500;

DuckConfig duckConfig = {
    1.5f, // body radius (smaller)
    0.8f, // head radius
    0.3f, // beak radius
    0.6f, // beak length
    4.0f, // stand height
    0.2f  // stand radius
};

// Animation control
float duckX = 8.0f; // start on right
float duckAngle = 0.0f;

// Meshes
CubeMesh *cubeMesh = nullptr;
QuadMesh *groundMesh = nullptr;
int meshSize = 16;

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
// OPENGL INIT
// ================================================================
void initOpenGL(int w, int h)
{
  // Lighting
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

  // ✅ enable colors as materials
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  // GL settings
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
}

// ================================================================
// DISPLAY
// ================================================================
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  // Booth
  drawBooth();

  // Duck
  glPushMatrix();
  glTranslatef(duckX, 2.0, 0.0);
  drawDuck();
  glPopMatrix();

  // Duck stand
  drawDuckStand();

  // Ground
  glPushMatrix();
  glTranslatef(0.0, -5.0, 0.0);
  glColor3f(0.3f, 0.7f, 0.3f);
  groundMesh->DrawMesh(meshSize);
  glPopMatrix();

  glutSwapBuffers();
}

// ================================================================
// DUCK DRAWING
// ================================================================
// ================= DUCK =================
void drawDuck()
{
  drawDuckBody();

  // head (slightly forward & above body)
  glPushMatrix();
  glTranslatef(0.0, duckConfig.bodyRadius * 0.6f, duckConfig.bodyRadius * 0.6f);
  drawDuckHead();

  // beak attached at front
  glTranslatef(duckConfig.headRadius * 0.9f, 0.0, 0.0);
  drawDuckBeak();
  glPopMatrix();

  // tail
  drawDuckTail();
}

void drawDuckBody()
{
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  glutSolidSphere(duckConfig.bodyRadius, 30, 30);
  glPopMatrix();

  drawDuckTarget();
}

void drawDuckHead()
{
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  glutSolidSphere(duckConfig.headRadius * 0.7f, 30, 30);
  glPopMatrix();
}

void drawDuckBeak()
{
  glPushMatrix();
  glColor3f(1.0, 0.2, 0.0);
  glRotatef(90, 0, 1, 0);
  glutSolidCone(duckConfig.beakRadius, duckConfig.beakLength, 20, 20);
  glPopMatrix();
}

void drawDuckTail()
{
  glPushMatrix();
  glColor3f(1.0, 1.0, 0.0);
  glTranslatef(0.0, 0.0, -duckConfig.bodyRadius * 0.9f);
  glRotatef(45, 1, 0, 0); // tilt up
  glutSolidCone(0.8, 1.2, 20, 20);
  glPopMatrix();
}

void drawDuckTarget()
{
  glPushMatrix();
  // Place target on body’s side
  glTranslatef(-duckConfig.bodyRadius * 1.0f, 0.0f, 0.0f);
  glScalef(0.3f, 0.3f, 0.1f);

  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(4.0, 30, 30); // outer red
  glColor3f(1.0, 1.0, 1.0);
  glutSolidSphere(2.5, 30, 30); // middle white
  glColor3f(1.0, 0.0, 0.0);
  glutSolidSphere(1.2, 30, 30); // inner red
  glPopMatrix();
}

void drawDuckStand()
{
  glPushMatrix();
  glColor3f(0.5, 0.3, 0.1);
  glTranslatef(duckX, -duckConfig.standHeight * 0.2f, 0.0f);
  glRotatef(-90, 1, 0, 0);
  gluCylinder(gluNewQuadric(), duckConfig.standRadius, duckConfig.standRadius,
              duckConfig.standHeight, 20, 20);
  glPopMatrix();
}

void drawWaterWave()
{
  glColor3f(0.5f, 0.8f, 1.0f);
  glBegin(GL_QUAD_STRIP);

  float width = 16.0f; // across booth
  float step = 0.2f;   // x step size
  float amp = 1.0f;    // amplitude
  float freq = 2.0f;   // frequency

  for (float x = -width / 2; x <= width / 2; x += step)
  {
    float y = amp * sinf(freq * x * 0.3f);

    // top wave line
    glVertex3f(x, y, 0.0f);

    // bottom straight line
    glVertex3f(x, -2.0f, 0.0f);
  }
  glEnd();
}

void drawBooth()
{
  // side pillars
  glColor3f(0.3, 0.3, 0.4);
  glPushMatrix();
  glTranslatef(-7.0, 2.5, 0.0);
  glScalef(1.0, 8.0, 1.0);
  glutSolidCube(1.0);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(7.0, 2.5, 0.0);
  glScalef(1.0, 8.0, 1.0);
  glutSolidCube(1.0);
  glPopMatrix();

  // top beam
  glPushMatrix();
  glTranslatef(0.0, 6.0, 0.0);
  glScalef(16.0, 1.0, 1.0);
  glutSolidCube(1.0);
  glPopMatrix();

  // base container
  glPushMatrix();
  glTranslatef(0.0, -1.0, 0.0);
  glColor3f(0.2, 0.2, 0.3);
  glScalef(16.0, 3.0, 6.0);
  glutSolidCube(1.0);
  glPopMatrix();

  // water sine wave
  glPushMatrix();
  glTranslatef(0.0, 0.0, 0.0);
  drawWaterWave();
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
    exit(0); // ESC
}

void animationHandler(int value)
{
  duckX -= 0.05f;
  if (duckX < -8.0f)
    duckX = 8.0f;

  glutPostRedisplay();
  glutTimerFunc(16, animationHandler, 0);
}
