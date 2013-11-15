#ifdef _WIN32
#define WIN32_EXTRALEAN     
#include <windows.h>
#endif

#include "ParticleSystem.h"
#include "display.h"
#include <GL/GL.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include <gl/glu.h>			// Header File For The GLu32 Library
#include <gl/glaux.h>		// Header File For The Glaux Library
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
using namespace std;
DWORD start, end;
int mode=1; // 1-fountain, 2-cyclone, 3-explode
int mstate=0; //0=up, 1=down
float mposx=0, mposy=0;
//fountain
vector<v3d*> plane; 
v3d normal;
/*Bezier*/
int nCtrl=4;
int nPts=100;
vector<v3d*> basePoints, curCtrlPts;
Bezier* base;
int curBPt;
Bezier* curbez;
//texture
GLuint texture;
// GL_ARB_point_parameters
PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB  = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;
//Type of points
bool antialiased = true;
bool sprite = false;
bool spriteSupported = true;
float maxSize = 10.0f;


PSystem::PSystem() {
     np = 10000; 
     isfree = new (nothrow) bool[np];
     if (isfree==0) {cout << "Error: memory could not be allocated: isfree" << endl; exit(0);}
     vel = new (nothrow) v3d[np];
     if (vel==0) {cout << "Error: memory could not be allocated: vel" << endl; exit(0);}
     points = new (nothrow) GLfloat[np*3];
     if (points==0) {cout << "Error: memory could not be allocated: points" << endl; exit(0);}
     pcolors = new (nothrow) GLfloat[np*4];
     if (pcolors==0) {cout << "Error: memory could not be allocated: pcolors" << endl; exit(0);}
     
     
     //for bezier
     basePoints.push_back( newv3d(-1,-1,-5) );
     basePoints.push_back( newv3d(1,-1,-5) );
     basePoints.push_back( newv3d(1,1,-5) );
     basePoints.push_back( newv3d(-1,1,-5) );
     
     //for fountain
     plane.push_back(newv3d(-1, -1, -4));
     plane.push_back(newv3d(1, -1, -4));
     plane.push_back(newv3d(1, -1, -6));
     plane.push_back(newv3d(-1, -1, -6));
     
     normal = cros(makev3d(plane.at(0)->x, plane.at(0)->y, plane.at(0)->z, plane.at(1)->x, plane.at(1)->y, plane.at(1)->z),
                   makev3d(plane.at(0)->x, plane.at(0)->y, plane.at(0)->z, plane.at(2)->x, plane.at(2)->y, plane.at(2)->z));
     
     changeMode(mode);
     
    if (!LoadGLTextures())								// Jump To Texture Loading Routine ( NEW )
	{
		cout << "Failed to load texture." << endl;
		spriteSupported = false;
	}
	
	if (spriteSupported) {
    	//char a;
    	//cout << "Sprites loaded." << endl;
    	//cout << "Enable point sprites (y/n): ";
    	//cin >> a;
    	//if (a=='y') {
                    spriteSupported = true;
                    antialiased = false;
                    sprite = true;
        //}
    	//else spriteSupported = false;
    	//cout << endl << endl;
    }
	
	glPointParameterfARB  = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
    glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");
    
    cout << "1  Fountain" << endl;
    cout << "2  Cyclone" << endl;
    cout << "3  Explode (Click for explosion)" << endl;
    cout << "4  Bezier" << endl;
    cout << "5  Starfield" << endl;
    cout << "6  Fire" << endl;
    cout << "7  Universe" << endl << endl;
    
    cout << "p  Pixels (square)" << endl;
    cout << "o  Antialiased points" << endl;
    if (spriteSupported) cout << "i  Sprites" << endl;
    cout << "[  Decrease pixel size" << endl;
    cout << "]  Increase pixel size" << endl;    
    
}

PSystem::~PSystem() {
    free.clear();
    delete[] points;
    delete[] isfree;
    delete[] pcolors;
    delete[] vel;
}


