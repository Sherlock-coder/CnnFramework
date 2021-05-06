#pragma once
#include<vector>
namespace Ros{
	class Stride
	{
		template<typename dtype>
		friend class NdArray;
	public:
		Stride(uint32_t ndim) : strides_(ndim, 1) {}

	public:
		int32_t operator[](int32_t index) const
		{
			auto idx = rectifyIndex(index);
			return strides_[idx];
		}
		int32_t& operator[](int32_t index)
		{
			auto idx = rectifyIndex(index);
			return strides_[idx];
		}
		bool operator==(const Stride& inStride) const { return strides_ == inStride.strides_; }
		bool operator!=(const Stride& inStride) const { return !this->operator==(inStride); }

	public:
		uint32_t size() const { return strides_.size(); }

	private:
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
		std::vector<int32_t> strides_;
	};
}