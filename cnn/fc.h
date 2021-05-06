#pragma once
#include "dim.h"
#include <cmath>
#include <cfloat>

//we define the notation as follows
//In:the input tensor(after activation);  Out:the output tensor(after activation);
//Ein:the input error(the same size as Out);    Eout:the output error(the same size as In);
//W:the weight tensor   B:the bias unit  bacthSize:the batch size;
//inSize:the size of In and Eout(after vectorization);  outSize:the size of Out and Ein
class fc
{
public:
    fc(double **In, double **Out, double **Ein, double **Eout, double **W, double *B, int batchSize, int inSize, int outSize);
    void ff();
    void bp();
    void updatew();
    void updateb();
    void changeLearningRate(double x);
protected:
    virtual void atv(double *vec, int dm); //the activation function for the full connected layer,w*In+b->vec,outSize->b
    //default=softmax,inheritance is supported
    virtual double datv(double x); //the derivative activation function for the previous function,In->x
    //default=(tanh)'=1-x*x;
protected:
    double **in;
    double **out;
    double **ein;
    double **eout;
    double **w;
    double *b;
    double alpha = 0.001;
    int batchsize, insize, outsize;
};

fc::fc(double **In, double **Out, double **Ein, double **Eout, double **W, double *B, int batchSize, int inSize, int outSize) : in(In), out(Out), ein(Ein), eout(Eout), w(W), b(B), batchsize(batchSize), insize(inSize), outsize(outSize) {}

void fc::atv(double *vec, int dm){
    double maxEntry = -DBL_MAX;
    for (int i = 0; i < dm; i++)
        maxEntry = std::max(vec[i], maxEntry);
    double tmp = 0;
    for (int i = 0; i < dm; i++)
        tmp += exp(vec[i] - maxEntry);
    for (int i = 0; i < dm; i++)
        vec[i] = exp(vec[i] - maxEntry) / tmp;
}

double fc::datv(double x){
    return 1 - x * x;
}

void fc::ff(){
    for (int index = 0; index < batchsize; index++)
    {
        for (int i = 0; i < outsize; i++)
        {
            out[index][i] = 0;
            for (int j = 0; j < insize; j++)
                out[index][i] += w[i][j] * in[index][j];
            out[index][i] += b[i];
        }
        atv(out[index], outsize);
    }
}

void fc::bp(){
    for (int index = 0; index < batchsize; index++)
    {
        for (int i = 0; i < insize; i++)
        {
            eout[index][i] = 0;
            for (int j = 0; j < outsize; j++)
                eout[index][i] += w[j][i] * ein[index][j];
            eout[index][i] *= datv(in[index][i]);
        }
    }
}

void fc::updatew(){
    for (int index = 0; index < batchsize; index++)
    {
        for (int i = 0; i < outsize; i++)
            for (int j = 0; j < insize; j++)
                w[i][j] -= alpha * ein[index][i] * in[index][j] / batchsize;
    }
}

void fc::updateb(){
    for (int index = 0; index < batchsize; index++)
        for (int i = 0; i < outsize; i++)
            b[i] -= alpha * ein[index][i] / batchsize;
}

void fc::changeLearningRate(double x){
    alpha=x;
}