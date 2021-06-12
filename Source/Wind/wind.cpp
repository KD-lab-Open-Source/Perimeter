#include "StdAfx.h"
#include "Wind.h"
float par1=3;
float par2=0;

float dtime=0;
static RandomGenerator rnd;
EffectKey* trn = NULL;
EffectKey* wind = NULL;
vector<cEffect*> trs; 
extern cInterfaceRenderDevice	*terRenderDevice;
extern cScene	*terScene;
cMapWind* mapw = NULL;
//==================================================================
//реализация cmGrid

void cmGrid::Clear()
{
	memset(nds,0,x_count*y_count*sizeof(*nds));
}
//------------------------------------------------------------------
void cmGrid::InitGrid(int w,int h, int shift_)
{
	shift = shift_;
	xassert(shift);
	x_count = w>>shift;
	y_count = h>>shift;
	ng = 1<<shift;
	if (nds)delete[] nds;
	nds = new WindNode[x_count*y_count];
}
//------------------------------------------------------------------
template<class Op> void cmGrid::Scan(float xp,float yp,float r,Op& op)
{
	int xb = max(round(xp-r)>>shift, 0);
	int y = max(round(yp-r)>>shift, 0);
	int x_lim = min(round(xp+r)>>shift, x_count-1);
	int y_lim = min(round(yp+r)>>shift, y_count-1);
	for(;y<y_lim; ++y)
	{
		int ym = y*x_count;
		for(int x = xb; x<x_lim; ++x)
			op(nds[x+ym], x<<shift, y<<shift);
	}
}
//------------------------------------------------------------------
inline
Vect2f bilinear(Vect2f& p00,Vect2f& p01,Vect2f& p10,Vect2f& p11,float cx,float cy)
{
	return (p00*(1-cx)+p01*cx)*(1-cy) + (p10*(1-cx)+p11*cx)*cy;
}

/*Vect2f cmGrid::GetVelocityXY(float x, float y)
{
}
Vect2f cmGrid::GetVelocity(float x, float y, float z)
{
}*/
Vect3f cmGrid::GetVelocity(Vect3f& p)
{
	float surz = mapw->GetZ(round(p.x),round(p.y));
	if (p.z<surz)
		return Vect3f::ZERO;
	int x =	round(p.x)&(-ng);
	int y =	round(p.y)&(-ng);
	if (x>p.x) x-=ng;
	if (y>p.y) y-=ng;
	float kx = (p.x - x)/ng;
	float ky = (p.y - y)/ng;
	x>>=shift;
	y>>=shift;
	if (x<0)
		x=0;
	else if (x>=x_count) 
		x = x_count - 1;
	if (y<0)
		y=0;
	else if (y>=y_count) 
		y = y_count - 1;
	int ym = y*x_count;
	int y1m = (y+1)*x_count;
	Vect2f v =  bilinear(nds[x+ym].vel, nds[x+1+ym].vel, nds[x+y1m].vel, nds[x+1+y1m].vel, kx, ky);
	float z = bilinear(nds[x+ym].maxz,nds[x+1+ym].maxz, nds[x+y1m].maxz, nds[x+1+y1m].maxz, kx, ky)+surz;
	z = (z<=p.z)? 0 : z-p.z;
	return Vect3f(v.x,v.y,z);
}
void cmGrid::DrawGrid()
{
	for(int x = x_count-1;x>=0;--x)
		for(int y = y_count-1;y>=0;--y)
		{
			Vect3f v1(x<<shift, y<<shift,100);
//			if (nds[x][y].vel==Vect3f::ZERO)
//				terRenderDevice->DrawPoint(v1,sColor4c(0,0,255,255));
//			else
			{
				Vect3f v2(nds[x+y*x_count].vel.x/10,nds[x+y*x_count].vel.y/10,0);
				v2+=v1;
				terRenderDevice->DrawLine(v1,v2,sColor4c(0,0,255,255));
			}
				terRenderDevice->DrawPoint(v1,sColor4c(255,0,0,255));
		}
}

void cmGrid::DrawZ()
{
	for(int x = x_count-1;x>=0;--x)
		for(int y = y_count-1;y>=0;--y)
		{
			Vect3f v1(x<<shift, y<<shift,100);
			if (nds[x+y*x_count].maxz==0)
				terRenderDevice->DrawPoint(v1,sColor4c(0,0,255,255));
			else
			{
				Vect3f v2 = v1 + Vect3f(0,0,nds[x+y*x_count].maxz)*10;
				terRenderDevice->DrawLine(v1,v2,sColor4c(0,0,255,255));
			}
		}
}

