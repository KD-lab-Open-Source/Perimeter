#pragma once

#ifdef _MSC_VER
#pragma pack(push,1)
#endif

#define VERTEX_FMT_XYZ                 (1<<0)
#define VERTEX_FMT_W                   (1<<1)
#define VERTEX_FMT_NORMAL              (1<<2)
#define VERTEX_FMT_DIFFUSE             (1<<3)
#define VERTEX_FMT_TEX1                (1<<4)
#define VERTEX_FMT_TEX2                (1<<5)
//TODO Special flag for Dot3 required data in D3D, need to review how to implement bumpmapping in Sokol
#define VERTEX_FMT_DOT3                (1<<6)

#define VERTEX_FMT_MAX                 ((1<<7)-1)

struct sVertexXYZ
{
	Vect3f	pos;
	const static int fmt = VERTEX_FMT_XYZ;;
};

struct sVertexXYZT1: public sVertexXYZ
{ 
	float			uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	inline Vect2f& GetTexel1()			{ return *((Vect2f*)&uv[0]); }
	const static int fmt = sVertexXYZ::fmt|VERTEX_FMT_TEX1;
};

struct sVertexXYZD : public sVertexXYZ
{
    sColor4c	diffuse;
    const static int fmt = sVertexXYZ::fmt|VERTEX_FMT_DIFFUSE;
};
struct sVertexXYZDT1 : public sVertexXYZD
{
	float			uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	inline Vect2f& GetTexel1()			{ return *((Vect2f*)&uv[0]); }
	const static int fmt = sVertexXYZD::fmt|VERTEX_FMT_TEX1;
};
struct sVertexXYZDT2 : public sVertexXYZDT1
{
	float			uv2[2];
	inline float& u2()					{ return uv2[0]; }
	inline float& v2()					{ return uv2[1]; }
	inline Vect2f& GetTexel2()			{ return *((Vect2f*)&uv2[0]); }
	const static int fmt = sVertexXYZDT1::fmt|VERTEX_FMT_TEX2;
};

struct sVertexXYZWD
{
	float			x,y,z,w;
    sColor4c	diffuse;
	inline int& xi()					{ return *((int*)&x); }
	inline int& yi()					{ return *((int*)&y); }
	inline int& zi()					{ return *((int*)&z); }
    Vect3f& GetPos()					{ return *(Vect3f*)&x; }
    Vect4f& GetPosW()					{ return *(Vect4f*)&x; }
	const static int fmt = VERTEX_FMT_XYZ|VERTEX_FMT_W|VERTEX_FMT_DIFFUSE;
};
struct sVertexXYZWDT1 : public sVertexXYZWD
{
	float			uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	const static int fmt = sVertexXYZWD::fmt|VERTEX_FMT_TEX1;
};

struct sVertexXYZWDT2 : public sVertexXYZWDT1
{
	float			uv[2];
	inline float& u2()					{ return uv[0]; }
	inline float& v2()					{ return uv[1]; }
	inline Vect2f& GetTexel2()			{ return *((Vect2f*)&uv[0]); }
	const static int fmt = sVertexXYZWDT1::fmt|VERTEX_FMT_TEX2;
};
struct sVertexXYZNT1 : public sVertexXYZ
{
	Vect3f	n;
	float			uv[2];
	inline Vect2f& GetTexel()			{ return *(Vect2f*)&uv[0]; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	const static int fmt = sVertexXYZ::fmt|VERTEX_FMT_NORMAL|VERTEX_FMT_TEX1;
};

struct sVertexDot3:public sVertexXYZNT1
{
	Vect3f S;
	Vect3f T;
	Vect3f SxT;
	const static int fmt = sVertexXYZNT1::fmt|VERTEX_FMT_DOT3;
};

#ifdef _MSC_VER
#pragma pack(pop)
#endif