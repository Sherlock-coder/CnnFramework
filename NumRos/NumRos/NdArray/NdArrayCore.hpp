#pragma once
#include<algorithm>
#include<Memory>
#include<functional>
#include<vector>
#include<iostream>
#include<algorithm>
#include<initializer_list>
#include"NdArrayIterators.hpp"
#include"../Core.hpp"
namespace Ros{
	template<typename dtype>
	class NdArray
	{
	public:
		NdArray() :strides_(0) { flags_.isValid = false; }
		NdArray(const std::initializer_list<dtype>& inList) :
			shape_(inList.size()), strides_(1), ndim_(1), size_(inList.size())
		{
			newArray();
			strides_[0] = 1;
			if(size() > 0)
				std::copy(inList.begin(), inList.end(), begin());
			flags_.isValid = true;
		}
		NdArray(const std::initializer_list<std::initializer_list<dtype>>& inList) :
			shape_(inList.size(), inList.begin()->size()), strides_(2), ndim_(2), size_(inList.size()* inList.begin()->size())
		{
			//Check each rows have the same elements and set the shape_
			strides_[1] = 1;
			auto numCols = shape()[1];
			strides_[0] = numCols;
			for(const auto& l : inList)
			{
				if(l.size() != numCols)
				{
					std::cerr << "All rows of the initializer list needs to have the same number of elements" << std::endl;
					throw("All rows of the initializer list needs to have the same number of elements");
				}
			}
			newArray();
			uint32_t r = 0;
			for(const auto& l : inList)
			{
				std::copy(l.begin(), l.end(), begin() + r * numCols);
				r++;
			}
			flags_.isValid = true;
		}
		template<typename uint32_t, typename... Args>
		NdArray(uint32_t head, Args... args) :shape_(head, args...), strides_(1 + sizeof...(args)), ndim_(1 + sizeof...(args))
		{
			strides_[ndim() - 1] = 1;
			size_ *= shape_[ndim() - 1];
			for(int32_t i = ndim() - 2; i >= 0; i--)
			{
				strides_[i] = strides_[i + 1] * shape_[i + 1];
				size_ *= shape_[i];
			}
			newArray();
			flags_.isValid = true;
		}
		NdArray(const Shape& inShape) :shape_(inShape), strides_(inShape.size()), ndim_(inShape.size())
		{
			strides_[ndim() - 1] = 1;
			size_ *= shape_[ndim() - 1];
			for(int32_t i = ndim() - 2; i >= 0; i--)
			{
				strides_[i] = strides_[i + 1] * shape_[i + 1];
				size_ *= shape_[i];
			}
			newArray();
			flags_.isValid = true;
		}
		NdArray(const NdArray& inOtherArray) :
			shape_(inOtherArray.shape_), strides_(inOtherArray.strides_), ndim_(inOtherArray.ndim_),
			flags_(inOtherArray.flags_), offset_(inOtherArray.offset_), base_(inOtherArray.base_), size_(inOtherArray.size_)
		{
			// It won't copy the data!
			// If you won't a clone, please use copy()!
		}
		NdArray(NdArray&& inOtherArray) noexcept :
			shape_(inOtherArray.shape_), strides_(inOtherArray.strides_), ndim_(inOtherArray.ndim_),
			flags_(inOtherArray.flags_), offset_(inOtherArray.offset_), base_(inOtherArray.base_), size_(inOtherArray.size_)
		{
		}
		~NdArray() { deleteArray(); }

