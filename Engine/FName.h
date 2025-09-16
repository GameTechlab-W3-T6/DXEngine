#pragma once
#include "UEngineStatics.h"
#include "TArray.h"

struct FName
{
private:
	static inline TArray<FString> data;
	static inline int nextIndex = 0;
public:
	int32 DisplayIndex;
	int32 ComparisonIndex;

	FName(char* pStr) : FName(FString(pStr)){}
	FName(FString str = "") {
		auto it = std::find(data.begin(), data.end(), str);
		if (it != data.end())
			ComparisonIndex = static_cast<int32>(std::distance(data.begin(), it));
		else {
			data.push_back(str);
			ComparisonIndex = nextIndex++;
		}
		DisplayIndex = ComparisonIndex;
	}
	int32 Compare(const FName& other) const {
		return ComparisonIndex - other.ComparisonIndex;
	}
	bool operator==(const FName& other) const {
		return Compare(other) == 0;
	}
	FString ToString() const { return data[DisplayIndex]; }
};

