#pragma once
#include <algorithm>
#include "TArray.h"
#include "UEngineStatics.h"

struct FDynamicBitset
{
	TArray<uint64> Data;

	void Set(size_t idx)
	{
		size_t chunk = idx / 64;
		if (chunk >= Data.size())
		{
			Resize(chunk + 1); // chunk 포함하도록 크기 확장
		}

		Data[chunk] |= 1ULL << (idx % 64);
	}

	void Reset(size_t idx)
	{
		size_t chunk = idx / 64;
		if (chunk >= Data.size())
		{
			Resize(chunk + 1);
		}

		Data[chunk] &= ~(1ULL << (idx % 64));
	}

	bool Test(size_t idx) const
	{
		size_t chunk = idx / 64;
		if (chunk >= Data.size()) return false;
		return (Data[chunk] & (1ULL << (idx % 64))) != 0;
	}

	void Clear() { std::fill(Data.begin(), Data.end(), 0); }

	void Reserve(int n) {
		int chunkCount = (n + 63) / 64;
		Data.reserve(chunkCount);
	}

	size_t Count() const {
		size_t count = 0;
		for (auto block : Data)
			count += __popcnt64(block); // GCC/Clang
		return count;
	}


	FDynamicBitset& operator|=(const FDynamicBitset& other)
	{
		if (other.Data.size() > Data.size())
		{
			Resize(other.Data.size());
		}
		for (size_t i = 0; i < Data.size(); ++i)
			Data[i] |= other.Data[i];
		return *this;
	}

	FDynamicBitset& operator&=(const FDynamicBitset& other)
	{
		size_t minSize = min(Data.size(), other.Data.size());
		for (size_t i = 0; i < minSize; ++i)
			Data[i] &= other.Data[i];

		// 나머지 블록은 0으로 초기화
		for (size_t i = minSize; i < Data.size(); ++i)
			Data[i] = 0;

		return *this;
	}

	void Resize(size_t size)
	{
		if (Data.size() < size)
		{
			Data.resize(size, 0); // push_back 대신 resize 사용
		}
	}
};