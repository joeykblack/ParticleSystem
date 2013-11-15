#ifndef V3D_H
#define V3D_H

struct v3d {
       float x;
       float y;
       float z;
} ;

float dot(v3d v1, v3d v2);
v3d cros(v3d v1, v3d v2);
v3d makev3d(float x1, float y1, float z1, float x2, float y2, float z2);
v3d makev3d(float x, float y, float z);
v3d* newv3d(float x, float y, float z);
bool vis(v3d n, v3d s);
v3d calcRu(v3d Nu, v3d Lu);
v3d normalize(v3d v);
float mag(v3d v);
v3d difv3d(v3d v1, v3d v2);
v3d scalMult(float x, v3d v);
void print(v3d v);
v3d refr(v3d Vu, v3d Nu, float ni, float nr);

#endif //V3D_H
