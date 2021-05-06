#pragma once
#include "cnn.h"
//change the activation function of the full connected layer and derivative activation of the previous layer
class fc1:public fc{
public:
    fc1(double **In, double **Out, double **Ein, double **Eout, double **W, double *B, int batchSize, int inSize, int outSize);
protected:
    virtual void atv(double *vec, int dm);
    virtual double datv(double x);
};

fc1::fc1(double **In, double **Out, double **Ein, double **Eout, double **W, double *B, int batchSize, int inSize, int outSize):fc(In,Out,Ein,Eout,W,B,batchSize,inSize,outSize){}

void fc1::atv(double *vec,int dm){
    for(int i=0;i<dm;i++)
        vec[i]=tanh(vec[i]);
}

double fc1::datv(double x){
    return 1;
}
