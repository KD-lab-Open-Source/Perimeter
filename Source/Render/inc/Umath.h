#ifndef __UMATH_H__
#define __UMATH_H__

#include "xmath.h"
#include <SDL_endian.h>

#define ABS(a)										((a) >= 0 ? (a) :-(a))

struct sColor4f
{
    union {
        struct {
            float r, g, b, a;
        };
        Vect4f v;
    };
	
	sColor4f() 											{ }
	sColor4f(float r_,float g_,float b_,float a_=1.f)		{ r = r_; g = g_; b = b_; a = a_; }
	sColor4f(const float v[4])                          { r = v[0]; g = v[1]; b = v[2]; a = v[3]; }
	inline explicit sColor4f(const struct sColor4c& color);

	inline void set(float r_,float g_,float b_,float a_)	{ r = r_; g = g_; b = b_; a = a_; }
	inline sColor4f& operator += (const sColor4f &color)	{ r+=color.r; g+=color.g; b+=color.b; a+=color.a; return *this; }
	inline sColor4f& operator -= (const sColor4f &color)	{ r-=color.r; g-=color.g; b-=color.b; a-=color.a; return *this; }
	inline sColor4f& operator *= (const sColor4f &color)	{ r*=color.r; g*=color.g; b*=color.b; a*=color.a; return *this; }
	inline sColor4f& operator *= (float f)			{ r*=f; g*=f; b*=f; a*=f; return *this; }
	inline sColor4f& operator /= (float f)			{ if(f!=0) f=1/f; else f=0.001f; r*=f; g*=f; b*=f; a*=f; return *this; }
	inline sColor4f	operator + (const sColor4f &color) const	{ sColor4f tmp(r+color.r,g+color.g,b+color.b,a+color.a); return tmp; }
	inline sColor4f	operator - (const sColor4f &color) const	{ sColor4f tmp(r-color.r,g-color.g,b-color.b,a-color.a); return tmp; }
	inline sColor4f	operator * (const sColor4f &color) const	{ sColor4f tmp(r*color.r,g*color.g,b*color.b,a*color.a); return tmp; }
	inline sColor4f	operator * (float f) const 		{ sColor4f tmp(r*f,g*f,b*f,a*f); return tmp; }
	inline sColor4f	operator / (float f) const 		{ if(f!=0.f) f=1/f; else f=0.001f; sColor4f tmp(r*f,g*f,b*f,a*f); return tmp; }
	inline int GetR() const 						{ return (int)xm::round(255*r); }
	inline int GetG() const 						{ return (int)xm::round(255*g); }
	inline int GetB() const 						{ return (int)xm::round(255*b); }
	inline int GetA() const 						{ return (int)xm::round(255*a); }
	inline uint32_t RGBA() const 						{ return ((int)xm::round(255*r) << 16) | ((int)xm::round(255*g) << 8) | (int)xm::round(255*b) | ((int)xm::round(255*a) << 24); }
	inline uint32_t GetRGB() const 					{ return ((int)xm::round(255*r) << 16) | ((int)xm::round(255*g) << 8) | (int)xm::round(255*b); }
	inline uint32_t RGBGDI() const 					{ return (int)xm::round(255*r) | ((int)xm::round(255*g) << 8) | ((int)xm::round(255*b) << 16); }
	inline void interpolate(const sColor4f &u,const sColor4f &v,float f) { r=u.r+(v.r-u.r)*f; g=u.g+(v.g-u.g)*f; b=u.b+(v.b-u.b)*f; a=u.a+(v.a-u.a)*f; }
	inline void interpolate3(const sColor4f &u,const sColor4f &v,float f) { r=u.r+(v.r-u.r)*f; g=u.g+(v.g-u.g)*f; b=u.b+(v.b-u.b)*f; }
	inline bool operator == (const sColor4f &color) const { return (r == color.r) && (g == color.g) && (b == color.b) && (a == color.a); }
	inline bool operator != (const sColor4f &color) const { return !(*this == color); }
};

#define CONVERT_COLOR_TO_ARGB(c) (SDL_SwapLE32(c))
#define CONVERT_COLOR_TO_ABGR(c) ((SDL_SwapBE32(c) & 0xFFFFFF00)  >> 8 | (SDL_SwapLE32(c) & 0xFF000000))

struct sColor4c
{
    union {
        struct {
            uint8_t b, g, r, a;
        };
        uint32_t v;
    };
	
