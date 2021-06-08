#include "stdafxTr.h"


/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#include <math.h>

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];
static int start = 1;

static void init(void);

#define s_curve(t) ( t * t * (3.f - 2.f * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
		r0 = t - (int)t;\
	r1 = r0 - 1.f;

float noise1(float arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

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
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

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
}

float noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (start) {
		start = 0;
		init();
	}
/*
#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;
*/
	//setup(0, bx0,bx1, rx0,rx1);
	t = vec[0] + N;
	bx0 = ((int)t) & BM;
	bx1 = (bx0+1)  & BM;
	rx0 = t - (int)t;
	rx1 = rx0 - 1.0f;

	//setup(1, by0,by1, ry0,ry1);
	t = vec[1] + N;
	by0 = ((int)t) & BM;
	by1 = (by0+1)  & BM;
	ry0 = t - (int)t;
	ry1 = ry0 - 1.0f;

	//setup(2, bz0,bz1, rz0,rz1);
	t = vec[2] + N;
	bz0 = ((int)t) & BM;
	bz1 = (bz0+1)  & BM;
	rz0 = t - (int)t;
	rz1 = rz0 - 1.0f;

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	//t  = s_curve(rx0);
	t = rx0*rx0*(3.f - 2.f*rx0);
	//sy = s_curve(ry0);
	sy= ry0*ry0*(3.f - 2.f*ry0);
	//sz = s_curve(rz0);
	sz= rz0*rz0*(3.f - 2.f*rz0);

//#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u=rx0*q[0] + ry0*q[1] + rz0*q[2]; //u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v=rx1*q[0] + ry0*q[1] + rz0*q[2]; //v = at3(rx1,ry0,rz0);
	a = u + t*(v-u); //a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u=rx0*q[0] + ry1*q[1] + rz0*q[2]; //u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v=rx1*q[0] + ry1*q[1] + rz0*q[2]; //v = at3(rx1,ry1,rz0);
	b = u + t*(v-u); //b = lerp(t, u, v);

	c = a + sy*(b-a); //c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u=rx0*q[0] + ry0*q[1] + rz1*q[2]; //u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v=rx1*q[0] + ry0*q[1] + rz1*q[2]; //v = at3(rx1,ry0,rz1);
	a = u + t*(v-u); //a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u=rx0*q[0] + ry1*q[1] + rz1*q[2]; //u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v=rx1*q[0] + ry1*q[1] + rz1*q[2]; //v = at3(rx1,ry1,rz1);
	b = u + t*(v-u); //b = lerp(t, u, v);

	d = a + sy*(b-a); //d = lerp(sy, a, b);

	return (c + sz*(d-c)); //lerp(sz, c, d);
}

static void normalize2(float v[2])
{
	float s;

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(float v[3])
{
	float s;

	s = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}
//#include "_xtool.h"
//#define rand() XRnd(B + B)
//	XRndVal=0x8000cc00;
static void init(void)
{
	srand(0x1000d580);

	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((rand() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((rand() % (B + B)) - B) / B;
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