//==================================================================
cQuantumWind::~cQuantumWind()
{
	xassert(ef);
	ef->StopAndReleaseAfterEnd();
}
void cQuantumWind::operator()(WindNode& nd, float x, float y)
{
	x-=pos.x;
	y-=pos.y;
	float r1 = sqrt(x*x+y*y);
	if (r1>=r) return;
	Vect2f nr1(x,y); FastNormalize(nr1);
	Vect2f nv(vel);	 FastNormalize(nv);
	float ca = /*sqrt*/(abs(nv.dot(nr1)));
	float r1r = r1/r;
	float k = (1 - r1r*ca);
	nd.vel.x+=vel.x*k;
	nd.vel.y+=vel.y*k;
	nd.maxz +=maxz*r1r;
}
void cQuantumWind::Animate(float dt)
{
	pos.x+=vel.x*dt;
	pos.y+=vel.y*dt;
	r+=dr*dt;
	float k = k_fading*dt;
	vel.x+=k;
	vel.y+=k;
}
bool cQuantumWind::IsLive()
{
	return sqrt(vel.x*vel.x + vel.y*vel.y)>10;
}

void cQuantumWind::Draw()
{
	xassert(ef);
	ef->SetPosition(MatXf(Mat3f(M_PI/2+atan2(vel.y,vel.x),Z_AXIS), Vect3f(pos.x, pos.y, surf_z + maxz)));
}

//====================================================================
void cQuantumWindW::operator()(WindNode& nd, float x, float y)
{
	x-=pos.x;
	y-=pos.y;
	float r1 = sqrt(x*x+y*y);
	if (r1>=r) return;
	float k = sin(r1/r*M_PI);
	nd.vel.x += w*y*k;
	nd.vel.y += -w*x*k;
	float ss = 0.3f+rnd.frand()*0.3f;
	if (r1>ss*r)
	{
		Vect2f next(x+nd.vel.x*dtime, y+ nd.vel.y*dtime);
		float nr = sqrt(sqr(next.x)+sqr(next.y));
		if(r1<nr)
		{
			next = next - next*(nr/r1);
			nd.vel += next/dtime;
		}
	}
	nd.vel.x+=vel.x*k;
	nd.vel.y+=vel.y*k;
	nd.maxz =(maxz*r1/r+nd.maxz)/2;
}
void cQuantumWindW::Draw()
{
	xassert(ef);
	ef->SetPosition(MatXf(Mat3f::ID, Vect3f(pos.x, pos.y, surf_z)));
}

//==================================================================
//реализация cMapWind
cMapWind::cMapWind()
{
	show_grid = false;
	show_z = false;
	gb_VisGeneric->SetEffectLibraryPath("resource\\fx","resource\\fx\\textures");
	EffectLibrary* lib=gb_VisGeneric->GetEffectLibrary("tornado");
	trn = lib->Get("effect 1");
	lib=gb_VisGeneric->GetEffectLibrary("wind");
	wind = lib->Get("effect 1");
	mapw = this;
}
//------------------------------------------------------------------
inline Vect3f cMapWind::GetVelocity(Vect3f& pos)
{
	return grid.GetVelocity(pos);
}
cMapWind::~cMapWind()
{
	//temp
	vector<ObjNodes>::iterator it;
	FOR_EACH(objects,it)
		RELEASE(it->obj);
}

void ObjNodes::Set(cObject3dx* pObj)
{
	xassert(pObj);
	obj = pObj;
	vector<cStaticNode>::iterator it;
	int i=0;
	FOR_EACH(pObj->GetStatic()->nodes, it)
	{
		if (memcmp(it->name.begin(),"wind",4)==0)
		{
			inds.push_back(i);
		}
		i++;
	}
}

//------------------------------------------------------------------
void cMapWind::Animate(float dt)
{
	dt/=1000;
	dtime = dt;
	grid.Clear();
	vector<cQuantumWind*>::iterator it;
	for(it = quants.begin();it<quants.end();++it)
	{
		cQuantumWind* q = *it;
		q->Animate(dt);
		int x = round(q->pos.x);
		int y = round(q->pos.y);
		if (x>=map_width || y>=map_heigth ||!q->IsLive())
		{
			delete q;
			it = quants.erase(it);
			++it;
			continue;
		}
		q->surf_z = GetZ(x, y);
		grid.Scan(q->pos.x, q->pos.y, q->r, *q);
	}
	vector<ObjNodes>::iterator obj;
	static float wind_force = 1.f;
	static float k = 15.0f;
	static float kr = 0.6f;
	FOR_EACH(objects, obj)
	{
		vector<NodeObj>::iterator i;
		FOR_EACH(obj->inds, i)
		{
			MatXf mx = obj->obj->GetNodePosition(i->ix);
			Vect3f v = mx.trans();
			v = GetVelocity(v);
			i->vel+=(v*(wind_force*dt) - i->dis*(k*dt/2));
			//i->vel*=kr/(1.f-dt);
			float kr_dt=kr*dt;
			kr_dt=min(kr_dt,0.9f);
			i->vel-=i->vel*kr_dt;
			Vect3f dds = i->vel*dt;
			i->dis += dds;
/*
			if (i->dis.norm2()<0.1f)
			{
				obj->obj->RestoreUserTransform(i->ix);
				continue;
			}
*/
			mx.rot().invert();
			v = mx.rot()*i->dis;
			MatXf u(Mat3f::ID,v);
			obj->obj->SetUserTransform(i->ix,u);
		}
	}
}
//------------------------------------------------------------------
void cMapWind::Draw()
{
	vector<cQuantumWind*>::iterator q;
	FOR_EACH(quants, q)
		(*q)->Draw();
	//temp
	if(show_grid)
		grid.DrawGrid();
	if(show_z)
		grid.DrawZ();
}
void cMapWind::CreateQuant(Vect2f& pos, Vect2f& vel, float max_z, float r, float dr, float fading)
{
	cQuantumWind *t = new cQuantumWind();
	t->pos = pos;
	t->r = r;
	t->dr = dr;
	t->vel = vel;
	t->maxz = max_z;
	t->k_fading = fading;
	wind->RelativeScale(r/50);
	t->ef = terMapPoint->GetScene()->CreateEffect(*wind,NULL,1.0f,false);
	xassert(t->ef);
	t->ef->SetAutoDeleteAfterLife(true);
	t->surf_z = GetZ(round(pos.x),round(pos.y));
	t->ef->AddZ(t->maxz);
	t->Draw();
	wind->RelativeScale(50/r);
	quants.push_back(t);
}

