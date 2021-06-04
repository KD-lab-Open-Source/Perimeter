#ifndef _CZPLANE_H_
#define _CZPLANE_H_

class cZPlane:public cIUnkObjScale
{
	class cFrame1
	{
	public:
		cFrame1(){dcount=count=0;};
		void Set(float Period){dcount=1.0f/Period;};
		void AddPhase(float dt){count+=dt*dcount;};

		inline float GetPhase()							{ return count; }

	private:
		float			count;
		float			dcount;
	};
//Использует Scale и GlobalMatrix для определения положения
	cFrame		Frame0,Frame1;
	cFrame1		Frame0s,Frame1s;
	cTexture    *pTex0,*pTex1;

	float texturek0,texturek1;
	int stage2op;

	float speed0x,speed0y;
	float speed1x,speed1y;
public:
	cZPlane();
	~cZPlane();

	void SetTexture(cTexture* pTex0,cTexture* pTex1,float k0,float k1,int op);
	void SetSpeed(int n,float sx,float sy);

	virtual void Animate(float dt);
	virtual void PreDraw(cCamera *UClass);
	virtual void Draw(cCamera *UClass);

	double sfmod(double a,double b)
	{
		if(a>0)
			return fmod(a,b);
		return b-fmod(-a,b);
	}
};

class cPlane:public cUnkObj
{
	float umin,vmin,umax,vmax;
public:
	cPlane();
	/*  Что можно применять
		SetTexture(0,...)
		SetAttr ATTRUNKOBJ_IGNORE
		SetScale
		SetPosition
	*/
	virtual void SetUV(float umin,float vmin,float umax,float vmax);
	virtual void PreDraw(cCamera *UClass);
	virtual void Draw(cCamera *UClass);

	virtual Vect3f GetCenterObject();
};

#endif  _CZPLANE_H_