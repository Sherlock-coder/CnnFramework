#pragma once
#include "dim.h"
#include <cmath>
#include <cstdlib>

class func{
public:
    static double sigmoid(double x);
    static double dsigmoid(double z);
    static double relu(double x);
    static double drelu(double x);
    static double random(double epsilon);
    static void assignRandom(double ** a,dim dm);
    static void assignRandom(double **** a,dim dm);
    static void unfold(double **** a,double ** res,dim dm);
    static void fold(double **** res,double ** a,dim dm);
    static int argmax(double *a,int dm);
    static void assign(double val, double* a, int dm1);
    static void assign(double val, double**** a, dim  dm);
};

double func::sigmoid(double x){
    return 1/(1+exp(-x));
}

double func::dsigmoid(double z){
    return z*(1-z);
}

double func::relu(double x){
    return x>0?x:0;
}

double func::drelu(double x){
    return x>0?1:0;
}

double func::random(double epsilon){
    return 2*epsilon*rand()/RAND_MAX-epsilon;
}

void func::assignRandom(double **** a,dim dm){
    for(int i=0;i<dm.dim1;i++)
        for(int j=0;j<dm.dim2;j++)
            for(int k=0;k<dm.dim3;k++)
                for(int s=0;s<dm.dim4;s++)
                    a[i][j][k][s]=random(1);
}

void func::assignRandom(double ** a,dim dm){
    for(int i=0;i<dm.dim1;i++)
        for(int j=0;j<dm.dim2;j++)
            a[i][j]=random(1);
}
void func::unfold(double **** a,double ** res,dim dm){
    for(int i=0;i<dm.dim1;i++){
        int index=0;
        for(int tmpi=0;tmpi<dm.dim2;tmpi++)
            for(int tmpj=0;tmpj<dm.dim3;tmpj++)
                for(int tmpk=0;tmpk<dm.dim4;tmpk++)
                    res[i][index++]=a[i][tmpi][tmpj][tmpk];
    }
}

void func::fold(double **** res,double ** a,dim dm){
    for(int i=0;i<dm.dim1;i++){
        int index=0;
        for(int tmpi=0;tmpi<dm.dim2;tmpi++)
            for(int tmpj=0;tmpj<dm.dim3;tmpj++)
                for(int tmpk=0;tmpk<dm.dim4;tmpk++)
                    res[i][tmpi][tmpj][tmpk]=a[i][index++];
    }
}

int func::argmax(double *a,int dm){
    int res=0;
    for(int i=0;i<dm;i++)
        if(a[i]>a[res]) res=i;
    return res;
}

void func::assign(double val, double* a, int dm1) {
    for (int i = 0; i < dm1; i++)
        a[i] = val;
}

void func::assign(double val, double**** a, dim dm) {
    for (int index = 0; index < dm.dim1; index++)
        for (int k = 0; k < dm.dim2; k++)
            for (int i = 0; i < dm.dim3; i++)
                for (int j = 0; j < dm.dim4; j++)
                    a[index][k][i][j] = val;
}