	sColor4c()										{ }
	sColor4c(const sColor4f& color)					{ set(color.GetR(),color.GetG(),color.GetB(),color.GetA()); }
	sColor4c(int rc,int gc,int bc)					{ r=rc; g=gc; b=bc; a=255; }
	sColor4c(int rc,int gc,int bc,int ac)			{ r=rc; g=gc; b=bc; a=ac; }
	typedef int Int4[4];
	sColor4c(const Int4& v) { r = v[0]; g = v[1]; b = v[2]; a = v[3]; }
	inline void set(int rc,int gc,int bc)			{ r=rc; g=gc; b=bc; a=255; }
	inline void set(int rc,int gc,int bc,int ac)	{ r=rc; g=gc; b=bc; a=ac; }
	inline void set(const sColor4f& color)			{ set(color.GetR(),color.GetG(),color.GetB(),color.GetA()); }
	inline sColor4c& operator *= (float f)			{ r=xm::round(r*f); g=xm::round(g*f); b=xm::round(b*f); a=xm::round(a*f); return *this; }
	inline sColor4c& operator += (sColor4c &p)		{ r+=p.r; g+=p.g; b+=p.b; a+=p.a; return *this; }
	inline sColor4c& operator -= (sColor4c &p)		{ r-=p.r; g-=p.g; b-=p.b; a-=p.a; return *this; }
	inline sColor4c operator + (sColor4c &p)		{ return sColor4c(r+p.r,g+p.g,b+p.b,a+p.a); }
	inline sColor4c operator - (sColor4c &p)		{ return sColor4c(r-p.r,g-p.g,b-p.b,a-p.a); }
	inline sColor4c operator * (int f) const 		{ return sColor4c(r*f,g*f,b*f,a*f); }
	inline sColor4c operator / (int f) const 		{ if(f!=0) f=(1<<16)/f; else f=1<<16; return sColor4c((r*f)>>16,(g*f)>>16,(b*f)>>16,(a*f)>>16); }
	unsigned char& operator[](int i)				{ return ((unsigned char*)this)[i];}
    inline uint32_t ARGB() const                    { return CONVERT_COLOR_TO_ARGB(v); }
    inline void ARGB(uint32_t argb)                 { v = SDL_SwapLE32(argb); }
    inline uint32_t ABGR() const                    { return CONVERT_COLOR_TO_ABGR(v); }

	inline void interpolate(const sColor4c &u,const sColor4c &v,float f) { r=xm::round(u.r+(v.r-u.r)*f); g=xm::round(u.g+(v.g-u.g)*f); b=xm::round(u.b+(v.b-u.b)*f); a=xm::round(u.a+(v.a-u.a)*f); }
};

std::string toColorCode(const sColor4f& color);

sColor4f::sColor4f(const sColor4c& color) 
{ 
	r = color.r/255.f; 
	g = color.g/255.f; 
	b = color.b/255.f; 
	a = color.a/255.f; 
}

struct sRectangle4f
{
	Vect2f	min;
	Vect2f	max;
	
	sRectangle4f()									{ }
	sRectangle4f(float xmin,float ymin,float xmax,float ymax)	{ min.x=xmin; min.y=ymin; max.x=xmax; max.y=ymax; }
	sRectangle4f(sRectangle4f &rect)				{ min=rect.min; max=rect.max; }
	inline void set(float xmin,float ymin,float xmax,float ymax){ min.x=xmin; min.y=ymin; max.x=xmax; max.y=ymax; }
	inline float xmin() const						{ return min.x; }
	inline float ymin() const						{ return min.y; }
	inline float xmax() const						{ return max.x; }
	inline float ymax() const						{ return max.y; }
	inline float& xmin()							{ return min.x; }
	inline float& ymin()							{ return min.y; }
	inline float& xmax()							{ return max.x; }
	inline float& ymax()							{ return max.y; }
};

struct sBox6f
{
	Vect3f	min;
	Vect3f	max;

	sBox6f()												{ }
	sBox6f(float xmin,float ymin,float zmin,float xmax,float ymax,float zmax)	{ min.x=xmin; min.y=ymin; min.z=zmin; max.x=xmax; max.y=ymax; max.z=zmax; }
	sBox6f(const sBox6f &rect)					{ min=rect.min; max=rect.max; }
	inline void AddBound(const Vect3f& v)
	{
		if(min.x>v.x) min.x=v.x; if(max.x<v.x) max.x=v.x;
		if(min.y>v.y) min.y=v.y; if(max.y<v.y) max.y=v.y;
		if(min.z>v.z) min.z=v.z; if(max.z<v.z) max.z=v.z; 
	}

