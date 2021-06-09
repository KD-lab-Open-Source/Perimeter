#include "../shade.inl"
//-----------------------------------------------------------------------------
// 4x4 percentage closer filter
//-----------------------------------------------------------------------------

// jittered sample offset table for 1k shadow maps
def c0, -0.000692, -0.000868, -0.002347, 0.000450
def c1, 0.000773, -0.002042, -0.001592, 0.001880
def c2, -0.001208, -0.001198, -0.000425, -0.000915
def c3, -0.000050, 0.000105, -0.000753, 0.001719
def c4, -0.001855, -0.000004, 0.001140, -0.001212
def c5, 0.000684, 0.000273, 0.000177, 0.000647
def c6, -0.001448, 0.002095, 0.000811, 0.000421
def c7, 0.000542, 0.001491, 0.000537, 0.002367

// (0, 1, sample weight (1/16), z-bias)
def c18, SHADE, 1.0, 0.0625, -0.0025

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
add r4.xy, t1, c2.xyzw
add r5.xy, t1, c2.wzyx
add r6.xy, t1, c3.xyzw
add r7.xy, t1, c3.wzyx

// fetch two rows
texld r0, r0, s1
texld r1, r1, s1
texld r2, r2, s1
texld r3, r3, s1
texld r4, r4, s1
texld r5, r5, s1
texld r6, r6, s1
texld r7, r7, s1

// compare first row
sub r0.r, r8, r0.r
sub r0.g, r8, r1.r
sub r0.b, r8, r2.r
sub r0.a, r8, r3.r
cmp r9, -r0, c18.y, c18.x

// sum of weighted first row
dp4 r10, r9, c18.z

// compare second row
sub r0.r, r8, r4.r
sub r0.g, r8, r5.r
sub r0.b, r8, r6.r
sub r0.a, r8, r7.r
cmp r9, -r0, c18.y, c18.x

// sum of weighted second row
dp4 r9, r9, c18.z

// accumulate second row
add r10, r10, r9

// coordinates for third and fourth rows
add r0.xy, t1, c4.xyzw
add r1.xy, t1, c4.wzyx
add r2.xy, t1, c5.xyzw
add r3.xy, t1, c5.wzyx
add r4.xy, t1, c6.xyzw
add r5.xy, t1, c6.wzyx
add r6.xy, t1, c7.xyzw
add r7.xy, t1, c7.wzyx

// fetch two rows
texld r0, r0, s1
texld r1, r1, s1
texld r2, r2, s1
texld r3, r3, s1
texld r4, r4, s1
texld r5, r5, s1
texld r6, r6, s1
texld r7, r7, s1

// compare third row
sub r0.r, r8, r0.r
sub r0.g, r8, r1.r
sub r0.b, r8, r2.r
sub r0.a, r8, r3.r
cmp r9, -r0, c18.y, c18.x

// sum of weighted third row
dp4 r9, r9, c18.z

// accumulate third row
add r10, r10, r9

// compare fourth row
sub r0.r, r8, r4.r
sub r0.g, r8, r5.r
sub r0.b, r8, r6.r
sub r0.a, r8, r7.r
cmp r9, -r0, c18.y, c18.x

// sum of weighted fourth row
dp4 r9, r9, c18.z

// accumulate fourth row
add r10, r10, r9

//r10 - затенение