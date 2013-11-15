#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "v3d.h"
#include "Bezier.h"
#include <GL/GL.h>
#include <vector>
using namespace std;

class PSystem {
public:
       PSystem();
       ~PSystem();
       void draw();
       void update();
       
       void resetMatrix();
       void multMatrix();
       void translate(float tx, float ty, float tz);
	   void move(float x, float y, float z);
       void scale(float sx, float sy, float sz, float fx, float fy, float fz);
	   void rotx(float th, float yc, float zc);
	   void roty(float th, float xc, float zc);
	   void rotz(float th, float xc, float yc);
	   void rotab(float th, float xb, float yb, float zb, float xa, float ya, float za);
	   void zshx(float m);
	   void zshy(float m);
	   void xshy(float m);
	   void xshz(float m);
	   void yshx(float m);
	   void yshz(float m);
	   
	   void ignite(int num, v3d source, v3d normal);
	   void move();
	   void cool();
	   void accelerate(v3d dir, float a);
	   void clearps();
	   void fountain();
	   void setCyclone(v3d orig, v3d top, float diamOrig, float diamTop);
	   void cyclone(v3d orig, v3d top, float diamOrig, float diamTop);
	   void explode(int num, v3d source);
	   void expand();
	   void setBezier(int nc, vector<v3d*> baseP, int nPts);
	   void bezierMove();
	   void starfield(int stars, float vel);
	   void burn(int num, v3d source, v3d normal);
	   void fire(v3d center, float r);
	   void setUniverse(v3d center, v3d up, float r);
	   void universe(v3d center, v3d up, float r);
	   void rise();
	   
	   void changeMode(int newm);
	   void changePointType(char pt);
	   void setMouseState(int state);
	   void setMousePos(float x, float y);
       
private:      
      int np;
      GLfloat * points;
      GLuint vboId;  
      vector<int> free;
      bool * isfree;
      GLfloat * pcolors;
      v3d * vel;
      float random(float min, float max);
      bool isFree(int p);
      float trans[4][4];
      void combine(float newt[4][4]);
      int LoadGLTextures();
};

#endif //PARTICLESYSTEM_H

