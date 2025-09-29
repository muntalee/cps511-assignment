#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <iostream>

// simple triangle
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f( 0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f( 0.0f,  0.5f);
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char** argv)
{
    // --- test GLM ---
    glm::vec3 v1(1.0f, 2.0f, 3.0f);
    glm::vec3 v2(4.0f, 5.0f, 6.0f);
    glm::vec3 v3 = glm::cross(v1, v2);
    std::cout << "Cross product (GLM test): "
              << v3.x << ", " << v3.y << ", " << v3.z << std::endl;

    // --- init GLUT ---
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("FreeGLUT + GLEW + GLM test");

    // --- init GLEW ---
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "GLEW error: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    std::cout << "Using GLEW version: " << glewGetString(GLEW_VERSION) << std::endl;

    // --- OpenGL info ---
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    // set display callback
    glutDisplayFunc(display);

    // start event loop
    glutMainLoop();

    return 0;
}

