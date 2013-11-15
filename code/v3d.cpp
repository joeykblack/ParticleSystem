#include "v3d.h"
#include <math.h>
#include <iostream>

using namespace std;


float dot(v3d v1, v3d v2) {
    return ((v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z));// / (mag(v1) * mag(v2));
}


v3d cros(v3d v1, v3d v2) {
    v3d v3;
    v3.x = (v1.y*v2.z) - (v1.z*v2.y);
    v3.y = (v1.z*v2.x) - (v1.x*v2.z);
    v3.z = (v1.x*v2.y) - (v1.y*v2.x);
    return v3;
}

v3d makev3d(float x1, float y1, float z1, float x2, float y2, float z2) {
    v3d v;
    v.x = x2 - x1;
    v.y = y2 - y1;
    v.z = z2 - z1;
    return v;
}

v3d makev3d(float x, float y, float z) {
    v3d v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

v3d* newv3d(float x, float y, float z) {
    v3d* v = new v3d;
    v->x = x;
    v->y = y;
    v->z = z;
    return v;
}
v3d normalize(v3d v) {
    float m = mag(v);
    return makev3d(v.x/m, v.y/m, v.z/m);
}

bool vis(v3d n, v3d s) {
     return dot(n, s) > 0;
}

v3d calcRu(v3d Nu, v3d Lu) {
    v3d v;
    v.x = Nu.x; v.y = Nu.y; v.z = Nu.z; 
    float d = dot(Nu, Lu);
    v.x *= 2*d; v.y *= 2*d; v.z *= 2*d;
    v.x -= Lu.x; v.y -= Lu.y; v.z -= Lu.z;
    v=normalize(v);
    return v;
}

float mag(v3d v) {
    return sqrt((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}

v3d difv3d(v3d v1, v3d v2) {
    v3d v;
    v.x = v1.x - v2.x;
    v.y = v1.y - v2.y;
    v.z = v1.z - v2.z;
    return v;
}

v3d scalMult(float a, v3d v1) {
    v3d v;
    v.x = v1.x * a;
    v.y = v1.y * a;
    v.z = v1.z * a;
    return v;
}

void print(v3d v) {
    cout << "<" << v.x << ", " << v.y << ", " << v.z << ">" << endl;
}

v3d refr(v3d Vu, v3d Nu, float ni, float nr) {
    float costhi = dot(scalMult(-1,Vu), Nu);
    float b = ni/nr;
    float costhr = sqrt(1 - (pow(b,2)*(1-pow(costhi,2))));
    float a = costhr - (b*costhi);
    return difv3d(scalMult(b,Vu), scalMult(a,Nu));
}




