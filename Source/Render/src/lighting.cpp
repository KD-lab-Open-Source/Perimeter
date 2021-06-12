#include "StdAfxRD.h"
#include "lighting.h"

LightingParameters::LightingParameters()
{
	generate_time=0.1f;
	texture_name="Resource\\Effect\\freeze.tga";
	strip_width_begin=5;
	strip_width_time=15;
	strip_length=40;
	fade_time=0.5f;
	lighting_amplitude=40;
}

cLighting::cLighting()
:cIUnkClass(0)
{
	time=0;
	pTexture=GetTexLibrary()->GetElement(param.texture_name.c_str());
}

cLighting::~cLighting()
{
	list<OneLight*>::iterator it;
	FOR_EACH(lights,it)
		delete *it;
	lights.clear();

	RELEASE(pTexture);
}

void cLighting::SetParameters(LightingParameters& param_)
{
	param=param_;
	RELEASE(pTexture);
	pTexture=GetTexLibrary()->GetElement(param.texture_name.c_str());
	param.fade_time=max(param.fade_time,0.01f);
}

void cLighting::PreDraw(cCamera *pCamera)
{
	pCamera->Attach(SCENENODE_OBJECTSORT,this);

	vector<PreGenerate>::iterator it;
	FOR_EACH(pre_generate,it)
	{
		Generate(it->pos_begin,it->pos_end,pCamera);
	}
	pre_generate.clear();
}

void cLighting::Draw(cCamera *pCamera)
{
	list<OneLight*>::iterator it;
	FOR_EACH(lights,it)
		(*it)->Draw(pCamera,this);
}

void cLighting::OneLight::Draw(cCamera *pCamera,cLighting* parent)
{
/*
	sColor4c color(255,255,255);
	for(int i=1;i<position.size();i++)
	{
		Vect3f& n0=position[i-1];
		Vect3f& n1=position[i];
		gb_RenderDevice->DrawLine(n0,n1,color);
	}
/**/
/*
	float a=1-time;
	sColor4c diffuse(255,255*a,255,a*255);
	gb_RenderDevice->SetNoMaterial(ALPHA_ADDBLENDALPHA,0,pTexture);
	DrawStrip strip;
	strip.Begin();
	float size=5+time*15;
	sVertexXYZDT1 v1,v2;
	v1.diffuse=diffuse;
	v2.diffuse=diffuse;
	for(int i=0;i<position.size();i++)
	{
		Vect3f& p=position[i];
		Vect3f n;
		if(i==0)
			n=position[i+1]-position[i];
		else
		if(i==position.size()-1)
			n=position[i]-position[i-1];
		else
			n=position[i+1]-position[i-1];
		n.Normalize();
		n=n%Vect3f(0,0,1);
			
		float t=5*i/(float)position.size();
		float sz=size;
//		if(i==0)
//			sz=0;
		v1.pos=p;
		v2.pos=p;
//		v1.pos.y-=sz;
//		v2.pos.y+=sz;
		v1.pos-=sz*n;
		v2.pos+=sz*n;

		v1.u1()=v2.u1()=t;
		v1.v1()=0;v2.v1()=1;
		strip.Set(v1,v2);
	}
	strip.End();
/**/
	float a=1-time;
	sColor4c diffuse(255,255*a,255,a*255);
	gb_RenderDevice->SetNoMaterial(ALPHA_ADDBLENDALPHA,0,parent->pTexture);
	DrawStrip strip;
	strip.Begin();
	float size=parent->param.strip_width_begin+time*parent->param.strip_width_time;
	sVertexXYZDT1 v1,v2;
	v1.diffuse=diffuse;
	v2.diffuse=diffuse;
	for(int i=0;i<strip_list.size();i++)
	{
		OneStrip& p=strip_list[i];
		v1.pos=p.pos;
		v2.pos=p.pos;
		v1.pos-=size*p.n;
		v2.pos+=size*p.n;

		v1.u1()=v2.u1()=p.u;
		v1.v1()=0;v2.v1()=1;
		strip.Set(v1,v2);
	}
	strip.End();
}

