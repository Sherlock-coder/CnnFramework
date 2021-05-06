#pragma once

#include<vector>
#include<iostream>
#include"MLP/MLP.hpp"
using namespace Ros;
using namespace std;
int test2()
{
	auto sizes = vector<int>(3);
	sizes[0] = 10;
	sizes[1] = 100;
	sizes[2] = 2;
	MultilayerPerceptron m(sizes);
	for (int j = 0; j < 1000000; j++) {
		nc::NdArray<double> t1 = { 0.1,-0.1 ,0.2,-0.2,0.1,-0.1 ,0.2,-0.2,0.1,-0.1 };
		nc::NdArray<double> t2 = t1;
		nc::NdArray<double> l = { 1,0 };
		t1.reshape(10, 1), t2.reshape(10, 1), l.reshape(2, 1);
		m.feedForward(t1);
		auto p = m.backPropagation(t2, l);
		//cout << "W:" << endl;
		//for (auto i : p.first)
		//	cout << i;
		//cout << "B:" << endl;
		//for (auto i : p.second)
		//	cout << i;
	}
	system("pause");
	return 0;
}