void PSystem::draw() {
    //start = GetTickCount();
    //cout << "draw" << endl;
    
    // This is how will our point sprite's size will be modified by 
    // distance from the viewer
    float quadratic[] =  { 1.0f, 0.0f, 0.01f };
    glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
    
    // Query for the max point size supported by the hardware
    //float maxSize = 0.0f;
    //glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );

    // Clamp size to 100.0f or the sprites could get a little too big on some  
    // of the newer graphic cards. My ATI card at home supports a max point 
    // size of 1024.0f!
    //if( maxSize > 20.0f ) maxSize = 20.0f;
    glPointSize( maxSize );

    // The alpha of a point is calculated to allow the fading of points 
    // instead of shrinking them past a defined threshold size. The threshold 
    // is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
    // the minimum and maximum point sizes.
    glPointParameterfARB( GL_POINT_FADE_THRESHOLD_SIZE_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, 1.0f );
    glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, maxSize );


    /*
      Point sprites
    */
    // Specify point sprite texture coordinate replacement mode for each 
    // texture unit
    if (sprite && spriteSupported) {
        glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
        //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glEnable( GL_POINT_SPRITE_ARB );
    }
    
    
    
    ///////////
    //blending
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc (GL_GREATER);
    glEnable (GL_BLEND); 
    //glEnable (GL_ALPHA_TEST); 
    if (sprite) glBlendFunc (GL_ONE, GL_ONE);
    else glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_SRC_COLOR, GL_ONE);
    //glBlendFunc (GL_ONE_MINUS_SRC_COLOR, GL_ONE);
    
    
    //antialiasing
    if (antialiased) glEnable(GL_POINT_SMOOTH);
    
    //Draw
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, pcolors);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, points);    
    glDrawArrays(GL_POINTS, 0, np);
    //glDrawElements(GL_POINTS, np, GL_FLOAT, points);
    
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    glDisable( GL_POINT_SPRITE_ARB );
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    //cout << "end draw" << endl;
    //end = GetTickCount();
    //cout << "draw: " << (end-start) << endl;
}



void PSystem::update() {
    //start = GetTickCount();
    if (mode==1) {
        fountain();
    }
    else if (mode==2) {
        cyclone(makev3d(0,-1,-5), makev3d(0,1,-5), 0.2, 1);
    }
    else if (mode==3) {
        if (mstate==1) explode(10, makev3d(mposx, mposy, -5));
        expand();
    }
    else if (mode==4) {
        bezierMove();
    }
    else if (mode==5) {
         starfield(10, 1);     
    }
    else if (mode==6) {
         fire(makev3d(0,-1,-5), 0.1);
    }
    else if (mode==7) {
         universe(makev3d(0,0,-5), makev3d(0,1,0), 1.5);
    }
    //end = GetTickCount();
    //cout << "update: " << (end-start) << endl;
}







void PSystem::changeMode(int newm) {
    mstate=0; mposx=0; mposy=0;
    mode = newm;
    if (mode==1 || mode==5 || mode==6) clearps();
    else if (mode==2) setCyclone(makev3d(0,-1,-5), makev3d(0,1,-5), 0.2, 1);
    else if (mode==3) {
        clearps();
    }
    else if (mode==4) {
        setBezier(nCtrl, basePoints, nPts);
    }
    else if (mode==7) {
         clearps();
         setUniverse(makev3d(0,0,-5), makev3d(0,1,0), 2);
    }
}

void PSystem::changePointType(char pt) {
     if (pt=='o') {
        antialiased = true;
        sprite = false;
     }     
     else if (pt=='i' && spriteSupported) {
        antialiased = false;
        sprite = true;
     }
     else if (pt=='p') {
        antialiased = false;
        sprite = false;
     }
     else if (pt=='[') {
          maxSize--;
          if (maxSize<1) maxSize=1;
     }
     else if (pt==']') {
          maxSize++;
     }
}

void PSystem::setMouseState(int state) {
    mstate = state;
}

void PSystem::setMousePos(float x, float y) {
    if (mstate==0) return;
    mposx=x; mposy=y;
}

float PSystem::random(float min, float max) {
    float r = (float)rand()/RAND_MAX;
    r = r*(max-min) + min;
    return r;
}




/*
    Fountain
*/

//add num particles at source traveling in normal dirrection
void PSystem::ignite(int num, v3d source, v3d normal) {
    if (num>free.size()) num = free.size();
    for (int i=0; i<num; i++) {
        int p = free.back();
        free.pop_back();
        isfree[p] = false;
        points[p*3] = source.x;
        points[p*3+1] = source.y;
        points[p*3+2] = source.z;
        vel[p].x = random((normal.x-0.3)/100.0f, (normal.x+0.3)/100.0f);
        vel[p].y = random((normal.y-0.3)/100.0f, (normal.y+0.3)/100.0f);
        vel[p].z = random((normal.z-0.3)/100.0f, (normal.z+0.3)/100.0f);
        pcolors[p*4] = random(0.3,0.5);
        pcolors[p*4+1] = random(0.3,0.5);
        pcolors[p*4+2] = random(0.8,1);
        pcolors[p*4+3] = 0.5;
    }
}


