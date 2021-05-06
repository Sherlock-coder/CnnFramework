#pragma once
#include"full.hpp"
#include"../NdArray.hpp"
#include"../core.hpp"
namespace Ros{
	//template<typename dtype>
	//NdArray<dtype> ones(uint32_t numSingle) { return full<dtype>(numSingle, 1); }
	//template<typename dtype>
	//NdArray<dtype> ones(uint32_t numRows, uint32_t numCols) { return full<dtype>(numRows, numCols, 1); }
	template<typename dtype>
	NdArray<dtype> ones(const std::initializer_list<uint32_t>& inList) { return full<dtype>(inList, static_cast<double>(1)); }
	template<typename dtype>
	NdArray<dtype> ones(const Shape& inShape) { return full<dtype>(inShape, static_cast<double>(1)); }
	/*Variable parameters TBD*/
}