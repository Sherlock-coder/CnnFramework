#pragma once
#include"full.hpp"
#include"../NdArray.hpp"
#include"../core.hpp"
namespace Ros{
	/*template<typename dtype>
	NdArray<dtype> zeros(uint32_t numSingle) { return full<dtype>(numSingle, 0); }
	template<typename dtype>
	NdArray<dtype> zeros(uint32_t numRows, uint32_t numCols) { return full<dtype>(numRows, numCols, 0); }*/
	template<typename dtype>
	NdArray<dtype> zeros(const std::initializer_list<uint32_t>& inList) { return full<dtype>(inList, static_cast<dtype>(0)); }
	template<typename dtype>
	NdArray<dtype> zeros(const Shape& inShape) { return full<dtype>(inShape, static_cast<dtype>(0)); }
}