void cMapWind::CreateQuantW(Vect2f& pos, Vect2f& vel, float max_z, float r, float dr, float fading, float w)
{
	cQuantumWindW *t = new cQuantumWindW();
	t->pos = pos;
	t->r = r;
	t->dr = dr;
	t->vel = vel;
	t->maxz = max_z;
	t->k_fading = fading;
	t->w = w;
	trn->RelativeScale(r/50);
	t->ef = terMapPoint->GetScene()->CreateEffect(*trn,NULL,1.0f,false);
	xassert(t->ef);
	t->ef->SetAutoDeleteAfterLife(true);
	t->surf_z = GetZ(round(pos.x),round(pos.y));
	t->Draw();
	trn->RelativeScale(50/r);
	quants.push_back(t);
}

//------------------------------------------------------------------
void cMapWind::Init(int w, int h, int n,cWater* pWater_)
{
	pWater = pWater_;
	grid.InitGrid(w, h, n);
	const int no = 10;
	objects.resize(no);
	for(int i=0;i<no;i++)
	{
		cObject3dx* obj = terScene->CreateObject3dx("resource\\balmer\\wind.3DX");
		float x = rnd(200)+1000;
		float y = rnd(200)+1000;
		MatXf pos(Mat3f(rnd.frand()*2*M_PI,Z_AXIS), Vect3f(x,y,GetZ(x,y)));
		obj->SetPosition(pos);
		obj->Update();
		obj->SetScale(rnd.frand()*0.5f+0.3f);
		objects[i].Set(obj);
	}
	map_width = w;
	map_heigth = h;
}

void cMapWind::Again()
{
	vector<cEffect*>::iterator it;
	FOR_EACH(trs,it)
	{
		cEmitterBase* em = (*it)->GetEmitN(0);
		int n = em->GetParticleCount();
		for(int i=em->GetParticleCount()-1;i>=0;--i)
			em->GetParticlePos(i).x = 1e20f;
	}
	vector<ObjNodes>::iterator obj;
	FOR_EACH(objects, obj)
	{
		vector<NodeObj>::iterator i;
		FOR_EACH(obj->inds, i)
		{
			i->dis = Vect3f(50,50,0);
			i->vel = Vect3f::ZERO;
		}
	}
}


void cMapWind::Space(int mode)
{
	static n = 10;
	static float maxv = 500;
	for(int i=0;i<n;i++)
	{
		Vect2f pos(rnd(1000),rnd(1000));
		float r = rnd(100)+100;
		float w = 1+5*(rnd.frand()-0.5);
		Vect2f vel(rnd(maxv)+50,rnd(maxv)+50);
		float fad = 0;//rnd.frand()-.5;
		float dr = rnd(5);
		if (!mode)
			CreateQuant(pos, vel, rnd(100), r, dr, fad);
		else
			CreateQuantW(pos, vel,rnd(100), r, dr, fad, w);
	}
//	CreateQuant(Vect2f(1000,1000), Vect2f(maxv,maxv), 10, 1000, 0, 0);
}


void cMapWind::GetSpeed(BYTE* data,int pitch,float mul)
{
	Vect2i size=grid.GetSize();
	BYTE* curdata=data;
	WindNode* nd = grid.nds;
	for(int y=0;y<size.y;y++)
	{
		for(int x=0;x<size.x;x++)
		{
			Vect2f& vel=nd->vel;
			++nd;
			float f=fabs(vel.x)+fabs(vel.y);
			int p=round(f*mul);
			p=clamp(p,0,255);
			curdata[x]=p;
		}
		curdata+=pitch;
	}
}
