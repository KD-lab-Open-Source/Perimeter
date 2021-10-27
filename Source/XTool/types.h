#ifndef PERIMETER_TYPES_H
#define PERIMETER_TYPES_H


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

//Win32 types and their equivalent
//TODO these Win32 types should be replaced with standard types in the codebase for portability

#ifdef _WIN32
#else //_WIN32
#include <cstdint>

using BYTE     = uint8_t;     //176 usages
using UINT     = uint32_t;    //80 usages
using HRESULT  = int32_t;     //154 usages
using DWORD    = uint32_t;    //373 usages
using LPCSTR   = const char*; //53 usages
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

#ifdef PERIMETER_HEADLESS
struct POINT {
    int32_t x;
    int32_t y;
};

struct RECT {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

struct SIZE {
    int32_t cx;
    int32_t cy;
};
#endif

#endif //PERIMETER_TYPES_H
