#pragma once
#include "dim.h"
class tensor{
public:
    static double * create(int dm1);
    static double ** create(int dm1,int dm2);
    static double *** create(int dm1,int dm2,int dm3);
    static double **** create(int dm1,int dm2,int dm3,int dm4);
    static double **** create(dim dm);
    static void del(double * a,int dm1);
    static void del(double ** a,int dm1,int dm2);
    static void del(double *** a,int dm1,int dm2,int dm3);
    static void del(double **** a,int dm1,int dm2,int dm3,int dm4);
    static void del(double **** a,dim dm);
private:
    tensor();
    ~tensor();
};

double * tensor::create(int dm1){
    double * res=new double [dm1];
    return res;
}
double ** tensor::create(int dm1,int dm2){
    double ** res=new double * [dm1];
    for(int i=0;i<dm1;i++)
        res[i]=new double [dm2];
    return res;
}

double *** tensor::create(int dm1,int dm2,int dm3){
    double *** res=new double ** [dm1];
    for(int i=0;i<dm1;i++)
        res[i]=create(dm2,dm3);
    return res;
}

double **** tensor::create(int dm1,int dm2,int dm3,int dm4){
    double **** res=new double*** [dm1];
    for(int i=0;i<dm1;i++)
        res[i]=create(dm2,dm3,dm4);
    return res;
}

double **** tensor::create(dim dm){
    return create(dm.dim1,dm.dim2,dm.dim3,dm.dim4);
}

void tensor::del(double * a,int dm1){
    delete [] a;
}
void tensor::del(double ** a,int dm1,int dm2){
    for(int i=0;i<dm1;i++){
        delete [] a[i];
    }
    delete [] a;
}

void tensor::del(double *** a,int dm1,int dm2,int dm3){
    for(int i=0;i<dm1;i++){
        del(a[i],dm2,dm3);
    }
    delete [] a;
}

void tensor::del(double **** a,int dm1,int dm2,int dm3,int dm4){
    for(int i=0;i<dm1;i++){
        del(a[i],dm2,dm3,dm4);
    }
    delete [] a;
}
void tensor::del(double **** a,dim dm){
    del(a,dm.dim1,dm.dim2,dm.dim3,dm.dim4);
}
