#pragma once
#include<random>
namespace Ros{
	namespace random{
		static std::mt19937_64 generator_;
		inline void seed(uint32_t inSeed) { generator_.seed(inSeed); }
	}
}