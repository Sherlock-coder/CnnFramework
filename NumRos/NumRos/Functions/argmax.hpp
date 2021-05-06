#pragma once
#include"../NdArray.hpp"
namespace Ros{
	template<typename dtype>
	uint32_t argmax(const NdArray<dtype> &inArray)
	{
		dtype max = static_cast<dtype>(0);
		uint32_t argmax = 0;
		for(auto i = 0; i < inArray.size(); i++)
			if(inArray[i] > max)
				argmax = i, max = inArray[i];
		return argmax;
	}
}