	public:
		NdArray<dtype>& operator=(dtype inValue)
		{
			this->fill(inValue);
			return *this;
		}
		NdArray<dtype>& operator=(const NdArray<dtype>& rhs)
		{
			if(flags().isValid == false)
			{
				shape_ = rhs.shape();
				strides_ = rhs.strides();
				base_ = rhs.base_;
				offset_ = rhs.offset_;
				flags_ = rhs.flags();
				ndim_ = rhs.ndim();
				size_ = rhs.size();
			}
			else
			{
				if(shape() != rhs.shape())
				{
					std::cerr << "The broadcast mechanism haven't been supported";
					throw"Unsupported operation";
				}
				std::transform(begin(), end(), rhs.begin(), begin(), [](const dtype& lhs, const dtype& rhs){ return rhs; });
			}
			return *this;
		}
		//NdArray operator()(const std::initializer_list<Slice>& inList)
		//{
		//	NdArray returnArray;
		//	bool shouldCopy = flags().isReshapeView;//It should be more complicated.
		//	std::vector<Slice> list(ndim());
		//	std::copy(inList.begin(), inList.end(), list.begin());
		//	for(auto i = inList.size(); i < ndim(); i++)
		//		list[i] = Slice(0, shape()[i]);
		//	if(!shouldCopy)
		//	{
		//		uint32_t beginIndex = 0, size = 1;
		//		for(auto i = 0; i < list.size(); i++)
		//		{
		//			auto slice = list.begin() + i;
		//			auto length = (slice->stop - slice->start) / slice->step;
		//			if(length > 1)
		//			{
		//				returnArray.ndim_++;
		//				returnArray.shape_.dimensions_.push_back(length);
		//				returnArray.strides_.strides_.push_back(strides_[i] * slice->step);
		//			}
		//			size *= length;
		//			beginIndex += slice->start * strides_[i];
		//		}
		//		returnArray.base_ = base_;
		//		returnArray.data_ = returnArray.base_.get() + beginIndex;
		//		returnArray.flags_.isSliceView = true;
		//	}
		//	else
		//	{
		//		uint32_t size = 1;
		//		for(auto i = 0; i < list.size(); i++)
		//		{
		//			auto slice = list.begin() + i;
		//			auto length = (slice->stop - slice->start) / slice->step;
		//			if(length > 1)
		//			{
		//				returnArray.ndim_++;
		//				returnArray.shape_.dimensions_.push_back(length);
		//				returnArray.strides_.strides_.push_back(size);
		//			}
		//			size *= length;
		//		}
		//		returnArray.newArray();
		//		std::vector<int32_t> coordinate(inList.size(), 0);
		//		for(auto i = 0; i < coordinate.size(); i++)
		//			coordinate[i] = (list.begin() + i)->start;
		//		//copy the data
		//		auto step = (list.end() - 1)->step;
		//		for(auto i = 0; i < returnArray.size(); i++)
		//		{
		//			*(returnArray.data_ + i) = this->operator()(coordinate);
		//			auto rank = coordinate.size() - 1;
		//			coordinate[rank] += step;
		//			while(coordinate[rank] > (list.begin() + rank)->stop)
		//			{
		//				coordinate[rank] = (list.begin() + rank)->start;
		//				coordinate[++rank] += (list.begin() + rank)->step;
		//			}
		//		}
		//	}
		//	returnArray.flags_.isValid = true;
		//	return returnArray;
		//}
		//NdArray operator()(const std::initializer_list<Slice>& inList)const
		//{
		//	NdArray returnArray;
		//	bool shouldCopy = flags().isReshapeView;//It should be more complicated.
		//	std::vector<Slice> list(ndim());
		//	std::copy(inList.begin(), inList.end(), list.begin());
		//	for(auto i = inList.size(); i < ndim(); i++)
		//		list[i] = Slice(0, shape()[i]);
		//	if(!shouldCopy)
		//	{
		//		uint32_t beginIndex = 0, size = 1;
		//		for(auto i = 0; i < list.size(); i++)
		//		{
		//			auto slice = list.begin() + i;
		//			auto length = (slice->stop - slice->start) / slice->step;
		//			if(length > 1)
		//			{
		//				returnArray.ndim_++;
		//				returnArray.shape_.dimensions_.push_back(length);
		//				returnArray.strides_.strides_.push_back(strides_[i] * slice->step);
		//			}
		//			returnArray.size_ *= length;
		//			beginIndex += slice->start * strides_[i];
		//		}
		//		returnArray.base_ = base_;
		//		returnArray.data_ = returnArray.base_.get() + beginIndex;
		//		returnArray.flags_.isSliceView = true;
		//	}
		//	else
		//	{
		//		uint32_t size = 1;
		//		for(auto i = 0; i < list.size(); i++)
		//		{
		//			auto slice = list.begin() + i;
		//			auto length = (slice->stop - slice->start) / slice->step;
		//			if(length > 1)
		//			{
		//				returnArray.ndim_++;
		//				returnArray.shape_.dimensions_.push_back(length);
		//				returnArray.strides_.strides_.push_back(size);
		//			}
		//			size *= length;
		//		}
		//		returnArray.newArray();
		//		std::vector<int32_t> coordinate(inList.size(), 0);
		//		for(auto i = 0; i < coordinate.size(); i++)
		//			coordinate[i] = (list.begin() + i)->start;
		//		//copy the data
		//		auto step = (list.end() - 1)->step;
		//		for(auto i = 0; i < returnArray.size(); i++)
		//		{
		//			*(returnArray.data_ + i) = this->operator()(coordinate);
		//			auto rank = coordinate.size() - 1;
		//			coordinate[rank] += step;
		//			while(coordinate[rank] > (list.begin() + rank)->stop)
		//			{
		//				coordinate[rank] = (list.begin() + rank)->start;
		//				coordinate[++rank] += (list.begin() + rank)->step;
		//			}
		//		}
		//	}
		//	returnArray.flags_.isValid = true;
		//	return returnArray;
		//}
		//template<typename... Args>
		//NdArray operator()(Slice head, Args... args)
		//{
		//	std::vector<Slice>  vec;
		//	return operator()(constructVector(vec, head, args...));
		//}
		//template<typename... Args>
		//NdArray operator()(Slice head, Args... args) const
		//{
		//	std::vector<Slice>  vec;
		//	return operator()(constructVector(vec, head, args...));
		//}
		//template<typename... Args>
		//NdArray operator()(const std::initializer_list<int32_t>& head, Args... args)
		//{
		//	std::vector<Slice>  vec;
		//	return operator()(constructVector(vec, head, args...));
		//}
		//template<typename... Args>
		//NdArray operator()(const std::initializer_list<int32_t>& head, Args... args) const
		//{
		//	std::vector<Slice>  vec;
		//	return operator()(constructVector(vec, head, args...));
		//}
		// There seems no way to implement such situation
		// array a's shape is 3*4 and a[0] should return a array
		// But here it would only throw a exception
		// User should write all the dimension's slice as paramter
		// And get a Array as return value.
		/*dtype& operator()(std::initializer_list<int32_t> inList)
		{
			if(inList.size() != ndim())
			{
				std::cerr << "Dimension unmatched." << std::endl;
				std::cerr << "Library can't support all default unenough single index paramters input." << std::endl;
				throw"Dimension unmatched.";
			}
			uint32_t realIndex = 0;
			for(uint32_t i = 0; i < ndim(); i++)
				realIndex += rectifyIndex(*(inList.begin() + i), i) * strides()[i];
			return data_[realIndex];
		}
		dtype operator()(std::initializer_list<int32_t> inList)const
		{
			if(inList.size() != ndim())
			{
				std::cerr << "Dimension unmatched." << std::endl;
				std::cerr << "Library can't support all default unenough single index paramters input." << std::endl;
				throw"Dimension unmatched.";
			}
			uint32_t realIndex = 0;
			for(uint32_t i = 0; i < ndim(); i++)
				realIndex += rectifyIndex(*(inList.begin() + i), i) * strides()[i];
			return data_[realIndex];
		}*/
		template<typename... Args>
		NdArray<dtype> operator()(const Args&... args)const
		{
			if(sizeof...(args) > ndim())
			{
				std::cerr << "Too much parameters";
				throw "Too much parameters";
			}
			NdArray<dtype> returnArray;
			int32_t beginIndex = 0;
			bool shouldCopy = flags().isReshapeView;
			std::vector<Slice> slices;
			setSliceArray(returnArray, beginIndex, slices, args...);
			if(sizeof...(args) < ndim())
			{
				for(auto i = slices.size(); i < ndim(); i++)
				{
					slices.push_back(Slice(0, shape()[i]));
					returnArray.strides_.strides_.push_back(strides()[i]);
					returnArray.shape_.dimensions_.push_back(shape()[i]);
					returnArray.size_ *= returnArray.shape_[returnArray.ndim_++];
				}
			}
			if(!shouldCopy)
			{
				returnArray.base_ = base_;
				returnArray.offset_ = offset_ + beginIndex;
			}
			else
			{
				returnArray.newArray();
				std::vector<int32_t> coordinate(slices.size());
				for(auto i = 0; i < coordinate.size(); i++)
					coordinate[i] = slices[i].start;
				//copy the data
				auto step = slices.back().step;
				for(auto i = 0; i < returnArray.size() - 1; i++)
				{
					*(returnArray.base_.get() + returnArray.offset_ + i) = at(coordinate);
					auto rank = coordinate.size() - 1;
					coordinate[rank] += step;
					while(coordinate[rank] >= slices[rank].stop)
					{
						coordinate[rank] = slices[rank].start;
						--rank;
						coordinate[rank] += slices[rank].step;
					}
				}
				*(returnArray.base_.get() + returnArray.offset_ + returnArray.size() - 1) = at(coordinate);
				auto size = returnArray.size();
				for(auto i = 0; i < returnArray.ndim(); i++)
					returnArray.strides_[i] = (size /= returnArray.shape()[i]);
			}
			if(returnArray.ndim() == 0)
			{
				std::cerr << "What you get is actually a number, is not a valid array";
				throw"Invalid array";
			}
			returnArray.flags_.isValid = true;
			return returnArray;
		}
		//template<typename... Args>
		//NdArray<dtype> operator()(std::initializer_list<int32_t> list, const Args&... args)const
		//{
		//	if(sizeof...(args) > ndim())
		//	{
		//		std::cerr << "Too much parameters";
		//		throw "Too much parameters";
		//	}
		//	NdArray<dtype> returnArray;
		//	uint32_t beginIndex = 0;
		//	bool shouldCopy = flags().isReshapeView;
		//	std::vector<Slice> slices;
		//	setSliceArray(returnArray, beginIndex, slices, args...);
		//	if(sizeof...(args) < ndim())
		//	{
		//		for(auto i = slices.size(); i < ndim(); i++)
		//		{
		//			slices.push_back(Slice(0, shape()[i]));
		//			returnArray.strides_.strides_.push_back(strides()[i]);
		//			returnArray.shape_.dimensions_.push_back(shape()[i]);
		//			returnArray.size_ *= returnArray.shape_[returnArray.ndim_++];
		//		}
		//	}
		//	if(!shouldCopy)
		//	{
		//		returnArray.base_ = base_;
		//		returnArray.data_ = returnArray.base_.get() + beginIndex;
		//	}
		//	else
		//	{
		//		returnArray.newArray();
		//		std::vector<int32_t> coordinate(slices.size());
		//		for(auto i = 0; i < coordinate.size(); i++)
		//			coordinate[i] = slices[i].start;
		//		//copy the data
		//		auto step = slices.back().step;
		//		for(auto i = 0; i < returnArray.size(); i++)
		//		{
		//			*(returnArray.data_ + i) = at(coordinate);
		//			auto rank = coordinate.size() - 1;
		//			coordinate[rank] += step;
		//			while(coordinate[rank] > slices[rank].stop)
		//			{
		//				coordinate[rank] = slices[rank].start;
		//				coordinate[++rank] += slices[rank].step;
		//			}
		//		}
		//		auto size = returnArray.size();
		//		for(auto i = 0; i < returnArray.ndim(); i++)
		//			returnArray.strides_[i] = (size /= returnArray.shape()[i]);
		//	}
		//	if(ndim() == 0)
		//	{
		//		std::cerr << "What you get is actually a number, is not a valid array";
		//		throw"Invalid array";
		//	}
		//	return returnArray;
		//}
		dtype& operator[](uint32_t index)
		{
			if(index >= size())
			{
				std::cerr << "Index out of range";
				throw "Index out of range";
			}
			auto shape = this->shape();
			auto ndim = this->ndim();
			std::vector<int32_t> coordinate(ndim, 0);
			for(int32_t i = ndim - 1; i >= 0; i--)
			{
				coordinate[i] = index % shape[i];
				index /= shape[i];
			}
			return at(coordinate);
		}
		dtype operator[](uint32_t index)const
		{
			if(index >= size())
			{
				std::cerr << "Index out of range";
				throw "Index out of range";
			}
			auto shape = this->shape();
			auto ndim = this->ndim();
			std::vector<int32_t> coordinate(ndim, 0);
			for(int32_t i = ndim - 1; i >= 0; i--)
			{
				coordinate[i] = index % shape[i];
				index /= shape[i];
			}
			return at(coordinate);
		}
		NdArray<dtype>& operator+=(const NdArray<dtype>& rhs)
		{
			if(shape() != rhs.shape())
			{
				throw"Array dimensions do not match.";
				std::cerr << "Array dimensions do not match.";
			}
			std::transform(begin(), end(), rhs.begin(), begin(), std::plus<dtype>());
			return *this;
		}
		template<typename dtype>
		NdArray<dtype>& operator+=(dtype rhs)
		{
			std::for_each(begin(), end(), [rhs](dtype& value)->dtype{return value += rhs; });
			return *this;
		}
		NdArray<dtype>& operator-=(const NdArray<dtype>& rhs)
		{
			if(shape() != rhs.shape())
			{
				throw"Array dimensions do not match.";
				std::cerr << "Array dimensions do not match.";
			}
			std::transform(begin(), end(), rhs.begin(), begin(), std::minus<dtype>());
		}
		NdArray<dtype>& operator-=(dtype rhs)
		{
			std::for_each(begin(), end(), [rhs](dtype& value)->dtype{return value -= rhs; });
			return *this;
		}
		NdArray<dtype>& operator*=(const NdArray<dtype>& rhs)
		{
			if(shape() != rhs.shape())
			{
				throw"Array dimensions do not match.";
				std::cerr << "Array dimensions do not match.";
			}
			std::transform(begin(), end(), rhs.begin(), begin(), std::multiplies<dtype>());
		}
		NdArray<dtype>& operator*=(dtype rhs)
		{
			std::for_each(begin(), end(), [rhs](dtype& value)->dtype{return value *= rhs; });
			return *this;
		}
		NdArray<dtype>& operator/=(const NdArray<dtype>& rhs)
		{
			if(shape() != rhs.shape())
			{
				throw"Array dimensions do not match.";
				std::cerr << "Array dimensions do not match.";
			}
			std::transform(begin(), end(), rhs.begin(), begin(), std::divides<dtype>());
		}
		NdArray<dtype>& operator/=(dtype rhs)
		{
			std::for_each(begin(), end(), [rhs](dtype& value)->dtype{return value /= rhs; });
			return *this;
		}
		NdArray<dtype>& operator%=(const NdArray<dtype>& rhs)
		{
			if(shape() != rhs.shape())
			{
				throw"Array dimensions do not match.";
				std::cerr << "Array dimensions do not match.";
			}
			std::transform(begin(), end(), rhs.begin(), begin(), std::modulus<dtype>());
		}
		NdArray<dtype>& operator%=(dtype rhs)
		{
			std::for_each(begin(), end(), [rhs](dtype& value)->dtype{return value %= rhs; });
			return *this;
		}

