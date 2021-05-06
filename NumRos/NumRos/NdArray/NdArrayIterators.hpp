#pragma once
#include<iterator>
#include<vector>
#include"../Core.hpp"
namespace Ros{
	template  <
		typename dtype,
		typename Category = std::random_access_iterator_tag,
		typename Distance = uint32_t,
		typename Pointer = dtype*,
		typename Reference = dtype&
	>
		class NdArrayIterator
	{
	public:
		typedef dtype     value_type;
		typedef Distance  difference_type;
		typedef Pointer   pointer;
		typedef Reference reference;
		typedef Category  iterator_category;

	public:
		NdArrayIterator(pointer base, uint32_t offset, const Shape& shape, const Stride& stride) :
			shape_(shape), strides_(stride), base_(base), offset_(offset)
		{
			coordinate_ = std::vector<difference_type>(shape.size(), 0);
		}
		NdArrayIterator(const NdArrayIterator& rhs) = default;
		~NdArrayIterator() = default;


	public:
		reference operator*() { return base_[offset_ + calculateRealIndex(coordinate_)]; }
		reference operator*()const { return base_[offset_ + calculateRealIndex(coordinate_)]; }
		pointer operator->()const { return base_ + offset_ + calculateRealIndex(coordinate_); }
		NdArrayIterator& operator++()
		{
			int32_t rank = coordinate_.size() - 1;
			coordinate_[rank]++;
			while(coordinate_[rank] == shape_[rank])
			{
				coordinate_[rank] = 0;
				if(--rank < 0)
				{
					pos_ = PositionTag::TAIL;
					break;
				}
				coordinate_[rank]++;
			}
			return *this;
		}
		NdArrayIterator operator++(int)
		{
			NdArrayIterator returnIterator = *this;
			int32_t rank = coordinate_.size() - 1;
			coordinate_[rank]++;
			while(coordinate_[rank] == shape_[rank])
			{
				coordinate_[rank] = 0;
				if(--rank < 0)
				{
					pos_ = PositionTag::TAIL;
					break;
				}
				coordinate_[rank]++;
			}
			return returnIterator;
		}
		//NdArrayIterator& operator++(int)
		//{
		//	NdArrayIterator returnIterator(*this);
		//	int32_t rank = coordinate_.size() - 1;
		//	coordinate_[rank]++;
		//	while(coordinate_[rank] == shape_[rank])
		//	{
		//		coordinate_[rank] = 0;
		//		coordinate_[--rank]++;
		//	}
		//	return returnIterator;
		//}
		NdArrayIterator& operator--()
		{
			int32_t rank = coordinate_.size() - 1;
			while(coordinate_[rank] == 0)
			{
				coordinate_[rank] = shape_[rank] - 1;
				if(--rank < 0)
				{
					pos_ = PositionTag::HEAD;
					break;
				}
				coordinate_[rank]--;
			}
			coordinate_[rank]--;
			return *this;
		}
		NdArrayIterator& operator+=(difference_type n)
		{
			while(n >= shape_[shape_.size() - 1] - coordinate_.back())
			{
				int32_t rank = shape_.size() - 1;
				n -= (shape_[rank] - coordinate_.back());
				coordinate_[rank] = shape_[rank];
				while(coordinate_[rank] == shape_[rank])
				{
					coordinate_[rank] = 0;
					if(--rank < 0)
					{
						pos_ = PositionTag::TAIL;
						break;
					}
					coordinate_[rank]++;
				}
			}
			coordinate_.back() += n;
			return *this;
		}
		NdArrayIterator& operator-=(difference_type n)
		{
			while(n > coordinate_.back())
			{
				int32_t rank = 0;
				coordinate_[rank] = 0;
				n -= coordinate_.back();
				while(coordinate_[rank] == 0)
				{
					coordinate_[rank] = shape_[rank] - 1;
					if(--rank < 0)
					{
						pos_ = PositionTag::HEAD;
						break;
					}
					coordinate_[rank]--;
				}
			}
			coordinate_.back() -= n;
			return *this;
		}
		bool operator==(const NdArrayIterator<dtype>& rhs)const
		{
			return (base_ == rhs.base_) && (offset_ == rhs.offset_) && (coordinate_ == rhs.coordinate_) &&
				(shape_ == rhs.shape_) && (strides_ == rhs.strides_) && pos_ == rhs.pos_;
		}
		bool operator!=(const  NdArrayIterator<dtype>& rhs) const { return !this->operator==(rhs); }
		//Here is some bugs
		bool operator>(const  NdArrayIterator<dtype>& rhs)const { return coordinate_ > rhs.coordinate_; }
		bool operator<(const NdArrayIterator<dtype>& rhs)const { return coordinate_ < rhs.coordinate_; }
		bool operator>=(const NdArrayIterator<dtype>& rhs)const { return coordinate_ > rhs.coordinate_ || coordinate_ == rhs.coordinate_; }
		bool operator<=(const NdArrayIterator<dtype>& rhs)const { return coordinate_ < rhs.coordinate_ || coordinate_ == rhs.coordinate_; }
		value_type& operator[](difference_type index)const { return *(this->operator+(index)); }


