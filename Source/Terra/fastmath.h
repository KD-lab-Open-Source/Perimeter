
#define FP_BITS(fp) (*(DWORD *)&(fp))
#define FP_ABS_BITS(fp) (FP_BITS(fp)&0x7FFFFFFF)
#define FP_SIGN_BIT(fp) (FP_BITS(fp)&0x80000000)
#define FP_ONE_BITS 0x3F800000


// r = 1/p
#define FP_INV(r,p)                                                          \
{                                                                            \
    int _i = 2 * FP_ONE_BITS - *(int *)&(p);                                 \
    r = *(float *)&_i;                                                       \
    r = r * (2.0f - (p) * r);                                                \
}

#define FP_EXP(e,p)                                                          \
{                                                                            \
    int _i;                                                                  \
    e = -1.44269504f * (float)0x00800000 * (p);                              \
    _i = (int)e + 0x3F800000;                                                \
    e = *(float *)&_i;                                                       \
}

#define FP_NORM_TO_BYTE(i,p)                                                 \
{                                                                            \
    float _n = (p) + 1.0f;                                                   \
    i = *(int *)&_n;                                                         \
    if (i >= 0x40000000)     i = 0xFF;                                       \
    else if (i <=0x3F800000) i = 0;                                          \
    else i = ((i) >> 15) & 0xFF;                                             \
}

inline unsigned long FP_NORM_TO_BYTE2(float p)                                                 
{                                                                            
  float fpTmp = p + 1.0f;                                                      
  return ((*(unsigned *)&fpTmp) >> 15) & 0xFF;  
}

inline unsigned long FP_NORM_TO_BYTE3(float p)     
{
  float ftmp = p + 12582912.0f;                                                      
  return ((*(unsigned long *)&ftmp) & 0xFF);
}


extern unsigned int fast_sqrt_table[0x10000];

inline float fastsqrt(float n)
{
  
  if (FP_BITS(n) == 0)
    return 0.0;                 // check for square root of 0
  
  FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);
  
  return n;
}

extern float  fastsqrtN (float x);


extern unsigned short sqrtTable4IntegerCalculate[1024];
extern void init_sqrtTable4IntegerCalculate(void);

inline int fastsqrtI(int s)
{
    //TODO convert this to C
#ifdef _MSC_VER
	_asm{
		xor eax,eax
		xor esi,esi
		lea esi, sqrtTable4IntegerCalculate
		mov ebx, s
		mov edx, 11
		bsr ecx, ebx
		sub ecx, 9
		jle loc_skip
		shr ecx, 1
		adc ecx, 0
		sub edx, ecx
		shl ecx, 1
		shr ebx, cl
loc_skip:
		mov ax, [esi+ebx*2]
		mov ecx, edx
		shr eax, cl
		mov s, eax
	}
#else
	s = (int)fastsqrt((float)s);
#endif
	return s;
}