	public:
		template<typename... Args>
		dtype& at(int32_t head, Args... args)
		{
			if(1 + sizeof...(Args) != ndim())
			{
				std::cerr << "Dimension unmatched." << std::endl;
				std::cerr << "Library can't support all default unenough single index paramters input." << std::endl;
				throw"Dimension unmatched.";
			}
			return base_.get()[offset_ + calculateRealIndex(head, args...)];
		}
		template<typename... Args>
		dtype at(int32_t head, Args... args)const
		{
			if(1 + sizeof...(Args) != ndim())
			{
				std::cerr << "Dimension unmatched." << std::endl;
				std::cerr << "Library can't support all default unenough single index paramters input." << std::endl;
				throw"Dimension unmatched.";
			}
			return base_.get()[offset_ + calculateRealIndex(head, args...)];
		}
		NdArrayIterator<dtype> begin() { return NdArrayIterator<dtype>(base_.get(), offset_, shape_, strides_); }
		NdArrayConstIterator<dtype> begin() const { return NdArrayConstIterator<dtype>(base_.get(), offset_, shape_, strides_); }//Don't want to implement the const iterator
		NdArrayIterator<dtype> end() { return NdArrayIterator<dtype>(base_.get(), offset_, shape_, strides_) += size(); }
		NdArrayConstIterator<dtype> end() const { return NdArrayConstIterator<dtype>(base_.get(), offset_, shape_, strides_) += size(); }//Don't want to implement the const iterator
		uint32_t size() const { return size_; }
		uint32_t ndim() const { return ndim_; }
		const Shape& shape() const { return shape_; }
		uint32_t shape(int32_t index) const { return shape_[index]; }
		const Stride& strides()const { return strides_; }
		uint32_t strides(int32_t index)const { return strides_[index]; }
		const Flag& flags()const { return flags_; }
		NdArray<dtype> copy()
		{
			auto returnArray = NdArray(shape());
			returnArray.newArray();
			returnArray.offset_ = offset_;
			returnArray.flags_ = flags();
			returnArray.strides_ = strides();
			returnArray.ndim_ = ndim();
			returnArray.size_ = size();
			return returnArray;
		}
		Slice getSliceL(int32_t end, int32_t dimension, int32_t step = 1)const
		{
			return Slice(0, rectifyIndex(end, dimension), step);
		}
		Slice getSliceR(int32_t begin, int32_t dimension, int32_t step = 1)const
		{
			return Slice(rectifyIndex(begin, dimension), shape()[dimension], step);
		}
		Slice getSliceA(int32_t dimension, int32_t step = 1)const
		{
			if(step >= 0)
				return Slice(0, shape()[dimension], step);
			else
				return Slice(shape()[dimension] - 1, -1, step);
		}

