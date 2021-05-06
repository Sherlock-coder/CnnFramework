#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#include <cstdlib>
#include "cnn.h"
#include "fc1.h"
#include "ImagePreprocess.hpp"

using namespace std;
using namespace Ros;

const int batchSize=40;
const int Epoch=1;


//with reference to the url="https://www.kaggle.com/mrhippo/covid-19-lungs-x-rays-prediction"
//define size
dim inputSize(batchSize,1,150,150);
dim cwSize1(32,1,3,3);
dim coutSize1(batchSize,32,148,148);
dim poutSize1(batchSize,32,74,74);
dim cwSize2(48,32,3,3);
dim coutSize2(batchSize,48,72,72);
dim poutSize2(batchSize,48,36,36);
dim cwSize3(64,48,3,3);
dim coutSize3(batchSize,64,34,34);
dim poutSize3(batchSize,64,17,17);
int finSize=18496;
int foutSize1=32;
int foutSize2=2;

//get space
//input layer
static double ****  input=tensor::create(inputSize);
static double ****  inputerror;
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
//pooling layer 3
static double **** pout3=tensor::create(poutSize3);
static double **** perror3=tensor::create(poutSize3);
//flatten
//cout3->fin fe->cerror3
static double ** fin=tensor::create(batchSize,finSize);
static double ** fe=tensor::create(batchSize,finSize);
//full connected layer 1
static double ** fw1=tensor::create(foutSize1,finSize);
static double * fb1=tensor::create(foutSize1);
static double ** fout1=tensor::create(batchSize,foutSize1);
static double ** ferror1=tensor::create(batchSize,foutSize1);
//full connected layer 2(output layer)
static double ** fw2=tensor::create(foutSize2,foutSize1);
static double * fb2=tensor::create(foutSize2);
static double ** fout2=tensor::create(batchSize,foutSize2);
static double ** ferror2=tensor::create(batchSize,foutSize2);
//label
static double * label=tensor::create(batchSize);

//instantiate layers
conv c1(input,cout1,cerror1,inputerror,cw1,cb1,coutSize1,cwSize1);
pool p1(cout1,pout1,perror1,cerror1,2,coutSize1);
conv c2(pout1,cout2,cerror2,perror1,cw2,cb2,coutSize2,cwSize2);
pool p2(cout2,pout2,perror2,cerror2,2,coutSize2);
conv c3(pout2,cout3,cerror3,perror2,cw3,cb3,coutSize3,cwSize3);
pool p3(cout3,pout3,perror3,cerror3,2,coutSize3);
fc1 f1(fin,fout1,ferror1,fe,fw1,fb1,batchSize,finSize,foutSize1);
fc f2(fout1,fout2,ferror2,ferror1,fw2,fb2,batchSize,foutSize1,foutSize2); 

void init(){
    func::assign(0,cb1,cwSize1.dim1);
    func::assign(0,cb2,cwSize2.dim1);
    func::assign(0,cb3,cwSize3.dim1);
    func::assign(0,fb1,foutSize1);
    func::assign(0,fb2,foutSize2);
    srand((unsigned int)time(NULL));
    func::assignRandom(cw1,cwSize1);
    func::assignRandom(cw2,cwSize2);
    func::assignRandom(cw3,cwSize3);
    func::assignRandom(fw1,dim(foutSize1,finSize));
    func::assignRandom(fw2,dim(foutSize2,foutSize1));
}

void ff(){
    c1.ff();
    p1.ff();
    c2.ff();
    p2.ff();
    c3.ff();
    p3.ff();
    func::unfold(pout3,fin,poutSize3);
    f1.ff();
    f2.ff();
}

