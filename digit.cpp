#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <cstdlib>
#include "cnn.h"
#include "ImagePreprocess.hpp"

using namespace std;
using namespace Ros;

const int batchSize=100;
const int Epoch=10;

//define size
dim inputSize(batchSize,1,28,28);
dim cwSize1(6,1,5,5);
dim coutSize1(batchSize,6,24,24);
dim poutSize1(batchSize,6,12,12);
dim cwSize2(16,6,5,5);
dim coutSize2(batchSize,16,8,8);
dim poutSize2(batchSize,16,4,4);
dim cwSize3(120,16,4,4);
dim coutSize3(batchSize,120,1,1);
int finSize1=120;
int foutSize1=10;

//get space
//input layer
static double ****  input=tensor::create(inputSize);
static double ****  inputerror=tensor::create(inputSize);
//convolution layer 1
static double **** cw1=tensor::create(cwSize1);
static double * cb1=tensor::create(cwSize1.dim1);
static double **** cout1=tensor::create(coutSize1);
static double **** cerror1=tensor::create(coutSize1);
//pooling layer 1
static double **** pout1=tensor::create(poutSize1);
static double **** perror1=tensor::create(poutSize1);
//convolution layer 2
static double **** cw2=tensor::create(cwSize2);
static double * cb2=tensor::create(cwSize2.dim1);
static double **** cout2=tensor::create(coutSize2);
static double **** cerror2=tensor::create(coutSize2);
//pooling layer 2
static double **** pout2=tensor::create(poutSize2);
static double **** perror2=tensor::create(poutSize2);
//convolutio layer 3
static double **** cw3=tensor::create(cwSize3);
static double * cb3=tensor::create(cwSize3.dim1);
static double **** cout3=tensor::create(coutSize3);
static double **** cerror3=tensor::create(coutSize3);
//flatten
//cout3->fin fe->cerror3
static double ** fin=tensor::create(batchSize,finSize1);
static double ** fe=tensor::create(batchSize,finSize1);
//full connected layer
static double ** fw1=tensor::create(foutSize1,finSize1);
static double * fb1=tensor::create(foutSize1);
static double ** fout1=tensor::create(batchSize,foutSize1);
static double ** ferror1=tensor::create(batchSize,foutSize1);
//label
static double * label=tensor::create(batchSize);

//instantiate layers
conv c1(input,cout1,cerror1,inputerror,cw1,cb1,coutSize1,cwSize1);
pool p1(cout1,pout1,perror1,cerror1,2,coutSize1);
conv c2(pout1,cout2,cerror2,perror1,cw2,cb2,coutSize2,cwSize2);
pool p2(cout2,pout2,perror2,cerror2,2,coutSize2);
conv c3(pout2,cout3,cerror3,perror2,cw3,cb3,coutSize3,cwSize3);
fc f1(fin,fout1,ferror1,fe,fw1,fb1,batchSize,finSize1,foutSize1);

void init(){
    func::assign(0,cb1,cwSize1.dim1);
    func::assign(0,cb2,cwSize2.dim1);
    func::assign(0,cb3,cwSize3.dim1);
    func::assign(0,fb1,foutSize1);
    srand((unsigned int)time(NULL));
    func::assignRandom(cw1,cwSize1);
    func::assignRandom(cw2,cwSize2);
    func::assignRandom(cw3,cwSize3);
    func::assignRandom(fw1,dim(foutSize1,finSize1));
}

void ff(){
    c1.ff();
    p1.ff();
    c2.ff();
    p2.ff();
    c3.ff();
    func::unfold(cout3,fin,coutSize3);
    f1.ff();
}

void bp(){
    for(int index=0;index<batchSize;index++)
        for(int i=0;i<10;i++)
            ferror1[index][i]=fout1[index][i]-(label[index]==i?1:0);
    f1.bp();
    func::fold(cerror3,fe,coutSize3);
    c3.bp();
    p2.bp();
    c2.bp();
    p1.bp();
}

void update(){
    c1.updatew();
    c1.updateb();
    c2.updatew();
    c2.updateb();
    c3.updatew();
    c3.updateb();
    f1.updatew();
    f1.updateb();
}

