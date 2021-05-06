#pragma once
#include<vector>
#include<iostream>
#include<utility>
#include<tuple>
#include<fstream>
#include "NumCpp.hpp"
#include "Image\ImagePreprocess.hpp"
namespace Ros {
	using namespace std;
	using namespace nc;
	class ConvolutionalNeuralNetwork
	{
		//All that I do is just let it run with the limit of numcpp.
		//There isn't any optimization and design structure, I will try when I finish the much more powerful mathmatical library.
	public:
		struct Filter
		{
			vector<vector<NdArray<double>>> kernels;
			//kernels.size() is the amount of convolution kernel namely the num of output feature channel
			//kernel[0].size() should be the channels of the input feature.
			Filter() = default;
			Filter(int numKernel, int numRows, int numCols, int numCh) :kernels(numKernel)//Construct in random
			{
				for (auto& i : kernels)
				{
					i.resize(numCh);
					for (auto& j : i)
						j = random::randFloat(Shape(numRows, numCols), -1.0, 1.0);
				}
			}
		};
		struct Feature
		{
			vector<NdArray<double>> f;
			Feature() = default;
			Feature(int numCh, int numRows, int numCols) :f(numCh)//Construct in zeros
			{
				for (auto& i : f)
					i = zeros<double>(numRows, numCols);
			}

			void readin(BMPImage& pic)
			{
				for (auto& i : f)
				{
					for (int j = 0; j < i.numRows(); j++)
						for (int k = 0; k < i.numCols(); k++)
						{
							BMPImage::Pixel p1 = pic.getPixel(j, k);
							int zz = (int)p1.r;
							//i(j, k) = (double)p1.r;
							//i(j, k) = 1.0 * i(j, k) / 255.0 - 0.5;//¹éÒ»»¯
							i(j, k) = zz * 1.0 / 255. - 0.5;
							//cout << i(j, k) << endl;
						}
				}
			}

			Feature operator*(const Feature& rhs)
			{
				Feature r(rhs);
				for (int i = 0; i < f.size(); i++)
					r.f[i] = f[i] * rhs.f[i];
				return r;
			}
		};
	public:
		ConvolutionalNeuralNetwork() :filters(2), filtersBiases(2), weights(3), biases(3)
		{
			filters[0] = Filter(6, 6, 6, 1);
			filtersBiases[0] = random::randFloat({ 6,1 }, -1.0, 1.0);
			filters[1] = Filter(16, 6, 6, 6);
			filtersBiases[1] = random::randFloat({ 16,1 }, -1.0, 1.0);
			weights[0] = random::randFloat({ 120,80656 }, -1.0, 1.0);
			weights[1] = random::randFloat({ 84,120 }, -1.0, 1.0);
			weights[2] = random::randFloat({ 4,84 }, -1.0, 1.0);
			biases[0] = random::randFloat({ 120,1 }, -1.0, 1.0);
			biases[1] = random::randFloat({ 84,1 }, -1.0, 1.0);
			biases[2] = random::randFloat({ 4,1 }, -1.0, 1.0);
		}