void bp(){
    for(int index=0;index<batchSize;index++)
        for(int i=0;i<foutSize2;i++)
            ferror2[index][i]=fout2[index][i]-(label[index]==i?1:0);
    f2.bp();
    f1.bp();
    func::fold(perror3,fe,poutSize3);
    p3.bp();
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
    f2.updatew();
    f2.updateb();
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
    tensor::del(pout3,poutSize3);
    tensor::del(perror3,poutSize3);
    tensor::del(fin,batchSize,finSize);
    tensor::del(fe,batchSize,finSize);
    tensor::del(fw1,foutSize1,finSize);
    tensor::del(fb1,foutSize1);
    tensor::del(fout1,batchSize,foutSize1);
    tensor::del(ferror1,batchSize,foutSize1);
    tensor::del(fw2,foutSize2,foutSize1);
    tensor::del(fb2,foutSize2);
    tensor::del(fout2,batchSize,foutSize2);
    tensor::del(ferror2,batchSize,foutSize2);
    tensor::del(label,batchSize);
}
int epochIndex=0;
int batchIndex=0;
int covidIndex=0;
int normalIndex=0;
const int covidTotal=3000;
const int normalTotal=3000;
string path="ctData/";
ofstream wout("Data/wout.txt");
ofstream lossout("Data/loss.txt");
ofstream  resout("Data/resout.txt");
ofstream statusout("Data/statusout.txt");
ifstream win("Data/win.txt");
ofstream qout("Data/qout.txt");
BMPLoader loader;
vector<BMPImage*> vc= loader.loadBatchFile(path+"COVID");
vector<BMPImage*> vn= loader.loadBatchFile(path+"Normal");
void readImage(){
    for(int index=0;index<batchSize;index++){
        if(index%2){
            for(int i=0;i<inputSize.dim3;i++)
                for(int j=0;j<inputSize.dim4;j++)
                    input[index][0][i][j]=((int)(vc[covidIndex%covidTotal]->getPixel(j,i)).r)/double(255);
            label[index]=1;
            covidIndex++;
        }
        else{
            for(int i=0;i<inputSize.dim3;i++)
                for(int j=0;j<inputSize.dim4;j++)
                    input[index][0][i][j]=((int)(vn[normalIndex%normalTotal]->getPixel(j,i)).r)/double(255);
            label[index]=0;
            normalIndex++;
        }
    }
}

void printw(){
    wout<<"epoch"<<epochIndex<<endl;
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
        for(int j=0;j<finSize;j++)
            wout<<fw1[i][j]<<' ';
    wout<<endl;
    for(int i=0;i<foutSize1;i++)
        wout<<fb1[i]<<' ';
    wout<<endl;

    for(int i=0;i<foutSize2;i++)
        for(int j=0;j<foutSize1;j++)
            wout<<fw2[i][j]<<' ';
    wout<<endl;
    for(int i=0;i<foutSize2;i++)
        wout<<fb2[i]<<' ';
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
        for(int j=0;j<finSize;j++)
            win>>fw1[i][j];
    for(int i=0;i<foutSize1;i++)
        win>>fb1[i];
    for(int i=0;i<foutSize2;i++)
        for(int j=0;j<foutSize1;j++)
            win>>fw2[i][j];
    for(int i=0;i<foutSize2;i++)
        win>>fb2[i];
}

void printLoss(){
    covidIndex=0;
    normalIndex=0;
    batchIndex=0;
    double loss=0;
    while(batchIndex<150){
        readImage();
        ff();
        for(int i=0;i<batchSize;i++)
            loss+=-log(label[i]?fout2[i][1]:fout2[i][0]);
        batchIndex++;
    }
    lossout<<"epoch: "<<epochIndex<<" loss: "<<loss<<endl;
}