void PSystem::move() {
    for (int p=0; p<np; p++) {
        if (isfree[p]) continue;
        points[p*3] += vel[p].x;
        points[p*3+1] += vel[p].y;
        points[p*3+2] += vel[p].z;
    }
}

void PSystem::cool() {
    for (int p=0; p<np; p++) {
        if (isFree(p)) continue;
        pcolors[p*4] -= 0.003f;
        pcolors[p*4+1] -= 0.003f;
        pcolors[p*4+2] -= 0.003f;
        pcolors[p*4+3] = 1;
        if ((pcolors[p*4]<=0) && 
            (pcolors[p*4+1]<=0) &&
            (pcolors[p*4+2])<=0) {
                free.push_back(p);
                isfree[p] = true;
        }
    }
}

bool PSystem::isFree(int p) {
    return isfree[p];
}


void PSystem::accelerate(v3d dir, float a) {
    dir = normalize(dir);
    for (int i=0; i<np; i++) {
        vel[i].x += dir.x*a;
		vel[i].y += dir.y*a;
		vel[i].z += dir.z*a;
    }
}


void PSystem::clearps() {
    for (int i=0; i<np; i++) {
         points[i*3] = 0;
         points[i*3+1] = 0;
         points[i*3+2] = 0;
         pcolors[i*4] = 0;
         pcolors[i*4+1] = 0;
         pcolors[i*4+2] = 0;
         pcolors[i*4+3] = 0.5;
         vel[i].x = 0;
         vel[i].y = 0;
         vel[i].z = 0;
         free.push_back(i);
         isfree[i] = true;
     }
}





float x,y,z;

void PSystem::fountain() {
    ignite(10, makev3d(0,0,-5), makev3d(0,1,0));
    v3d dir = makev3d(0,-1,0);
    float a = 0.00005f;
    for (int p=0; p<np; p++) {
        if (isfree[p]) continue;
        //cool
        pcolors[p*4] -= 0.001f;
        pcolors[p*4+1] -= 0.001f;
        pcolors[p*4+2] -= 0.001f;
        pcolors[p*4+3] = 1;
        if ((pcolors[p*4]<=0) && 
            (pcolors[p*4+1]<=0) &&
            (pcolors[p*4+2])<=0) {
                free.push_back(p);
                isfree[p] = true;
                continue;
        }
        //move
        points[p*3] += vel[p].x;
        points[p*3+1] += vel[p].y;
        points[p*3+2] += vel[p].z;
        //accelerate
        vel[p].x += dir.x*a;
		vel[p].y += dir.y*a;
		vel[p].z += dir.z*a;
    }
}




/*
     Cyclone
*/

void PSystem::setCyclone(v3d orig, v3d top, float diamOrig, float diamTop) {
    float a,b,c;
    a = fabs(diamOrig - diamTop);
    b = sqrt(orig.x*top.x + orig.y*top.y + orig.z*top.z);
    c = sqrt(a*a + b*b);
    float chx,chy,chz;
    v3d rightAngle = normalize(cros(makev3d(top.x-orig.x, top.y-orig.y, top.z-orig.z), 
        makev3d(top.x-orig.x+1, top.y-orig.y, top.z-orig.z))); //make a unit vector that is perp. (any will do)
    float tx,ty,tz;
    tx = rightAngle.x*diamTop/2 + top.x;
    ty = rightAngle.y*diamTop/2 + top.y;
    tz = rightAngle.z*diamTop/2 + top.z;
    
    float x,y,z;
    x = rightAngle.x*diamOrig/2 + orig.x;
    y = rightAngle.y*diamOrig/2 + orig.y;
    z = rightAngle.z*diamOrig/2 + orig.z;
    
    chx = (tx - x) / np;
    chy = (ty - y) / np;
    chz = (tz - z) / np;
    
    for (int p=0; p<np; p++) {
        points[p*3] = x + random(-0.2,0.2);
        points[p*3+1] = y + random(-0.2,0.2);
        points[p*3+2] = z + random(-0.2,0.2);
        x += chx;
        y += chy;
        z += chz;
        pcolors[p*4] = 0.5;
        pcolors[p*4+1] = 0.5;
        pcolors[p*4+2] = 0.5;
        pcolors[p*4+3] = 0.5;
        vel[p].x = random(1, 20); //vel.x used as theta
        vel[p].y = 0;
        vel[p].z = 0;
    }
}