	public:
		friend	difference_type operator-(const NdArrayIterator& lhs, const  NdArrayIterator& rhs)
		{
			return lhs.calculateRealIndex(lhs.coordinate_) - rhs.calculateRealIndex(rhs.coordinate_);
		}

	private:
		difference_type calculateRealIndex(const std::vector<difference_type>& inArray)const
		{
			difference_type realIndex = 0;
			for(auto i = 0; i < coordinate_.size(); i++)

				realIndex += inArray[i] * strides_[i];
			return realIndex;
		}

	private:
		std::vector<difference_type> coordinate_;
		Shape shape_;
		Stride strides_;
		uint32_t offset_;
		pointer base_;
		enum class PositionTag
		{
			HEAD,
			MID,
			TAIL
		}pos_{PositionTag::MID};
	};
	template<typename dtype>
	NdArrayIterator<dtype> operator+(const NdArrayIterator<dtype>& iterator, uint32_t n)
	{
		return (NdArrayIterator<dtype>(iterator) += n);
	}
	template<typename dtype>
	NdArrayIterator<dtype> operator+(uint32_t n, const NdArrayIterator<dtype>& iterator)
	{
		return (NdArrayIterator<dtype>(iterator) += n);
	}
	template<typename dtype>
	NdArrayIterator<dtype> operator-(const NdArrayIterator<dtype>& iterator, uint32_t n)
	{
		return (NdArrayIterator<dtype>(iterator) -= n);
	}



	//---------------------------------------------------------------------------------------------------------------------------



	template  <
		typename dtype,
		typename Category = std::random_access_iterator_tag,
		typename Distance = uint32_t,
		typename Pointer = const dtype*,
		typename Reference = const dtype&
	>
		class NdArrayConstIterator
	{
	public:
		typedef dtype     value_type;
		typedef Distance  difference_type;
		typedef Pointer   pointer;
		typedef Reference reference;
		typedef Category  iterator_category;

	public:
		NdArrayConstIterator(pointer base, uint32_t offset, const Shape& shape, const Stride& stride) :
			shape_(shape), strides_(stride), base_(base), offset_(offset)
		{
			coordinate_ = std::vector<difference_type>(shape.size(), 0);
		}
		NdArrayConstIterator(const NdArrayIterator<dtype>& iterator) :
			shape_(iterator.shape_), strides_(iterator.stride_), base_(iterator.base_), offset_(iterator.offset_), coordinate_(iterator.coordinate_)
		{
		}
		~NdArrayConstIterator() = default;

	public:
		reference operator*() { return base_[offset_ + calculateRealIndex(coordinate_)]; }
		reference operator*()const { return base_[offset_ + calculateRealIndex(coordinate_)]; }
		pointer operator->()const { return base_ + offset_ + calculateRealIndex(coordinate_); }
		NdArrayConstIterator& operator++()
		{
			int32_t rank = coordinate_.size() - 1;
			coordinate_[rank]++;
			while(coordinate_[rank] == shape_[rank])
			{
				coordinate_[rank] = 0;
				if(--rank < 0)
				{
					pos_ = PositionTag::TAIL;
					break;
				}
				coordinate_[rank]++;
			}
			return *this;
		}
		NdArrayConstIterator& operator--()
		{
			int32_t rank = coordinate_.size() - 1;
			while(coordinate_[rank] == 0)
			{
				coordinate_[rank] = shape_[rank] - 1;
				if(--rank < 0)
				{
					pos_ = PositionTag::HEAD;
					break;
				}
				coordinate_[rank]--;
			}
			coordinate_[rank]--;
			return *this;
		}

