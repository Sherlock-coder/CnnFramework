#pragma once
#include <algorithm>
#include "dim.h"
#include "func.h"

//we define the notation as follows
//In:input tensor(after activation);    A:output tensor(after activation)
//Ein:input error(the same size as A)   Eout:output error(the same size of In)
//Psize:the size of pooling windows, only stride same as Psize can be supported;    inputSize:the size of In and Eout
class pool{
public:
    pool(double ****In, double ****A, double ****Ein, double ****Eout, int Psize, dim inputSize);
    void ff();
    void bp();

protected:
    double datv(double x); //the derivative activation function for the previous layer,In->x
    //default=(tanh)'=1-y*y,inheritance is supported
protected:
    double ****in;
    double ****a;
    double ****ein;
    double ****eout;
    int psize;
    int batch_size, dm1, dm2, dm3; //input size
};

pool::pool(double ****In, double ****A, double ****Ein, double ****Eout, int Psize, dim inputSize) : in(In), a(A), ein(Ein), eout(Eout), psize(Psize), batch_size(inputSize.dim1), dm1(inputSize.dim2), dm2(inputSize.dim3), dm3(inputSize.dim4) {}

void pool::ff(){
    func::assign(0, eout, dim(batch_size, dm1, dm2, dm3));
    int posi = 0, posj = 0;
    for (int index = 0; index < batch_size; index++)
        for (int k = 0; k < dm1; k++)
        {
            int idi = 0, idj = 0;
            for (int i = 0; i < dm2; i += psize)
            {
                for (int j = 0; j < dm3; j += psize)
                {
                    double tmp = -1;
                    for (int tmpi = 0; tmpi < psize; tmpi++)
                        for (int tmpj = 0; tmpj < psize; tmpj++)
                            if (in[index][k][i + tmpi][j + tmpj] > tmp)
                            {
                                tmp = in[index][k][i + tmpi][j + tmpj];
                                posi = i + tmpi;
                                posj = j + tmpj;
                            }
                    a[index][k][idi][idj++] = tmp;
                    eout[index][k][posi][posj] = 1;
                }
                idi++;
                idj = 0;
            }
        }
}

void pool::bp(){
    for (int index = 0; index < batch_size; index++)
        for (int k = 0; k < dm1; k++)
        {
            int idi = 0, idj = 0;
            for (int i = 0; i < dm2; i += psize)
            {
                for (int j = 0; j < dm3; j += psize)
                    for (int tmpi = 0; tmpi < psize; tmpi++)
                        for (int tmpj = 0; tmpj < psize; tmpj++) //traversal all eout[index][k][i][j] in the order of blocks
                            if (eout[index][k][i + tmpi][j + tmpj])
                            {
                                eout[index][k][i + tmpi][j + tmpj] = ein[index][k][idi][idj++];
                            }
                idi++;
                idj = 0;
            }
        }
    for (int index = 0; index < batch_size; index++)
        for (int k = 0; k < dm1; k++)
            for (int i = 0; i < dm2; i++)
                for (int j = 0; j < dm3; j++)
                    eout[index][k][i][j] *= datv(in[index][k][i][j]);
}

double pool ::datv(double x){
    return 1 - x * x;
}