#pragma once

#ifdef _MSC_VER
#pragma pack(push,1)
#endif

struct sVertexXYZ
{
	Vect3f	pos;
	static int fmt;
};
struct sVertexXYZD : public sVertexXYZ
{
	sColor4c	diffuse;
	static int fmt;
};
struct sVertexXYZT1: public sVertexXYZ
{ 
	float			uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	inline Vect2f& GetTexel1()			{ return *((Vect2f*)&uv[0]); }
	static int fmt;
};
struct sVertexXYZDT1 : public sVertexXYZD
{
	float			uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	inline Vect2f& GetTexel1()			{ return *((Vect2f*)&uv[0]); }
	static int fmt;
};
struct sVertexXYZDT2 : public sVertexXYZDT1
{
	float			uv2[2];
	inline float& u2()					{ return uv2[0]; }
	inline float& v2()					{ return uv2[1]; }
	inline Vect2f& GetTexel2()			{ return *((Vect2f*)&uv2[0]); }
	static int fmt;
};

struct sVertexXYZW
{
	float			x,y,z,w;
	inline int& xi()					{ return *((int*)&x); }
	inline int& yi()					{ return *((int*)&y); }
	inline int& zi()					{ return *((int*)&z); }
	Vect3f& GetVect3f()					{ return *(Vect3f*)&x; }
	static int fmt;
};

struct sVertexXYZWD:public sVertexXYZW
{
	sColor4c	diffuse;
	static int fmt;
};
struct sVertexXYZWDT1 : public sVertexXYZWD
{
	float			uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	static int fmt;
};

struct sVertexXYZWDT2 : public sVertexXYZWDT1
{
	float			uv[2];
	inline float& u2()					{ return uv[0]; }
	inline float& v2()					{ return uv[1]; }
	inline Vect2f& GetTexel2()			{ return *((Vect2f*)&uv[0]); }
	static int fmt;
};
struct sVertexXYZN : public sVertexXYZ
{
	Vect3f	n;
	static int fmt;
};
struct sVertexXYZNT1 : public sVertexXYZN
{
	float			uv[2];
	inline Vect2f& GetTexel()			{ return *(Vect2f*)&uv[0]; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	static int fmt;
};

struct sVertexDot3:public sVertexXYZNT1
{
	Vect3f S;
	Vect3f T;
	Vect3f SxT;
	static int fmt;
};

#ifdef _MSC_VER
#pragma pack(pop)
#endif