void PSystem::cyclone(v3d orig, v3d top, float diamOrig, float diamTop) {
    
    
    for (int p=0; p<np; p++) {
        
        float th = vel[p].x;
        float s = sin((th*M_PI)/180);
    	float c = cos((th*M_PI)/180);
        
        float th1;
        if ((top.x-orig.x)==0) th1=-90;
    	else th1=-atan( ((top.y-orig.y)/(top.x-orig.x)) ) *180/M_PI;
        float s1 = sin((th1*M_PI)/180);
        float c1 = cos((th1*M_PI)/180);
        float xa2 = (top.x*c1) - (top.x*s1);
        float xb2 = (orig.x*c1) - (orig.y*s1);
        float th2;
        if ((top.z-orig.z)==0) th2=-90; 
        else th2=-atan( ((xa2-xb2)/(top.z-orig.z)) ) *180/M_PI;
        float s2 = sin((th2*M_PI)/180);
    	float c2 = cos((th2*M_PI)/180);
        float x,y,z;
        float sn1 = sin((-th1*M_PI)/180);
    	float cn1 = cos((-th1*M_PI)/180);
    	float sn2 = sin((-th2*M_PI)/180);
    	float cn2 = cos((-th2*M_PI)/180);
        
       
        
        
        points[p*3] -= orig.x;
        points[p*3+1] -= orig.y;
        points[p*3+2] -= orig.z;
        
        //rotz th1
        x=points[p*3];  y=points[p*3+1];
        points[p*3] = (x*c1) - (y*s1);
		points[p*3+1] = (x*s1) + (y*c1);
		//roty th2
		x=points[p*3]; z=points[p*3+2];
		points[p*3] = (z*s2) + (x*c2);
		points[p*3+2] = (z*c2) - (x*s2);
		//rotz th
		x=points[p*3];  y=points[p*3+1];
        points[p*3] = (x*c) - (y*s);
		points[p*3+1] = (x*s) + (y*c);
        //roty -th2
		x=points[p*3]; z=points[p*3+2];
		points[p*3] = (z*sn2) + (x*cn2);
		points[p*3+2] = (z*cn2) - (x*sn2);
		//rotz -th1
        x=points[p*3];  y=points[p*3+1];
        points[p*3] = (x*cn1) - (y*sn1);
		points[p*3+1] = (x*sn1) + (y*cn1);
    
        points[p*3] += orig.x;
        points[p*3+1] += orig.y;
        points[p*3+2] += orig.z;
    }
}



/*
    Explode
*/

void PSystem::explode(int num, v3d source) {
    if (num>free.size()) num = free.size();
    for (int i=0; i<num; i++) {
        int p = free.back();
        free.pop_back();
        isfree[p] = false;
        points[p*3] = source.x;
        points[p*3+1] = source.y;
        points[p*3+2] = source.z;
        vel[p].x = random(-0.01, 0.01);
        vel[p].y = random(-0.01, 0.01);
        vel[p].z = random(-0.01, 0.01);
        pcolors[p*4] = random(0.75,1);
        pcolors[p*4+1] = random(0.1,0.5);
        pcolors[p*4+2] = random(0.1,0.5);
        pcolors[p*4+3] = 0.5;
    }
}


//expand = move/cool
void PSystem::expand() {
    for (int p=0; p<np; p++) {
        if (isfree[p]) continue;
        //cool
        pcolors[p*4] -= 0.001f;
        pcolors[p*4+1] -= 0.001f;
        pcolors[p*4+2] -= 0.001f;
        pcolors[p*4+3] = 1;
        if ((pcolors[p*4]<=0) && 
            (pcolors[p*4+1]<=0) &&
            (pcolors[p*4+2])<=0) {
                free.push_back(p);
                isfree[p] = true;
                continue;
        }
        //move
        points[p*3] += vel[p].x;
        points[p*3+1] += vel[p].y;
        points[p*3+2] += vel[p].z;
    }
}




/*
    Bezier
    All particles travel allong bezier curves created by previous n particles.

int nCtrl;
int nPts;
vector<v3d*> basePoints, curCtrlPts;
Bezier* base;
int curBPt;
Bezier* curbez; */

void PSystem::setBezier(int nc, vector<v3d*> baseP, int nPts) {
    clearps();
    curBPt=0;
    nCtrl = nc;
    basePoints = baseP;
    base = new Bezier(nCtrl, baseP, nPts);
    
    for (int i=0; i<np; i++) {
         pcolors[i*4] = random(0,0.1);
         pcolors[i*4+1] = random(0,1);
         pcolors[i*4+2] = random(0,0.1);
         pcolors[i*4+3] = 0.5;
    }
}