void cLighting::Animate(float dt)
{
	dt*=1e-3f;
	time+=dt;
	while(time>=param.generate_time)
	{
		time-=param.generate_time;
		PreGenerate g;
		g.pos_begin=pos_begin;
		g.pos_end=pos_end[xm_random_generator()%pos_end.size()];
		pre_generate.push_back(g);
	}

	float fade_dt=dt/param.fade_time;
	list<OneLight*>::iterator it;
	FOR_EACH(lights,it)
	{
		OneLight* p=*it;
		p->Animate(fade_dt);
		if(p->time>=1)
		{
			delete *it;
			it=lights.erase(it);
		}
	}
}

void cLighting::Init(Vect3f pos_begin_,vector<Vect3f>& pos_end_)
{
	global_pos.set(Mat3f::ID,pos_begin);
	pos_begin=pos_begin_;
	pos_end=pos_end_;
	xassert(!pos_end.empty());
}

void cLighting::Generate(Vect3f pos_begin,Vect3f pos_end,cCamera *pCamera)
{
	OneLight* p=new OneLight;
	p->Generate(pos_begin,pos_end,pCamera,this);
	lights.push_back(p);
}

cLighting::OneLight::OneLight()
{
	time=0;
}

cLighting::OneLight::~OneLight()
{
}

void cLighting::OneLight::Animate(float dt)
{
	time+=dt;
	if(time>1)
		time=1;
}

void cLighting::OneLight::Generate(Vect3f pos_begin_,Vect3f pos_end_,cCamera *pCamera,cLighting* parent)
{
	pos_begin=pos_begin_;
	pos_end=pos_end_;

	int size=32;
	position.resize(size+2);

	vector<float> pos(size+2);
	pos[0]=pos[pos.size()-1]=0;

	float amplitude=parent->param.lighting_amplitude;
	for(int i=2;i<=size;i*=2)
	{
		GenerateInterpolate(pos,i,amplitude);
		amplitude*=0.5f;
	}

	Vect3f tangent=pos_end-pos_begin;
	tangent.Normalize();
	Vect3f vCameraToObject=pCamera->GetPos()-(pos_end+pos_begin)*0.5f;
	vCameraToObject.Normalize();
	Vect3f orientation;
	orientation.cross(vCameraToObject,tangent);

	for(int i=0;i<position.size();i++)
	{
		float t=i/(float)position.size();
		Vect3f p;
		p.x=LinearInterpolate(pos_begin.x,pos_end.x,t);
		p.y=LinearInterpolate(pos_begin.y,pos_end.y,t);
		p.z=LinearInterpolate(pos_begin.z,pos_end.z,t);
		p+=pos[i]*orientation;
		position[i]=p;
	}

	BuildStrip(pCamera,parent);
}

void cLighting::OneLight::GenerateInterpolate(vector<float>& pos,int size,float amplitude)
{

	RandomGenerator& r=xm_random_generator;
	vector<float> p(size);\
	int i;
	for(i=0;i<size;i++)
		p[i]=r.frnd(amplitude);

	for(i=1;i<pos.size()-1;i++)
	{
		float t=i/(float)pos.size();
		float dy=get(p,t);
		pos[i]+=dy;
	}

}

float cLighting::OneLight::get(vector<float>& p,float t)//линейная интерполяция
{

	int size=p.size()+2;
	int i=int(t*size);
	float dt=t*size-i;
	i-=1;
	float p0=0,p1=0;
	if(i>=0 && i<p.size())
		p0=p[i];
	if(i+1>=0 && i+1<p.size())
		p1=p[i+1];

	return LinearInterpolate(p0,p1,dt);
//	return CosInterpolate(p0,p1,dt);
}

void cLighting::OneLight::BuildStrip(cCamera *pCamera,cLighting* parent)
{
	float tn=round(pos_end.distance(pos_begin)/parent->param.strip_length);
	strip_list.resize(position.size());
	for(int i=0;i<position.size();i++)
	{
		Vect3f& p=position[i];
		Vect3f vCameraToObject=pCamera->GetPos()-p;
		vCameraToObject.Normalize();
		OneStrip& s=strip_list[i];
		Vect3f n;
		if(i==0)
			n=position[i+1]-position[i];
		else
		if(i==position.size()-1)
			n=position[i]-position[i-1];
		else
			n=position[i+1]-position[i-1];
		n.Normalize();
		n=n%vCameraToObject;
			
		float t=tn*i/(float)position.size();
		s.pos=p;
		s.n=n;
		s.u=t;
	}
}
