#include "stdafxTr.h"

/***************************************************************** 
NOTE: I DID NOT WRITE THE FOLLOWING CODE 
noise function over R3 - implemented by a pseudorandom tricubic spline 
By Ken Perlin, New York University 
*****************************************************************/ 
#include <stdlib.h>
#include <stdio.h>

#define DOT(a,b) (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]) 
#define B 256 
static int p[B + B + 2]; 
static float g[B + B + 2][3]; 

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + 10000.f;\
	b0 = ((int)t) & (B-1);\
	b1 = (b0+1) & (B-1);\
	r0 = t - (int)t;\
	r1 = r0 - 1.f;

float noise3D(float vec[3]) 
{ 
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11; 
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sx, sy, sz, a, b, c, d, t, u, v; 
	int i, j;

	setup(0, bx0,bx1, rx0,rx1); 
	setup(1, by0,by1, ry0,ry1); 
	setup(2, bz0,bz1, rz0,rz1); 

	i = p[ bx0 ]; 
	j = p[ bx1 ]; 

	b00 = p[ i + by0 ]; 
	b10 = p[ j + by0 ]; 
	b01 = p[ i + by1 ]; 
	b11 = p[ j + by1 ]; 
	#define at(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] ) 
	#define surve(t) ( t * t * (3.f - 2.f * t) ) 
	#define lerp(t, a, b) ( a + t * (b - a) ) 

	sx = surve(rx0); 
	sy = surve(ry0); 
	sz = surve(rz0); 

	q = g[ b00 + bz0 ] ; u = at(rx0,ry0,rz0); 
	q = g[ b10 + bz0 ] ; v = at(rx1,ry0,rz0); 
	a = lerp(sx, u, v); 

	q = g[ b01 + bz0 ] ; u = at(rx0,ry1,rz0); 
	q = g[ b11 + bz0 ] ; v = at(rx1,ry1,rz0); 
	b = lerp(sx, u, v); 

	c = lerp(sy, a, b); /* interpolate in y at lo x */ 

	q = g[ b00 + bz1 ] ; u = at(rx0,ry0,rz1); 
	q = g[ b10 + bz1 ] ; v = at(rx1,ry0,rz1); 
	a = lerp(sx, u, v); 

	q = g[ b01 + bz1 ] ; u = at(rx0,ry1,rz1); 
	q = g[ b11 + bz1 ] ; v = at(rx1,ry1,rz1); 
	b = lerp(sx, u, v); 

	d = lerp(sy, a, b); /* interpolate in y at hi x */ 

	return 1.5f * lerp(sz, c, d); /* interpolate in z */ 
}
void initNoise() 
{ 
	/*long random();*/ 
	int i, j, k; 
	float v[3], s; 
	/* Create an array of random gradient vectors uniformly on the unit sphere */ 
	/*srandom(1);*/ 
	srand(1); 
	for (i = 0 ; i < B ; i++) {
		do {
			/* Choose uniformly in a cube */ 
			for (j=0 ; j<3 ; j++)
				v[j] = (float)((rand() % (B + B)) - B) / B;
			s = DOT(v,v); 
		} while (s > 1.0); 
		/* If not in sphere try again */ 
		s = (float)xm::sqrt(s); 
		for (j = 0 ; j < 3 ; j++) /* Else normalize */ 
			g[i][j] = v[j] / s; 
	} 

	/* Create a pseudorandom permutation of [1..B] */ 
	for (i = 0 ; i < B ; i++) 
		p[i] = i; 
	for (i = B ; i > 0 ; i -= 2) { 
		k = p[i]; 
		p[i] = p[j = rand() % B]; 
		p[j] = k; 
	} 

	/* Extend g and p arrays to allow for faster indexing */ 
	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i]; 
		for (j = 0 ; j < 3 ; j++) 
			g[B + i][j] = g[i][j]; 
	} 
} 

float turbulence(float point[3], float lofreq, float hifreq) 
{ 
	float freq, t, p[3]; 

	p[0] = point[0] + 123.456f; 
	p[1] = point[1]; 
	p[2] = point[2]; 

	t = 0; 
	for (freq = lofreq ; freq < hifreq ; freq *= 2.) {
extern float noise3(float vec[3]);
		t += (float) xm::abs(noise3(p)) / freq; 
		p[0] *= 2.; 
		p[1] *= 2.; 
		p[2] *= 2.; 
	} 
	return t - 0.3f; /* readjust to make mean value = 0.0 */ 
} 


//turbulence(VAR, 0.1f, 1.6f)
/*float turbulence01_08(float point[3])
{ 
	float freq, t;

	point[0] = point[0] + 123.456; 

	t = 0; 
//	for (freq = 0.1f ; freq < 1.6f ; freq *= 2.) {
extern float noise3(float vec[3]);
	t += fabs(noise3(point)) * 10.f; 
	point[0] *= 2.; 
	point[1] *= 2.; 
	point[2] *= 2.; 
	t += fabs(noise3(point)) * 5.f; 
	point[0] *= 2.; 
	point[1] *= 2.; 
	point[2] *= 2.; 
	t += fabs(noise3(point)) * 2.5f; 
	point[0] *= 2.; 
	point[1] *= 2.; 
	point[2] *= 2.; 
	t += xm::fabs(noise3(point)) *1.25f; 
	return t - 0.3; 
} */

