#pragma once
#include"../NdArray.hpp"
#include"../Core.hpp"
namespace Ros{
	//template<typename dtype>
	//NdArray<dtype> full(uint32_t numSingle, dtype inFillValue)
	//{
	//	NdArray<dtype> returnArray(numSingle);
	//	returnArray.fill(inFillValue);
	//	return returnArray;
	//}
	//template<typename dtype>
	//NdArray<dtype> full(uint32_t numRows, uint32_t numCols, dtype inFillValue)
	//{
	//	NdArray<dtype> returnArray(numRows, numCols);
	//	returnArray.fill(inFillValue);
	//	return returnArray;
	//}
	template<typename dtype>
	NdArray<dtype> full(const std::initializer_list<uint32_t>& inList, dtype inFillValue)
	{
		auto s = Shape(inList);
		NdArray<dtype> returnArray(s);
		returnArray.fill(inFillValue);
		return returnArray;
	}
	template<typename dtype>
	NdArray<dtype> full(const Shape& inShape, dtype inFillValue)
	{
		NdArray<dtype> returnArray(inShape);
		returnArray.fill(inFillValue);
		return returnArray;
	}
	/*Variable parameters TBD*/
}