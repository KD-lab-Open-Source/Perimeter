#ifndef __UMATH_H__
#define __UMATH_H__

#include <math.h>
#include "xmath.h"

#define ABS(a)										((a) >= 0 ? (a) :-(a))

struct sVect3c
{
	char x,y,z;

	sVect3c()										{ }
	sVect3c(int _x,int _y,int _z)						{ x=_x; y=_y; z=_z; }
	sVect3c(sVect3c &v)								{ x=v.x; y=v.y; z=v.z; }
	sVect3c(Vect3f& v)								{ x=round(v.x); y=round(v.y); z=round(v.z); }

	inline int norm()								{ return round(sqrtf((float)(x*x+y*y+z*z))); }
	inline int norm2()								{ return x*x+y*y+z*z; }
	inline int distance(const sVect3c &v)			{ int dx=v.x-x,dy=v.y-y,dz=v.z-z; return round(sqrtf((float)(dx*dx+dy*dy+dz*dz))); }
	inline void normalize(int norma)					{ float f=(float)norma/(float)sqrtf((float)(x*x+y*y+z*z)); x=round(x*f); y=round(y*f); z=round(z*f); }
	inline void set(int x,int y,int z)				{ sVect3c::x=x; sVect3c::y=y; sVect3c::z=z; }
	inline sVect3c& operator += (const sVect3c& v)	{ x+=v.x; y+=v.y; z+=v.z; return *this; }
	inline sVect3c& operator -= (const sVect3c& v)	{ x-=v.x; y-=v.y; z-=v.z; return *this; }
	inline sVect3c& operator *= (const sVect3c& v)	{ x*=v.x; y*=v.y; z*=v.z; return *this; }
	inline sVect3c& operator *= (float f)			{ x=round(x*f); y=round(y*f); z=round(z*f); return *this; }
	inline sVect3c& operator /= (float f)			{ if(f!=0.f) f=1/f; else f=0.0001f; x=round(x*f); y=round(y*f); z=round(z*f); return *this; }
	inline sVect3c& operator = (const sVect3c& v)	{ x=v.x; y=v.y; z=v.z; return *this; }
	inline sVect3c operator - (const sVect3c &v)	{ sVect3c tmp(x-v.x,y-v.y,z-v.z); return tmp; }
	inline sVect3c operator + (const sVect3c &v)	{ sVect3c tmp(x+v.x,y+v.y,z+v.z); return tmp; }
	inline sVect3c operator * (const sVect3c &v)	{ sVect3c tmp(x*v.x,y*v.y,z*v.z); return tmp; }
	inline sVect3c operator * (float f)				{ sVect3c tmp(round(x*f),round(y*f),round(z*f)); return tmp; }
	inline sVect3c operator / (float f)				{ if(f!=0.f) f=1/f; else f=0.0001f; sVect3c tmp(round(x*f),round(y*f),round(z*f)); return tmp; }
	inline int operator == (const sVect3c& v)		{ return ((x==v.x)&&(y==v.y)&&(z==v.z)); }
	inline operator float* () const					{ return (float*)this; }
};

struct sColor4f
{
	float r,g,b,a;
	
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
	inline int GetR() const 						{ return round(255*r); }
	inline int GetG() const 						{ return round(255*g); }
	inline int GetB() const 						{ return round(255*b); }
	inline int GetA() const 						{ return round(255*a); }
	inline DWORD RGBA() const 						{ return (round(255*r) << 16) | (round(255*g) << 8) | round(255*b) | (round(255*a) << 24); }
	inline DWORD GetRGB() const 					{ return (round(255*r) << 16) | (round(255*g) << 8) | round(255*b); }
	inline DWORD RGBGDI() const 					{ return round(255*r) | (round(255*g) << 8) | (round(255*b) << 16); }
	inline void interpolate(const sColor4f &u,const sColor4f &v,float f) { r=u.r+(v.r-u.r)*f; g=u.g+(v.g-u.g)*f; b=u.b+(v.b-u.b)*f; a=u.a+(v.a-u.a)*f; }
	inline void interpolate3(const sColor4f &u,const sColor4f &v,float f) { r=u.r+(v.r-u.r)*f; g=u.g+(v.g-u.g)*f; b=u.b+(v.b-u.b)*f; }
	inline bool operator == (const sColor4f &color) const { return (r == color.r) && (g == color.g) && (b == color.b) && (a == color.a); }
	inline bool operator != (const sColor4f &color) const { return !(*this == color); }
};

struct sColor4c
{
	unsigned char b,g,r,a;
	
