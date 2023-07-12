#pragma once

#pragma pack(push,1)

const static vertex_fmt_t VERTEX_FMT_NORMAL    = (1<<0);
const static vertex_fmt_t VERTEX_FMT_DIFFUSE   = (1<<1);
const static vertex_fmt_t VERTEX_FMT_TEX1      = (1<<2);
const static vertex_fmt_t VERTEX_FMT_TEX2      = (1<<3);
#ifdef PERIMETER_D3D9
const static vertex_fmt_t VERTEX_FMT_DOT3      = (1<<4);
#endif

const static vertex_fmt_t VERTEX_FMT_BITS      = 5;
const static vertex_fmt_t VERTEX_FMT_MAX       = (1<<VERTEX_FMT_BITS)-1;

struct sVertexXYZ
{
    union {
        Vect3f pos;
        struct { float x, y, z; };
    };
	const static int fmt = 0;
};
struct sVertexXYZDT1 : public sVertexXYZ {
    uint32_t diffuse;
    float uv[2];
    inline int& xi()					{ return *((int*)&x); }
    inline int& yi()					{ return *((int*)&y); }
    inline int& zi()					{ return *((int*)&z); }
    Vect3f& GetPos()					{ return *(Vect3f*)&x; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
    const static vertex_fmt_t fmt = sVertexXYZ::fmt|VERTEX_FMT_DIFFUSE|VERTEX_FMT_TEX1;
};
struct sVertexXYZDT2 : public sVertexXYZDT1
{
	float			uv2[2];
	inline float& u2()					{ return uv2[0]; }
	inline float& v2()					{ return uv2[1]; }
	inline Vect2f& GetTexel2()			{ return *((Vect2f*)&uv2[0]); }
	const static vertex_fmt_t fmt = sVertexXYZDT1::fmt|VERTEX_FMT_TEX2;
};
struct sVertexXYZNT1 : public sVertexXYZ
{
	Vect3f n;
	float uv[2];
	inline Vect2f& GetTexel()			{ return *(Vect2f*)&uv[0]; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	const static vertex_fmt_t fmt = sVertexXYZ::fmt|VERTEX_FMT_NORMAL|VERTEX_FMT_TEX1;
};
//Used for rendering terrain
struct sVertexXYZT1: public sVertexXYZ
{ 
	float uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	const static vertex_fmt_t fmt = sVertexXYZ::fmt|VERTEX_FMT_TEX1;
};
#ifdef PERIMETER_D3D9
//Used for bump mapping
struct sVertexDot3:public sVertexXYZNT1
{
	Vect3f S;
	Vect3f T;
	Vect3f SxT;
	const static vertex_fmt_t fmt = sVertexXYZNT1::fmt|VERTEX_FMT_DOT3;
};
#endif

#pragma pack(pop)
