#pragma once
#include"../NdArray.hpp"
namespace Ros{
	template<typename dtype>
	NdArray<dtype> fliplr(NdArray<dtype> inArray) { return inArray(inArray.getSliceA(0), inArray.getSliceA(1, -1)); }
}