#include "Bezier.h"
#include <math.h>
#include <stdlib.h>


Bezier::Bezier(int nCtrl, vector<v3d*> p, int nPts):
    nCtrl(nCtrl), points(p), nPts(nPts), cur(0)
{
    C = new int[nCtrl];
    binomialCoeffs(nCtrl-1, C);
}

Bezier::~Bezier() {
    delete[] C;
    points.clear();
}


void Bezier::binomialCoeffs(int n, int * C) {
    int k, j;
    
    for (k=0; k<=n; k++) {
        C[k] = 1;
        for (j=n; j>=k+1; j--) C[k] *= j;
        for (j=n-k; j>=2; j--) C[k] /= j;
    }
}



v3d Bezier::getP(int i) {
    v3d p = makev3d(0,0,0);
    int k;
    int n = nCtrl - 1;
    float bezBlendFcn;
    float u = (float)i / (float)nPts;
    
    for (k=0; k<nCtrl; k++) {
        bezBlendFcn = C[k] * pow(u, k) * pow(1-u, n-k);
        p.x += points[k]->x * bezBlendFcn;
        p.y += points[k]->y * bezBlendFcn;
        p.z += points[k]->z * bezBlendFcn;
    }
    
    return p;
}

int Bezier::getNPts() {
    return nPts;
}


v3d* Bezier::next() {
    v3d* p = new v3d;
    int k;
    int n = nCtrl - 1;
    float bezBlendFcn;
    float u = (float)cur / (float)nPts;
    
    for (k=0; k<nCtrl; k++) {
        bezBlendFcn = C[k] * pow(u, k) * pow(1-u, n-k);
        p->x += points[k]->x * bezBlendFcn;
        p->y += points[k]->y * bezBlendFcn;
        p->z += points[k]->z * bezBlendFcn;
    }
    
    cur++;
    if (cur >= nPts) cur=0;
    
    return p;
}





