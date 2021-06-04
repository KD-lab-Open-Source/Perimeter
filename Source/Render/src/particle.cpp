#include "StdAfxRD.h"
#include "Particle.h"

int cParticle::RandomVal=0x83838383;

///////////////////////////////////////////////////////////////////////////////
//				cParticle
///////////////////////////////////////////////////////////////////////////////
//#define check_vector(v) if(v.x>=1e6 || v.x<-1e6 || v.y>=1e6 || v.y<-1e6 || v.z>=1e6 || v.z<-1e6 ) { __asm { int 3 }; }
#define check_vector(v)

cParticle::cParticle(float fTimeLife,Vect2f &vTexSize) : cAnimUnkObj(KIND_PARTICLE)
{
	TimeLife=fTimeLife;
	CurrentTime=0;
	dTime=0;
	Key=0;
	Dumping.set(0,0,0);
	Force.set(0.00001f,0.00001f,-0.0001f);
	Bound.min.set(1e30f,1e30f,1e30f);
	Bound.max.set(-1e30f,-1e30f,-1e30f);
	TexSize=vTexSize;
	bNotUpdate=0;
	LastParticle=0;
}
cParticle::~cParticle()
{
}
void cParticle::Animate(float dt)
{
	if(CurrentTime==0) dt=0.1f;
	CurrentTime+=dt;
	dTime+=dt;
	GetFrame()->AddPhase(dt);
	if(GetRef()>1) return; // запрещено удаление
	if(Particle.empty()||(CurrentTime-Particle[LastParticle].time*TimeLife)>TimeLife||
		CurrentTime>30000) 
		Release();
}
void cParticle::PreDraw(cCamera *DrawNode)
{
	int pUpdate;
	if(Bound.min.x>Bound.max.x)return;
	
	if(DrawNode->TestVisible(GetGlobalMatrix(),Bound.min,Bound.max))
	{
		DrawNode->Attach(SCENENODE_OBJECTSORT,this);
		pUpdate=RAND_MAX;
	}else
		pUpdate=RAND_MAX/10;
	if(dTime&&(rand()<pUpdate)) 
	{
//		start_timer(Particle_Update,1);
		Bound.min.set(1e30f,1e30f,1e30f);
		Bound.max.set(-1e30f,-1e30f,-1e30f);
		if(!bNotUpdate) Update();
		dTime=0;
//		stop_timer(Particle_Update,1);
	}
}
void cParticle::Update()
{	// эволюция системы
	float dt=(CurrentTime-dTime)/TimeLife;
	if(Dumping.norm2())
		if(Force.norm2())
			for(int i=0;i<Particle.size();i++)
			{
				if(Particle[i].time>=0)
				{
					float time=(Particle[i].time<dt)?dTime:CurrentTime-Particle[i].time*TimeLife;
					Particle[i].pos+=Particle[i].v*time;
					Bound.AddBound(Particle[i].pos);
					Particle[i].v+=(Particle[i].v*Dumping+Force)*time;
				}
			}
		else
			for(int i=0;i<Particle.size();i++)
			{
				if(Particle[i].time>=0)
				{
					float time = Particle[i].time < dt ? dTime : CurrentTime - Particle[i].time*TimeLife;
					Particle[i].pos+=Particle[i].v*time;
					Bound.AddBound(Particle[i].pos);
					Particle[i].v+=Particle[i].v*Dumping*time;
				}
			}
	else
		if(Force.norm2())
			for(int i=0;i<Particle.size();i++)
			{
				if(Particle[i].time>=0)
				{
					float time=(Particle[i].time<dt)?dTime:CurrentTime-Particle[i].time*TimeLife;
					Particle[i].pos+=Particle[i].v*time;
					Bound.AddBound(Particle[i].pos);
					Particle[i].v+=Force*time;
				}
			}
		else
			for(int i=0;i<Particle.size();i++)
			{
				if(Particle[i].time>=0)
				{
					float time=(Particle[i].time<dt)?dTime:CurrentTime-Particle[i].time*TimeLife;
					Particle[i].pos+=Particle[i].v*time;
					Bound.AddBound(Particle[i].pos);
				}
			}
}
void cParticle::Draw(cCamera *DrawNode)
{
	Particle.clear();
//	start_timer(Draw_Particle,1);
	int nVertex=0;
	cVertexBuffer<sVertexXYZDT1>* pBuf=DrawNode->GetRenderDevice()->GetBufferXYZDT1();

	DrawNode->GetRenderDevice()->SetNoMaterial(GetAttribute(ATTRUNKOBJ_ADDBLEND)?ALPHA_ADDBLENDALPHA:ALPHA_BLEND,
				GetFrame()->GetPhase(),GetTexture());
	sVertexXYZDT1 *Vertex=pBuf->Lock();

	float d=DrawNode->GetPos().distance(GetGlobalMatrix().trans()+(Bound.min+Bound.max)*0.5f);
	float ScrScale=DrawNode->GetFocusViewPort().x/d;
	int nLOD=round(0.5f*8*ScrScale*(1-Option_NearDistanceLOD*0.1f));
	float dt=CurrentTime/TimeLife,dt1=dt-1;

	for(int i=0;i<Particle.size();i++)
	if(dt1<=Particle[i].time)
	{
		Vect3f sx,sy;
		if((i&0x00000007)>=nLOD) continue;
		sVertexXYZDT1 *v=&Vertex[nVertex];
		float phase=dt-Particle[i].time;
		sParticleKey &key=Key[round(phase*(NumberKey-1))];
		VISASSERT(0<=phase&&phase<=1);
		DrawNode->GetMatrix().invXformVect(Vect3f(+key.rotate.x,-key.rotate.y,0),sx);
		DrawNode->GetMatrix().invXformVect(Vect3f(+key.rotate.y,+key.rotate.x,0),sy);

		Vect3f v0,v1,v2,v3;
		sColor4c color(round(GetDiffuse().r*key.diffuse.r),
					   round(GetDiffuse().g*key.diffuse.g),
					   round(GetDiffuse().b*key.diffuse.b),
					   round(GetDiffuse().a*key.diffuse.a));
		
		v0=Particle[i].pos-sx-sy;
		v1=Particle[i].pos-sx+sy;
		v2=Particle[i].pos+sx-sy;
		v3=Particle[i].pos+sx+sy;

		v[0].pos=v0; v[0].GetTexel().set(key.TexPos.x,key.TexPos.y); 
		v[0].diffuse=color;
		v[1].pos=v1; v[1].GetTexel().set(key.TexPos.x,key.TexPos.y+TexSize.y);
		v[1].diffuse=color;
		v[2].pos=v2; v[2].GetTexel().set(key.TexPos.x+TexSize.x,key.TexPos.y);
		v[2].diffuse=color;

		v[3].pos=v1; v[3].GetTexel().set(key.TexPos.x,key.TexPos.y+TexSize.y);
		v[3].diffuse=color;
		v[4].pos=v2; v[4].GetTexel().set(key.TexPos.x+TexSize.x,key.TexPos.y);
		v[4].diffuse=color;
		v[5].pos=v3; v[5].GetTexel().set(key.TexPos.x+TexSize.x,key.TexPos.y+TexSize.y);
		v[5].diffuse=color;

		nVertex+=6;

		if(nVertex+6>=pBuf->GetSize())
		{
			pBuf->Unlock(nVertex);
			pBuf->DrawPrimitive(PT_TRIANGLELIST,nVertex/3,GetGlobalMatrix());
			Vertex=pBuf->Lock();
			nVertex=0;
		}
	}else
		SetFreeParticle(i);

	pBuf->Unlock(nVertex);
	if(nVertex>0)
		pBuf->DrawPrimitive(PT_TRIANGLELIST,nVertex/3,GetGlobalMatrix());
//	stop_timer(Draw_Particle,1);
}

