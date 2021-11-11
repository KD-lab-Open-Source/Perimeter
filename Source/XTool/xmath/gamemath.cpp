//Uses gamemath math functions

#include "xmath.h"

#ifdef XMATH_USE_GAMEMATH
#include <gmath.h>

namespace xm {
    double floor(double x) { return gm_floor(x); }    
    double ceil(double x) { return gm_ceil(x); }    
    double round(double x) { return gm_round(x); }
    int abs(int x) { return x < 0 ? x * -1 : x; }
    float abs(float x) { return gm_fabsf(x); }
    double abs(double x) { return gm_fabs(x); }    
    double sqrt(double x) { return gm_sqrt(x); }
    float sqrt(float x) { return gm_sqrtf(x); }
    double log(double x) { return gm_log(x); }    
    double exp(double x) { return gm_exp(x); }    
    double sin(double x) { return gm_sin(x); }    
    float sin(float x) { return gm_sinf(x); }    
    double cos(double x) { return gm_cos(x); }    
    float cos(float x) { return gm_cosf(x); }
    double acos(double x) { return gm_acos(x); }    
    float acos(float x) { return gm_acosf(x); }    
    double tan(double x) { return gm_tan(x); }    
    float tan(float x) { return gm_tanf(x); }
    float atan(float x) { return gm_atanf(x); };
    double atan2(double x, double y) { return gm_atan2(x, y); };
    float atan2(float x, float y) { return gm_atan2f(x, y); };
    double fmod(double x, double y) { return gm_fmod(x, y); };
    float fmod(float x, float y) { return gm_fmodf(x, y); };
    double pow(double x, double y) { return gm_pow(x, y); };
}
#endif