		NdArrayConstIterator& operator+=(difference_type n)
		{
			while(n >= shape_[shape_.size() - 1] - coordinate_.back())
			{
				int32_t rank = shape_.size() - 1;
				n -= (shape_[rank] - coordinate_.back());
				coordinate_[rank] = shape_[rank];
				while(coordinate_[rank] == shape_[rank])
				{
					coordinate_[rank] = 0;
					if(--rank < 0)
					{
						pos_ = PositionTag::TAIL;
						break;
					}
					coordinate_[rank]++;
				}
			}
			coordinate_.back() += n;
			return *this;
		}
		bool operator==(const NdArrayConstIterator<dtype>& rhs)const
		{
			return (base_ == rhs.base_) && (offset_ == rhs.offset_) && (coordinate_ == rhs.coordinate_)
				&& (shape_ == rhs.shape_) && (strides_ == rhs.strides_) && pos_ == rhs.pos_;
		}
		bool operator!=(const  NdArrayConstIterator<dtype>& rhs) const { return !this->operator==(rhs); }
		NdArrayConstIterator& operator-=(difference_type n)
		{
			while(n > coordinate_.back())
			{
				int32_t rank = 0;
				coordinate_[rank] = 0;
				n -= coordinate_.back();
				while(coordinate_[rank] == 0)
				{
					coordinate_[rank] = shape_[rank] - 1;
					coordinate_[++rank]--;
				}
			}
			coordinate_.back() -= n;
			return *this;
		}
		bool operator>(const  NdArrayConstIterator<dtype>& rhs)const { return coordinate_ > rhs.coordinate_; }
		bool operator<(const NdArrayConstIterator<dtype>& rhs)const { return coordinate_ < rhs.coordinate_; }
		bool operator>=(const NdArrayConstIterator<dtype>& rhs)const { return coordinate_ > rhs.coordinate_ || coordinate_ == rhs.coordinate_; }
		bool operator<=(const NdArrayConstIterator<dtype>& rhs)const { return coordinate_ < rhs.coordinate_ || coordinate_ == rhs.coordinate_; }
		value_type operator[](difference_type index)const { return *(this->operator+(index)); }

	public:
		friend	difference_type operator-(const NdArrayConstIterator& lhs, const  NdArrayConstIterator& rhs)
		{
			return lhs.calculateRealIndex(lhs.coordinate_) - rhs.calculateRealIndex(rhs.coordinate_);
		}

	private:
		difference_type calculateRealIndex(const std::vector<difference_type>& inArray)const
		{
			difference_type realIndex = 0;
			for(auto i = 0; i < coordinate_.size(); i++)
				realIndex += inArray[i] * strides_[i];
			return realIndex;
		}

	private:
		std::vector<difference_type> coordinate_;
		Shape shape_;
		Stride strides_;
		uint32_t offset_;
		pointer base_;
		enum class PositionTag
		{
			HEAD,
			MID,
			TAIL
		}pos_{PositionTag::MID};
	};
	template<typename dtype>
	NdArrayConstIterator<dtype> operator+(const NdArrayConstIterator<dtype>& iterator, uint32_t n)
	{
		return (NdArrayConstIterator<dtype>(iterator) += n);
	}
	template<typename dtype>
	NdArrayConstIterator<dtype> operator+(uint32_t n, const NdArrayConstIterator<dtype>& iterator)
	{
		return (NdArrayConstIterator<dtype>(iterator) += n);
	}
	template<typename dtype>
	NdArrayConstIterator<dtype> operator-(const NdArrayConstIterator<dtype>& iterator, uint32_t n)
	{
		return (NdArrayConstIterator<dtype>(iterator) -= n);
	}
}
