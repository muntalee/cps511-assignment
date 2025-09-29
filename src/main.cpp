/*******************************************************************
		   Hierarchical Multi-Part Model Example
********************************************************************/
#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>

#define GLEW_STATIC
#include <GL/glew.h>
#ifdef _WIN32
#include <GL/wglew.h> // For wglSwapInterval
#endif

#define FREEGLUT_STATIC
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>



// Lighting/shading and material properties - upcoming lecture - just copy for now
GLfloat mat_ambient[] = { 0.4f, 0.2f, 0.0, 1.0 };
GLfloat mat_specular[] = { 0.4f, 0.4f, 0.0, 1.0 };
GLfloat mat_diffuse[] = { 0.9f, 0.5f, 0.0, 1.0 };
GLfloat mat_shininess[] = { 0.0 };

GLfloat light_position0[] = { -5.0,5.0,5.0,1.0 };
GLfloat light_position1[] = { 5.0,5.0,5.0,1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat model_ambient[] = { 0.0, 0.0, 0.0, 1.0 };


static GLfloat spin = 0.0;

// Prototypes
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(750, 750);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("3D Example");

	initOpenGL(750, 750);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMainLoop();
	return 0;
}

// Initialize the system 
void initOpenGL(int w, int h)
{
	// Clear the framebuffer, set to black background R = 0, G = 0, B = 0
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 0.1, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Setup lighting and material properties. Just copy this for now
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
	// Set up light properties and position
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	// Set shading style to smooth shading. GL_FLAT is also available.
	glShadeModel(GL_SMOOTH);
	// Enable (turn on) lighting. Once you do this, glColor() function calls are ignored.
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	// This is a 3D program. We need to enable depth testing (hidden surface removal).
	glEnable(GL_DEPTH_TEST);
}


// Callback where we draw objects. This function is never called directly by us.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// Set the camera (i.e. set up the Viewing Transformation (V matrix)):
	// Position is (0, 0, 10), aimed at the origin, up direction along the positive y axis.
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Perform a rotation about z (i.e. set up Modeling Transformation (M matrix))
	// This rotation will be applied to all objects defined after this line.
	// We will discuss how the transformation matrices are built in an upcoming lecture
	glRotatef(spin, 0.0, 0.0, 1.0);

	// Draw objects
	glPushMatrix();
	glTranslatef(-2.0, 0.0, 0.0);
	glScalef(0.5, 4.0, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.0, 0.0, 0.0);
	glScalef(0.5, 4.0, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 2.0, 0.0);
	glScalef(4.0, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, -2.0, 0.0);
	glScalef(4.0, 0.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.0, 2.0, 0.0);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.0, 2.0, 0.0);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.0, -2.0, 0.0);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(2.0, -2.0, 0.0);
	glutSolidSphere(1.0, 20, 20);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap front/back buffers
}



// Called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)w / (double)h, 0.1, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void spinDisplay(int v)
{
	spin += 2.0;
	if (spin > 360.0)
		spin -= 360.0;

	glutPostRedisplay();
	glutTimerFunc(20, spinDisplay, 0);
}


void mouse(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			glutTimerFunc(20, spinDisplay, 0);//	glutIdleFunc(spinDisplay);
		break;
		/*
		case GLUT_MIDDLE_BUTTON:
		  if (state == GLUT_DOWN)
			glutIdleFunc(NULL);
		  break;
		*/
	default:
		break;
	}
}


