//-----------------------------------------------------------------------------
// 2xx percentage closer filter
//-----------------------------------------------------------------------------

#include "../shade.inl"
// jittered sample offset table for 1k shadow maps
#define ccx 0.0005
def c0, -ccx, -ccx, ccx, ccx
def c1, -ccx, ccx, -ccx, ccx

// (0, 1, sample weight (1/16), z-bias)
def c18, SHADE, 1.0, 0.25, -0.005

dcl t1.xy
dcl t2.x
dcl_2d s1

// add z-bias 
add r8, t2.x, c18.w

// coordinates for first two rows
add r0.xy, t1, c0.xyzw
add r1.xy, t1, c0.wzyx
add r2.xy, t1, c1.xyzw
add r3.xy, t1, c1.wzyx

// fetch two rows
texld r0, r0, s1
texld r1, r1, s1
texld r2, r2, s1
texld r3, r3, s1

// compare first row
sub r0.r, r8, r0.r
sub r0.g, r8, r1.r
sub r0.b, r8, r2.r
sub r0.a, r8, r3.r
cmp r9, -r0, c18.y, c18.x

// sum of weighted first row
dp4 r10, r9, c18.z

//r10 - затенение