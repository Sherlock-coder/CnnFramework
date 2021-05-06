#pragma once
#include"../NdArray.hpp"
namespace Ros{
	template<typename dtype>
	NdArray<dtype> flipud(NdArray<dtype> inArray) { return inArray(inArray.getSliceA(0, -1)); }
}