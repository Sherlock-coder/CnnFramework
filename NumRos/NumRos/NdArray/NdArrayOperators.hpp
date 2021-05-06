#pragma once
#include<functional>
#include<iostream>
#include<algorithm>
#include"NdArrayCore.hpp"
namespace Ros{
	//Broadcast mechanism haven't been implemented;

	template<typename dtype>
	NdArray<dtype> operator+(const NdArray<dtype>& lhs, const NdArray<dtype>& rhs)
	{
		if(lhs.shape() != rhs.shape())
		{
			throw"Array dimensions do not match.";
			std::cerr << "Array dimensions do not match.";
		}
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), rhs.begin(), returnArray.begin(), std::plus<dtype>());
		return returnArray;
	}

	template<typename dtype>
	NdArray<dtype> operator+(const NdArray<dtype>& lhs, dtype rhs)
	{
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), returnArray.begin(), [rhs](dtype value)->dtype{return value + rhs; });
		return returnArray;
	}

	template<typename dtype>
	NdArray<dtype> operator+(dtype lhs, const NdArray<dtype>& rhs) { return rhs + lhs; }


	template<typename dtype>
	NdArray<dtype> operator-(const NdArray<dtype>& lhs, const NdArray<dtype>& rhs)
	{
		if(lhs.shape() != rhs.shape())
		{
			throw"Array dimensions do not match.";
			std::cerr << "Array dimensions do not match.";
		}
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), rhs.begin(), returnArray.begin(), std::minus<dtype>());
		return returnArray;
	}

	template<typename dtype>
	NdArray<dtype> operator-(const NdArray<dtype>& lhs, dtype rhs)
	{
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), returnArray.begin(), [rhs](dtype value)->dtype{return value - rhs; });
		return returnArray;
	}


	template<typename dtype>
	NdArray<dtype> operator*(const NdArray<dtype>& lhs, const NdArray<dtype>& rhs)
	{
		if(lhs.shape() != rhs.shape())
		{
			throw"Array dimensions do not match.";
			std::cerr << "Array dimensions do not match.";
		}
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), rhs.begin(), returnArray.begin(), std::multiplies<dtype>());
		return returnArray;
	}

	template<typename dtype>
	NdArray<dtype> operator*(const NdArray<dtype>& lhs, dtype rhs)
	{
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), returnArray.begin(), [rhs](dtype value)->dtype{return value * rhs; });
		return returnArray;
	}

	template<typename dtype>
	NdArray<dtype> operator*(dtype lhs, const NdArray<dtype>& rhs) { return rhs * lhs; }


	template<typename dtype>
	NdArray<dtype> operator/(const NdArray<dtype>& lhs, const NdArray<dtype>& rhs)
	{
		if(lhs.shape() != rhs.shape())
		{
			throw"Array dimensions do not match.";
			std::cerr << "Array dimensions do not match.";
		}
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), rhs.begin(), returnArray.begin(), std::divides<dtype>());
		return returnArray;
	}

	template<typename dtype>
	NdArray<dtype> operator/(const NdArray<dtype>& lhs, dtype rhs)
	{
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), returnArray.begin(), [rhs](dtype value)->dtype{return value / rhs; });
		return returnArray;
	}


	template<typename dtype>
	NdArray<dtype> operator%(const NdArray<dtype>& lhs, const NdArray<dtype>& rhs)
	{
		if(lhs.shape() != rhs.shape())
		{
			throw"Array dimensions do not match.";
			std::cerr << "Array dimensions do not match.";
		}
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), rhs.begin(), returnArray.begin(), std::modulus<dtype>());
		return returnArray;
	}

	template<typename dtype>
	NdArray<dtype> operator%(const NdArray<dtype>& lhs, dtype rhs)
	{
		NdArray<dtype> returnArray(lhs.shape());
		std::transform(lhs.begin(), lhs.end(), returnArray.begin(), [rhs](dtype value)->dtype{return value % rhs; });
		return returnArray;
	}

	template<typename dtype>
	std::ostream& operator<<(std::ostream& os, const NdArray<dtype>& array)
	{
		os << '[';
		if(array.ndim() == 1)
		{
			for(int32_t i = 0; i < array.size() - 1; i++)
				os << array.at(i) << ", ";
			os << array.at(static_cast<int32_t>(array.size() - 1)) << ']';
		}
		else
		{
			for(auto i = 0; i < array.shape()[0] - 1; i++)
				os << array(i) << std::endl;
			os << array(static_cast<int32_t>(array.shape()[0] - 1)) << ']';
		}
		return os;
	}
}