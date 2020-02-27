#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <GL/glut.h>

using namespace glm;

void RenderSceneCB() {
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers(); 
}

int main(int argc, char **argv) {
    glutInit(&argc, argv); 
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
    
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tutorial 01"); 

    glutDisplayFunc(RenderSceneCB); 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); 
    
    glutMainLoop(); 
    
    std::cerr << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << std::endl;

}
