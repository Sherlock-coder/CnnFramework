#pragma once
#include<initializer_list>
#include<iostream>
namespace Ros{
	class Slice
	{
	public:
		int32_t start{0};
		int32_t stop{1};
		int32_t step{1};
	public:
		Slice() = default;
		constexpr Slice(int32_t inStart) : start(inStart), stop(inStart + 1) {}
		constexpr Slice(int32_t inStart, int32_t inStop) : start(inStart), stop(inStop) {}
		constexpr Slice(int32_t inStart, int32_t inStop, int32_t inStep) : start(inStart), stop(inStop), step(inStep) {}
		constexpr Slice(const std::initializer_list<int32_t>& inList)
		{
			if(inList.size() == 1)
				start = *inList.begin(), stop = (*inList.begin()) + 1;
			else if(inList.size() == 2)
				start = *inList.begin(), stop = *(inList.begin() + 1);
			else if(inList.size() == 3)
				start = *inList.begin(), stop = *(inList.begin() + 1), step = *(inList.begin() + 2);
			else
			{
				std::cerr << "Invalid Slice parameter List";
				throw"Invalid Slice parameter List";
			}
		}
	};
	bool operator==(const Slice& lhs, const Slice& rhs)
	{
		return (rhs.start == lhs.start)
			&& (rhs.stop == lhs.stop)
			&& (rhs.step == lhs.step);
	}
}