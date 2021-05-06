#pragma once
#include<vector>
#include<ctime>
#include<iostream>
#include<utility>
#include<tuple>
#include"NumCpp.hpp"
namespace Ros{
	using namespace std;
	using namespace nc;
	class ConvolutionalNeuralNetwork
	{
	public:
		struct Filter
		{
			vector<vector<NdArray<double>>> kernels;
			//kernels.size() is the amount of convolution kernel namely the num of output feature channel
			//kernel[0].size() should be the channels of the input feature.
			Filter() = default;
			Filter(int numKernel, int numRows, int numCols, int numCh) :kernels(numKernel)//Construct in random
			{
				for(auto& i : kernels)
				{
					i.resize(numCh);
					for(auto& j : i)
						j = random::randFloat(Shape(numRows, numCols), -1.0, 1.0);
				}
			}
			void print()
			{
				for(auto i = 0; i < kernels.size(); i++)
				{
					std::cout << "Kernel" << i << ":\n";
					for(int j = 0; j < kernels[i].size(); j++)
					{
						std::cout << "Channel" << j << ":\n";
						std::cout << kernels[i][j];
					}
				}
			}
		};
		struct Feature
		{
			vector<NdArray<double>> f;
			Feature() = default;
			Feature(int numCh, int numRows, int numCols) :f(numCh)//Construct in zeros
			{
				for(auto& i : f)
					i = zeros<double>(numRows, numCols);
			}
			Feature(int numCh, int numRows, int numCols, bool rand) :f(numCh)//Construct in zeros
			{
				for(auto& i : f)
					i = random::rand<double>(Shape(numRows, numCols));
			}
			Feature operator*(const Feature& rhs)
			{
				Feature r(rhs);
				for(int i = 0; i < f.size(); i++)
					r.f[i] = f[i] * rhs.f[i];
				return r;
			}
			void print()
			{
				for(int i = 0; i < f.size(); i++)
					std::cout << "Channel" << i << ":\n" << f[i];
			}
		};
	public:
		ConvolutionalNeuralNetwork() :filters(2), filtersBiases(2), weights(3), biases(3)
		{
			filters[0] = Filter(6, 6, 6, 1);
			filters[1] = Filter(16, 6, 6, 6);
			filtersBiases[0] = random::randFloat({6,1}, -1.0, 1.0);
			filtersBiases[1] = random::randFloat({16,1}, -1.0, 1.0);
			weights[0] = random::randFloat({120,80656}, -1.0, 1.0);
			weights[1] = random::randFloat({84,120}, -1.0, 1.0);
			weights[2] = random::randFloat({4,84}, -1.0, 1.0);
			biases[0] = random::randFloat({120,1}, -1.0, 1.0);
			biases[1] = random::randFloat({84,1}, -1.0, 1.0);
			biases[2] = random::randFloat({4,1}, -1.0, 1.0);

			//std::cout << "Filter0:\n";
			//filters[0].print();
			//std::cout << "FilterBiases0:\n" << filtersBiases[0];
			//std::cout << "Filter1:\n";
			//filters[1].print();
			//std::cout << "FilterBiases0:\n" << filtersBiases[1];
			//std::cout << "Weight0:\n" << weights[0];
			//std::cout << "Weight1:\n" << weights[1];
			//std::cout << "Weight2:\n" << weights[2];
			//std::cout << "Biases0:\n" << biases[0];
			//std::cout << "Biases1:\n" << biases[1];
			//std::cout << "Biases2:\n" << biases[2];
		}
	public:
		NdArray<double> feedForward(Feature& x)
		{
			auto convFeature1 = addBias(convolute(x, filters[0]), filtersBiases[0]);
			convFeature1.print();
			auto reluFeature1 = relu(convFeature1);
			reluFeature1.print();
			auto poolInfo1 = pool(reluFeature1);

			auto poolFeature1 = poolInfo1.first;
			poolFeature1.print();
			auto convFeature2 = addBias(convolute(poolFeature1, filters[1]), filtersBiases[1]);
			convFeature2.print();
			auto reluFeature2 = relu(convFeature2);
			reluFeature2.print();
			auto poolInfo2 = pool(reluFeature2);
			auto poolFeature2 = poolInfo2.first;
			poolFeature2.print();

			auto straightFeature = straighten(poolFeature2);
			std::cout << "straightFeature:\n" << straightFeature << std::endl;
			auto fullConnect1z = dot(weights[0], straightFeature) + biases[0];
			std::cout << "fullConnect1z:\n" << fullConnect1z << std::endl;
			auto fullConnect1a = relu(fullConnect1z);
			std::cout << "fullConnect1a:\n" << fullConnect1a << std::endl;

			auto fullConnect2z = dot(weights[1], fullConnect1a) + biases[1];
			std::cout << "fullConnect2z:\n" << fullConnect2z << std::endl;
			auto fullConnect2a = relu(fullConnect2z);
			std::cout << "fullConnect2a:\n" << fullConnect2a << std::endl;

			auto fullConnect3z = dot(weights[2], fullConnect2a) + biases[2];
			std::cout << "fullConnect3z:\n" << fullConnect3z << std::endl;
			auto fullConnect3a = softmax(fullConnect3z);
			std::cout << "fullConnect3a:\n" << fullConnect3a << std::endl;
			return fullConnect3z;
		}
		auto backPropagation(Feature& x, NdArray<double> y)
		{
			auto start = clock();
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
			cout << "Forward" << endl;
			auto end = clock();
			cout << (double)(end - start) / CLOCKS_PER_SEC << endl;

			start = clock();
			auto deltaFullConnection3 = fullConnect3a - y;
			auto deltaFullConnection2 = dot(weights[2].transpose(), deltaFullConnection3) * reluPrime(fullConnect2z);
			auto deltaFullConnection1 = dot(weights[1].transpose(), deltaFullConnection2) * reluPrime(fullConnect1z);
			auto deltaStraightInput = dot(weights[0].transpose(), deltaFullConnection1);
			cout << "DeltaFC" << endl;
			end = clock();
			cout << (double)(end - start) / CLOCKS_PER_SEC << endl;

			start = clock();
			auto deltaPool2 = destraighten(deltaStraightInput, poolFeature2.f.size(), poolFeature2.f[0].numRows(), poolFeature2.f[0].numCols());
			auto deltaConv2 = poolDeltaBp(deltaPool2, poolOutMaxLocation2) * reluPrime(convFeature2);
			auto deltaPool1 = convolute(padding(deltaConv2, filters[1].kernels[0][0].numRows() - 1), swapAxes(rot180(filters[1])));
			auto deltaConv1 = poolDeltaBp(deltaPool1, poolOutMaxLocation1) * reluPrime(convFeature1);
			cout << "DeltaConv&Pool" << endl;
			end = clock();
			cout << (double)(end - start) / CLOCKS_PER_SEC << endl;

			start = clock();
			auto nablaW2 = dot(deltaFullConnection3, fullConnect2a.transpose());
			auto nablaB2 = deltaFullConnection3;
			auto nablaW1 = dot(deltaFullConnection2, fullConnect1a.transpose());
			auto nablaB1 = deltaFullConnection2;
			auto nablaW0 = dot(deltaFullConnection1, straightFeature.transpose());
			auto nablaB0 = deltaFullConnection1;
			cout << "NablaFC" << endl;
			end = clock();
			cout << (double)(end - start) / CLOCKS_PER_SEC << endl;

			start = clock();
			auto nablaFilters1 = convDerivativeW(deltaConv2, poolFeature1);
			auto nablaFilters0 = convDerivativeW(deltaConv1, x);
			auto nablaBiases1 = convDerivativeB(deltaConv2);
			auto nablaBiases0 = convDerivativeB(deltaConv1);
			cout << "NablaFilter" << endl;
			end = clock();
			cout << (double)(end - start) / CLOCKS_PER_SEC << endl;

			vector<Filter> nableFilters;
			vector<NdArray<double>> nableFiltersBiases;
			vector<NdArray<double>> nableWeights;
			vector<NdArray<double>> nableBiases;
			nableFilters.push_back(nablaFilters0), nableFilters.push_back(nablaFilters1);
			nableFiltersBiases.push_back(nablaBiases0), nableFiltersBiases.push_back(nablaBiases1);
			nableWeights.push_back(nablaW0), nableWeights.push_back(nablaW1), nableWeights.push_back(nablaW2);
			nableBiases.push_back(nablaB0), nableBiases.push_back(nablaB1), nableBiases.push_back(nablaB2);
			cout << "Finish!" << endl;
			return make_tuple(nableFilters, nableFiltersBiases, nableWeights, nableBiases);
		}
	private:
		Feature convolute(Feature image, Filter convFilter)
		{
			//image.print();
			//convFilter.print();

			uint32 imageH = image.f[0].numRows(), imageW = image.f[0].numCols(), imageCh = image.f.size();
			uint32 filterNum = convFilter.kernels.size(), filterH = convFilter.kernels[0][0].numRows(), filterW = convFilter.kernels[0][0].numCols();
			uint32 featureH = imageH - filterH + 1, featureW = imageW - filterW + 1;

			Feature imageOut(filterNum, featureH, featureW);
			auto imageMatrix = zeros<double>(featureH * featureW, filterH * filterW * imageCh);
			auto filterMatrix = zeros<double>(filterH * filterW * imageCh, filterNum);

			for(int i = 0; i < featureH * featureW; i++)
				for(int j = 0; j < imageCh; j++)
				{
					imageMatrix[i, Slice(j * filterH * filterW, (j + 1) * filterH * filterW)] =
						image.f[j](Slice(i / featureW, i / featureW + filterH), Slice(i% featureW, i% featureW + filterW)).reshape(filterH * filterW, 1);
				}
			std::cout << imageMatrix;
			for(int i = 0; i < filterNum; i++)
				for(int j = 0; j < imageCh; j++)
					filterMatrix(Slice(j * filterH * filterW, (j + 1) * (filterH * filterW)), i) = convFilter.kernels[i][j].reshape(filterH * filterW, 1);
			std::cout << filterMatrix;
			auto featureMatrix = dot(imageMatrix, filterMatrix);
			std::cout << featureMatrix;

			for(int i = 0; i < filterNum; i++)
				imageOut.f[i] = featureMatrix(featureMatrix.rSlice(), i).reshape(featureH, featureW);

			imageOut.print();
			return imageOut;
		}
		NdArray<double> convolute(NdArray<double>& image, NdArray<double>& convFilter)
		{
			NdArray<double> result(image.numRows() - convFilter.numRows() + 1, image.numCols() - convFilter.numCols() + 1);
			for(int r = 0; r < result.numRows(); r++)
				for(int c = 0; c < result.numCols(); c++)
					result[r, c] = sum(image(Slice(r, r + convFilter.numRows()), Slice(c, c + convFilter.numCols())) * convFilter)(0, 0);
			return result;
		}
		Filter convDerivativeW(Feature& outDelta, Feature& inFeature)//For it have the shape.
		{
			Filter nableConv(outDelta.f.size(), inFeature.f[0].numRows() - outDelta.f[0].numRows() + 1,
							 inFeature.f[0].numCols() - outDelta.f[0].numCols() + 1, inFeature.f.size());
			for(int filterNum = 0; filterNum < nableConv.kernels.size(); filterNum++)
				for(int chNum = 0; chNum < nableConv.kernels[filterNum].size(); chNum++)
					nableConv.kernels[filterNum][chNum] = convolute(inFeature.f[chNum], outDelta.f[chNum]);
			return nableConv;
		}
		NdArray<double> convDerivativeB(Feature& delta)
		{
			auto nableB = zeros<double>(delta.f.size(), 1);
			for(int i = 0; i < delta.f.size(); i++)
				nableB[i] = sum(delta.f[i])(0, 0);
			return nableB;
		}
		Feature addBias(Feature convFeature, NdArray<double>& bias)
		{
			for(int i = 0; i < convFeature.f.size(); i++)
				convFeature.f[i] += bias(i, 0);
			return convFeature;
		}
		Filter& rot180(Filter& f)
		{
			for(int filterNum = 0; filterNum < f.kernels.size(); filterNum++)
				for(int imgCh = 0; imgCh < f.kernels[filterNum].size(); imgCh++)
					f.kernels[filterNum][imgCh] = flipud(fliplr(f.kernels[filterNum][imgCh]));
			return f;
		}
		Filter swapAxes(Filter& f)
		{
			Filter r;
			r.kernels.resize(f.kernels[0].size());
			for(auto& i : r.kernels)
				i.resize(f.kernels.size());
			for(int filterNum = 0; filterNum < r.kernels.size(); filterNum++)
				for(int imgCh = 0; imgCh < r.kernels[filterNum].size(); imgCh++)
					r.kernels[filterNum][imgCh] = f.kernels[imgCh][filterNum];
			return r;
		}
		Feature padding(Feature& feature, int zeroNum)
		{
			Feature r;
			r.f.resize(feature.f.size());
			for(int i = 0; i < feature.f.size(); i++)
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
			for(int chNum = 0; chNum < feature.f.size(); chNum++)
			{
				for(int r = 0, rOut = 0; r < feature.f[0].numRows(); r += stride, rOut++)
				{
					for(int c = 0, cOut = 0; c < feature.f[0].numCols(); c += stride, cOut++)
					{
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
			for(int chNum = 0; chNum < poolOutDelta.f.size(); chNum++)
			{
				for(int r = 0; r < poolOutDelta.f[chNum].numRows(); r++)
				{
					for(int c = 0; c < poolOutDelta.f[chNum].numCols(); c++)
					{
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
			for(auto& i : feature.f)
				for(int j = 0; j < i.numRows(); j++)
					for(int k = 0; k < i.numCols(); k++)
						i(j, k) = max(i(j, k), 0.0);
			return feature;
		}
		Feature& reluPrime(Feature& feature)
		{
			for(int chNum = 0; chNum < feature.f.size(); chNum++)
				for(int r = 0; r < feature.f[0].numRows(); r++)
					for(int c = 0; c < feature.f[0].numRows(); c++)
						if(feature.f[chNum](r, c) > 0)
							feature.f[chNum](r, c) = 1;
			return feature;
		}
		NdArray<double>& relu(NdArray<double>& feature)
		{
			for(int i = 0; i < feature.size(); i++)
				feature(i, 0) = max(feature(i, 0), 0.0);
			return feature;
		}
		NdArray<double> reluPrime(NdArray<double>& feature)
		{
			for(int i = 0; i < feature.size(); i++)
				if(feature(i, 0) > 0)
					feature(i, 0) = 1;
			return feature;
		}
		NdArray<double> straighten(Feature feature)
		{
			auto r = zeros<double>(feature.f.size() * feature.f[0].size(), 1);
			for(int i = 0; i < feature.f.size(); i++)
				r(Slice(i * feature.f[i].size(), (i + 1) * feature.f[i].size()), 0)
				= feature.f[i].reshape(feature.f[i].size(), 1);
			return r;
		}
		Feature destraighten(NdArray<double>& feature, int numLayers, int numRows, int numCols)
		{
			Feature r(numLayers, numRows * numCols, 1);
			for(int i = 0; i < numLayers; i++)
			{
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