	inline sBox6f& operator += (const Vect3f& v){ min+=v; max+=v; return *this; }
	inline sBox6f& operator -= (const Vect3f& v){ min-=v; max-=v; return *this; }
	void SetInvalidBox()						{min.set(1e30f,1e30f,1e30f);max.set(-1e30f,-1e30f,-1e30f);}
	float GetRadius()							{ if(min.x>max.x) return 0; return max.distance(min)*0.5f;}

	inline float xmin() const					{ return min.x; }
	inline float xmax() const					{ return max.x; }
	inline float ymin() const					{ return min.y; }
	inline float ymax() const					{ return max.y; }
	inline float zmin() const					{ return min.z; }
	inline float zmax() const					{ return max.z; }
	inline float& xmin()						{ return min.x; }
	inline float& xmax()						{ return max.x; }
	inline float& ymin()						{ return min.y; }
	inline float& ymax()						{ return max.y; }
	inline float& zmin()						{ return min.z; }
	inline float& zmax()						{ return max.z; }
};

struct sPlane4f
{
	float A,B,C,D;

	sPlane4f()												{ }
	sPlane4f(float a,float b,float c,float d)				{ A=a,B=b,C=c,D=d; }
	sPlane4f(const Vect3f& a,const Vect3f& b,const Vect3f& c)
	{ // инициализация плоскости по трем точкам
		A=(b.y-a.y)*(c.z-a.z)-(c.y-a.y)*(b.z-a.z);
		B=(b.z-a.z)*(c.x-a.x)-(c.z-a.z)*(b.x-a.x);
		C=(b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
		GetNormal().normalize();
		D=-A*a.x-B*a.y-C*a.z;
	}
	inline void Set(const Vect3f& a,const Vect3f& b,const Vect3f& c)
	{ // инициализация плоскости по трем точкам
		A=(b.y-a.y)*(c.z-a.z)-(c.y-a.y)*(b.z-a.z);
		B=(b.z-a.z)*(c.x-a.x)-(c.z-a.z)*(b.x-a.x);
		C=(b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
		GetNormal().normalize();
		D=-A*a.x-B*a.y-C*a.z;
	}
	inline float GetDistance(const Vect3f& a)
	{ // расстояние от точки до плоскости
		float t=A*a.x+B*a.y+C*a.z+D;
		return t;
	}
	inline float GetCross(const Vect3f& a,const Vect3f& b)
	{ // поиск пересечения данной плоскости с прямой заданной двумя точками a и b
		Vect3f v=a-b;
		float t=A*v.x+B*v.y+C*v.z;
		if(t==0) return 0;			// прямая и плоскость параллельны
		t=(A*a.x+B*a.y+C*a.z+D)/t;
		return t;
	}
	Vect3f& GetNormal()							{ return *(Vect3f*)&A; }
	inline void xform(MatXf &mat)
	{
		Vect3f p;
		mat.xformPoint(GetNormal()*D,p);
		GetNormal()=mat.xformVect(GetNormal());
		D=-A*p.x-B*p.y-C*p.z;
	}
	inline void GetReflectionMatrix(const MatXf &in,MatXf &out)
	{ // out - ReflectionMatrix
		MatXf RefSurface(	Mat3f(	1-2*A*A,	-2*A*B,		-2*A*C,
									-2*B*A,		1-2*B*B,	-2*B*C,
									-2*C*A,		-2*C*B,		1-2*C*C),
							Vect3f(	-2*D*A,		-2*D*B,		-2*D*C));
		out=in*RefSurface;
	}
};

inline float GetDistFromPointToLine(Vect3f& Point,Vect3f& aLine,Vect3f& bLine)
{
	Vect3f v=bLine-aLine, n; 
	n.cross( v, Point-aLine );
	return xm::sqrt(n.norm2() / v.norm2());
}

inline int ReturnBit(int a)
{
	int count=0;
	while((a>>=1)>=1) count++;
	return count;
}
inline float LinearInterpolate(float a,float b,float x)
{
	return a+(b-a)*x;
}
inline float CosInterpolate(float a,float b,float x)
{
	float f= (1- xm::cos(x * 3.14159f)) * 0.5f;
	return a*(1-f)+b*f;
}
inline float ArcLengthInterpolate(float a,float b,float x)
{
	return a+((1-x)*x+x)*(b-a);
}
inline float QuadratInterpolate(float v0,float v1,float v2,float x) // v0 <= x <= v1 <= v2
{
	float b=(4*v1-3*v0-v2)*0.5f;
	float a=v1-v0-b;
	float c=v0;
	return (a*x+b)*x+c;
}
inline float CubicInterpolate(float v0,float v1,float v2,float v3,float x) // v0 <= v1 <= x <= v2 <= v3
{
	float p=(v3-v2)-(v0-v1);
	float q=(v0-v1)-p;
	float r=v2-v0;
	float s=v1;
	return ((p*x+q)*x+r)*x+s;
}

void Vect3fNormalize(Vect3f* out, const Vect3f* pv);
void Vect3fTransformNormal(Vect3f* out, const Vect3f* pv, const Mat4f* pm);
Mat4f* Mat4fInverse(Mat4f* out, float* determinant, const Mat4f* mp);
Mat4f* Mat4fLookAtLH(Mat4f *out, const Vect3f *eye, const Vect3f *at, const Vect3f *up);
void Mat4fConvert(const Mat4f& m, const Vect3f& in,Vect3f& pv,Vect3f& pe);

inline void Mat4fSetTransposedMatXf(Mat4f& mat4, const MatXf& matX) {
    mat4.set(matX).xpose();
}

void MatrixInterpolate(MatXf& out,const MatXf& a,const MatXf& b,float f);

float HermitSpline(float t,float p0,float p1,float p2,float p3);
Vect3f HermitSpline(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3);
float HermitSplineDerivation(float t,float p0,float p1,float p2,float p3);
Vect3f HermitSplineDerivation(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3);
float HermitSplineDerivation2(float t,float p0,float p1,float p2,float p3);
Vect3f HermitSplineDerivation2(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3);

void CheckMatrix(const MatXf& m);

void SetOrthographic(Mat4f& out, int width, int height, int near, int far);

inline MatXf GetMatrix(const Vect3f& Pos,const Vect3f& Angle)
{
	return MatXf(
		Mat3f((float)G2R(Angle.x), X_AXIS)*
		Mat3f((float)G2R(Angle.y), Y_AXIS)*
		Mat3f((float)G2R(Angle.z), Z_AXIS), 
		Pos);
}
inline void SetPosition(MatXf &Matrix,const Vect3f& Pos,const Vect3f& Angle)
{
	Matrix = MatXf(
		Mat3f((float)G2R(Angle.x), X_AXIS)*
		Mat3f((float)G2R(Angle.y), Y_AXIS)*
		Mat3f((float)G2R(Angle.z), Z_AXIS), 
		Pos);
}
inline void SetPosition(MatXf &Matrix,float *matrix)
{
	Matrix.rot()[0][0]=matrix[0]; Matrix.rot()[1][0]=matrix[1]; Matrix.rot()[2][0]=matrix[2]; 
	Matrix.rot()[0][1]=matrix[3]; Matrix.rot()[1][1]=matrix[4]; Matrix.rot()[2][1]=matrix[5]; 
	Matrix.rot()[0][2]=matrix[6]; Matrix.rot()[1][2]=matrix[7]; Matrix.rot()[2][2]=matrix[8]; 
	Matrix.trans().set(matrix[9],matrix[10],matrix[11]);
}
inline void Identity(MatXf &Matrix)
{
	Matrix.set(Mat3f(Vect3f(1,0,0),Vect3f(0,1,0),Vect3f(0,0,1)),Vect3f(0,0,0));
}
inline void Translate(MatXf &Matrix,const Vect3f& v)
{
	MatXf t(Mat3f::ID,v);
	Matrix*=t;	
}
inline void Rotate(MatXf &Matrix,const QuatF &q)
{
	MatXf r(Mat3f(q),Vect3f(0,0,0)); 
	Matrix*=r;
}
inline void Rotate(MatXf &Matrix,const Vect3f& Angle)
{
	MatXf r(Mat3f((float)G2R(Angle.x), X_AXIS)*
			Mat3f((float)G2R(Angle.y), Y_AXIS)*
			Mat3f((float)G2R(Angle.z), Z_AXIS),
			Vect3f(0,0,0));
	Matrix*=r;
}
inline void Scale(MatXf &Matrix,const Vect3f& v)
{
	MatXf s(Mat3f(
		Vect3f(	v.x,	0,		0),
		Vect3f(	0,		v.y,	0),
		Vect3f(	0,		0,		v.z)),
		Vect3f(	0,		0,		0)); 
	Matrix*=s;
}
inline void GetScale(const MatXf &Matrix,Vect3f& v)
{
	Vect3f xcol=Matrix.rot().xcol();
	Vect3f ycol=Matrix.rot().ycol();
	Vect3f zcol=Matrix.rot().zcol();
	v.set(xcol.norm(),ycol.norm(),zcol.norm());
}
inline Vect3f GetMin(const Vect3f& u,const Vect3f& v)
{
	Vect3f tmp=v;
	if(u.x<tmp.x) tmp.x=u.x;
	if(u.y<tmp.y) tmp.y=u.y;
	if(u.z<tmp.z) tmp.z=u.z;
	return tmp;
}
inline Vect3f GetMax(const Vect3f& u,const Vect3f& v)
{
	Vect3f tmp=v;
	if(u.x>tmp.x) tmp.x=u.x;
	if(u.y>tmp.y) tmp.y=u.y;
	if(u.z>tmp.z) tmp.z=u.z;
	return tmp;
}

inline void SetMatrix(MatXf &Matrix,const Vect3f& vPos, const Vect3f& vTarget)
{
	Vect3f z_axis=vTarget-vPos;
	z_axis.normalize();
	Vect3f x_axis=z_axis%Vect3f::K;
	x_axis.normalize();
	Vect3f y_axis=z_axis%x_axis;
	Mat3f R(
		Vect3f::I, Vect3f::J, Vect3f::K, // from_basis
		x_axis, y_axis, z_axis // to_basis
		);
	Matrix.set(R,vPos);
}
#define XR2L(a)						(+(a))
#define YR2L(a)						(-(a))
#define ZR2L(a)						(+(a))
inline void RightToLeft(MatXf &Matrix)
{
	Matrix.rot()(1,1)= XR2L(XR2L(Matrix.rot()(1,1)));
	Matrix.rot()(1,2)= XR2L(YR2L(Matrix.rot()(1,2)));
	Matrix.rot()(1,3)= XR2L(ZR2L(Matrix.rot()(1,3)));

	Matrix.rot()(2,1)= YR2L(XR2L(Matrix.rot()(2,1)));
	Matrix.rot()(2,2)= YR2L(YR2L(Matrix.rot()(2,2)));
	Matrix.rot()(2,3)= YR2L(ZR2L(Matrix.rot()(2,3)));

	Matrix.rot()(3,1)= ZR2L(XR2L(Matrix.rot()(3,1)));
	Matrix.rot()(3,2)= ZR2L(YR2L(Matrix.rot()(3,2)));
	Matrix.rot()(3,3)= ZR2L(ZR2L(Matrix.rot()(3,3)));

	Matrix.trans().x= XR2L(Matrix.trans().x);
	Matrix.trans().y= YR2L(Matrix.trans().y);
	Matrix.trans().z= ZR2L(Matrix.trans().z);
}
inline void RightToLeft(Vect3f& v)
{
	v.set(XR2L(v.x),YR2L(v.y),ZR2L(v.z));
}
inline void RightToLeft(Vect2f &v)
{
	v.set(XR2L(v.x),YR2L(v.y));
}

inline float FastInvSqrt(float x)
{ // return 1/xm::sqrt(x)
    const float x2 = x * 0.5F;
    const float threehalfs = 1.5F;

    union {
        float f;
        uint32_t i;
    } conv  = { x };
    conv.i  = 0x5f3759df - ( conv.i >> 1 );
    conv.f  *= threehalfs - ( x2 * conv.f * conv.f );
    return conv.f;

    /*
	float y;
	_asm
	{
		mov     eax, 07F000000h+03F800000h // (ONE_AS_INTEGER<<1) + ONE_AS_INTEGER
		sub     eax, x
		sar     eax, 1

		mov     y, eax                      // y
		fld     _0_47                       // 0.47
		fmul    uint32_t PTR x                 // x*0.47

		fld     uint32_t PTR y
		fld     st(0)                       // y y x*0.47
		fmul    st(0), st(1)                // y*y y x*0.47

		fld     _1_47                       // 1.47 y*y y x*0.47
		fxch    st(3)                       // x*0.47 y*y y 1.47
		fmulp   st(1), st(0)                // x*0.47*y*y y 1.47
		fsubp   st(2), st(0)                // y 1.47-x*0.47*y*y
		fmulp   st(1), st(0)                // result
		fstp    y
		and     y, 07FFFFFFFh
	}
	return y;
//	return (3.0f-x*(y*y))*y*0.5f; // для повышенной точности
     */
}
inline void FastNormalize(Vect2f &v)
{ 
	v*=FastInvSqrt(v.norm2());
}
inline void FastNormalize(Vect3f& v)
{ 
	v*=FastInvSqrt(v.norm2());
}

#endif // __UMATH_H__