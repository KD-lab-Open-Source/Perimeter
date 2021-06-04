#include "stdafxTr.h"

#include <math.h>
#include <wtypes.h>
#include "fastmath.h"



//static unsigned int fast_sqrt_table[0x10000];  // declare table of square roots 
unsigned int fast_sqrt_table[0x10000];  // declare table of square roots 

typedef union FastSqrtUnion
{
  float f;
  unsigned int i;
} FastSqrtUnion;

void  build_sqrt_table()
{
  unsigned int i;
  FastSqrtUnion s;
  
  for (i = 0; i <= 0x7FFF; i++)
  {
    
    // Build a float with the bit pattern i as mantissa
    //  and an exponent of 0, stored as 127
    
    s.i = (i << 8) | (0x7F << 23);
    s.f = (float)sqrt(s.f);
    
    // Take the square root then strip the first 7 bits of
    //  the mantissa into the table
    
    fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);
    
    // Repeat the process, this time with an exponent of 1, 
    //  stored as 128
    
    s.i = (i << 8) | (0x80 << 23);
    s.f = (float)sqrt(s.f);
    
    fast_sqrt_table[i] = (s.i & 0x7FFFFF);
  }
}




// At the assembly level the recommended workaround for the second FIST bug is the same for the first; 
// inserting the FRNDINT instruction immediately preceding the FIST instruction. 

__forceinline void FloatToInt(int *int_pointer, float f) 
{
	__asm  fld  f
  __asm  mov  edx,int_pointer
  __asm  FRNDINT
  __asm  fistp dword ptr [edx];

}


/*
int main(int argc, char* argv[])
{

  float t, it, test_sqrt;
  int i = 0;

  build_sqrt_table();

  t = 1234.121234f;
  
  test_sqrt = fastsqrt(t);
  printf("sqrt expected %20.10f  approx %20.10f\n", sqrt(t), test_sqrt);

  FP_INV(it,t);
  printf("inv  expected %20.10f  approx %20.10f\n", 1/t, it);


  i = 0xdeafbabe;

  FloatToInt(&i, t);
  printf("ftol expected %d  actual %d %08X\n", (int)t, i, i);



  return 0;
}
*/


//#define eps 0.0001f
#define eps 1.f
float  fastsqrtN (float x)
{
    float  sq, sqold, EPS;

    if (x < 0.0f)
        return -1.0f;
    if (x == 0.0f)
        return 0.0f;  /* может привести к делению на 0 */
    EPS = x * eps;
    sq = x;
    sqold = x + 30.0f;         /* != sq */
    while (fabs (sq * sq - x) >= EPS) {
    /*     fabs( sq - sqold )>= EPS    */
        sqold = sq;
        sq = 0.5f * (sq + x / sq);
    }
    return sq;
}


unsigned short sqrtTable4IntegerCalculate[1024];
void init_sqrtTable4IntegerCalculate(void)
{
	int i;
	for(i=0; i<1024; i++) sqrtTable4IntegerCalculate[i]=round(sqrtf(i)*2048);
}


/*

       db 66h; xor si, si	    { xor esi, esi }
       les si, dword ptr SqrtTable
       db 66h; mov bx, word ptr S   { mov ebx, S }
       mov dx, 11
       db 66h, 0Fh, 0BDh, 0CBh	    { bsr ecx, ebx }
       sub cx, 9; jle @less
       shr cx, 1
       adc cx, 0
       sub dx, cx
       shl cx, 1
       db 66h; shr bx, cl	    { shr ebx, cl }
@less:
       db 26h, 67h, 8Bh, 04h, 5Eh   { mov ax, es:[esi+ebx*2] }
       mov cx, dx
       shr ax, cl
end;

begin
  New( SqrtTable );
  for i:=0 to 1023 do
    SqrtTable^[i]:=Round(Sqrt(i)*2048);
end.
*/