int dir=1;
void PSystem::bezierMove() {
    //p1
    v3d p = base->getP(curBPt);
    points[0] = p.x;
    points[1] = p.y;
    points[2] = p.z;
    //p2
    curCtrlPts.clear();
    curCtrlPts.push_back(basePoints.at(1));
    curCtrlPts.push_back(basePoints.at(2));
    curCtrlPts.push_back(basePoints.at(3));
    curCtrlPts.push_back(newv3d( points[0], points[1], points[2] ));
    curbez = new Bezier(nCtrl, curCtrlPts, nPts);
    p = curbez->getP(curBPt);
    points[3] = p.x;
    points[4] = p.y;
    points[5] = p.z;
    //p3
    curCtrlPts.clear();
    curCtrlPts.push_back(basePoints.at(2));
    curCtrlPts.push_back(basePoints.at(3));
    curCtrlPts.push_back(newv3d( points[0], points[1], points[2] ));
    curCtrlPts.push_back(newv3d( points[3], points[4], points[5] ));
    curbez = new Bezier(nCtrl, curCtrlPts, nPts);
    p = curbez->getP(curBPt);
    points[6] = p.x;
    points[7] = p.y;
    points[8] = p.z;
    //p4
    curCtrlPts.clear();
    curCtrlPts.push_back(basePoints.at(3));
    curCtrlPts.push_back(newv3d( points[0], points[1], points[2] ));
    curCtrlPts.push_back(newv3d( points[3], points[4], points[5] ));
    curCtrlPts.push_back(newv3d( points[6], points[7], points[8] ));
    curbez = new Bezier(nCtrl, curCtrlPts, nPts);
    p = curbez->getP(curBPt);
    points[9] = p.x;
    points[10] = p.y;
    points[11] = p.z;
    
    v3d point;
    for (int p=4; p<np; p++) {
        curCtrlPts.clear();
        curCtrlPts.push_back(newv3d( points[p*3-12], points[p*3-11], points[p*3-10] ));
        curCtrlPts.push_back(newv3d( points[p*3-9], points[p*3-8], points[p*3-7] ));
        curCtrlPts.push_back(newv3d( points[p*3-6], points[p*3-5], points[p*3-4] ));
        curCtrlPts.push_back(newv3d( points[p*3-3], points[p*3-2], points[p*3-1] ));
        curbez = new Bezier(nCtrl, curCtrlPts, nPts);
        point = curbez->getP(curBPt);
        points[p*3] = point.x;
        points[p*3+1] = point.y;
        points[p*3+2] = point.z; 
        //cout << points[p*3] << ", " << points[p*3+1] << ", " << points[p*3+2] << endl;
    }
    
    curBPt+=dir;
    if (curBPt>=nPts || curBPt<=0) dir*=-1;
}



/*
  Starfield
*/

void PSystem::starfield(int stars, float vel) {
    if (stars>free.size()) stars = free.size();
    for (int i=0; i<stars; i++) {
        int p = free.back();
        free.pop_back();
        isfree[p] = false;
        points[p*3] = random(-50, 50);
        points[p*3+1] = random(-50, 50);
        points[p*3+2] = -200;
        pcolors[p*4] = 1;
        pcolors[p*4+1] = 1;
        pcolors[p*4+2] = 1;
        pcolors[p*4+3] = 0.5;
    }
    
    for (int p=0; p<np; p++) {
        if (isfree[p]) continue;
        if (points[p*3+2] > -5) {
                free.push_back(p);
                isfree[p] = true;
                continue;
        }
        points[p*3+2] += vel;
    }
}





/*
   Fire
*/

void PSystem::burn(int num, v3d source, v3d normal) {
    if (num>free.size()) num = free.size();
    float shift=0.5;
    for (int i=0; i<num; i++) {
        int p = free.back();
        free.pop_back();
        isfree[p] = false;
        points[p*3] = source.x;
        points[p*3+1] = source.y;
        points[p*3+2] = source.z;
        vel[p].x = random((normal.x-shift)/100.0f, (normal.x+shift)/100.0f);
        vel[p].y = random((normal.y-shift)/100.0f, (normal.y+shift)/100.0f);
        vel[p].z = random((normal.z-shift)/100.0f, (normal.z+shift)/100.0f);
        pcolors[p*4] = random(0.75,1);
        pcolors[p*4+1] = random(0.1,0.5);
        pcolors[p*4+2] = random(0.1,0.5);
        pcolors[p*4+3] = 0.5;
    }
}

