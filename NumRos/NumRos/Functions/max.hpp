#pragma once
#include"../NdArray.hpp"
namespace Ros{
	template<typename dtype>
	dtype max(const NdArray<dtype>& inArray)
	{
		dtype max = *inArray.begin();
		for(auto i = inArray.begin(); i != inArray.end(); ++i)
		{
			if(*i > max)
				max = *i;
		}
		return max;
	}
}