	public:
		NdArray<dtype> reshape(const std::initializer_list<uint32_t>& inList)
		{
			NdArray<dtype> returnArray(*this);
			if(std::accumulate(inList.begin(), inList.end(), 1, [](uint32_t a, uint32_t b){return a * b; }) != size())
			{
				std::cerr << "Can not reshape this array because the size can not match" << std::endl;
				throw "Can not reshape this array because the size can not match";
			}
			return returnArray.setShape(std::vector<uint32_t>(inList));
		}
		template<typename... Args>
		NdArray<dtype> reshape(uint32_t head, Args... args)
		{
			NdArray<dtype> returnArray(*this);
			std::vector<uint32_t> inList;
			getList(inList, head, args...);
			if(std::accumulate(inList.begin(), inList.end(), 1, [](uint32_t a, uint32_t b){return a * b; }) != size())
			{
				std::cerr << "Can not reshape this array because the size can not match" << std::endl;
				throw "Can not reshape this array because the size can not match";
			}
			return returnArray.setShape(std::vector<uint32_t>(inList));
		}
		NdArray<dtype> reshape(Shape inShape)
		{
			NdArray<dtype> returnArray(*this);
			if(std::accumulate(inShape.dimensions_.begin(), inShape.dimensions_.end(), 1, [](uint32_t a, uint32_t b){return a * b; }) != size())
			{
				std::cerr << "Can not reshape this array because the size can not match" << std::endl;
				throw "Can not reshape this array because the size can not match";
			}
			return returnArray.setShape(std::vector<uint32_t>(inShape.dimensions_));
		}
		NdArray<dtype>& fill(dtype inFillValue)
		{
			std::fill(begin(), end(), inFillValue);
			return *this;
		}
		NdArray<dtype> transpose()
		{
			NdArray<dtype> returnArray(*this);
			reverse(returnArray.shape_.dimensions_.begin(), returnArray.shape_.dimensions_.end());
			reverse(returnArray.strides_.strides_.begin(), returnArray.strides_.strides_.end());
			returnArray.flags_.isSliceView = true;
			return returnArray;
		}
		NdArray<dtype> swapaxes(int32_t axis1, int32_t axis2)
		{
			NdArray<dtype> returnArray(*this);
			std::swap(returnArray.shape_.dimensions_[returnArray.shape_.rectifyIndex(axis1)], 
					  returnArray.shape_.dimensions_[returnArray.shape_.rectifyIndex(axis2)]);
			std::swap(returnArray.strides_.strides_[returnArray.strides_.rectifyIndex(axis1)], 
					  returnArray.strides_.strides_[returnArray.strides_.rectifyIndex(axis2)]);
			returnArray.flags_.isSliceView = true;
			return returnArray;
		}

