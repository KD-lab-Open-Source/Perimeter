#ifndef _CHAOS_H_
#define _CHAOS_H_

class CChaos
{
	cChaos* ZPlane;
	cIUnkClass* pSkySphere;
public:
	CChaos(int hsize,int vsize,bool bump);
	~CChaos();
	void Draw();
protected:

};
#endif //_CHAOS_H_