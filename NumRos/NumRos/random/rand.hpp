#pragma once
#include<algorithm>
#include<random>
#include<vector>
#include"../NdArray.hpp"
#include"../Core.hpp"
#include"generator.hpp"

namespace Ros{
	namespace random{
		/*template<typename dtype>
		NdArray<dtype> rand(uint32_t numSingle)
		{
			NdArray<dtype> returnArray(numSingle);
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			std::for_each(returnArray.begin(), returnArray.end(), [&dist](dtype& value){value = static_cast<dtype>(dist(generator_)); });
			return returnArray;
		}
		template<typename dtype>
		NdArray<dtype> rand(uint32_t numRows, uint32_t numCols)
		{
			NdArray<dtype> returnArray(numRows, numCols);
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			std::for_each(returnArray.begin(), returnArray.end(), [&dist](dtype& value){value = static_cast<dtype>(dist(generator_)); });
			return returnArray;
		}*/
		template<typename dtype, typename... Args>
		NdArray<dtype> rand(uint32_t head, Args... args)
		{
			std::vector<uint32_t> inList;
			getList(inList, head, args...);
			auto s = Shape(inList);
			NdArray<dtype> returnArray(s);
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			std::for_each(returnArray.begin(), returnArray.end(), [&dist](dtype& value){value = static_cast<dtype>(dist(generator_)); });
			return returnArray;
		}
		template<typename dtype>
		NdArray<dtype> rand(const std::vector<uint32_t>& inList)
		{
			auto s = Shape(inList);
			NdArray<dtype> returnArray(s);
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			std::for_each(returnArray.begin(), returnArray.end(), [&dist](dtype& value){value = static_cast<dtype>(dist(generator_)); });
			return returnArray;
		}
		template<typename dtype>
		NdArray<dtype> rand(const std::initializer_list<uint32_t>& inList)
		{
			auto s = Shape(inList);
			NdArray<dtype> returnArray(s);
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			std::for_each(returnArray.begin(), returnArray.end(), [&dist](dtype& value){value = static_cast<dtype>(dist(generator_)); });
			return returnArray;
		}
		template<typename dtype>
		NdArray<dtype> rand(const Shape& inShape)
		{
			NdArray<dtype> returnArray(inShape);
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			std::for_each(returnArray.begin(), returnArray.end(), [&dist](dtype& value){value = static_cast<dtype>(dist(generator_)); });
			return returnArray;
		}
	}
}
