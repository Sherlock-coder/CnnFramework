#pragma once
#include<vector>
#include<algorithm>
#include<ctime>
#include<iostream>
#include<utility>
#include<tuple>
#include"NumRos.hpp"
namespace Ros{
	using namespace std;
	class ConvolutionalNeuralNetwork
	{
	public:
		ConvolutionalNeuralNetwork() :filters(2), filtersBiases(2), weights(3), biases(3)
		{
			Ros::random::seed(time(0));
			filters[0] = random::randFloat({6, 6, 6, 1}, -1, 1);
			filtersBiases[0] = random::randFloat({6, 1}, -1, 1);
			filters[1] = random::randFloat({16, 6, 6, 6}, -1, 1);
			filtersBiases[1] = random::randFloat({16, 1}, -1, 1);
			weights[0] = random::randFloat({120, 80656}, -1, 1);
			weights[1] = random::randFloat({84, 120}, -1, 1);
			weights[2] = random::randFloat({4, 84}, -1, 1);
			biases[0] = random::randFloat({120,1}, -1, 1);
			biases[1] = random::randFloat({84,1}, -1, 1);
			biases[2] = random::randFloat({4,1}, -1, 1);
		}
	public:
		NdArray<double> feedForward(NdArray<double>& x)
		{
			auto convFeature1 = addBias(convolute(x, filters[0]), filtersBiases[0]);
			auto reluFeature1 = relu(convFeature1);
			auto poolInfo1 = pool(reluFeature1);
			auto poolFeature1 = poolInfo1.first;

			//std::cout << convFeature1;
			//std::cout << reluFeature1;
			//std::cout << poolFeature1;

			auto convFeature2 = addBias(convolute(poolFeature1, filters[1]), filtersBiases[1]);
			auto reluFeature2 = relu(convFeature2);
			auto poolInfo2 = pool(reluFeature2);
			auto poolFeature2 = poolInfo2.first;

			//std::cout << convFeature2;
			//std::cout << reluFeature2;
			//std::cout << poolFeature2;

			auto straightFeature = poolFeature2.reshape(poolFeature2.shape(0) * poolFeature2.shape(1) * poolFeature2.shape(2), 1);
			auto fullConnect1z = dot(weights[0], straightFeature) + biases[0];
			auto fullConnect1a = relu(fullConnect1z);


			auto fullConnect2z = dot(weights[1], fullConnect1a) + biases[1];
			auto fullConnect2a = relu(fullConnect2z);

			std::cout << dot(weights[2], fullConnect2a) << std::endl;
			std::cout << dot(weights[2], fullConnect2a) + biases[2] << std::endl;

			auto fullConnect3z = dot(weights[2], fullConnect2a) + biases[2];
			auto fullConnect3a = softmax(fullConnect3z);

			//std::cout << "straightFeature:\n" << straightFeature << std::endl;
			//std::cout << "fullConnect1z:\n" << fullConnect1z << std::endl;
			//std::cout << "fullConnect1a:\n" << fullConnect1a << std::endl;
			//std::cout << "fullConnect2z:\n" << fullConnect2z << std::endl;
			//std::cout << "fullConnect2a:\n" << fullConnect2a << std::endl;
			/*std::cout << "fullConnect3z:\n" << fullConnect3z << std::endl;
			std::cout << "fullConnect3a:\n" << fullConnect3a << std::endl;*/
			return fullConnect3z;
		}
		auto backPropagation(NdArray<double>& x, NdArray<double> y)
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

			auto straightFeature = poolFeature2.reshape(poolFeature2.shape(0) * poolFeature2.shape(1) * poolFeature2.shape(2), 1);
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
			auto deltaPool2 = deltaStraightInput.reshape(poolFeature2.shape());
			auto deltaConv2 = poolDeltaBp(deltaPool2, poolOutMaxLocation2) * reluPrime(convFeature2);
			auto deltaPool1 = convolute(padding(deltaConv2, filters[1].shape(1) - 1), rot180(filters[1]).swapaxes(0, 3));
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

