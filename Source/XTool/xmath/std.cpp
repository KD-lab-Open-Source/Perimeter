//Uses std math functions

#include "xmath.h"

#ifndef XMATH_USE_GAMEMATH
#include <cmath>

namespace xm {
    double floor(double x) { return std::floor(x); }    
    double ceil(double x) { return std::ceil(x); }    
    double round(double x) { return std::round(x); }
    int abs(int x) { return std::abs(x); }
    float abs(float x) { return std::fabs(x); }
    double abs(double x) { return std::abs(x); }    
    double sqrt(double x) { return std::sqrt(x); }
    float sqrt(float x) { return std::sqrt(x); }
    double log(double x) { return std::log(x); }    
    double exp(double x) { return std::exp(x); }    
    double sin(double x) { return std::sin(x); }    
    float sin(float x) { return std::sin(x); }    
    double cos(double x) { return std::cos(x); }    
    float cos(float x) { return std::cos(x); }
    double acos(double x) { return std::acos(x); }    
    float acos(float x) { return std::acos(x); }    
    double tan(double x) { return std::tan(x); }    
    float tan(float x) { return std::tan(x); }
    float atan(float x) { return std::atan(x); };
    double atan2(double x, double y) { return std::atan2(x, y); };
    float atan2(float x, float y) { return std::atan2(x, y); };
    double fmod(double x, double y) { return std::fmod(x, y); };
    float fmod(float x, float y) { return std::fmod(x, y); };
    double pow(double x, double y) { return std::pow(x, y); };
}
#endif
