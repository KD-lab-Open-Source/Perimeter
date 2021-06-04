#include "stdafxTr.h"

#include <math.h>


#include "pnint.h"

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static int g3[B + B + 2][3];
static int g2[B + B + 2][2];
static int g1[B + B + 2];
static int start = 1;

//#define MUL64(v1, v2) (((__int64)(v1)*(__int64)(v2))>>NOISE_FRACTION)
#define MUL64(v1, v2) (((v1)*(v2))>>NOISE_FRACTION)
//inline __int64 MUL64(__int64 v1, __int64 v2){
//	return (v1*v2)>>NOISE_FRACTION;
//} 
/*inline int MUL64(int v1, int v2){
	int var;
	_asm{
		mov eax, v1
		imul dword ptr v2
		shrd eax, edx, NOISE_FRACTION //edx не меняется !
		mov var, eax
	}
	return var;
}*/


//#define s_curve(t) ( t * t * (3. - 2. * t) )
#define s_curve(t) ( MUL64(MUL64((t), (t)), ((3<<NOISE_FRACTION)-MUL64(2<<NOISE_FRACTION, (t))) ) )

#define lerp(t, a, b) ( (a) + MUL64((t), ((b)-(a))) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + (N<<NOISE_FRACTION);\
	b0 = (t>>NOISE_FRACTION) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t-(t&(~NOISE_FRACTION_MASK));\
	r1 = r0 - (1<<NOISE_FRACTION);
/*
double noise1(double arg)
{
	int bx0, bx1;
	int rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	int rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}*/

float noise3(int vec[3])
{
//	int vec[3];
//	vec[0]=(double)vv[0]*(1<<NOISE_FRACTION);
//	vec[1]=(double)vv[1]*(1<<NOISE_FRACTION);
//	vec[2]=(double)vv[2]*(1<<NOISE_FRACTION);
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	int *q;
	__int64 rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
	register int i, j;


	if (start) {
		start = 0;
		pnintInit();
	}
	setup(0, bx0,bx1, rx0,rx1);

	setup(1, by0,by1, ry0,ry1);

	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

#define at3(rx,ry,rz) ( MUL64(rx, q[0]) + MUL64(ry, q[1]) + MUL64(rz, q[2]) )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	//return (double)lerp(sz, c, d)/(1<<NOISE_FRACTION);
	return (float)lerp(sz, c, d);
}

static void normalize2(float v[2])
{
	float s;

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(int vv[3])
{
	float s;
	double v[3];
	v[0]=(double)vv[0]/(1<<NOISE_FRACTION);
	v[1]=(double)vv[1]/(1<<NOISE_FRACTION);
	v[2]=(double)vv[2]/(1<<NOISE_FRACTION);

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	vv[0] = round((v[0] / s)*(1<<NOISE_FRACTION));
	vv[1] = round((v[1] / s)*(1<<NOISE_FRACTION));
	vv[2] = round((v[2] / s)*(1<<NOISE_FRACTION));
}
//#include "_xtool.h"
//#define rand() XRnd(B + B)
//	XRndVal=0x8000cc00;
void pnintInit(void)
{
	srand(0x1000d580);

	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = round((float)((rand() % (B + B)) - B) / B);

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = round((float)((rand() % (B + B)) - B) / B);
//		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (int)(((float)((rand() % (B + B)) - B) / B)*(1<<NOISE_FRACTION));
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}

void pnintInit(int begRnd)
{
	srand(begRnd);

	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = round((float)((rand() % (B + B)) - B) / B);

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = round((float)((rand() % (B + B)) - B) / B);
//		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (int)(((float)((rand() % (B + B)) - B) / B)*(1<<NOISE_FRACTION));
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}

float turbulence01_08(int point[3])
{
	float t;

	//point[0] = point[0] + 123.456; 
	point[0] = point[0] + 2022703;//14//4045406;//15//505675;//12//8090812;//16//(123.456*(1<<NOISE_FRACTION)); 

	t = 0; 
//	for (freq = 0.1f ; freq < 1.6f ; freq *= 2.) {
	t += (float)fabs(noise3(point)*NOISE_DIVIDER) * 10.f; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += (float)fabs(noise3(point)*NOISE_DIVIDER) * 5.f; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += (float)fabs(noise3(point)*NOISE_DIVIDER) * 2.5f; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += (float)fabs(noise3(point)*NOISE_DIVIDER) *1.25f; 
	return t - 0.3f; /* readjust to make mean value = 0.0 */ 
} 


int turbulence01_08i(int point[3])
{
	//float t;
	int t;

	//point[0] = point[0] + 123.456; 
	point[0] = point[0] + 2022703;//14//4045406;//15//505675;//12//8090812;//16//(123.456*(1<<NOISE_FRACTION)); 

	t = 0; 
//	for (freq = 0.1f ; freq < 1.6f ; freq *= 2.) {
	t += abs(noise3(point)) * 10; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += abs(noise3(point)) * 5; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += abs(noise3(point)) * 2;//2.5f; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += abs(noise3(point)) *1;//1.25f; 
	return t; /* readjust to make mean value = 0.0 */
} 

float turbulence05_10(int point[3])
{
	float t;

	//point[0] = point[0] + 123.456; 
	point[0] = point[0] + 2022703;//14//4045406;//15//505675;//12//8090812;//16//(123.456*(1<<NOISE_FRACTION)); 

	t = 0; 
	t += (float)fabs(noise3(point)*NOISE_DIVIDER) * 2.f; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += (float)fabs(noise3(point)*NOISE_DIVIDER); 
	return t - 0.3f; /* readjust to make mean value = 0.0 */ 
} 

int turbulence05_10i(int point[3])
{
	int t;

	t = 0; 
	t += abs(noise3(point)) * 2; 
	point[0] *= 2; 
	point[1] *= 2; 
	point[2] *= 2; 
	t += abs(noise3(point)); 
	return t; /* readjust to make mean value = 0.0 */
} 