	sColor4c()										{ }
	sColor4c(const sColor4f& color)					{ set(color.GetR(),color.GetG(),color.GetB(),color.GetA()); }
	sColor4c(int rc,int gc,int bc)					{ r=rc; g=gc; b=bc; a=255; }
	sColor4c(int rc,int gc,int bc,int ac)			{ r=rc; g=gc; b=bc; a=ac; }
	typedef int Int4[4];
	sColor4c(const Int4& v) { r = v[0]; g = v[1]; b = v[2]; a = v[3]; }
	inline void set(int rc,int gc,int bc)			{ r=rc; g=gc; b=bc; a=255; }
	inline void set(int rc,int gc,int bc,int ac)	{ r=rc; g=gc; b=bc; a=ac; }
	inline void set(const sColor4f& color)			{ set(color.GetR(),color.GetG(),color.GetB(),color.GetA()); }
	inline sColor4c& operator *= (float f)			{ r=round(r*f); g=round(g*f); b=round(b*f); a=round(a*f); return *this; }
	inline sColor4c& operator += (sColor4c &p)		{ r+=p.r; g+=p.g; b+=p.b; a+=p.a; return *this; }
	inline sColor4c& operator -= (sColor4c &p)		{ r-=p.r; g-=p.g; b-=p.b; a-=p.a; return *this; }
	inline sColor4c operator + (sColor4c &p)		{ return sColor4c(r+p.r,g+p.g,b+p.b,a+p.a); }
	inline sColor4c operator - (sColor4c &p)		{ return sColor4c(r-p.r,g-p.g,b-p.b,a-p.a); }
	inline sColor4c operator * (int f) const 		{ return sColor4c(r*f,g*f,b*f,a*f); }
	inline sColor4c operator / (int f) const 		{ if(f!=0) f=(1<<16)/f; else f=1<<16; return sColor4c((r*f)>>16,(g*f)>>16,(b*f)>>16,(a*f)>>16); }
	inline DWORD  RGBA() const 						{ return ((const DWORD*)this)[0]; }
	inline DWORD& RGBA()							{ return ((DWORD*)this)[0]; }
	unsigned char& operator[](int i)				{ return ((unsigned char*)this)[i];}

	inline void interpolate(const sColor4c &u,const sColor4c &v,float f) { r=round(u.r+(v.r-u.r)*f); g=round(u.g+(v.g-u.g)*f); b=round(u.b+(v.b-u.b)*f); a=round(u.a+(v.a-u.a)*f); }
};

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
	inline void GetReflectionVector(const Vect3f& in,Vect3f& out)
	{ // out - поиск отражение вектора от плоскости
		out=in-2*dot(GetNormal(),in)*GetNormal();
	}
};

