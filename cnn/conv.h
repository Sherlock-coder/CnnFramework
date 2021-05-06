#pragma once
#include "dim.h"

//  we define the notation as follows:
//  In:input tensor(after activation);  Out:output tensor(after activation);
//  Ein:input error(the same size as Out);  Eout:output error(the same size as In);
//  W:convolutional kernel;    B:the bias unit;    outSize:the size of Out and Ein;    kernelSize:the size of W
class conv{
public:
    conv(double ****In, double ****Out, double ****Ein, double ****Eout, double ****W, double *B, dim outSize, dim kernelSize);
    void ff();
    void bp();
    void updatew();
    void updateb();
    virtual double atv(double x); //the activation function for the convolutional layer
    //default=tanh,you can change it through inheritance
    virtual double datv(double x); // the derivative activation function for the previous layer,In->x
    //default=(y=x)'=y=1,you can change it through inheritance as well
    void changeLearningRate(double x);
protected:
    double ****in;
    double ****out;
    double ****ein;
    double ****eout;
    double ****w;
    double *b;
    int batch_size, dm1, dm2, dm3;
    int kn, kdm1, kdm2, kdm3;
    double alpha = 0.001; //learning rate
protected:
    double val(double **ts, int i, int j, int x, int y);
};

conv::conv(double ****In, double ****Out, double ****Ein, double ****Eout, double ****W, double *B, dim outSize, dim kernelSize) : in(In), out(Out), ein(Ein), eout(Eout), w(W), b(B), batch_size(outSize.dim1), dm1(outSize.dim2), dm2(outSize.dim3), dm3(outSize.dim4), kn(kernelSize.dim1), kdm1(kernelSize.dim2), kdm2(kernelSize.dim3), kdm3(kernelSize.dim4) {}

/*  given mtx[x][x],padding mtx with dimension y with value 0
    as a result,mtx_size=(x+2*y,x+2*y)
    now we need to find the value of the new mtx with the new index (i,j)
*/
double conv::val(double **mtx, int i, int j, int x, int y){ //(i,j)=position after padding;  x=original size;   y=padding size
    int reali = i - y, realj = j - y;
    if (reali >= 0 && reali < x && realj >= 0 && realj < x)
        return mtx[reali][realj];
    else
        return 0;
}

double conv::atv(double x){
    return tanh(x);
}

double conv::datv(double x){
    return 1;
}

void conv::ff(){
    for (int index = 0; index < batch_size; index++)
    {
        for (int k = 0; k < dm1; k++)
        { //the index of filters
            for (int i = 0; i < dm2; i++)
                for (int j = 0; j < dm3; j++)
                {
                    double tmp = 0;
                    for (int tmpk = 0; tmpk < kdm1; tmpk++) //the index of channels
                        for (int tmpi = 0; tmpi < kdm2; tmpi++)
                            for (int tmpj = 0; tmpj < kdm3; tmpj++)
                            {
                                tmp += in[index][tmpk][i + tmpi][j + tmpj] * w[k][tmpk][tmpi][tmpj];
                            }
                    out[index][k][i][j] = atv(tmp + b[k]);
                }
        }
    }
}

void conv::bp(){
    int h = dm2 + kdm2 - 1, d = dm3 + kdm3 - 1;
    for (int index = 0; index < batch_size; index++)
    {
        for (int k = 0; k < kdm1; k++) //k=index of output errors and input channels
            for (int i = 0; i < h; i++)
                for (int j = 0; j < d; j++)
                {
                    double tmp = 0;
                    for (int tmpk = 0; tmpk < kn; tmpk++)
                    { //tmpk=index of filters
                        for (int tmpi = 0; tmpi < kdm2; tmpi++)
                            for (int tmpj = 0; tmpj < kdm3; tmpj++)
                            {
                                tmp += w[tmpk][k][kdm2 - 1 - tmpi][kdm3 - 1 - tmpj] * val(ein[index][tmpk], i + tmpi, j + tmpj, dm2, kdm2 - 1);
                            }
                    }
                    eout[index][k][i][j] = tmp * datv(in[index][k][i][j]);
                }
    }
}

void conv::updatew(){
    int h = dm2 + kdm2 - 1, d = dm3 + kdm3 - 1;
    for (int index = 0; index < batch_size; index++)
    {
        for (int k = 0; k < dm1; k++)         //index of filters
            for (int ic = 0; ic < kdm1; ic++) //index of input channels
                for (int i = 0; i < kdm2; i++)
                    for (int j = 0; j < kdm3; j++)
                    {
                        double tmp = 0;
                        for (int tmpi = 0; tmpi < dm2; tmpi++)
                            for (int tmpj = 0; tmpj < dm3; tmpj++)
                                tmp += in[index][ic][i + tmpi][j + tmpj] * ein[index][k][tmpi][tmpj];
                        w[k][ic][i][j] -= alpha * tmp / batch_size;
                    }
    }
}

void conv::updateb(){
    for (int k = 0; k < dm1; k++)
    {
        double tmp = 0;
        for (int index = 0; index < batch_size; index++)
        {
            for (int i = 0; i < dm2; i++)
                for (int j = 0; j < dm3; j++)
                    tmp += ein[index][k][i][j];
        }
        b[k] -= alpha * tmp / batch_size;
    }
}

void conv::changeLearningRate(double x){
    alpha=x;
}