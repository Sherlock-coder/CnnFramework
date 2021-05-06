#pragma once 

class dim{
public:
    dim(int Dim1,int Dim2,int Dim3=0,int Dim4=0):
        dim1(Dim1),dim2(Dim2),dim3(Dim3),dim4(Dim4){}
    int dim1,dim2,dim3,dim4;
};