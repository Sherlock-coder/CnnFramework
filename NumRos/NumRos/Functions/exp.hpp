#pragma once
#include<cmath>
#include"../NdArray.hpp"

namespace Ros{
	template<typename dtype>
	NdArray<dtype> exp(const NdArray<dtype>& inArray)
	{
		NdArray<dtype> returnArray(inArray);
		dtype max = static_cast<dtype>(0);
		for(auto i = returnArray.begin(); i != returnArray.end(); ++i)
			*i = std::exp(*i);
		return returnArray;
	}
}