void PSystem::fire(v3d center, float r) {
     v3d wind = makev3d(random(-0.0005,0.0003),0,0);
     for (int p=0; p<r*100; p++) {
         burn(10, makev3d(random(-1*r,r)+center.x, center.y, random(-1*r,r)+center.z), makev3d(0,1,0));
     }
     for (int p=0; p<np; p++) {
         if (isfree[p]) continue;
         if ((pcolors[p*4]<=0) && 
            (pcolors[p*4+1]<=0) &&
            (pcolors[p*4+2])<=0) {
                free.push_back(p);
                isfree[p] = true;
                continue;
         }
         //pull up
         if (vel[p].x>0) vel[p].x-=0.0001;
         else if (vel[p].x<0) vel[p].x+=0.0001;
         if (vel[p].z>0) vel[p].z-=0.0001;
         else if (vel[p].z<0) vel[p].z+=0.0001;
         // cool and turn to smoke
         if (pcolors[p*4] > 0.1) pcolors[p*4] -= 0.01;
         if (pcolors[p*4+1] > 0.1) pcolors[p*4+1] -= 0.01;
         if (pcolors[p*4+2] > 0.1) pcolors[p*4+2] -= 0.01;
         if ( (pcolors[p*4] <= 0.1) && (pcolors[p*4+1] <= 0.1) && (pcolors[p*4+2] <= 0.1) ) {
              pcolors[p*4] -= 0.003;
              pcolors[p*4+1] -= 0.003;
              pcolors[p*4+2] -= 0.003;
         }
         //wind
         vel[p].x += wind.x;
         vel[p].y += wind.y;
         vel[p].z += wind.z;
         //move
         points[p*3] += vel[p].x + wind.x;
         points[p*3+1] += vel[p].y + wind.y;
         points[p*3+2] += vel[p].z + wind.z;
     }
}



/*
   Universe
*/

void PSystem::setUniverse(v3d center, v3d up, float r) {
     float x,y,z,v,ry;
     for (int i=0; i<np; i+=10) {   
      x = random(0,r) + center.x;
      ry = random(0, ( (-0.5*pow(x,2))/(0.5+pow(x,2)) + 0.5) ); //gives bell curve
      y = pow(-1, (int)(random(1,3)))  *  ry  +  center.y - random(0,ry/4);
      z =  center.z;
      v = random(0, (2 - x)) + 0.5;
      for (int p=i; p<i+10; p++) {
        points[p*3] = x + random(-0.0001,0.001);
        points[p*3+1] = y + random(-0.0001,0.001);
        points[p*3+2] = z + random(-0.0001,0.001);
        pcolors[p*4] = 0.5;
        pcolors[p*4+1] = 0.5;
        pcolors[p*4+2] = 0.5;
        pcolors[p*4+3] = 0.5;
        //vel[p].x =  v;//vel.x used as theta
        vel[p].x =  random(v-0.4, v+0.4);//vel.x used as theta
        vel[p].y = 0;
        vel[p].z = 0;
      }
     }
     //rotate to up
}

void PSystem::universe(v3d center, v3d up, float r) {
     v3d orig = center;
     v3d top = makev3d(center.x+up.x, center.y+up.y, center.z+up.z);
     for (int p=0; p<np; p++) {
        
        float th = vel[p].x;
        float s = sin((th*M_PI)/180);
    	float c = cos((th*M_PI)/180);
        
        float th1;
        if ((top.x-orig.x)==0) th1=-90;
    	else th1=-atan( ((top.y-orig.y)/(top.x-orig.x)) ) *180/M_PI;
        float s1 = sin((th1*M_PI)/180);
        float c1 = cos((th1*M_PI)/180);
        float xa2 = (top.x*c1) - (top.x*s1);
        float xb2 = (orig.x*c1) - (orig.y*s1);
        float th2;
        if ((top.z-orig.z)==0) th2=-90; 
        else th2=-atan( ((xa2-xb2)/(top.z-orig.z)) ) *180/M_PI;
        float s2 = sin((th2*M_PI)/180);
    	float c2 = cos((th2*M_PI)/180);
        float x,y,z;
        float sn1 = sin((-th1*M_PI)/180);
    	float cn1 = cos((-th1*M_PI)/180);
    	float sn2 = sin((-th2*M_PI)/180);
    	float cn2 = cos((-th2*M_PI)/180);
        
       
        
        
        points[p*3] -= orig.x;
        points[p*3+1] -= orig.y;
        points[p*3+2] -= orig.z;
        
        //rotz th1
        x=points[p*3];  y=points[p*3+1];
        points[p*3] = (x*c1) - (y*s1);
		points[p*3+1] = (x*s1) + (y*c1);
		//roty th2
		x=points[p*3]; z=points[p*3+2];
		points[p*3] = (z*s2) + (x*c2);
		points[p*3+2] = (z*c2) - (x*s2);
		//rotz th
		x=points[p*3];  y=points[p*3+1];
        points[p*3] = (x*c) - (y*s);
		points[p*3+1] = (x*s) + (y*c);
        //roty -th2
		x=points[p*3]; z=points[p*3+2];
		points[p*3] = (z*sn2) + (x*cn2);
		points[p*3+2] = (z*cn2) - (x*sn2);
		//rotz -th1
        x=points[p*3];  y=points[p*3+1];
        points[p*3] = (x*cn1) - (y*sn1);
		points[p*3+1] = (x*sn1) + (y*cn1);
    
        points[p*3] += orig.x;
        points[p*3+1] += orig.y;
        points[p*3+2] += orig.z;
    }
}



