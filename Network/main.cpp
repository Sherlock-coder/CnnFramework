//该主函数会用到ImagePreprocess.hpp头文件，还会用到肺炎图片，应将肺炎图片文件放在当前项目的目录下
#include<iostream>
#include<ctime>
#include<cmath>
#include<random>
#include<fstream>
#include"NumCpp.hpp"
#include"CNN.hpp"
#include "Image\ImagePreprocess.hpp"

using namespace std;
using namespace nc;
using namespace Ros;

const int channel = 1;
const int height = 299;
const int width = 299;
const int class_num = 4;

const int epoch = 10;
const int batch = 32;
const double lr = 0.1;

const int COVID_num = 3600;//训练集中各类图片的数目
const int Viral_num = 1300;
const int Opacity_num = 6000;
const int Normal_num = 1000;
int pic_num = COVID_num + Viral_num + Opacity_num + Normal_num;

string* Filename = new string[pic_num];
int* Label = new int[pic_num];

int argmax(NdArray<double> x)
{
	int y = 0;
	for (int i = 0; i < class_num; i++)
		if (x(i, 0) > x(y, 0))
			y = i;
	return y;
}

double Loss(NdArray<double> output, NdArray<double> label)
{
	int i = argmax(label);
	return -1.0 * log(output(i, 0));
}

void write_path(string file)
{
	ofstream pos;
	pos.open(file, ios::trunc | ios::out);
	if (!pos.is_open())
	{
		cout << "open file error!\n";
		return;
	}
	for (int i = 1; i <= COVID_num; i++)
	{
		pos << "COVID\\COVID-" << i << ".bmp " << 3 << "\n";
	}
	for (int i = 1; i <= Viral_num; i++)
	{
		pos << "Viral_Pneumonia\\Viral_Pneumonia(" << i << ").bmp " << 2 << "\n";
	}
	for (int i = 1; i <= Opacity_num; i++)
	{
		pos << "Lung_Opacity\\Lung_Opacity-" << i << ".bmp " << 1 << "\n";
	}
	for (int i = 1; i <= Normal_num; i++)
	{
		pos << "Normal\\Normal-" << i << ".bmp " << 0 << "\n";
	}
	pos.close();
}

void read_pic_label(string file)
{
	ifstream write_file(file, ios::in);
	if (!write_file.is_open())
	{
		cout << "open file error!\n";
		return;
	}
	int cur = 0;
	while (!write_file.eof())
	{
		write_file >> Filename[cur] >> Label[cur];
		//cout << Filename[cur] << " " << Label[cur] << endl;
		cur++;
	}
	write_file.close();
}
int main()
{
	
	write_path("train.txt");

	read_pic_label("train.txt");

	srand((int)time(0));

	BMPLoader loader;
	ConvolutionalNeuralNetwork c;
	ConvolutionalNeuralNetwork::Feature x(channel, height, width);

	for (int i = 1; i <= epoch; i++)
	{
		cout << "\n----------epoch " << i << "----------\n";
		for (int j = 1; j <= pic_num / batch; j++)
		{
			cout << "-----batch" << j << "-----\n";
			double loss = 0;
			auto start = clock();
			int correct_num = 0;
			for (int k = 1; k <= batch; k++)
			{
				int num = rand() % pic_num;
				string s = Filename[num];
				int l = Label[num];
				NdArray<double> la(4, 1);
				la(l, 0) = 1;

				BMPImage& image = loader.loadSingleFile(s, BMPLoader::LoadMode::GREY);
				x.readin(image);
				cout << s << "  " << l << endl;
				auto r1 = c.feedForward(x);
				cout << r1 << endl;
				auto r2 = c.backPropagation(x, la);
				c.updateParameter(r2, lr);

				loss += Loss(r1, la);
				if (argmax(r1) == l)
					correct_num++;
			}
			auto end = clock();
			double acc = 1.0 * correct_num / batch;
			cout << "loss: " << loss << endl;
			cout << "accuracy: " << acc << endl;
			cout << "time consumed: " << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
		}
	}
	
}