			vector<NdArray<double>> nableFilters;
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
		NdArray<double> convolute(NdArray<double> image, NdArray<double> convFilter)
		{
			if(image.shape().size() == 3)
			{
				uint32_t imageH = image.shape(0), imageW = image.shape(1), imageCh = image.shape(2);
				uint32_t filterNum = convFilter.shape(0), filterH = convFilter.shape(1), filterW = convFilter.shape(2);
				uint32_t featureH = imageH - filterH + 1, featureW = imageW - filterW + 1;

				NdArray<double> imageOut(featureH, featureW, filterNum);
				auto imageMatrix = zeros<double>({featureH * featureW, filterH * filterW * imageCh});
				auto filterMatrix = zeros<double>({filterH * filterW * imageCh, filterNum});

				for(int i = 0; i < featureH * featureW; i++)
					for(int j = 0; j < imageCh; j++)
						imageMatrix(i, Slice(j * filterH * filterW, (j + 1) * filterH * filterW)) =
						image(Slice(i / featureW, i / featureW + filterH), Slice(i % featureW, i % featureW + filterW), j).reshape(filterH * filterW);
				for(int i = 0; i < filterNum; i++)
					filterMatrix(filterMatrix.getSliceA(0), i) = convFilter(i).reshape(filterW * filterH * imageCh);
				auto featureMatrix = dot(imageMatrix, filterMatrix);
				for(int i = 0; i < filterNum; i++)
					imageOut(imageOut.getSliceA(0), imageOut.getSliceA(1), i) = featureMatrix(featureMatrix.getSliceA(0), i).reshape(featureW, featureH);
				/*NdArray<double> imageOut(image.shape(0) - convFilter.shape(1) + 1, image.shape(1) - convFilter.shape(2) + 1, convFilter.shape(0));
				for(auto filterNum = 0; filterNum < convFilter.shape(0); filterNum++)
				{
					auto currFilter = convFilter(filterNum);
					auto convMap = convolute(image(image.getSliceA(0), image.getSliceA(1), 0),
											 currFilter(currFilter.getSliceA(0), currFilter.getSliceA(1), 0));
					for(auto chNum = 1; chNum = currFilter.shape(-1); chNum++)
						convMap = convMap + convolute(image(image.getSliceA(0), image.getSliceA(1), chNum),
													  currFilter(currFilter.getSliceA(0), currFilter.getSliceA(1), chNum));

					imageOut(imageOut.getSliceA(0), imageOut.getSliceA(1), filterNum) = convMap;
				}*/
				return imageOut;
			}
			else if(image.shape().size() == 2)
			{
				NdArray<double> result(image.shape(0) - convFilter.shape(0) + 1, image.shape(1) - convFilter.shape(1) + 1);
				for(int r = 0; r < result.shape(0); r++)
					for(int c = 0; c < result.shape(1); c++)
						result.at(r, c) = sum(image(Slice(r, r + convFilter.shape(0)), Slice(c, c + convFilter.shape(1))) * convFilter);
				return result;
			}
		}
		NdArray<double> convDerivativeW(NdArray<double> outDelta, NdArray<double>& inFeature)//For it have the shape.
		{
			NdArray<double> nableConv(Shape(outDelta.shape(-1), inFeature.shape(0) - outDelta.shape(0) + 1,
									  inFeature.shape(1) - outDelta.shape(1) + 1, inFeature.shape(-1)));
			for(int filterNum = 0; filterNum < nableConv.shape(0); filterNum++)
				for(int chNum = 0; chNum < nableConv.shape(-1); chNum++)
					nableConv(filterNum, nableConv.getSliceA(1), nableConv.getSliceA(2), chNum) =
					convolute(inFeature(inFeature.getSliceA(0), inFeature.getSliceA(1), chNum),
							  outDelta(outDelta.getSliceA(0), outDelta.getSliceA(1), filterNum));
			return nableConv;
		}
		NdArray<double> convDerivativeB(NdArray<double> delta)
		{
			auto nableB = zeros<double>({delta.shape(-1), 1});
			for(int i = 0; i < delta.shape(-1); i++)
				nableB[i] = sum(delta(delta.getSliceA(0), delta.getSliceA(1), i));
			return nableB;
		}
		NdArray<double> addBias(NdArray<double> convFeature, NdArray<double>& bias)
		{
			for(int i = 0; i < bias.shape(0); i++)
				convFeature(convFeature.getSliceA(0), convFeature.getSliceA(1), i) += bias.at(i, 0);
			return convFeature;
		}
		NdArray<double> rot180(NdArray<double> f)
		{
			NdArray<double> rot180f = zeros<double>(f.shape());
			for(int filterNum = 0; filterNum < f.shape(0); filterNum++)
				for(int imgCh = 0; imgCh < f.shape(-1); imgCh++)
					rot180f(filterNum, rot180f.getSliceA(1), rot180f.getSliceA(2), imgCh) =
					flipud(fliplr(f(filterNum, f.getSliceA(1), f.getSliceA(2), imgCh)));
			return rot180f;
		}
		NdArray<double> padding(NdArray<double> feature, uint32_t zeroNum)
		{
			if(feature.shape().size() == 4)
			{
				NdArray<double> r = zeros<double>({feature.shape(0), feature.shape(1) + 2 * zeroNum, feature.shape(2) + 2 * zeroNum, feature.shape(3)});
				r(r.getSliceA(0), Slice(zeroNum, feature.shape(1) + zeroNum), Slice(zeroNum, feature.shape(2) + zeroNum), feature.getSliceA(3));
				return r;
			}
			else if(feature.shape().size() == 3)
			{
				NdArray<double> r = zeros<double>({feature.shape(0) + 2 * zeroNum, feature.shape(1) + 2 * zeroNum, feature.shape(2)});
				r(Slice(zeroNum, feature.shape(0) + zeroNum), Slice(zeroNum, feature.shape(1) + zeroNum), feature.getSliceA(2));
				return r;
			}
		}
		pair<NdArray<double>, NdArray<uint32_t>> pool(NdArray<double> feature, int size = 2, int stride = 2)
		{
			NdArray<double> poolOut((feature.shape(0) - size) / stride + 1,
									(feature.shape(1) - size) / stride + 1,
									feature.shape(2));
			NdArray<uint32_t> poolOutMaxLocation = zeros<uint32_t>(poolOut.shape());
			for(int chNum = 0; chNum < feature.shape(-1); chNum++)
			{
				for(int r = 0, rOut = 0; r < feature.shape(0) - size + 1; r += stride, rOut++)
				{
					for(int c = 0, cOut = 0; c < feature.shape(1) - size + 1; c += stride, cOut++)
					{
						auto slice = feature(Slice(r, r + size), Slice(c, c + size), chNum);
						poolOut.at(rOut, cOut, chNum) = max(slice);
						poolOutMaxLocation.at(rOut, cOut, chNum) = argmax(slice);
					}
				}
			}
			return pair<NdArray<double>, NdArray<uint32_t>>(poolOut, poolOutMaxLocation);
		}
		NdArray<double> poolDeltaBp(NdArray<double> poolOutDelta, NdArray<uint32_t> poolOutMaxLocation, int size = 2, int stride = 2)
		{
			NdArray<double> delta = zeros<double>({(poolOutDelta.shape(0) - 1) * stride + size,
										  (poolOutDelta.shape(1) - 1) * stride + size,
										  poolOutDelta.shape(2)});
			for(int chNum = 0; chNum < poolOutDelta.shape(-1); chNum++)
			{
				for(int r = 0; r < poolOutDelta.shape(0); r++)
				{
					for(int c = 0; c < poolOutDelta.shape(1); c++)
					{
						uint32_t order = poolOutMaxLocation.at(r, c, chNum);
						auto m = stride * r + order / size;
						auto n = stride * c + order % size;
						delta.at(m, n, chNum) = poolOutDelta.at(r, c, chNum);
					}
				}
			}
			return delta;
		}
		NdArray<double> relu(NdArray<double> feature)
		{
			NdArray<double> featureOut = zeros<double>(feature.shape());
			if(featureOut.shape().size() > 2)
				for(auto chNum = 0; chNum < feature.shape(-1); chNum++)
					for(int r = 0; r < feature.shape(0); r++)
						for(int c = 0; c < feature.shape(1); c++)
							featureOut.at(r, c, chNum) = std::max(feature.at(r, c, chNum), 0.0);
			else
				for(int r = 0; r < feature.shape(0); r++)
					featureOut.at(r, 0) = std::max(feature.at(r, 0), 0.0);
			return featureOut;
		}
		NdArray<double> reluPrime(NdArray<double> feature)
		{
			NdArray<double> featureOut = zeros<double>({feature.shape()});
			if(feature.shape().size() > 2)
			{
				for(int chNum = 0; chNum < feature.shape(-1); chNum++)
					for(int r = 0; r < feature.shape(0); r++)
						for(int c = 0; c < feature.shape(1); c++)
							if(feature.at(r, c, chNum) > 0)
								featureOut.at(r, c, chNum) = 1;
			}
			else
				for(int r = 0; r < feature.shape(0); r++)
					if(feature.at(r, 0) > 0)
						feature.at(r, 0) = 1;
			return featureOut;
		}
		NdArray<double> softmax(NdArray<double> feature)
		{
			auto temp = max(feature);
			feature -= temp;
			feature = exp(feature);
			temp = sum(feature);
			feature /= temp;
			return feature;
		}

	private:
		vector<NdArray<double>> filters;
		vector<NdArray<double>> filtersBiases;
		vector<NdArray<double>> weights;
		vector<NdArray<double>> biases;
	};
}
