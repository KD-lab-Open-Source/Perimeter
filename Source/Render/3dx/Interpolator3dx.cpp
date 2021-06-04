#include "StdAfxRd.h"
#include "Interpolator3dx.h"

//out=a0+a1*t5+a2*t5^2+a3*t5^3
//out=a0+a1*(t-0.5)+a2*(t-0.5)^2+a3*(t-0.5)^3
//out=a0+a1*()

//a0new=a0-0.5*a1+0.25*a2-0.125*a3
//a1new=a1-a2+0.75*a3
//a2new=a2-1.5*a3
//a3new=a3


//интерполирует 1 float константно


Interpolator3dx<1> temp;