void PSystem::rise() {
     for (int p=0; p<np; p++) {
         points[p*3] += random(-0.0001,0.0001);
         points[p*3+1] += random(0.01,0.05);
         points[p*3+2] += random(-0.0001,0.0001);
     }
}



















/***********************
*** Transoformations ***
***********************/


void PSystem::resetMatrix() {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            trans[i][j]=0;
        }
    }
    trans[0][0]=1;
    trans[1][1]=1;
    trans[2][2]=1;
    trans[3][3]=1;
}

//multiplies transformation matrix by points
void PSystem::multMatrix() {
    float x,y,z;
    for (int i=0; i<np; i++) {
		x = (points[i*3]*trans[0][0]) + (points[i*3+1]*trans[0][1]) + (points[i*3+2]*trans[0][2]) + trans[0][3];
		y = (points[i*3]*trans[1][0]) + (points[i*3+1]*trans[1][1]) + (points[i*3+2]*trans[1][2]) + trans[1][3];
		z = (points[i*3]*trans[2][0]) + (points[i*3+1]*trans[2][1]) + (points[i*3+2]*trans[2][2]) + trans[2][3];
		points[i*3] = x;
		points[i*3+1] = y;
		points[i*3+2] = z;
	}
}

//combines new transformation matrix with old one
void PSystem::combine(float newt[4][4]) {
    float temp[4];
    for (int j=0; j<4; j++) {
        for (int i=0; i<4; i++) {
            temp[i] = (trans[0][j]*newt[i][0]) + (trans[1][j]*newt[i][1]) + (trans[2][j]*newt[i][2]) + (trans[3][j]*newt[i][3]);
        }
        for (int i=0; i<4; i++) {
            trans[i][j] = temp[i];
        }
    }
}


void PSystem::translate(float tx, float ty, float tz) {
    float temp[4][4] = {
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {tx,ty,tz,1}
        };
    combine(temp);
}


void PSystem::move(float x, float y, float z) {
    translate(-1*points[0], -1*points[1], -1*points[2]);
    translate(x,y,z);
}


void PSystem::scale(float sx, float sy, float sz, float fx, float fy, float fz) {
	translate(-fx, -fy, -fz);
	float temp[4][4] = {
        {sx,0,0,0},
        {0,sy,0,0},
        {0,0,sz,0},
        {0,0,0,1}
        };
    combine(temp);
	translate(fx, fy, fz);
}

void PSystem::rotx(float th, float yc, float zc) {
	float s = sin((th*M_PI)/180);
	float c = cos((th*M_PI)/180);
	float y,z;
	translate(0, -yc, -zc);
	for (int i=0; i<np; i++) {
		y = points[i*3+1];
		z = points[i*3+2];
		points[i*3+1] = (y*c) - (z*s);
		points[i*3+2] = (y*s) + (z*c);
	}
	translate(0, yc, zc);
}

void PSystem::roty(float th, float xc, float zc){
	float s = sin((th*M_PI)/180);
	float c = cos((th*M_PI)/180);
	float x,z;
	translate(-xc, 0, -zc);
	for (int i=0; i<np; i++) {
		x = points[i*3];
		z = points[i*3+2];
		points[i*3] = (z*s) + (x*c);
		points[i*3+2] = (z*c) - (x*s);
	}
	translate(xc, 0, zc);
}


