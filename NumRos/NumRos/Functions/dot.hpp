#pragma once
#include<iostream>
#include"../NdArray.hpp"
#include"sum.hpp"
namespace Ros{
	template<typename dtype>
	NdArray<dtype> dot(const NdArray<dtype>& lhs, const NdArray<dtype>& rhs)
	{
		if(lhs.ndim() == 1 && rhs.ndim() == 1)
		{
			NdArray<double> returnArray(1);
			returnArray[0] = sum(lhs * rhs);
			return returnArray;
		}
		else if(lhs.ndim() == 2 && rhs.ndim() == 2)
		{
			NdArray<double> returnArray(lhs.shape()[0], rhs.shape()[1]);
			for(uint32_t i = 0; i < returnArray.shape()[0]; i++)
				for(uint32_t j = 0; j < returnArray.shape()[1]; j++)
					returnArray.at(i, j) = sum(lhs(i, lhs.getSliceA(1)) * rhs(rhs.getSliceA(0), j));
			return returnArray;
		}
		else
		{
			std::cerr << "Haven't support the complex situation";
			throw"unsupport operation";
		}
	}
}