	public:
		NdArray<double> feedForward(Feature& x)
		{
			auto convFeature1 = addBias(convolute(x, filters[0]), filtersBiases[0]);//6@294*294
			auto reluFeature1 = relu(convFeature1);
			auto poolInfo1 = pool(reluFeature1);//6@147*147

			auto poolFeature1 = poolInfo1.first;
			auto convFeature2 = addBias(convolute(poolFeature1, filters[1]), filtersBiases[1]);//16@142*142
			auto reluFeature2 = relu(convFeature2);
			auto poolInfo2 = pool(reluFeature2);//16@71*71
			auto poolFeature2 = poolInfo2.first;

			auto straightFeature = straighten(poolFeature2);//16*71*71=80656, 80656->120
			auto fullConnect1z = dot(weights[0], straightFeature) + biases[0];
			auto fullConnect1a = relu(fullConnect1z);

			auto fullConnect2z = dot(weights[1], fullConnect1a) + biases[1];//120->84
			auto fullConnect2a = relu(fullConnect2z);

			auto fullConnect3z = dot(weights[2], fullConnect2a) + biases[2];//84->4
			auto fullConnect3a = softmax(fullConnect3z);
			//auto fullConnect3a = fullConnect3z;

			return fullConnect3a;
		}
		auto backPropagation(Feature& x, NdArray<double> y)
		{
			auto convFeature1 = addBias(convolute(x, filters[0]), filtersBiases[0]);
			auto reluFeature1 = relu(convFeature1);
			auto poolInfo1 = pool(reluFeature1);
			auto poolOutMaxLocation1 = poolInfo1.second;

			auto poolFeature1 = poolInfo1.first;
			auto convFeature2 = addBias(convolute(poolFeature1, filters[1]), filtersBiases[1]);
			auto reluFeature2 = relu(convFeature2);
			auto poolInfo2 = pool(reluFeature2);
			auto poolFeature2 = poolInfo2.first;
			auto poolOutMaxLocation2 = poolInfo2.second;

			auto straightFeature = straighten(poolFeature2);
			auto fullConnect1z = dot(weights[0], straightFeature) + biases[0];
			auto fullConnect1a = relu(fullConnect1z);

			auto fullConnect2z = dot(weights[1], fullConnect1a) + biases[1];
			auto fullConnect2a = relu(fullConnect2z);

			auto fullConnect3z = dot(weights[2], fullConnect2a) + biases[2];
			auto fullConnect3a = softmax(fullConnect3z);
			//cout << "Forward" << endl;

			auto deltaFullConnection3 = fullConnect3a - y;
			auto deltaFullConnection2 = dot(weights[2].transpose(), deltaFullConnection3) * reluPrime(fullConnect2z);
			auto deltaFullConnection1 = dot(weights[1].transpose(), deltaFullConnection2) * reluPrime(fullConnect1z);
			auto deltaStraightInput = dot(weights[0].transpose(), deltaFullConnection1);
			//cout << "DeltaFC" << endl;

			auto deltaPool2 = destraighten(deltaStraightInput, poolFeature2.f.size(), poolFeature2.f[0].numRows(), poolFeature2.f[0].numCols());
			auto deltaConv2 = poolDeltaBp(deltaPool2, poolOutMaxLocation2) * reluPrime(convFeature2);
			auto deltaPool1 = convolute(padding(deltaConv2, filters[1].kernels[0][0].numRows() - 1), swapAxes(rot180(filters[1])));
			auto deltaConv1 = poolDeltaBp(deltaPool1, poolOutMaxLocation1) * reluPrime(convFeature1);
			//cout << "DeltaConv&Pool" << endl;

			auto nablaW2 = dot(deltaFullConnection3, fullConnect2a.transpose());
			auto nablaB2 = deltaFullConnection3;
			auto nablaW1 = dot(deltaFullConnection2, fullConnect1a.transpose());
			auto nablaB1 = deltaFullConnection2;
			auto nablaW0 = dot(deltaFullConnection1, straightFeature.transpose());
			auto nablaB0 = deltaFullConnection1;
			//cout << "NablaFC" << endl;

			auto nablaFilters1 = convDerivativeW(deltaConv2, poolFeature1);
			auto nablaFilters0 = convDerivativeW(deltaConv1, x);
			auto nablaBiases1 = convDerivativeB(deltaConv2);
			auto nablaBiases0 = convDerivativeB(deltaConv1);
			//cout << "NablaFilter" << endl;

			vector<Filter> nableFilters;
			vector<NdArray<double>> nableFiltersBiases;
			vector<NdArray<double>> nableWeights;
			vector<NdArray<double>> nableBiases;
			nableFilters.push_back(nablaFilters0), nableFilters.push_back(nablaFilters1);
			nableFiltersBiases.push_back(nablaBiases0), nableFiltersBiases.push_back(nablaBiases1);
			nableWeights.push_back(nablaW0), nableWeights.push_back(nablaW1), nableWeights.push_back(nablaW2);
			nableBiases.push_back(nablaB0), nableBiases.push_back(nablaB1), nableBiases.push_back(nablaB2);
			//cout << "Finish!" << endl;
			return make_tuple(nableFilters, nableFiltersBiases, nableWeights, nableBiases);
		}