void test(){
    resout<<"epoch:"<<epochIndex<<endl;
    const int realCovidTotal=3616;//3616
    const int realNormalTotal=3616;//10192
    covidIndex=0;
    normalIndex=0;
    batchIndex=0;
    int truePrediction=0;
    while(batchIndex<=realCovidTotal/batchSize){
        for(int index=0;index<batchSize;index++){
            for(int i=0;i<inputSize.dim3;i++)
                for(int j=0;j<inputSize.dim4;j++){
                    input[index][0][i][j]=((double)(vc[covidIndex%realCovidTotal]->getPixel(j,i)).r)/double(255);
                }
            covidIndex++;
        }
        ff();
        for(int i=0;i<batchSize;i++){
            if(batchIndex*batchSize+i>=realCovidTotal) break;
            if(func::argmax(fout2[i],foutSize2)==1){
                truePrediction++;
                resout<<"true: Covid-19  prediction: Covid-19"<<endl;
            }
            else{
                resout<<"true: Covid-19  prediction: Normal"<<endl;
            }
        }
        batchIndex++;
    }
    resout<<"accuracy:"<<truePrediction<<"/"<<realCovidTotal<<"="<<(double)truePrediction/realCovidTotal<<endl;
    batchIndex=0;
    truePrediction=0;
    while(batchIndex<=realNormalTotal/batchSize){
        for(int index=0;index<batchSize;index++){
            for(int i=0;i<inputSize.dim3;i++)
                for(int j=0;j<inputSize.dim4;j++)
                    input[index][0][i][j]=((double)(vn[normalIndex%realNormalTotal]->getPixel(j,i)).r)/double(255);
            normalIndex++;
        }
        ff();
        for(int i=0;i<batchSize;i++){
            if(batchIndex*batchSize+i>=realNormalTotal) break;
            if(func::argmax(fout2[i],foutSize2)==0){
                truePrediction++;
                resout<<"true: Normal  prediction: Normal"<<endl;
            }
            else{
                resout<<"true: Normal  prediction: Covid-19"<<endl;
            }
        }
        batchIndex++;
    }
    resout<<"accuracy"<<truePrediction<<"/"<<realNormalTotal<<endl;
}
int query(){
    for(int index=40*batchIndex;index<40*batchIndex+20;index++)
        for(int i=0;i<inputSize.dim3;i++)
            for(int j=0;j<inputSize.dim4;j++){
                input[index%40][0][i][j]=((int)(vc[index]->getPixel(j,i)).r)/double(255);
            }
    for(int index=40*batchIndex+20;index<40*batchIndex+40;index++)
        for(int i=0;i<inputSize.dim3;i++)
            for(int j=0;j<inputSize.dim4;j++){
                input[index%40][0][i][j]=((int)(vn[index]->getPixel(j,i)).r)/double(255);
            }
    ff();
    int truePrediction=0;
    for(int i=0;i<batchSize;i++){
        if(func::argmax(fout2[i],foutSize2)==(i<20)){
            truePrediction++;
        }
    }
    return truePrediction;
}

int main(){
    int choice=2;
    //   cin>>choice;
    //case(choice){0:train;1:test;2:covidTest}
    if(choice==0){
        init();
        clock_t t_start,t_end;
        while(epochIndex<Epoch){
            covidIndex=0;
            normalIndex=0;
            batchIndex=0;
            int runningAccuracy=0;
            while(batchIndex<150){
                t_start=clock();
                readImage();
                ff();
                bp();
                update();
                t_end=clock();
                statusout<<"epoch:"<<epochIndex<<" batch:"<<batchIndex<<" time:"<<(t_end-t_start)/CLOCKS_PER_SEC<<"s"<<endl;
                cout<<"epoch:"<<epochIndex<<" batch:"<<batchIndex<<" time:"<<(t_end-t_start)/CLOCKS_PER_SEC<<"s"<<endl;
                runningAccuracy+=query();
                if(batchIndex%5==4){
                    qout<<runningAccuracy<<'/'<<batchSize*5<<endl;
                    runningAccuracy=0;
                }
                batchIndex++;
            }
            printLoss();
            epochIndex++;
        }
        printw();
        test();
    }
    else if(choice==1){
        readw();
        cout<<"start test"<<endl;
        test();
    }
    else{
        //you can test a batch at most in this way
        readw();
        vector<BMPImage*> testImage= loader.loadBatchFile("covidTest");
        ofstream testout("covidTest/result.txt");
        int n=testImage.size();
        for(int index=0;index<n;index++){
            for(int i=0;i<inputSize.dim3;i++)
                for(int j=0;j<inputSize.dim4;j++)
                    input[index][0][i][j] = (double)(testImage[index]->getPixel(j,i).r)/(double)255;
        }
        cout<<"test start!"<<endl;
        ff();
        for(int i=0;i<n;i++)
            testout<<(func::argmax(fout2[i],foutSize2)?"Covid":"Normal")<<endl;
        cout<<"finished!"<<endl;
    }
    del();
    system("pause");
    return 0;
}