#pragma once
#include<algorithm>
#include<random>
#include<vector>
#include"../NdArray.hpp"
#include"../Core.hpp"
#include"generator.hpp"

namespace Ros{
	namespace random{
		NdArray<double> randFloat(const std::initializer_list<uint32_t>& inList, double inLow = 0.0, double inHigh = 1.0)
		{
			auto s = Shape(inList);
			NdArray<double> returnArray(s);
			std::uniform_real_distribution<double> dist(static_cast<double>(inLow), static_cast<double>(inHigh));
			std::for_each(returnArray.begin(), returnArray.end(),
						  [&dist](double& value){value = dist(generator_); });
			return returnArray;
		}
		NdArray<double> randFloat(const Shape& inShape, double inLow = 0.0, double inHigh = 1.0)
		{
			NdArray<double> returnArray(inShape);
			std::uniform_real_distribution<double> dist(static_cast<double>(inLow), static_cast<double>(inHigh));
			std::for_each(returnArray.begin(), returnArray.end(),
						  [&dist](double& value){value = dist(generator_); });
			return returnArray;
		}
	}
}