void del(){
    tensor::del(input,inputSize);
    tensor::del(cw1,cwSize1);
    tensor::del(cb1,cwSize1.dim1);
    tensor::del(cout1,coutSize1);
    tensor::del(cerror1,coutSize1);
    tensor::del(pout1,poutSize1);
    tensor::del(perror1,poutSize1);
    tensor::del(cw2,cwSize2);
    tensor::del(cb2,cwSize2.dim1);
    tensor::del(cout2,coutSize2);
    tensor::del(cerror2,coutSize2);
    tensor::del(pout2,poutSize2);
    tensor::del(perror2,poutSize2);
    tensor::del(cw3,cwSize3);
    tensor::del(cb3,cwSize3.dim1);
    tensor::del(cout3,coutSize3);
    tensor::del(cerror3,coutSize3);
    tensor::del(fin,batchSize,finSize1);
    tensor::del(fe,batchSize,finSize1);
    tensor::del(fw1,foutSize1,finSize1);
    tensor::del(fb1,foutSize1);
    tensor::del(fout1,batchSize,foutSize1);
    tensor::del(ferror1,batchSize,foutSize1);
    tensor::del(label,batchSize);
}

int epochIndex=0;
int batchIndex=0;
const int maxn=100000;
char buff[maxn];
int a[1000];
void readNumber(){
    int uIndex=0;
    int sum=0,i=0;
    while(buff[i]!='\0'){
        if(buff[i]>='0'&&buff[i]<='9'){ 
            while(buff[i]>='0'&&buff[i]<='9') {sum=sum*10+buff[i]-'0';i++;}
            a[uIndex++]=sum;
            sum=0;
        }
        else i++;
    }
}

string path="mnist\\";
ofstream out(path+"out.txt");
ofstream wout(path+"w.txt");
ofstream lossout(path+"loss.txt");
ofstream tout(path+"time.txt");
ifstream win(path+"win.txt");
ofstream resout("digitTest\\result.txt");
ofstream evaout(path+"eva.txt");
void readImage(ifstream & in){
    if(batchIndex==0) in.getline(buff,maxn);
    for(int i=0;i<batchSize;i++){
        in.getline(buff, maxn);
        readNumber();
        label[i] = a[0];
        int j = 1;
        for (int tmpj = 0; tmpj < inputSize.dim2; tmpj++)
            for (int tmpk = 0; tmpk < inputSize.dim3; tmpk++)
                for (int tmps = 0; tmps < inputSize.dim4; tmps++)
                    input[i][tmpj][tmpk][tmps] = a[j++];
    }
}


void test(ifstream & in){
    in.seekg(0, ios_base::beg);
    batchIndex=0;
    int ans=0;
    while(batchIndex<420){
            readImage(in);
            ff();
            for(int i=0;i<batchSize;i++){
                int prediction=func::argmax(fout1[i],foutSize1);
                out<<"real:"<<label[i]<<" prediction:"<<prediction<<endl;
                if(label[i]==prediction) ans++;
            }
            batchIndex++;
    }
    out<<"true prediction: "<<ans<<"/42000"<<endl;
    out<<"currency: "<<(double)ans/42000<<endl;           
}


void printLoss(ifstream & in){
    in.seekg(0, ios_base::beg);
    batchIndex=0;
    double loss=0;
    while(batchIndex<400){
            readImage(in);
            ff();
            for(int i=0;i<batchSize;i++){
                for(int j=0;j<10;j++)
                    loss+=(label[i]==j?-log(fout1[i][j]):0);
            }
            batchIndex++;
    }
    lossout<<"epoch:"<<epochIndex<<" loss:"<<loss<<endl;
}
void printw(){
    for(int index=0;index<cwSize1.dim1;index++)
        for(int k=0;k<cwSize1.dim2;k++)
            for(int i=0;i<cwSize1.dim3;i++)
                for(int j=0;j<cwSize1.dim4;j++)
                    wout<<cw1[index][k][i][j]<<' ';
    wout<<endl;
    for(int i=0;i<cwSize1.dim1;i++)
        wout<<cb1[i]<<' ';
    wout<<endl;
    for(int index=0;index<cwSize2.dim1;index++)
        for(int k=0;k<cwSize2.dim2;k++)
            for(int i=0;i<cwSize2.dim3;i++)
                for(int j=0;j<cwSize2.dim4;j++)
                    wout<<cw2[index][k][i][j]<<' ';
    wout<<endl;
    for(int i=0;i<cwSize2.dim1;i++)
        wout<<cb2[i]<<' ';
    wout<<endl;
    for(int index=0;index<cwSize3.dim1;index++)
        for(int k=0;k<cwSize3.dim2;k++)
            for(int i=0;i<cwSize3.dim3;i++)
                for(int j=0;j<cwSize3.dim4;j++)
                    wout<<cw3[index][k][i][j]<<' ';
    wout<<endl;
    for(int i=0;i<cwSize3.dim1;i++)
        wout<<cb3[i]<<' ';
    wout<<endl;
    for(int i=0;i<foutSize1;i++)
        for(int j=0;j<finSize1;j++)
            wout<<fw1[i][j]<<' ';
    wout<<endl;
    for(int i=0;i<foutSize1;i++)
        wout<<fb1[i]<<' ';
    wout<<endl;
}

