#pragma once
#include<vector>
#include<utility>
#include<iostream>
#include "NumCpp.hpp"

namespace Ros {
	class MultilayerPerceptron
	{
	public:
		MultilayerPerceptron(std::vector<int>& sizes)
		{
			numLayers_ = sizes.size();
			sizes_ = sizes;
			for (int i = 0; i < numLayers_ - 1; i++)
			{
				weights_.push_back(nc::random::randFloat<double>({ (nc::uint32)sizes_[i + 1], (nc::uint32)sizes_[i] }, -1.0, 1.0));
				biases_.push_back(nc::random::randFloat<double>({ (nc::uint32)sizes_[i + 1], 1 }, -1.0, 1.0));
			}
			//Init parameters
		}

		nc::NdArray<double> sigmoid(nc::NdArray<double> z) { return 0.5 * nc::add(nc::tanh(0.5 * z), 1.0); }
		nc::NdArray<double> sigmoidPrime(nc::NdArray<double> z) { return sigmoid(z) * (nc::subtract(1.0, sigmoid(z))); }

		nc::NdArray<double> feedForward(nc::NdArray<double>& x)
		{
			for (int i = 0; i < numLayers_ - 1; i++)
				x = sigmoid(nc::dot(weights_[i], x) + biases_[i]);
			return x;
		}
		std::pair<std::vector<nc::NdArray<double>>, std::vector<nc::NdArray<double>>> backPropagation(nc::NdArray<double>& x, nc::NdArray<double>& y)
		{
			std::vector<nc::NdArray<double>> nablaWeights(weights_.size());
			std::vector<nc::NdArray<double>> nablaBiases(biases_.size());
			for (int i = 0; i < numLayers_ - 1; i++)
			{
				nablaWeights[i] = nc::zeros<double>(weights_[i].shape());
				nablaBiases[i] = nc::zeros<double>(biases_[i].shape());
			}

			std::vector<nc::NdArray<double>> activations;
			activations.push_back(x);
			std::vector<nc::NdArray<double>> zs;
			for (int i = 0; i < numLayers_ - 1; i++)
			{
				auto z = nc::dot(weights_[i], activations.back()) + biases_[i];
				zs.push_back(z);
				activations.push_back(sigmoid(z));
			}
			auto delta = (activations.back() - y) * sigmoidPrime(zs.back());
			nablaBiases.back() = delta;
			nablaWeights.back() = nc::dot(delta, activations[activations.size() - 2].transpose());
			for (int l = numLayers_ - 3; l > 0; l--)
			{
				delta = nc::dot(weights_[l + 1], delta) * sigmoidPrime(zs[l]);
				nablaBiases[l] = delta;
				nablaWeights[l] = nc::dot(delta, activations[l - 1].transpose());
			}
			return std::pair<std::vector<nc::NdArray<double>>, std::vector<nc::NdArray<double>>>(nablaWeights, nablaBiases);
		}

		void updateMiniBatch(std::vector<nc::NdArray<double>>& miniBatchImage, std::vector<nc::NdArray<double>>& miniBatchLabel,
			double eta, int miniBatchSize)
		{
			std::vector<nc::NdArray<double>> nablaWeights(weights_.size());
			std::vector<nc::NdArray<double>> nablaBiases(biases_.size());
			for (int i = 0; i < numLayers_ - 1; i++)
			{
				nablaWeights[i] = nc::zeros<double>(weights_[i].shape());
				nablaBiases[i] = nc::zeros<double>(biases_[i].shape());
			}
			for (int i = 0; i < miniBatchSize; i++)
			{
				auto r = backPropagation(miniBatchImage[i], miniBatchLabel[i]);
				for (int i = 0; i < numLayers_ - 1; i++)
				{
					nablaWeights[i] = nc::add(nablaWeights[i], r.first[i]);
					nablaBiases[i] = nc::add(nablaBiases[i], r.second[i]);
				}
			}
			for (int i = 0; i < numLayers_ - 1; i++)
			{
				weights_[i] = nc::subtract(weights_[i], (eta / miniBatchSize) * nablaWeights[i]);
				biases_[i] = nc::subtract(biases_[i], (eta / miniBatchSize) * nablaBiases[i]);
			}
		}

		void stochasticGradientDescent(std::vector<nc::NdArray<double>>& trainImage, std::vector<nc::NdArray<double>>& trainLabel,
			std::vector<nc::NdArray<double>>& testImage, std::vector<nc::NdArray<double>>& testLabel, int epochs, int miniBatchSize, double eta)
		{
			for (int i = 0; i < epochs; i++)
			{
				for (int j = 0; j < trainImage.size() / miniBatchSize - 1; j++)
				{
					auto miniBatchImage =
						std::vector<nc::NdArray<double>>((trainImage.begin() + j * miniBatchSize), (trainImage.begin() + (j + 1) * miniBatchSize));
					auto miniBatchLabel =
						std::vector<nc::NdArray<double>>((trainLabel.begin() + j * miniBatchSize), (trainLabel.begin() + (j + 1) * miniBatchSize));
					updateMiniBatch(miniBatchImage, miniBatchLabel, eta, miniBatchSize);
				}
			}
		}

		int evaluate(std::vector<nc::NdArray<double>>& images, std::vector<nc::NdArray<double>>& labels)
		{
			int result = 0;
			for (int j = 0; j < images.size(); j++)
			{
				auto predictLabel = feedForward(images[j]);
				result += (nc::argmax(predictLabel)[0] == nc::argmax(labels[j])[0]);
			}
			return result;
		}
	private:
		int numLayers_;
		std::vector<int> sizes_;
		std::vector<nc::NdArray<double>> weights_;
		std::vector<nc::NdArray<double>> biases_;
	};
}
