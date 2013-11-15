#ifdef _WIN32
#define WIN32_EXTRALEAN   
#include <windows.h>
#endif

#include "display.h"
#include "ParticleSystem.h"
#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/glu.h>



int delay = 1000/FPS;
int win=0;
PSystem *ps;
int mmode=1;
int rotx=0, roty=0;

void draw() {
	 //Clear information from last draw
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	 glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
	 glLoadIdentity(); //Reset the drawing perspective
	 
	 //glScalef(.01,.01,1);
	 
	 glColor3f(1,1,1);
	 
	 glTranslatef(0.0f,0.0f,-5.0f);
	 glRotatef(roty,0.0f,1.0f,0.0f);
	 glRotatef(rotx,1.0f,0.0f,0.0f);
     glTranslatef(0.0f,0.0f,5.0f);
     
     ps->draw();
     
	 
     
     glutSwapBuffers();
}



//Called when the window is resized
void Reshape(int w, int h) {
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(45.0,                  //The camera angle
				   (double)w / (double)h, //The width-to-height ratio
				   1.0,                   //The near z clipping coordinate
				   200.0);                //The far z clipping coordinate
}


void creatDisplay() {
    glutInitWindowSize(W, H); //Set the window size
	
	//Create the window
	glutCreateWindow("Particle System");
	glEnable(GL_DEPTH_TEST);
	
	//Set handler functions for drawing, keypresses, and window resizes
	glutDisplayFunc(draw);
	//glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(Reshape);
	
	glutKeyboardFunc(keypress);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	
	ps = new PSystem();
	
	glutTimerFunc(delay, update, 0);
}



void update(int value) {
	ps->update();
	
	glutPostRedisplay(); //Tell GLUT that the display has changed
	
	//Tell GLUT to call update again in 'delay' milliseconds
	glutTimerFunc(delay, update, 0);
}


void keypress (unsigned char key, int x, int y) {
    //1
    if (key==49) {
        ps->changeMode(1);
        mmode=1;
    }
    //2
    else if (key==50) {
        ps->changeMode(2);
        mmode=2;
    }
    //3
    else if (key==51) {
        ps->changeMode(3);
        mmode=3;
    }
    //4
    else if (key==52) {
        ps->changeMode(4);
        mmode=4;
    }
    //5
    else if (key==53) {
        ps->changeMode(5);
        mmode=5;
    }
    //6
    else if (key==54) {
        ps->changeMode(6);
        mmode=6;
    }
    //7
    else if (key==55) {
        ps->changeMode(7);
        mmode=7;
    }
    
    ps->changePointType(key);
    
    if(key == 'a') roty += 5;
	else if(key == 'd') roty -= 5;	
	else if(key == 's') rotx += 5;
	else if(key == 'w') rotx -= 5;
	else if(key == 'r') {rotx = 0; roty = 0;}
}

void mouse(int button, int state, int x, int y) {
    if (mmode!=3) return;
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
        ps->setMouseState(1);
    }
    else if (button==GLUT_LEFT_BUTTON && state==GLUT_UP) {
        ps->setMouseState(0);
    }
}

void mouseMotion(int x, int y) {
    if (mmode!=3) return;
    ps->setMousePos((4.0f*x/W)-2, (-4.0f*y/H)+2);
}

