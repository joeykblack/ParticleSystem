#ifdef _WIN32
#define WIN32_EXTRALEAN     
#include <windows.h>
#endif

#include "display.h"
#include <GL/glut.h>
#include <GL/GL.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    
    creatDisplay();
    
    glutMainLoop();
}
