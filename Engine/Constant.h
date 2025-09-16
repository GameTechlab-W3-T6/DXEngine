#pragma once
#include "UEngineStatics.h"

enum class EViewModeIndex : uint32
{
    VMI_Lit,
    VMI_Unlit,
    VMI_Wireframe,
};

enum class EEngineShowFlags : uint64
{
    SF_Primitives,
    SF_BillboardText,
    Default
};

inline EEngineShowFlags operator|(EEngineShowFlags a, EEngineShowFlags b) {
    return (EEngineShowFlags)((uint64_t)a | (uint64_t)b);
}
inline EEngineShowFlags operator&(EEngineShowFlags a, EEngineShowFlags b) {
    return (EEngineShowFlags)((uint64_t)a & (uint64_t)b);
}
inline EEngineShowFlags operator~(EEngineShowFlags a) {
    return (EEngineShowFlags)(~(uint64_t)a);
}