void readw(){
    for(int index=0;index<cwSize1.dim1;index++)
        for(int k=0;k<cwSize1.dim2;k++)
            for(int i=0;i<cwSize1.dim3;i++)
                for(int j=0;j<cwSize1.dim4;j++)
                    win>>cw1[index][k][i][j];
    for(int i=0;i<cwSize1.dim1;i++)
            win>>cb1[i];

    for(int index=0;index<cwSize2.dim1;index++)
        for(int k=0;k<cwSize2.dim2;k++)
            for(int i=0;i<cwSize2.dim3;i++)
                for(int j=0;j<cwSize2.dim4;j++)
                    win>>cw2[index][k][i][j];
    for(int i=0;i<cwSize2.dim1;i++)
        win>>cb2[i];

    for(int index=0;index<cwSize3.dim1;index++)
        for(int k=0;k<cwSize3.dim2;k++)
            for(int i=0;i<cwSize3.dim3;i++)
                for(int j=0;j<cwSize3.dim4;j++)
                    win>>cw3[index][k][i][j];
    for(int i=0;i<cwSize3.dim1;i++)
        win>>cb3[i];

    for(int i=0;i<foutSize1;i++)
        for(int j=0;j<finSize1;j++)
            win>>fw1[i][j];
    for(int i=0;i<foutSize1;i++)
        win>>fb1[i];
}

void readTestImages(ifstream &in){
    if(batchIndex==0) in.getline(buff,maxn);
    for(int i=0;i<batchSize;i++){
        in.getline(buff, maxn);
        readNumber();
        int j = 0;
        for (int tmpj = 0; tmpj < inputSize.dim2; tmpj++)
            for (int tmpk = 0; tmpk < inputSize.dim3; tmpk++)
                for (int tmps = 0; tmps < inputSize.dim4; tmps++)
                    input[i][tmpj][tmpk][tmps] = a[j++];
    }
}
void evaluate(ifstream &in){
    in.seekg(0, ios_base::beg);
    batchIndex=0;
    while(batchIndex<280){
            readImage(in);
            ff();
            for(int i=0;i<batchSize;i++){
                int prediction=func::argmax(fout1[i],foutSize1);
                evaout<<prediction<<endl;
            }
            batchIndex++;
    }
}
int main(){
    int selection=1;
    //cin>>selection
    //case(selection){0:train;1:digitTest;2:evaluate test.csv;}
    if(selection==0){
        ifstream in(path+"train.csv");
        init();
        while(epochIndex<Epoch){
            clock_t t_start,t_end;
            batchIndex=0;
            in.seekg(0, ios_base::beg);
            t_start=clock();
            while(batchIndex<400){
                readImage(in);
                ff();
                bp();
                update();
                cout<<"epoch "<<epochIndex<<" batch "<<batchIndex<<endl;
                batchIndex++;
            }
            t_end=clock();
            tout<<epochIndex<<' '<<(t_end-t_start)/CLOCKS_PER_SEC<<endl;
            printLoss(in);
            if(epochIndex==0) test(in);
            epochIndex++;
        }
        printw();
        test(in);
    }
    else if(selection==1){
        //you can test a batch at most in this way
        readw();
        BMPLoader loader;
        vector<BMPImage*> testImage = loader.loadBatchFile("digitTest");
        int n=testImage.size();
        for(int index=0;index<n;index++){
            for(int i=0;i<inputSize.dim3;i++)
                for(int j=0;j<inputSize.dim4;j++)
                    input[index][0][i][j] = (int)testImage[index]->getPixel(j,i).r;
        }
        ff();
        for(int i=0;i<n;i++)
            resout<<func::argmax(fout1[i],foutSize1)<<endl;
        cout<<"finished!"<<endl;
    }
    else{
        readw();
        ifstream in(path+"test.csv");
        evaluate(in);
    }
    del();
    system("pause");
    return 0;
}









