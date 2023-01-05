#ifndef _CZPLANE_H_
#define _CZPLANE_H_

///Displays green/red rectangle under building/frame to show placement availability
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

#endif //_CZPLANE_H_