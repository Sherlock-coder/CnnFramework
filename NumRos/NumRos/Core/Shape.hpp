#pragma once
#include<vector>
#include<cmath>
#include<iostream>
#include<algorithm>
#include<numeric>
#include<initializer_list>
#include"Slice.hpp"
namespace Ros{
	class Shape
	{
		template<typename dtype>
		friend class NdArray;
	public:
		Shape(const std::initializer_list<uint32_t>& inList) :dimensions_(inList) {}
		template<typename... Args>
		Shape(uint32_t head, Args... args) { setDimensions(head, args...); }
		Shape(const std::vector<uint32_t>& inList) :dimensions_(inList) {}

	public:
		uint32_t operator[](int32_t index) const
		{
			auto idx = rectifyIndex(index);
			return dimensions_[idx];
		}
		uint32_t& operator[](int32_t index)
		{
			auto idx = rectifyIndex(index);
			return dimensions_[idx];
		}
		bool operator==(const Shape& inShape) const { return dimensions_ == inShape.dimensions_; }
		bool operator!=(const Shape& inShape) const { return !this->operator==(inShape); }

	public:
		uint32_t size() const { return dimensions_.size(); }
		//Slice& getSlice(int32_t index) { return dimensions_[rectifyIndex(index)]; }
		//const Slice& getSlice(int32_t index) const { return dimensions_[rectifyIndex(index)]; }

	private:
		Shape() {}
		template<typename... Tail>
		void setDimensions(uint32_t head, Tail... tail)
		{
			dimensions_.push_back(head);
			setDimensions(tail...);
		}
		template<>
		void setDimensions(uint32_t head) { dimensions_.push_back(head); }
		//rectify negative index to the positive
		uint32_t rectifyIndex(int32_t index)const
		{
			if(index >= 0)
			{
				if(index < size())
					return static_cast<uint32_t>(index);
				else
				{
					std::cerr << "Index out of range";
					throw"Index out of range";
				}
			}
			else
			{
				if(std::abs(index) > size())
				{
					std::cerr << "Index out of range";
					throw"Index out of range";
				}
				else
					return static_cast<uint32_t>(index + size());
			}
		}

	private:
		std::vector<uint32_t> dimensions_;
	};
	std::ostream& operator<<(std::ostream& os, const Shape& outShape)
	{
		os << '(';
		for(uint32_t i = 0; i < outShape.size() - 1; i++)
			os << outShape[i] << ", ";
		os << outShape[outShape.size() - 1] << ')' << std::endl;
		return os;
	}
}