inline float GetDistFromPointToLine(Vect3f& Point,Vect3f& aLine,Vect3f& bLine)
{
	Vect3f v=bLine-aLine, n; 
	n.cross( v, Point-aLine );
	return sqrtf( n.norm2() / v.norm2() );
}
inline float GetDistRaySphere(Vect3f& PointRay,Vect3f& DirectionRay,Vect3f& PosSphere,float Radius)
{
	Vect3f vRaySphere=PosSphere-PointRay, n;
	n.cross( DirectionRay, vRaySphere );
	float d = sqrtf( n.norm2()/DirectionRay.norm2() );
	if( d > Radius || vRaySphere.dot(DirectionRay)<0 ) return -1;
	return d;
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
	float f=(1-cosf(x*3.14159f))*0.5f;
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

void MatrixInterpolate(MatXf& out,const MatXf& a,const MatXf& b,float f);

float HermitSpline(float t,float p0,float p1,float p2,float p3);
Vect3f HermitSpline(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3);
float HermitSplineDerivation(float t,float p0,float p1,float p2,float p3);
Vect3f HermitSplineDerivation(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3);
float HermitSplineDerivation2(float t,float p0,float p1,float p2,float p3);
Vect3f HermitSplineDerivation2(float t,const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3);

float TriangleArea(float x1,float y1,float x2,float y2,float x3,float y3);
void CheckMatrix(const MatXf& m);

inline float TriangleArea(const Vect3f v1,const Vect3f v2,const Vect3f v3)
{
	Vect3f n;
	n.cross(v2-v1,v3-v1);
	return 0.5f*n.norm();
}

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
/*
	float xx2=q.x()*q.x()*2.f, yy2=q.y()*q.y()*2.f, zz2=q.z()*q.z()*2.f;
	float wx2=q.s()*q.x()*2.f, wy2=q.s()*q.y()*2.f, wz2=q.s()*q.z()*2.f;
	float xy2=q.x()*q.y()*2.f, xz2=q.x()*q.z()*2.f, yz2=q.y()*q.z()*2.f;
	MatXf r(Mat3f(
		Vect3f(	1.0f-yy2-zz2,	xy2-wz2,		xz2+wy2),
		Vect3f(	xy2+wz2,		1.0f-xx2-zz2,	yz2-wx2),
		Vect3f(	xz2-wy2,		yz2+wx2,		1.0f-xx2-yy2)),
		Vect3f(	0,				0,				0)); 
*/
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
inline Vect3f GetMin(const Vect3f& u,Vect3f& v)
{
	Vect3f tmp=v;
	if(u.x<tmp.x) tmp.x=u.x;
	if(u.y<tmp.y) tmp.y=u.y;
	if(u.z<tmp.z) tmp.z=u.z;
	return tmp;
}
inline Vect3f GetMax(const Vect3f& u,Vect3f& v)
{
	Vect3f tmp=v;
	if(u.x>tmp.x) tmp.x=u.x;
	if(u.y>tmp.y) tmp.y=u.y;
	if(u.z>tmp.z) tmp.z=u.z;
	return tmp;
}
inline void GetRotate(const MatXf &Matrix,QuatF &q)
{
	q.set(Matrix.rot());
}
inline void GetTranslate(const MatXf &Matrix,Vect3f& v)
{
	v=Matrix.trans();
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
inline void RightToLeft(QuatF &q)
{
	q.set(XR2L(YR2L(ZR2L(q.s()))),XR2L(q.x()),YR2L(q.y()),ZR2L(q.z()));
}
inline void BuildMatrixProjectByPlane(MatXf &m,const sPlane4f &p,const Vect3f& v)
{ // построение матрицы проецирования объекта на плоскость по направлению 
	float dot=p.A*v.x+p.B*v.y+p.C*v.z;
	m.rot()[0][0]=dot-v.x*p.A;
	m.rot()[0][1]=0.f-v.x*p.B;
	m.rot()[0][2]=0.f-v.x*p.C;
	m.trans().x  =0.f-v.x*p.D;

	m.rot()[1][0]=0.f-v.y*p.A;
	m.rot()[1][1]=dot-v.y*p.B;
	m.rot()[1][2]=0.f-v.y*p.C;
	m.trans().y  =0.f-v.y*p.D;
	
	m.rot()[2][0]=0.f-v.z*p.A;
	m.rot()[2][1]=0.f-v.z*p.B;
	m.rot()[2][2]=dot-v.z*p.C;
	m.trans().z  =0.f-v.z*p.D;
}
const float _0_47 = 0.47f;
const float _1_47 = 1.47f;
inline float FastInvSqrt(float x)
{ // return 1/sqrt(x)
	float y;
	_asm
	{
		mov     eax, 07F000000h+03F800000h // (ONE_AS_INTEGER<<1) + ONE_AS_INTEGER
		sub     eax, x
		sar     eax, 1

		mov     y, eax                      // y
		fld     _0_47                       // 0.47
		fmul    DWORD PTR x                 // x*0.47

		fld     DWORD PTR y
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
}
inline void FastNormalize(Vect2f &v)
{ 
	v*=FastInvSqrt(v.norm2());
}
inline void FastNormalize(Vect3f& v)
{ 
	v*=FastInvSqrt(v.norm2());
}

inline float CrossLine(float x1,float y1,float m1,float n1,
					   float x2,float y2,float m2,float n2)
{
	float f = n2*m1-n1*m2;
	if( fabs(f)<0.0001f ) return -1e30f; // parallel
	return (m2*(y1-y2)-n2*(x1-x2))/f;
}

struct D3DXMATRIX;
class CMatrix // : public D3DMATRIX
{
public:
	float _11,_12,_13,_14;
	float _21,_22,_23,_24; 
	float _31,_32,_33,_34;
	float _41,_42,_43,_44;

	operator D3DXMATRIX*(){return (D3DXMATRIX*)this;}

	CMatrix()													{}
	CMatrix(float m11,float m12,float m13,float m14,
			float m21,float m22,float m23,float m24,
			float m31,float m32,float m33,float m34,
			float m41,float m42,float m43,float m44)		{ _11=m11,_12=m12,_13=m13,_14=m14; _21=m21,_22=m22,_23=m23,_24=m24; _31=m31,_32=m32,_33=m33,_34=m34; _41=m41,_42=m42,_43=m43,_44=m44; }
	inline void set(	
			float m11,float m12,float m13,float m14,
			float m21,float m22,float m23,float m24,
			float m31,float m32,float m33,float m34,
			float m41,float m42,float m43,float m44)		{ _11=m11,_12=m12,_13=m13,_14=m14; _21=m21,_22=m22,_23=m23,_24=m24; _31=m31,_32=m32,_33=m33,_34=m34; _41=m41,_42=m42,_43=m43,_44=m44; }
	inline CMatrix& operator *= (float f)
	{
		_11*=f, _12*=f, _13*=f, _14*=f;
		_21*=f, _22*=f, _23*=f, _24*=f; 
		_31*=f, _32*=f, _33*=f, _34*=f;
		_41*=f, _42*=f, _43*=f, _44*=f;
		return *this;
	}
	inline CMatrix operator * (const CMatrix &m) const
	{
		return CMatrix( 
			_11*m._11+_12*m._21+_13*m._31+_14*m._41, _11*m._12+_12*m._22+_13*m._32+_14*m._42, _11*m._13+_12*m._23+_13*m._33+_14*m._43, _11*m._14+_12*m._24+_13*m._34+_14*m._44,
			_21*m._11+_22*m._21+_23*m._31+_24*m._41, _21*m._12+_22*m._22+_23*m._32+_24*m._42, _21*m._13+_22*m._23+_23*m._33+_24*m._43, _21*m._14+_22*m._24+_23*m._34+_24*m._44,
			_31*m._11+_32*m._21+_33*m._31+_34*m._41, _31*m._12+_32*m._22+_33*m._32+_34*m._42, _31*m._13+_32*m._23+_33*m._33+_34*m._43, _31*m._14+_32*m._24+_33*m._34+_34*m._44,
			_41*m._11+_42*m._21+_43*m._31+_44*m._41, _41*m._12+_42*m._22+_43*m._32+_44*m._42, _41*m._13+_42*m._23+_43*m._33+_44*m._43, _41*m._14+_42*m._24+_43*m._34+_44*m._44);
	}
	inline void postmult(const CMatrix &m)
	{
		set(_11*m._11+_12*m._21+_13*m._31+_14*m._41, _11*m._12+_12*m._22+_13*m._32+_14*m._42, _11*m._13+_12*m._23+_13*m._33+_14*m._43, _11*m._14+_12*m._24+_13*m._34+_14*m._44,
			_21*m._11+_22*m._21+_23*m._31+_24*m._41, _21*m._12+_22*m._22+_23*m._32+_24*m._42, _21*m._13+_22*m._23+_23*m._33+_24*m._43, _21*m._14+_22*m._24+_23*m._34+_24*m._44,
			_31*m._11+_32*m._21+_33*m._31+_34*m._41, _31*m._12+_32*m._22+_33*m._32+_34*m._42, _31*m._13+_32*m._23+_33*m._33+_34*m._43, _31*m._14+_32*m._24+_33*m._34+_34*m._44,
			_41*m._11+_42*m._21+_43*m._31+_44*m._41, _41*m._12+_42*m._22+_43*m._32+_44*m._42, _41*m._13+_42*m._23+_43*m._33+_44*m._43, _41*m._14+_42*m._24+_43*m._34+_44*m._44);
	}
	inline void premult(const CMatrix &m)
	{
		set(m._11*_11+m._12*_21+m._13*_31+m._14*_41, m._11*_12+m._12*_22+m._13*_32+m._14*_42, m._11*_13+m._12*_23+m._13*_33+m._14*_43, m._11*_14+m._12*_24+m._13*_34+m._14*_44,
			m._21*_11+m._22*_21+m._23*_31+m._24*_41, m._21*_12+m._22*_22+m._23*_32+m._24*_42, m._21*_13+m._22*_23+m._23*_33+m._24*_43, m._21*_14+m._22*_24+m._23*_34+m._24*_44,
			m._31*_11+m._32*_21+m._33*_31+m._34*_41, m._31*_12+m._32*_22+m._33*_32+m._34*_42, m._31*_13+m._32*_23+m._33*_33+m._34*_43, m._31*_14+m._32*_24+m._33*_34+m._34*_44,
			m._41*_11+m._42*_21+m._43*_31+m._44*_41, m._41*_12+m._42*_22+m._43*_32+m._44*_42, m._41*_13+m._42*_23+m._43*_33+m._44*_43, m._41*_14+m._42*_24+m._43*_34+m._44*_44);
	}
	inline CMatrix& operator *=(const CMatrix &m)
	{
		postmult(m);
		return *this;
	}
	inline void identity()
	{
		_11=1, _12=0, _13=0; _14=0;
		_21=0, _22=1, _23=0; _24=0;
		_31=0, _32=0, _33=1; _34=0;
		_41=0, _42=0, _43=0; _44=1;
	}
	inline void invert(const CMatrix &m)
	{
		_11=m._11, _12=m._21, _13=m._31; _14=m._14;
		_21=m._12, _22=m._22, _23=m._32; _24=m._24;
		_31=m._13, _32=m._23, _33=m._33; _34=m._34;
		_41=-(_11*m._41+_21*m._42+_31*m._43);
		_42=-(_12*m._41+_22*m._42+_32*m._43);
		_43=-(_13*m._41+_23*m._42+_33*m._43);
		_44=1;
	}
	inline void transpose(const CMatrix &m)
	{
		_11=m._11, _12=m._21, _13=m._31; _14=m._41;
		_21=m._12, _22=m._22, _23=m._32; _24=m._42;
		_31=m._13, _32=m._23, _33=m._33; _34=m._43;
		_41=m._14, _42=m._24, _43=m._34; _44=m._44;
	}
	inline void xformPoint(const Vect3f in,Vect3f& out) const
	{
		out.x=_11*in.x+_21*in.y+_31*in.z+_41;
		out.y=_12*in.x+_22*in.y+_32*in.z+_42;
		out.z=_13*in.x+_23*in.y+_33*in.z+_43;
	}
	inline void xformVect(const Vect3f in,Vect3f& out) const
	{
		out.x=_11*in.x+_21*in.y+_31*in.z;
		out.y=_12*in.x+_22*in.y+_32*in.z;
		out.z=_13*in.x+_23*in.y+_33*in.z;
	}
	inline void invXformPoint(const Vect3f in,Vect3f& out) const
	{
		out.x=_11*(in.x-_41)+_12*(in.y-_42)+_13*(in.z-_43);
		out.y=_21*(in.x-_41)+_22*(in.y-_42)+_23*(in.z-_43);
		out.z=_31*(in.x-_41)+_32*(in.y-_42)+_33*(in.z-_43);
	}
	inline void invXformVect(const Vect3f in,Vect3f& out) const
	{
		out.x=_11*in.x+_12*in.y+_13*in.z;
		out.y=_21*in.x+_22*in.y+_23*in.z;
		out.z=_31*in.x+_32*in.y+_33*in.z;
	}
	inline void Convert(const Vect3f& in,Vect3f& pv,Vect3f& pe) const
	{
		pv.x = _11*in.x+_21*in.y+_31*in.z+_41;
		pv.y = _12*in.x+_22*in.y+_32*in.z+_42;
		pv.z = _13*in.x+_23*in.y+_33*in.z+_43;
		float d=_14*in.x+_24*in.y+_34*in.z+_44;
		if(d==0) d=1e30f; else d=1/d;
		pe.x=pv.x*d;
		pe.y=pv.y*d;
		pe.z=pv.z*d;
	}
	inline void Convert(const Vect3f& in,Vect3f& out) const
	{
		out.x = _11*in.x+_21*in.y+_31*in.z+_41;
		out.y = _12*in.x+_22*in.y+_32*in.z+_42;
		out.z = _13*in.x+_23*in.y+_33*in.z+_43;
		float d=_14*in.x+_24*in.y+_34*in.z+_44;
		if(d==0) d=1e30f; else d=1/d;
		out.x=out.x*d;
		out.y=out.y*d;
		out.z=out.z*d;
	}
	inline void Convert(const Vect3f& in,Vect4f &out) const
	{
		out.x=_11*in.x+_21*in.y+_31*in.z+_41;
		out.y=_12*in.x+_22*in.y+_32*in.z+_42;
		out.z=_13*in.x+_23*in.y+_33*in.z+_43;
		out.w=_14*in.x+_24*in.y+_34*in.z+_44;
		if(out.w==0) out.w=1e-30f;
		float div=1/out.w;
		out.x=out.x*div;
		out.y=out.y*div;
		out.z=out.z*div;
		out.w=div;
	}
	inline Vect3f&  trans()							{ return *(Vect3f*)&_41; }
	inline Vect3f&  xrow()							{ return *(Vect3f*)&_11; }
	inline Vect3f&  yrow()							{ return *(Vect3f*)&_21; }
	inline Vect3f&  zrow()							{ return *(Vect3f*)&_31; }
	inline Vect4f&  row(int n)						{ return ((Vect4f*)this)[n]; }
};

#endif // __UMATH_H__