void PSystem::rotz(float th, float xc, float yc){
	float s = sin((th*M_PI)/180);
	float c = cos((th*M_PI)/180);
	float x,y;
	translate(-xc, -yc, 0);
	for (int i=0; i<np; i++) {
		x = points[i*3];
		y = points[i*3+1];
		points[i*3] = (x*c) - (y*s);
		points[i*3+1] = (x*s) + (y*c);
	}
	translate(xc, yc, 0);
}


void PSystem::rotab(float th, float xb, float yb, float zb, float xa, float ya, float za) {
	translate(-xb, -yb, -zb);
	float th1;
	if ((xa-xb)==0) th1=-90;
	else th1=-atan( ((ya-yb)/(xa-xb)) ) *180/M_PI;
	float s = sin((th1*M_PI)/180);
	float c = cos((th1*M_PI)/180);
	float xa2 = (xa*c) - (ya*s);
	float xb2 = (xb*c) - (yb*s);
	float th2;
	if ((za-zb)==0) th2=-90; 
	else th2=-atan( ((xa2-xb2)/(za-zb)) ) *180/M_PI;
	//cout << "rotab" << endl;
	//cout << "th1: " << th1 << endl;
	//cout << "th2: " << th2 << endl;
	rotz(th1,0,0);
	roty(th2,0,0);
	rotz(th,0,0);
	roty(-th2,0,0);
	rotz(-th1,0,0);
	translate(xb, yb, zb);
}


void PSystem::zshx(float m) {
	float x;
	for (int i=0; i<np; i++) {
		x = points[i*3];
		points[i*3] = x + (points[i*3+1]/m);
	}
	//glutSetWindow(getWindow());
	glutPostRedisplay();
}


void PSystem::zshy(float m){
	float y;
	for (int i=0; i<np; i++) {
		y = points[i*3+1];
		points[i*3+1] = y + (points[i*3]/m);
	}
	//glutSetWindow(getWindow());
	glutPostRedisplay();
}

void PSystem::xshy(float m){
	float y;
	for (int i=0; i<np; i++) {
		y = points[i*3+1];
		points[i*3+1] = y + (points[i*3+2]/m);
	}
	//glutSetWindow(getWindow());
	glutPostRedisplay();
}

void PSystem::xshz(float m){
	float z;
	for (int i=0; i<np; i++) {
		z = points[i*3+2];
		points[i*3+2] = z + (points[i*3+1]/m);
	}
	//glutSetWindow(getWindow());
	glutPostRedisplay();
}

void PSystem::yshx(float m){
	float x;
	for (int i=0; i<np; i++) {
		x = points[i*3];
		points[i*3] = x + (points[i*3+2]/m);
	}
	//glutSetWindow(getWindow());
	glutPostRedisplay();
}


void PSystem::yshz(float m){
	float z;
	for (int i=0; i<np; i++) {
		z = points[i*3+2];
		points[i*3+2] = z + (points[i*3]/m);
	}
	//glutSetWindow(getWindow());
	glutPostRedisplay();
}







/*********************
    Texture loading from NeHe OpenGL tutorial lesson 6 http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=06
**********************/



AUX_RGBImageRec *LoadBMP(char *Filename)				// Loads A Bitmap Image
{
	FILE *File=NULL;									// File Handle

	if (!Filename)										// Make Sure A Filename Was Given
	{
		return NULL;									// If Not Return NULL
	}

	File=fopen(Filename,"r");							// Check To See If The File Exists

	if (File)											// Does The File Exist?
	{
		fclose(File);									// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;										// If Load Failed Return NULL
}

int PSystem::LoadGLTextures()									// Load Bitmaps And Convert To Textures
{
	int Status=FALSE;									// Status Indicator

	AUX_RGBImageRec *TextureImage[1];					// Create Storage Space For The Texture

	memset(TextureImage,0,sizeof(void *)*1);           	// Set The Pointer To NULL

	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	if (TextureImage[0]=LoadBMP("./particle.bmp"))
	{
		Status=TRUE;									// Set The Status To TRUE

		glGenTextures(1, &texture);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}

	if (TextureImage[0])									// If Texture Exists
	{
		if (TextureImage[0]->data)							// If Texture Image Exists
		{
            delete TextureImage[0]->data;//free(TextureImage[0]->data);                            // Free The Texture Image Memory 
		}

		//delete TextureImage;//free(TextureImage[0]);								// Free The Image Structure
	}

	return Status;										// Return The Status
}




