#ifndef __PNINT_H__
#define __PNINT_H__


#define NOISE_FRACTION 14
const int NOISE_FRACTION_MASK=(1<<NOISE_FRACTION)-1;
const float NOISE_DIVIDER=1.0f/(1<<NOISE_FRACTION);

extern void pnintInit(void);
extern void pnintInit(int begRnd);
extern float turbulence01_08(int point[3]);
extern float turbulence05_10(int point[3]);
extern int turbulence05_10i(int point[3]);


extern int turbulence01_08i(int point[3]);

#endif //__PNINT_H__