#pragma once

#pragma pack(push,1)

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

struct sVertexXYZDS : public sVertexXYZD
{
	sColor4c	specular;
	static int fmt;
};
struct sVertexXYZDST1 : public sVertexXYZDS
{
	float			uv[2];
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
	inline Vect2f& GetTexel1()			{ return *((Vect2f*)&uv[0]); }
	static int fmt;
};
struct sVertexXYZDST2 : public sVertexXYZDST1
{
	float			uv[2];
	inline float& u2()					{ return uv[0]; }
	inline float& v2()					{ return uv[1]; }
	inline Vect2f& GetTexel2()			{ return *((Vect2f*)&uv[0]); }
	static int fmt;
};
struct sVertexXYZWT1
{
	float			x,y,z,w;
	float			uv[2];
	Vect3f& GetVect3f()					{ return *(Vect3f*)&x; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	inline Vect2f& GetTexel()			{ return *((Vect2f*)&uv[0]); }
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
struct sVertexXYZND : public sVertexXYZN
{
	sColor4c	diffuse;
	static int fmt;
};
struct sVertexXYZNDS : public sVertexXYZND
{
	sColor4c	specular;
	static int fmt;
};
struct sVertexXYZNDST1 : public sVertexXYZNDS
{
	float			uv[2];
	inline Vect2f& GetTexel()			{ return *(Vect2f*)&uv[0]; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	static int fmt;
};
struct sVertexXYZNDST2 : public sVertexXYZNDST1
{
	float			uv[2];
	inline float& u2()					{ return uv[0]; }
	inline float& v2()					{ return uv[1]; }
	inline Vect2f& GetTexel2()			{ return *(Vect2f*)&uv[0]; }
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

struct sVertexXYZDP : public sVertexXYZ
{
	float pointsize;
	sColor4c	diffuse;
	static int fmt;
};

struct sVertexDot3:public sVertexXYZNT1
{
	Vect3f S;
	Vect3f T;
	Vect3f SxT;
	static int fmt;
};

struct sVertexXYZINT1:public sVertexXYZ
{
	BYTE index[4];
	Vect3f	n;
	float			uv[2];
	inline Vect2f& GetTexel()			{ return *(Vect2f*)&uv[0]; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	static int fmt;
};

struct sVertexXYZW4INT1
{
	Vect3f	pos;
	float weight[4];
	BYTE index[4];
	Vect3f	n;
	float			uv[2];
	inline Vect2f& GetTexel()			{ return *(Vect2f*)&uv[0]; }
	inline float& u1()					{ return uv[0]; }
	inline float& v1()					{ return uv[1]; }
	static int fmt;
};

class cSkinVertex
{
/*формат
	Vect3f	pos;
	float weight[X];
	BYTE index[4];
	Vect3f	n;
	float			uv[2];
*/
	int num_weight;
	bool bump;
	int vb_size;
	void* p;
	float* cur;
public:
	cSkinVertex(int num_weight_,bool bump_):p(0),num_weight(num_weight_),vb_size(0),bump(bump_){}
	void SetVB(void* p_,int vb_size_){p=p_;vb_size=vb_size_;}
	void Select(int n){cur=(float*)(vb_size*n+(char*)p);}
	Vect3f& GetPos(){return *(Vect3f*)cur;}
	sColor4c& GetIndex(){return *(sColor4c*)(3+cur);}
	Vect3f& GetNorm(){return *(Vect3f*)(4+cur);}
	BYTE& GetWeight(int idx);
	Vect2f& GetTexel(){return *(Vect2f*)(7+(num_weight?1:0)+cur);}
	Vect3f& GetBumpS(){return *(Vect3f*)(9+(num_weight?1:0)+cur);}
	Vect3f& GetBumpT(){return *(Vect3f*)(12+(num_weight?1:0)+cur);}
	Vect3f& GetBumpSxT(){return *(Vect3f*)(15+(num_weight?1:0)+cur);}

	int GetFormat();
protected:
};

#pragma pack(pop)