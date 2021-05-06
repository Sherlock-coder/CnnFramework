#pragma once
#include"../NdArray.hpp"
namespace Ros{
	template<typename dtype>
	dtype sum(const NdArray<dtype>& inArray)
	{
		dtype result = 0;
		/*for(auto i = inArray.begin(); i != inArray.end(); ++i)
			result += *i;*/
		for(auto i = 0; i < inArray.size(); i++)
			result += inArray[i];
		return result;
	}
}