	private:
		//template<typename... Args>
		//std::vector<Slice>& constructVector(std::vector<Slice>& vec, Slice head, Args... args) const
		//{
		//	vec.push_back(head);
		//	return constructVector(vec, args);
		//}
		//template<>
		//std::vector<Slice>& constructVector(std::vector<Slice>& vec, Slice head)const
		//{
		//	vec.push_back(head);
		//	return vec;
		//}
		//template<typename... Args>
		//std::vector<Slice>& constructVector(std::vector<Slice>& vec, const std::initializer_list<int32_t>& head, Args... args)const
		//{
		//	vec.push_back(Slice(head));
		//	return constructVector(vec, args);
		//}
		//template<>
		//std::vector<Slice>& constructVector(std::vector<Slice>& vec, const std::initializer_list<int32_t>& head)const
		//{
		//	vec.push_back(Slice(head));
		//	return vec;
		//}
		template<typename... Args>
		uint32_t calculateRealIndex(int32_t head, Args... args) const
		{
			auto dimension = ndim() - sizeof...(Args) - 1;
			return rectifyIndex(head, dimension) * strides()[dimension] + calculateRealIndex(args...);
		}
		template<>
		uint32_t calculateRealIndex(int32_t head) const
		{
			return 	rectifyIndex(head, ndim() - 1) * strides()[ndim() - 1];
		}
		dtype at(const std::vector<int32_t>& inArray)const
		{
			uint32_t realIndex = 0;
			for(uint32_t i = 0; i < ndim(); i++)
				realIndex += rectifyIndex(inArray[i], i) * strides()[i];
			return base_.get()[offset_ + realIndex];
		}
		dtype& at(const std::vector<int32_t>& inArray)
		{
			uint32_t realIndex = 0;
			for(uint32_t i = 0; i < ndim(); i++)
				realIndex += rectifyIndex(inArray[i], i) * strides()[i];
			return base_.get()[offset_ + realIndex];
		}
		//NdArray operator()(std::vector<Slice>& inList)const
		//{
		//	NdArray returnArray;
		//	bool shouldCopy = flags().isReshapeView;//It should be more complicated.
		//	for(auto i = inList.size(); i < ndim(); i++)
		//		inList.push_back(Slice(0, shape()[i]));
		//	if(!shouldCopy)
		//	{
		//		uint32_t beginIndex = 0, size = 1;
		//		for(auto i = 0; i < inList.size(); i++)
		//		{
		//			auto slice = inList[i];
		//			auto length = (slice.stop - slice.start) / slice.step;
		//			if(length > 1)
		//			{
		//				returnArray.ndim_++;
		//				returnArray.shape_.dimensions_.push_back(length);
		//				returnArray.strides_.strides_.push_back(strides_[i] * slice.step);
		//			}
		//			size *= length;
		//			beginIndex += slice.start * strides_[i];
		//		}
		//		returnArray.size_ = size;
		//		returnArray.base_ = base_;
		//		returnArray.data_ = returnArray.base_.get() + beginIndex;
		//		returnArray.flags_.isSliceView = true;
		//	}
		//	else
		//	{
		//		uint32_t size = 1;
		//		for(auto i = 0; i < inList.size(); i++)
		//		{
		//			auto slice = inList[i];
		//			auto length = (slice.stop - slice.start) / slice.step;
		//			if(length > 1)
		//			{
		//				returnArray.ndim_++;
		//				returnArray.shape_.dimensions_.push_back(length);
		//				returnArray.strides_.strides_.push_back(size);
		//			}
		//			size *= length;
		//		}
		//		returnArray.size_ = size;
		//		returnArray.newArray();
		//		std::vector<int32_t> coordinate(inList.size(), 0);
		//		for(auto i = 0; i < coordinate.size(); i++)
		//			coordinate[i] = inList[i].start;
		//		//copy the data
		//		auto step = inList.back().step;
		//		for(auto i = 0; i < returnArray.size(); i++)
		//		{
		//			*(returnArray.data_ + i) = this->operator()(coordinate);
		//			auto rank = coordinate.size() - 1;
		//			coordinate[rank] += step;
		//			while(coordinate[rank] > inList[rank].stop)
		//			{
		//				coordinate[rank] = inList[rank].start;
		//				coordinate[++rank] += inList[rank].step;
		//			}
		//		}
		//	}
		//	returnArray.flags_.isValid = true;
		//	return returnArray;
		//}
		// Rectify index to the real index in this dimension
		template<typename... Args>
		void setSliceArray(NdArray<dtype>& array, int32_t& beginIndex, std::vector<Slice>& slices, int32_t index, Args... args)const
		{
			beginIndex += index * strides()[slices.size()];
			slices.push_back(Slice(index));
			setSliceArray(array, beginIndex, slices, args...);
		}
		template<typename... Args>
		void setSliceArray(NdArray<dtype>& array, int32_t& beginIndex, std::vector<Slice>& slices, Slice slice, Args... args)const
		{
			auto length = (slice.stop - slice.start) / slice.step;
			auto parameterPos = slices.size();
			array.shape_.dimensions_.push_back(length);
			array.strides_.strides_.push_back(strides()[parameterPos] * slice.step);
			array.ndim_++;
			array.size_ *= length;
			beginIndex += slice.start * strides()[parameterPos];
			slices.push_back(Slice(slice));
			setSliceArray(array, beginIndex, slices, args...);
		}
		template<typename... Args>
		void setSliceArray(NdArray<dtype>& array, int32_t& beginIndex, std::vector<Slice>& slices, std::initializer_list<int32_t> list, Args... args)const
		{
			setSliceArray(array, beginIndex, slices, Slice(list), args...);
		}
		void setSliceArray(NdArray<dtype>& array, int32_t& beginIndex, std::vector<Slice>& slices)const {}
		NdArray<dtype>& setShape(const std::vector<uint32_t>& inList)
		{
			bool shouldCopy = flags().isSliceView;
			if(!shouldCopy)
			{
				shape_ = Shape(inList);
				ndim_ = inList.size();
				strides_.strides_.resize(ndim());
				uint32_t stride = size();
				for(auto i = 0; i < ndim(); i++)
					strides_[i] = (stride /= shape_[i]);
				flags_.isReshapeView = true;
				return *this;
			}
			else
			{
				NdArray returnArray;
				returnArray.size_ = size();
				returnArray.ndim_ = inList.size();
				returnArray.shape_ = Shape(inList);
				returnArray.newArray();
				std::vector<int32_t> coordinate(inList.size(), 0);
				//copy the data
				for(auto i = 0; i < returnArray.size(); i++)
				{
					*(base_.get() + offset_ + i) = at(coordinate);
					auto rank = coordinate.size() - 1;
					coordinate[rank]++;
					while(coordinate[rank] > shape()[rank])
					{
						coordinate[rank] = 0;
						coordinate[++rank]++;
					}
				}
				return returnArray;
			}
		}
		uint32_t rectifyIndex(int32_t index, uint32_t dimension)const
		{
			if(index >= 0)
			{
				if(index < shape()[dimension])
					return index;
				else
				{
					std::cerr << "Index out of range" << std::endl;
					throw"Index out of range";
				}
			}
			else
			{
				if(std::abs(index) >= shape()[dimension])
				{
					std::cerr << "Index out of range" << std::endl;
					throw"Index out of range";
				}
				else
					return index + shape()[dimension];
			}
		}


	private:
		void newArray()
		{
			base_.reset(new dtype[size()]);
			offset_ = 0;
		}
		void deleteArray()
		{
			/*It shouldn't directly deallocate memory because the smart pointer would destory itself*/
		}

	private:
		Flag flags_;
		Shape shape_;
		Stride strides_;
		uint32_t ndim_{0};
		uint32_t size_{1};
		uint32_t offset_;
		std::shared_ptr<dtype> base_ = std::shared_ptr<dtype>(nullptr, [](dtype* p){
			/*It should automatically check the nullptr*/delete[] p; });
		// Use a smart pointer to manage the data.
	};
}

