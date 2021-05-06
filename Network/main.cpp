#include<iostream>
#include<ctime>
#include"NumRos.hpp"
#include"CNN/RosCNN.hpp"

using namespace std;
using namespace Ros;
int main()
{
	ConvolutionalNeuralNetwork cnn;
	NdArray<double> x = random::randFloat({299, 299, 1}, -1, 1);
	NdArray<double> y = random::randFloat({4, 1}, -1, 1);
	y[0] = 1;
	auto start = clock();
	//auto r1 = cnn.feedForward(x);
	auto end = clock();
	//cout << r1 << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	start = clock();
	auto r2 = cnn.backPropagation(x, y);
	end = clock();
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	system("pause");
	return 0;
}