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

	cChaos(Vect2f size, const char* str_tex0, const char* str_tex1, const char* str_bump, int tile, bool enable_bump);
	~cChaos();
	
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *DrawNode);
	virtual void Draw(cCamera *DrawNode);

	void RenderAllTexture();
protected:
	void CreateIB();
	void CreateVB();

	void RenderTexture();
	void RenderTex0();

	double sfmod(double a,double b)
	{
		if(a>0)
			return xm::fmod(a,b);
		return b-xm::fmod(-a,b);
	}

};

class CSkySpere:public cIUnkObj
{
	cObjectNodeRoot* pSkySphere;
public:
	CSkySpere(cObjLibrary* pObj, const char* str_name, const char* str_texture, int h_size);
	~CSkySpere();
	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *DrawNode);
	virtual void Draw(cCamera *DrawNode);
};
