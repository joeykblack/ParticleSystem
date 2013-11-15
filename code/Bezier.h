#ifndef BEZIER_H_
#define BEZIER_H_
 
#include <windows.h>
#include <vector>
#include <iostream>
#include "v3d.h"
using namespace std;

class Bezier {
public:
    Bezier(int nCtrl, vector<v3d*> p, int nPts);
    ~Bezier();
    v3d getP(int i);
    v3d* next();
    int getNPts();
    
private:
    void binomialCoeffs(int n, int * C);
    
    int * C;
    int nCtrl;
    vector<v3d*> points;
    int nPts;
    int cur;
};


#endif /*BEZIER_H_*/
