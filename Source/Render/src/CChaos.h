#pragma once

class cChaos:public cIUnkObj
{
	cTexture    *pTex0;
	cTexture    *pTex0_0,*pTex0_1;
	Vect2f stime_tex0,stime_tex1;

	cTexture    *pTexRender;
	cTexture    *pTexBump;
	float time;

	int size;

	enum BUMP_TYPE
	{
		BUMP_NONE=0,
		BUMP_RENDERTARGET,
		BUMP_PS14
	};

	BUMP_TYPE enablebump;
	float uvmul;

	enum
	{
		sub_div=5,
	};

	sPtrIndexBuffer ib;

	typedef sVertexXYZDT2 VTYPE;
	sPtrVertexBuffer vb;
	Vect2f plane_size;

	class VSChaos* pVS;
	class PSChaos* pPS;
public:

	cChaos(Vect2f size,LPCSTR str_tex0,LPCSTR str_tex1,LPCSTR str_bump,int tile,bool enable_bump);
	~cChaos();
	
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *DrawNode);
	virtual void Draw(cCamera *DrawNode);

	void RenderAllTexture();
protected:
	void CreateIB();
	void CreateVB();

	void InitBumpTexture1(cTexture* pTex);
	void InitBumpTexture2(cTexture* pTex);
	void ConvertToBump(cTexture*& pOut,cTexture* pIn);

	void RenderTexture();
	void RenderTex0();

	double sfmod(double a,double b)
	{
		if(a>0)
			return fmod(a,b);
		return b-fmod(-a,b);
	}

};


class CBox:public cIUnkObj
{
	int			box_face;
	std::vector<cTexture*> pBox;

	int num_vertex;
	int num_index;
	sPtrVertexBuffer vb;
	sPtrIndexBuffer ib;

	
	struct CubeVector
	{
		Vect3f pos;
		Vect2f t;
		static int fmt;
	};

	Vect3f sz_rect;
	bool fov90;

	bool enable_face[6];
public:
	CBox(Vect3f size,LPCSTR str_cube);
	~CBox();
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *DrawNode);
	virtual void Draw(cCamera *DrawNode);
protected:
	void CreateVB();
};

class CSkySpere:public cIUnkObj
{
	cObjectNodeRoot* pSkySphere;
public:
	CSkySpere(cObjLibrary* pObj,LPCSTR str_name,LPCSTR str_texture,int h_size);
	~CSkySpere();
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *DrawNode);
	virtual void Draw(cCamera *DrawNode);
};
