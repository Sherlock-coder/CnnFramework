#include<iostream>
#include<ctime>
#include"NumCpp.hpp"
#include"CNN/CNN.hpp"

using namespace std;
using namespace nc;
using namespace Ros;
int testCNN()
{
	ConvolutionalNeuralNetwork c;
	ConvolutionalNeuralNetwork::Feature x(1, 299, 299, true);
	NdArray<double> y(4, 1);
	y[0] = 1;
	auto start = clock();
	auto r1 = c.feedForward(x);
	auto end = clock();
	cout << r1 << endl;
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	start = clock();
	auto r2 = c.backPropagation(x, y);
	end = clock();
	cout << (double)(end - start) / CLOCKS_PER_SEC << endl;
	system("pause");
	return 0;
}