		void updateParameter(tuple<vector<Filter>, vector<NdArray<double>>, vector<NdArray<double>>, vector<NdArray<double>>> delta, double lr)
		{
			//update weight and bias
			auto nableWeights = get<2>(delta);
			auto nableBiases = get<3>(delta);

			auto nablaW2 = nableWeights[2];
			auto nablaW1 = nableWeights[1];
			auto nablaW0 = nableWeights[0];

			auto nablaB2 = nableBiases[2];
			auto nablaB1 = nableBiases[1];
			auto nablaB0 = nableBiases[0];

			weights[2] -= lr * nablaW2;
			weights[1] -= lr * nablaW1;
			weights[0] -= lr * nablaW0;
			biases[2] -= lr * nablaB2;
			biases[1] -= lr * nablaB1;
			biases[0] -= lr * nablaB0;

			//update filters and bias
			auto nableFilters = get<0>(delta);
			auto nableFiltersBiases = get<1>(delta);

			auto nablaFilters1 = nableFilters[1];
			auto nablaFilters0 = nableFilters[0];
			auto nablaBiases1 = nableFiltersBiases[1];
			auto nablaBiases0 = nableFiltersBiases[0];

			for (int i = 0; i < nablaFilters1.kernels.size(); i++)
				for (int j = 0; j < nablaFilters1.kernels[0].size(); j++)
					filters[1].kernels[i][j] -= nablaFilters1.kernels[i][j] * lr;


			for (int i = 0; i < nablaFilters0.kernels.size(); i++)
				for (int j = 0; j < nablaFilters0.kernels[0].size(); j++)
					filters[0].kernels[i][j] -= nablaFilters0.kernels[i][j] * lr;

			filtersBiases[1] -= lr * nablaBiases1;
			filtersBiases[0] -= lr * nablaBiases0;

			return;
		}
		/*
		void saveArg(string in)
		{
			fstream write_file(in, ofstream::in | ofstream::out);
			for (int i = 0; i < weights[0].numRows(); i++)
				for (int j = 0; j < weights[0].numCols(); j++)
					write_file << weights[0](i, j) << " ";
			for (int i = 0; i < weights[1].numRows(); i++)
				for (int j = 0; j < weights[1].numCols(); j++)
					write_file << weights[1](i, j) << " ";
			for (int i = 0; i < weights[2].numRows(); i++)
				for (int j = 0; j < weights[2].numCols(); j++)
					write_file << weights[2](i, j) << " ";
			for (int i = 0; i < biases[0].numRows(); i++)
				for (int j = 0; j < biases[0].numCols(); j++)
					write_file << biases[0](i, j) << " ";
			for (int i = 0; i < biases[1].numRows(); i++)
				for (int j = 0; j < biases[1].numCols(); j++)
					write_file << biases[1](i, j) << " ";
			for (int i = 0; i < biases[2].numRows(); i++)
				for (int j = 0; j < biases[2].numCols(); j++)
					write_file << biases[2](i, j) << " ";
		}
		*/
	private:
		Feature convolute(Feature image, Filter convFilter)
		{
			uint32 imageH = image.f[0].numRows(), imageW = image.f[0].numCols(), imageCh = image.f.size();
			uint32 filterNum = convFilter.kernels.size(), filterH = convFilter.kernels[0][0].numRows(), filterW = convFilter.kernels[0][0].numCols();
			uint32 featureH = imageH - filterH + 1, featureW = imageW - filterW + 1;

			Feature imageOut(filterNum, featureH, featureW);
			auto imageMatrix = zeros<double>(featureH * featureW, filterH * filterW * imageCh);
			auto filterMatrix = zeros<double>(filterH * filterW * imageCh, filterNum);

			for (int i = 0; i < featureH * featureW; i++)
				for (int j = 0; j < imageCh; j++)
					imageMatrix[i, Slice(j * filterH * filterW, (j + 1) * filterH * filterW)] = image.f[j](Slice(i / featureW, i / featureW + filterH), Slice(i% featureW, i% featureW + filterW)).reshape(filterH * filterW, 1);
			for (int i = 0; i < filterNum; i++)
				for (int j = 0; j < imageCh; j++)
					filterMatrix(Slice(j * filterH * filterW, (j + 1) * (filterH * filterW)), i) = convFilter.kernels[i][j].reshape(filterH * filterW, 1);
			auto featureMatrix = dot(imageMatrix, filterMatrix);

			for (int i = 0; i < filterNum; i++)
				imageOut.f[i] = featureMatrix(featureMatrix.rSlice(), i).reshape(featureH, featureW);

			/*
			for (int i = 0; i < filterNum; i++)
				cout << imageOut.f[i] << endl;
			*/
			return imageOut;
		}
		NdArray<double> convolute(NdArray<double>& image, NdArray<double>& convFilter)
		{
			NdArray<double> result(image.numRows() - convFilter.numRows() + 1, image.numCols() - convFilter.numCols() + 1);
			for (int r = 0; r < result.numRows(); r++)
				for (int c = 0; c < result.numCols(); c++)
					result[r, c] = sum(image(Slice(r, r + convFilter.numRows()), Slice(c, c + convFilter.numCols())) * convFilter)(0, 0);
			return result;
		}
		Filter convDerivativeW(Feature& outDelta, Feature& inFeature)//For it have the shape.
		{
			Filter nableConv(outDelta.f.size(), inFeature.f[0].numRows() - outDelta.f[0].numRows() + 1,
				inFeature.f[0].numCols() - outDelta.f[0].numCols() + 1, inFeature.f.size());
			for (int filterNum = 0; filterNum < nableConv.kernels.size(); filterNum++)
				for (int chNum = 0; chNum < nableConv.kernels[filterNum].size(); chNum++)
					nableConv.kernels[filterNum][chNum] = convolute(inFeature.f[chNum], outDelta.f[chNum]);
			return nableConv;
		}
		NdArray<double> convDerivativeB(Feature& delta)
		{
			auto nableB = zeros<double>(delta.f.size(), 1);
			for (int i = 0; i < delta.f.size(); i++)
				nableB[i] = sum(delta.f[i])(0, 0);
			return nableB;
		}
		Feature addBias(Feature convFeature, NdArray<double>& bias)
		{
			for (int i = 0; i < convFeature.f.size(); i++)
				convFeature.f[i] += bias(i, 0);
			return convFeature;
		}
		Filter& rot180(Filter& f)
		{
			for (int filterNum = 0; filterNum < f.kernels.size(); filterNum++)
				for (int imgCh = 0; imgCh < f.kernels[filterNum].size(); imgCh++)
					f.kernels[filterNum][imgCh] = flipud(fliplr(f.kernels[filterNum][imgCh]));
			return f;
		}
		Filter swapAxes(Filter& f)
		{
			Filter r;
			r.kernels.resize(f.kernels[0].size());
			for (auto& i : r.kernels)
				i.resize(f.kernels.size());
			for (int filterNum = 0; filterNum < r.kernels.size(); filterNum++)
				for (int imgCh = 0; imgCh < r.kernels[filterNum].size(); imgCh++)
					r.kernels[filterNum][imgCh] = f.kernels[imgCh][filterNum];
			return r;
		}
		Feature padding(Feature& feature, int zeroNum)
		{
			Feature r;
			r.f.resize(feature.f.size());
			for (int i = 0; i < feature.f.size(); i++)
			{
				r.f[i] = zeros<double>(feature.f[i].numRows() + 2 * zeroNum, feature.f[i].numCols() + 2 * zeroNum);
				r.f[i](Slice(zeroNum, zeroNum + feature.f[i].numRows()), Slice(zeroNum, zeroNum + feature.f[i].numCols())) = feature.f[i];
			}
			return r;
		}
		pair<Feature, Feature> pool(Feature& feature, int size = 2, int stride = 2)
		{
			Feature poolOut(feature.f.size(), (feature.f[0].numRows() - size) / stride + 1, (feature.f[0].numCols() - size) / stride + 1);
			Feature poolOutMaxLocation(poolOut);//Bacause it have the shape.
			for (int chNum = 0; chNum < feature.f.size(); chNum++) {
				for (int r = 0, rOut = 0; r < feature.f[0].numRows(); r += stride, rOut++) {
					for (int c = 0, cOut = 0; c < feature.f[0].numCols(); c += stride, cOut++) {
						poolOut.f[chNum](rOut, cOut) = max(feature.f[chNum](Slice(r, r + size), Slice(c, c + size)))(0, 0);
						poolOutMaxLocation.f[chNum](rOut, cOut) = argmax(feature.f[chNum](Slice(r, r + size), Slice(c, c + size)))(0, 0);
					}
				}
			}
			return pair<Feature, Feature>(poolOut, poolOutMaxLocation);
		}
		Feature poolDeltaBp(Feature& poolOutDelta, Feature& poolOutMaxLocation, int size = 2, int stride = 2)
		{
			Feature delta(poolOutDelta.f.size(), (poolOutDelta.f[0].numRows() - 1) * stride + size, (poolOutDelta.f[0].numCols() - 1) * stride + size);
			for (int chNum = 0; chNum < poolOutDelta.f.size(); chNum++) {
				for (int r = 0; r < poolOutDelta.f[chNum].numRows(); r++) {
					for (int c = 0; c < poolOutDelta.f[chNum].numCols(); c++) {
						int order = poolOutMaxLocation.f[chNum](r, c);
						int m = stride * r + order / size;
						int n = stride * c + order % size;
						delta.f[chNum](m, n) = poolOutDelta.f[chNum](r, c);
					}
				}
			}
			return delta;
		}
		Feature& relu(Feature& feature)
		{
			for (auto& i : feature.f)
				for (int j = 0; j < i.numRows(); j++)
					for (int k = 0; k < i.numCols(); k++)
						i(j, k) = max(i(j, k), 0.0);
			return feature;
		}
		Feature& reluPrime(Feature& feature)
		{
			for (int chNum = 0; chNum < feature.f.size(); chNum++)
				for (int r = 0; r < feature.f[0].numRows(); r++)
					for (int c = 0; c < feature.f[0].numRows(); c++)
						if (feature.f[chNum](r, c) > 0)
							feature.f[chNum](r, c) = 1;
			return feature;
		}
		NdArray<double>& relu(NdArray<double>& feature)
		{
			for (int i = 0; i < feature.size(); i++)
				feature(i, 0) = max(feature(i, 0), 0.0);
			return feature;
		}
		NdArray<double> reluPrime(NdArray<double>& feature)
		{
			for (int i = 0; i < feature.size(); i++)
				if (feature(i, 0) > 0)
					feature(i, 0) = 1;
			return feature;
		}
		NdArray<double> straighten(Feature feature)
		{
			auto r = zeros<double>(feature.f.size() * feature.f[0].size(), 1);
			for (int i = 0; i < feature.f.size(); i++)
				r(Slice(i * feature.f[i].size(), (i + 1) * feature.f[i].size()), 0)
				= feature.f[i].reshape(feature.f[i].size(), 1);
			return r;
		}
		Feature destraighten(NdArray<double>& feature, int numLayers, int numRows, int numCols)
		{
			Feature r(numLayers, numRows * numCols, 1);
			for (int i = 0; i < numLayers; i++) {
				r.f[i] = feature(Slice(i * numRows * numCols, (i + 1) * numRows * numCols), 0);
				r.f[i].reshape(numRows, numCols);
			}
			return r;
		}
		NdArray<double>& softmax(NdArray<double>& feature)
		{
			auto temp = max(feature)(0, 0);
			feature -= temp;
			feature = exp(feature);
			temp = sum(feature)(0, 0);
			feature /= temp;
			return feature;
		}
	private:
		vector<Filter> filters;
		//The lenght of vector here the amount of the filters;
		vector<NdArray<double>> filtersBiases;
		vector<NdArray<double>> weights;
		vector<NdArray<double>> biases;
	};
}
