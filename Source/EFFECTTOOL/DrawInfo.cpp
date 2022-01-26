
#include "stdafx.h"
#include "DrawInfo.h"

void DrawPointer(cInterfaceRenderDevice* pRenderDevice, const MatXf& m,int r,int g,int b,int a)
{
	float len=50;
	Vect3f p=Vect3f(len,0,0);
	Vect3f dx(0,len*0.1f,0),dy(len*0.1f,0,0),dz(0,0,len*0.1f);
	
	Vect3f p0=m*Vect3f::ZERO;
	Vect3f pp=m*p;

	sColor4c c(r,g,b,a);
	pRenderDevice->DrawLine(p0,pp,c);
	pRenderDevice->DrawLine(pp,m*(p+dx-dy),c);
	pRenderDevice->DrawLine(pp,m*(p-dx-dy),c);
	
	pRenderDevice->DrawLine(pp,m*(p+dz-dy),c);
	pRenderDevice->DrawLine(pp,m*(p-dz-dy),c);
}

void DrawXYZ(cInterfaceRenderDevice* pRenderDevice, const MatXf& pos)
{
	MatXf m=MatXf::ID;
/*	DrawPointer(pRenderDevice, pos*m,255,0,0,255);
	m.rot().set(Vect3f(0,0,1),M_PI/2);
	DrawPointer(pRenderDevice, pos*m,0,255,0,255);
*/
	m.rot().set(Vect3f(0,-1,0),M_PI/2);
	DrawPointer(pRenderDevice, pos*m,0,0,255,255);
}

void DrawBox(cInterfaceRenderDevice* pRenderDevice, const MatXf& m, const Vect3f& i, const Vect3f& a, const sColor4c& c)
{
	Vect3f v000,v001,v010,v011,
		   v100,v101,v110,v111;
	v000.set(i.x,i.y,i.z);v000=m*v000;
	v001.set(a.x,i.y,i.z);v001=m*v001;
	v010.set(i.x,a.y,i.z);v010=m*v010;
	v011.set(a.x,a.y,i.z);v011=m*v011;
	v100.set(i.x,i.y,a.z);v100=m*v100;
	v101.set(a.x,i.y,a.z);v101=m*v101;
	v110.set(i.x,a.y,a.z);v110=m*v110;
	v111.set(a.x,a.y,a.z);v111=m*v111;
	pRenderDevice->DrawLine(v000,v001,c);
	pRenderDevice->DrawLine(v001,v011,c);
	pRenderDevice->DrawLine(v011,v010,c);
	pRenderDevice->DrawLine(v010,v000,c);

	pRenderDevice->DrawLine(v100,v101,c);
	pRenderDevice->DrawLine(v101,v111,c);
	pRenderDevice->DrawLine(v111,v110,c);
	pRenderDevice->DrawLine(v110,v100,c);
	pRenderDevice->DrawLine(v000,v001,c);

	pRenderDevice->DrawLine(v000,v100,c);
	pRenderDevice->DrawLine(v001,v101,c);
	pRenderDevice->DrawLine(v011,v111,c);
	pRenderDevice->DrawLine(v010,v110,c);
}

const segments=64;
static bool init=false;
static Vect2f pos[segments];
static void InitCirclePos()
{
	if(init)return;

	init=true;
	for(int i=0;i<segments;i++)
	{
		float a=(2*M_PI*i)/segments;
		pos[i].x=xm::cos(a);
		pos[i].y=xm::sin(a);
	}
}

void DrawCircle(cInterfaceRenderDevice* pRenderDevice, const MatXf& m,float r, const sColor4c& c)
{
	InitCirclePos();
	Vect3f prev;
	int i=segments-1;
	prev.x=pos[i].x*r;
	prev.y=pos[i].y*r;
	prev.z=0;
	prev=m*prev;
	for(i=0;i<segments;i++)
	{
		Vect3f cur;
		cur.x=pos[i].x*r;
		cur.y=pos[i].y*r;
		cur.z=0;
		cur=m*cur;
		pRenderDevice->DrawLine(prev,cur,c);
		prev=cur;
	}

}

void DrawSphere(cInterfaceRenderDevice* pRenderDevice, const MatXf& pos,float r, const sColor4c& c1, const sColor4c& c2, const sColor4c& c3)
{
	MatXf m;
	DrawCircle(pRenderDevice, pos,r,c1);
	m.rot().set(Vect3f(0,1,0),M_PI/2);
	m.trans().set(0,0,0);
	DrawCircle(pRenderDevice, pos*m,r,c2);
	m.rot().set(Vect3f(1,0,0),M_PI/2);
	m.trans().set(0,0,0);
	DrawCircle(pRenderDevice, pos*m,r,c3);
}

void DrawCylinder(cInterfaceRenderDevice* pRenderDevice, const MatXf& mt,float r,float h, const sColor4c& c)
{
	InitCirclePos();
	MatXf m=MatXf::ID;
	MatXf p1,p2;
	p1=mt;
	DrawCircle(pRenderDevice, p1,r,c);
	m.trans().z=h;
	p2=mt*m;
	DrawCircle(pRenderDevice, p2,r,c);

	for(int i=0;i<segments;i+=2)
	{
		Vect3f cur,cur1,cur2;
		cur.x=pos[i].x*r;
		cur.y=pos[i].y*r;
		cur.z=0;
		cur1=p1*cur;
		cur2=p2*cur;
		pRenderDevice->DrawLine(cur1,cur2,c);
	}

}