sParticle& cParticle::GetFreeParticle()
{
	if(StoppedParticle.empty())
	{
		Particle.push_back(sParticle());
		return Particle.back();
	}else
	{
		int FreeParticle=StoppedParticle.back();
		StoppedParticle.pop_back();
		//VISASSERT(Particle[FreeParticle].time<0);
		return Particle[FreeParticle];
	}
}

void cParticle::SetFreeParticle(int n)
{
	sParticle &p=Particle[n];
	p.time=-1;
	StoppedParticle.push_back(n);
}

void cParticle::SetData(const Vect3f* pPos,const Vect3f* pVelocity)
{
	sParticle &p=GetFreeParticle();
	p.v=*pVelocity;
	p.pos=*pPos;
	p.time=CurrentTime/TimeLife;

	check_vector(p.pos);
	check_vector(p.v);

	Bound.AddBound(p.pos);
}

void cParticle::SetAnimKeys(sParticleKey *AnimKeys,int size)
{
	NumberKey=size;
	Key=AnimKeys;
}

void cParticle::SetProperties(const Vect3f *pDumping,const Vect3f *pForce,const Vect3f *pReflectionImpulse)
{ 
	if(pDumping) Dumping=*pDumping;
	if(pForce) Force=*pForce;
	check_vector(Dumping);
	check_vector(Force);
}

void CalcNumParticleClass(int& num_object,int& num_particle)
{
	num_object=0;
	num_particle=0;
#ifdef C_CHECK_DELETE
	for(cCheckDelete* cur=cCheckDelete::GetDebugRoot();cur;cur=cur->GetDebugNext())
	{
		cParticle* p = dynamic_cast<cParticle*>(cur);
		if(p)
		{
//			const type_info& ti=typeid(*p);
//			const char* name=ti.name();
			num_particle+=p->GetNumParticle();
			num_object++;
		}
	}
#endif C_CHECK_DELETE
}

void cParticle::Free()
{
	for(int i=0;i<Particle.size();i++)
	{
		SetFreeParticle(i);
	}
}
