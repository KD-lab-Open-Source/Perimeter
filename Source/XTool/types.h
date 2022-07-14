#ifndef PERIMETER_TYPES_H
#define PERIMETER_TYPES_H

#include <cstdint>

#ifndef PERIMETER_D3D9
constexpr const int TRUE  = 1;
constexpr const int FALSE = 0;
#endif

//These structs should match Win32 structs
struct sPoint {
    int32_t x;
    int32_t y;
};

struct sRect {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

/*
struct SIZE {
    int32_t cx;
    int32_t cy;
};
*/

//This function does type cast and checks if both types are same width
template<typename ST, typename CT>
constexpr CT checked_reinterpret_cast(ST r) {
    static_assert(sizeof(ST) == sizeof(CT), "Source type is not same size as destination!");
    return reinterpret_cast<CT>(r);
}

//This function does ptr cast and checks if both types are same width
template<typename ST, typename CT>
constexpr CT* checked_reinterpret_cast_ptr(ST* r) {
    static_assert(sizeof(ST) == sizeof(CT), "Source ptr type is not same size as destination ptr type!");
    return reinterpret_cast<CT*>(r);
}

//This function does ref cast and checks if both types are same width
template<typename ST, typename CT>
constexpr CT& checked_reinterpret_cast_ref(ST& r) {
    static_assert(sizeof(ST) == sizeof(CT), "Source ref type is not same size as destination ref type!");
    return reinterpret_cast<CT&>(r);
}


#ifndef _WIN32
//Win32 types and their equivalent
//TODO these Win32 types should be replaced with standard types in the codebase for portability

using HRESULT  = int32_t;     //154 usages
using HANDLE   = void*;       //20 usages
using HWND     = HANDLE;      //47 usages

/*
#ifdef PERIMETER_ARCH_64
typedef uint64_t UINT_PTR;
typedef int64_t LONG_PTR;
#else
typedef uint32_t UINT_PTR;
typedef int32_t LONG_PTR;
#endif
typedef UINT_PTR WPARAM;
typedef LONG_PTR LPARAM;
*/
#endif //_WIN32

#endif //PERIMETER_TYPES_H
