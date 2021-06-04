#include "StdAfxRD.h"
#include "NParticle.h"
#include "scene.h"
#include <algorithm>
#include "NParticleID.h"
#include "TileMap.h"
#include "ForceField.h"
#include "MeshBank.h"
#include "ObjMesh.h"

static RandomGenerator rnd;
static vector<Vect2f> rotate_angle;

KeyFloat::value KeyFloat::none=0;
KeyPos::value KeyPos::none=Vect3f::ZERO;
KeyRotate::value KeyRotate::none=QuatF::ID;
KeyColor::value KeyColor::none=sColor4f(0,0,0,0);
float KeyGeneral::time_delta=0.05f;//в секундах
static const float INV_2_PI=1/(2*M_PI);

float cEffect::GetParticleRateReal()const
{
	return particle_rate*Option_ParticleRate;
}
///////////////////////////cEmitterInterface///////////////////////////
cEmitterInterface::cEmitterInterface()
:cUnkObj(NULL)
{
	parent=NULL;

	time=0;
	old_time=0;
	b_pause=false;

	cycled=false;
	start_time=0;
	emitter_life_time=1;
	unical_id=NULL;
	no_show_obj_editor=false;
}

cEmitterInterface::~cEmitterInterface()
{
}

void cEmitterInterface::SetPause(bool b)
{
	b_pause=b;
}

void cEmitterInterface::SetParent(cEffect* parent_)
{
	parent=parent_;
}

void cEmitterInterface::SetCycled(bool cycled_)
{
	cycled=cycled_;
}
///////////////////////////cEmitterBase////////////////////////////////
cEmitterBase::cEmitterBase()
{
	parent=NULL;

	sprite_blend=ALPHA_BLEND;

	SetMaxTime(4.0f,4.0f);
	Bound.SetInvalidBox();
	InitRotateAngle();
	SetDummyTime(0.2f);
	prolonged=false;
	time_emit_prolonged=0;
	disable_emit_prolonged=false;
	init_prev_matrix=false;
	cur_one_pos=-1;
	chPlume = false;
	TraceCount = 1;
	PlumeInterval = 0.01f;
	other = NULL;
}

cEmitterBase::~cEmitterBase()
{
}

void cEmitterBase::InitRotateAngle()
{
	if(!rotate_angle.empty())return;
	rotate_angle.resize(rotate_angle_size);
	for(int i=0;i<rotate_angle_size;i++)
	{
		float a=(i*2*M_PI)/rotate_angle_size;
		Vect2f& p=rotate_angle[i];
		p.x=cos(a);
		p.y=sin(a);
	}
}

void cEmitterBase::SetMaxTime(float emitter_life,float particle_life)
{
	emitter_life_time=emitter_life;
	inv_emitter_life_time=1/max(emitter_life,0.001f);

	particle_life_time=particle_life;
	inv_particle_life_time=1/max(particle_life,0.001f);
}

Vect3f cEmitterBase::RndBox(float x,float y,float z)
{
	Vect3f f;
	if(0)
	{
		f.x=rnd.frnd(x*0.5f);
		f.y=rnd.frnd(y*0.5f);
		f.z=rnd.frnd(z*0.5f);
	}else
	{
		int rrr=rnd();
		float sgn=rrr&1?0.5:-0.5f;
		int n=(rrr>>1)%3;
		switch(n)
		{
		case 0:
			f.x=rnd.frnd(x*0.5f);
			f.y=rnd.frnd(y*0.5f);
			f.z=z*sgn;
			break;
		case 1:
			f.x=rnd.frnd(x*0.5f);
			f.y=y*sgn;
			f.z=rnd.frnd(z*0.5f);
			break;
		case 2:
			f.x=x*sgn;
			f.y=rnd.frnd(y*0.5f);
			f.z=rnd.frnd(z*0.5f);
			break;
		}
	}

	return f;
}

Vect3f cEmitterBase::RndSpere(float r)
{
	Vect3f v(rnd.frand()-0.5f,rnd.frand()-0.5f,rnd.frand()-0.5f);
	v.Normalize();
	return r*v;
}

Vect3f cEmitterBase::RndCylinder(float r,float dr,float z)
{
	Vect3f f;
	float a=rnd.frand()*2*M_PI;
	float rr=r+dr*(rnd.frand()-0.5f);
	f.x=rr*sin(a);
	f.y=rr*cos(a);
	f.z=z*(rnd.frand()-0.5f);
	return f;
}

Vect3f cEmitterBase::RndRing(float r,float alpha_min,float alpha_max,float teta_min,float teta_max)
{
	Vect3f f;
	float teta=teta_min+rnd.frand()*(teta_max-teta_min);
	float alpha=alpha_min+rnd.frand()*(alpha_max-alpha_min);
	f.x=r*sin(alpha)*cos(teta);
	f.y=r*cos(alpha)*cos(teta);
	f.z=r*sin(teta);
	return f;
}

bool cEmitterBase::IsVisible(cCamera *pCamera)
{
	if(Bound.min.x>Bound.max.x)
		return false;
	if (relative)
		return pCamera->TestVisible(GetGlobalMatrix(), Bound.min, Bound.max);
	else 
		return pCamera->TestVisible(Bound.min, Bound.max);
	return false;
}

void cEmitterBase::PreDraw(cCamera *pCamera)
{
}

void cEmitterBase::Animate(float dt)
{
	if(b_pause)return;
	dt*=1e-3f;
	if(dt>0.1f)
		dt=0.1f;
	CalcPosition();

	if(cycled && time+dt>emitter_life_time)
	{
		DummyQuant();
		time-=emitter_life_time;
		old_time=time;
	}

	if(!prolonged)
	{
		float tmin=time*inv_emitter_life_time;
		float tmax=(time+dt)*inv_emitter_life_time;
		EmitInstantly(tmin,tmax);
	}

	if(!disable_emit_prolonged)
	if(prolonged)
		EmitProlonged(dt);

	if(time-old_time>dummy_time || time==0)
		DummyQuant();

	time+=dt;
	init_prev_matrix=true;
	prev_matrix=GetGlobalMatrix();
}

void cEmitterBase::BeginInterpolateGlobalMatrix()
{
	next_matrix=GlobalMatrix;
}

void cEmitterBase::InterpolateGlobalMatrix(float f)
{
	if(!init_prev_matrix)
		return;

	MatrixInterpolate(GlobalMatrix,prev_matrix,next_matrix,f);
}

void cEmitterBase::EndInterpolateGlobalMatrix()
{
	GlobalMatrix=next_matrix;
}

void cEmitterBase::CalcPosition()
{
	float t=time*inv_emitter_life_time;
	LocalMatrix.trans()=position.Get(t);
	LocalMatrix.rot()=rotation.Get(t);
	switch(particle_position.type)
	{
	case EMP_3DMODEL:
		GlobalMatrix=parent->GetCenter3DModel()*GetLocalMatrix();
		break;
	case EMP_3DMODEL_INSIDE:
	default:
		GlobalMatrix=parent->GetGlobalMatrix()*GetLocalMatrix();
		break;
	}
}

void cEmitterBase::SetEmitterKey(EmitterKeyBase& k,cEmitter3dObject* models)
{
	if (k.texture_name.find(".avi")!=-1)
		SetTexture(0,GetTexLibrary()->GetElementAviScale(k.texture_name.c_str()));
	else
		SetTexture(0,GetTexLibrary()->GetElement(k.texture_name.c_str()));

	unical_id=&k;

	inv_life_time.resize(k.life_time.size());
	for(int i=0;i<k.life_time.size();i++)
	{
		inv_life_time[i].time=k.life_time[i].time;
		float l=max(k.life_time[i].f,1e-2f);
		inv_life_time[i].f=1/l;
	}

	life_time_delta=k.life_time_delta;
	begin_size=k.begin_size;
	begin_size_delta=k.begin_size_delta;
	num_particle=k.num_particle;
	start_time=k.emitter_create_time;
	SetMaxTime(k.emitter_life_time,k.particle_life_time);

	particle_position=k.particle_position;
	prolonged=k.generate_prolonged;
	cycled=k.cycled;
	chFill = k.chFill;
	relative = k.relative;

	switch(k.sprite_blend)
	{
	case EMITTER_BLEND_ADDING:
		sprite_blend=ALPHA_ADDBLENDALPHA;
		break;
	case EMITTER_BLEND_SUBSTRACT:
		sprite_blend=ALPHA_SUBBLEND;
		break;
	case EMITTER_BLEND_MULTIPLY:
		sprite_blend=ALPHA_BLEND;
		break;
	}

	position=k.emitter_position;
	rotation=k.emitter_rotation;
	rotation_direction=k.rotation_direction;
	GlobalMatrix.trans()=LocalMatrix.trans()=position.Get(0);
	GlobalMatrix.rot()=LocalMatrix.rot()=rotation.Get(0);
	if ((k.particle_position.type == EMP_3DMODEL_INSIDE))
	{
		normal_position = k.normal_position;
		begin_position = k.begin_position;
	}
	chPlume		  = k.chPlume;
	TraceCount    = k.TraceCount;
	PlumeInterval = k.PlumeInterval; 
//	PlumeTimeScaling = k.PlumeTimeScaling;
//	PlumeSizeScaling = k.PlumeSizeScaling;
}
inline Vect3f* cEmitterBase::GetNormal(const int& ix)
{
	switch(particle_position.type)
	{
	case EMP_3DMODEL:
		if ((UINT)ix<parent->GetNorm().size())
			return &parent->GetNorm()[ix];
		break;
	case EMP_3DMODEL_INSIDE:
		if ((UINT)ix<begin_position.size())
			return &normal_position[ix];
		break;
	}
	return NULL;
}

void cEmitterBase::OneOrderedPos(int i,Vect3f& pos)
{
	#define num		particle_position.num
	float size_pos=100;
	switch(particle_position.type)
	{
		case EMP_BOX:
		{
			i %= num.x*num.y*num.z;
			int ix = i%num.x;
			int iy = (i/num.x)%num.y;
			int iz = i/(num.x*num.y);
			pos.x = ix*(particle_position.size.x/(num.x<=1? 1: num.x-1)); 
			pos.y = iy*(particle_position.size.y/(num.y<=1? 1: num.y-1)); 
			pos.z = iz*(particle_position.size.z/(num.z<=1? 1: num.z-1)); 
			if (!chFill)
			{
				if ((ix!=0&&iy!=0&&iz!=0)&&(ix!=num.x-1&&iy!=num.y-1&&iz!=num.z-1))
				{
					int t = rnd(3);
					if (t!=0)pos.x = ix%2 ? 0 : particle_position.size.x;
					if (t!=1)pos.y = iy%2 ? 0 : particle_position.size.y;
					if (t!=2)pos.z = iz%2 ? 0 : particle_position.size.z;
				}
			}
			pos -= particle_position.size/2;
		}
			break;
		case EMP_CYLINDER:
		{
			i %= num.y*num.x*num.z;
			float alpha = (i%num.x)*2*M_PI/num.x;
			float r = (i/(num.x*num.y)+1)*(particle_position.size.x/num.z);
			pos.x = cos(alpha)*r;
			pos.y = -sin(alpha)*r;
			pos.z = ((i/num.x)%num.y)*(particle_position.size.y/(num.y<=1? 1:num.y-1)) - particle_position.size.y/2; 
		}
			break;
		case EMP_SPHERE:
		{
			i %= num.x*num.y*num.z;
			float alpha = (i%num.x)*2*M_PI/num.x;
			float theta = ((i/num.x)%num.y)*2*M_PI/num.y;
			float r = (i/(num.x*num.y)+1)*(particle_position.size.x/num.z);
			pos.setSpherical(alpha,theta,r);
		}
			break;
		case EMP_RING:
		{
			i %= num.x*num.y*num.z;
			float alpha = (i%num.x)*(particle_position.alpha_max-particle_position.alpha_min)/num.x + particle_position.alpha_min;
			float theta = ((i/num.x)%num.y)*(particle_position.teta_max - particle_position.teta_min)/num.y+ particle_position.teta_min;
			float r = (i/(num.x*num.y)+1)*(particle_position.size.x/num.z);
			pos.x=r*sin(alpha)*cos(theta);
			pos.y=r*cos(alpha)*cos(theta);
			pos.z=r*sin(theta);
		}
			break;
	}
	pos*=size_pos;
	#undef num
}

bool cEmitterBase::OnePos(int i,Vect3f& pos, Vect3f* norm /*= NULL*/)
{
	cur_one_pos=-1;
	float size_pos=100;
	if (chFill)
		size_pos = rnd(size_pos);
	if (other)
		return !other->GetRndPos(pos, norm);
	else switch(particle_position.type)
	{
	default:
	case EMP_BOX:
		if (particle_position.fix_pos)	OneOrderedPos(i, pos);
		else pos=size_pos*RndBox(particle_position.size.x,particle_position.size.y,particle_position.size.z);
		if(norm) 
		{
			*norm = pos;
		}
		break;
	case EMP_SPHERE:
		if (particle_position.fix_pos)	OneOrderedPos(i, pos);
		else pos=size_pos*RndSpere(particle_position.size.x);
		if(norm) 
		{
			*norm = pos;
		}
		break;
	case EMP_CYLINDER:
		if (particle_position.fix_pos)	OneOrderedPos(i, pos);
		else pos=size_pos*RndCylinder(particle_position.size.x,0,particle_position.size.y);
		if(norm) 
		{
			norm->set(pos.x,pos.y,0);
		}
		break;
	case EMP_LINE:
		pos.x=0;
		pos.y=0;
		pos.z=(rnd.frand()-0.5f)*size_pos*particle_position.size.x;
		if(norm)
		{
			norm->x = rnd.frand();
			norm->y = rnd.frand();
		}
		break;
	case EMP_RING:
		if (particle_position.fix_pos)	OneOrderedPos(i, pos);
		else pos=size_pos*RndRing(particle_position.size.x,
						particle_position.alpha_min,particle_position.alpha_max,
						particle_position.teta_min,particle_position.teta_max);
		if(norm)
		{
			*norm = pos;
		}
		break;
	case EMP_3DMODEL:
		if(!parent->GetPos().empty())
		{
			cur_one_pos=rnd()%parent->GetPos().size();
			pos=parent->GetPos()[cur_one_pos];
		}else
			pos.set(0,0,0);
		if (norm)
			if (parent->GetNorm().empty())
				norm->set(0,0,0);
			else norm->set(parent->GetNorm()[cur_one_pos]);
		break;
	case EMP_3DMODEL_INSIDE:
		if(!begin_position.empty())
		{
			cur_one_pos=rnd()%begin_position.size();
			pos=begin_position[cur_one_pos];
		}else
			pos.set(0,0,0);
		if (norm)
			if (parent->GetNorm().empty())
				norm->set(0,0,0);
			else norm->set(normal_position[cur_one_pos]);
		break;
	}
	return true;
}

int cEmitterBase::CalcProlongedNum(float dt)
{
	if(time>=emitter_life_time)return 0;
	float t=time*inv_emitter_life_time;
	float rate=max(num_particle.Get(t),1.0f)*parent->GetParticleRateReal();
	time_emit_prolonged+=dt;
	int n=round(time_emit_prolonged*rate);
	if(rate>1e-3f)
		time_emit_prolonged-=n/rate;
	else
		time_emit_prolonged=0;

	return n;
}

//////////////////////////cEmitterInt/////////////////////////////////////
cEmitterInt::cEmitterInt()
{
	g.set(0,0,1);
	use_light=true;
	calc_pos = true;
}
cEmitterInt::~cEmitterInt()
{
}

template<class nParticle> __forceinline int cEmitterBase::PutToBuf(nParticle& p, Vect3f& npos, float& dt,
										cQuadBuffer<sVertexXYZDT1>*& pBuf, 
										const sColor4c& color, const Vect3f& PosCamera,
										const float& size, const cTextureAviScale::RECT& rt,
										const UCHAR mode, MatXf* iGM)

{
	xassert(!p.plume_pos.empty());
	float dv1 = (rt.right - rt.left)/TraceCount;
	float v1 = rt.left;
	Vect3f prev_lt,prev_lb;
	Vect3f sy;
	Vect3f vCameraToObject;
	if(mode&1) vCameraToObject.set(0,0,1);
	else vCameraToObject = PosCamera - /*GetGlobalMatrix()*/npos;
	sy.cross(vCameraToObject, npos - p.plume_pos[0]);
	FastNormalize(sy);
	sy*=size;
	prev_lt = npos - sy;
	prev_lb = npos + sy;

	Vect3f prev_pos = npos;
	float interval = PlumeInterval/TraceCount;
	float real_interval;
	float bt;
	int i;
	if (p.time_summary<1)
	{
		bt = p.time_summary;
		i=0;
		if (bt<interval)
			real_interval= bt;
		else 
			real_interval = interval;
	}else
	{
		bt = p.time_summary;
		i=-1;
		while(bt>1)
		{
			bt-=interval;
			i++;
		}
		real_interval = 1-bt;
		bt =1;
		v1+=((interval-real_interval)+i*interval)/PlumeInterval*(rt.right - rt.left);
	}
	while(bt>=0&&i<TraceCount)
	{
		sVertexXYZDT1* v = pBuf->Get();
		v[0].pos=prev_lt; v[0].diffuse=color; v[0].GetTexel().set(v1, rt.top);		//	(0,0);
		v[1].pos=prev_lb; v[1].diffuse=color; v[1].GetTexel().set(v1, rt.bottom);	//	(0,1);

		Vect3f pos = p.plume_pos[i];
		pos+= (prev_pos - pos)*(dt/(real_interval+dt));
		if(mode&2) 
		{
			xassert(dynamic_cast<cEmitterZ*>(this));
			/*if (relative)
			{
				xassert(iGM);
				const MatXf& GM = GetGlobalMatrix();
				Vect3f pp; //word coordinate
				pp.x = (GM.rot().xrow().x*pos.x + GM.rot().xrow().y*pos.y)  +  GM.trans().x;
				pp.y = (GM.rot().yrow().x*pos.x + GM.rot().yrow().y*pos.y)  +  GM.trans().y;
				pp.z = ((cEmitterZ*)this)->CalcZ(pp.x,pp.y); 
				pos.z = (iGM->rot().zrow().x*pp.x + iGM->rot().zrow().y*pp.y + iGM->rot().zrow().z*pp.z) + iGM->trans().z;
			}else*/
				pos.z = ((cEmitterZ*)this)->CalcZ(pos.x,pos.y); 
		}
		if(mode&1) vCameraToObject.set(0,0,1);
		else vCameraToObject = PosCamera - /*GetGlobalMatrix()*/pos;

		sy.cross(vCameraToObject, prev_pos - pos);
		FastNormalize(sy);
		sy*=size;
		#ifdef  NEED_TREANGLE_COUNT
			parent->AddCountTriangle(2);
			parent->AddSquareTriangle(size*fabs(prev_pos.distance(pos)));
		#endif
		prev_lt = pos-sy;
		prev_lb = pos+sy;
		prev_pos = pos;

		if (p.time_summary>1)v1+=dv1*(real_interval/interval);
		bt-=interval;
		if (bt<0)
			real_interval = interval + bt;
		else
		{
			real_interval = interval;
			p.plume_pos[i] = pos;
		}
		if (p.time_summary<=1)v1+=dv1*(real_interval/interval);
		v[2].pos=prev_lt; v[2].diffuse=color; v[2].GetTexel().set(v1,rt.top);		//  (1,0);
		v[3].pos=prev_lb; v[3].diffuse=color; v[3].GetTexel().set(v1,rt.bottom);	//  (1,1);
		i++;
	}

	if (p.time_summary+dt>=0.99)
	{
		p.time_summary+=dt;
		
		if (p.time_summary-1>PlumeInterval)
			p.time=100;
		else return true;
	}
	return false;
}


const cTextureAviScale::RECT cTextureAviScale::RECT::ID(0.0f, 0.0f, 1.0f, 1.0f);
const float pl_death_time = 0.99f;
void cEmitterInt::Draw(cCamera *pCamera)
{
	MTG();
	if(no_show_obj_editor)
		return;
	cURenderDevice* rd=pCamera->GetRenderDevice();
	if(false)
	{
		vector<Vect3f>& begin_position=parent->GetPos();
		vector<Vect3f>& normal_position=parent->GetNorm();
		if(normal_position.empty())
		{
			vector<Vect3f>::iterator it;
			FOR_EACH(begin_position,it)
			{
				Vect3f p=*it;

				rd->DrawLine(p,p+Vect3f(2,2,2),sColor4c(255,255,0,255));
			}
		}else
		{
			xassert(begin_position.size()==normal_position.size());
			for(int i=0;i<begin_position.size();i++)
			{
				Vect3f p=begin_position[i];
				Vect3f n=normal_position[i];

				rd->DrawLine(p,p+n*3,sColor4c(255,255,0,255));
			}
		}
		rd->FlushPrimitive3D();
	}

	
	float dtime_global=(time-old_time);

	MatXf mat=pCamera->GetMatrix();
	Bound.SetInvalidBox();
	cTextureAviScale* texture = NULL;
//	cTextureAviScale* plume_texture = NULL;
	if (GetTexture(0) && GetTexture(0)->IsAviScaleTexture())
		texture = (cTextureAviScale*)GetTexture(0);

	cQuadBuffer<sVertexXYZDT1>* pBuf=rd->GetQuadBufferXYZDT1();
/*
	Vect3f pos;
	float dtime;
	bool NeedPlume = chPlume;
	if (NeedPlume&&GetTexture(1))
	{
		if (GetTexture(1)->IsAviScaleTexture())
			plume_texture = (cTextureAviScale*)GetTexture(1);
		rd->SetNoMaterial(sprite_blend, 0, GetTexture(1));
		pBuf->BeginDraw();
		int size=Particle.size();
		for(int i=size-1;i>=0;i--)
		{
			nParticle& p=Particle[i];
			if(p.key<0)continue;
			PutPlumeParticle(dtime_global, Bound, texture, plume_texture, 
									mat, p,	pBuf, pos, dtime );
		}
		pBuf->EndDraw();
		NeedPlume = false;
	}
*/
	rd->SetNoMaterial(sprite_blend, 0, GetTexture(0));
	Vect3f CameraPos;
	if (relative)
	{
		if (chPlume)
		{
			MatXf invGlobMatrix(GetGlobalMatrix());
			invGlobMatrix.invert();
			CameraPos = invGlobMatrix*pCamera->GetPos();
		}
		pBuf->BeginDraw(GetGlobalMatrix());
	}else
	{
		if (chPlume) CameraPos = pCamera->GetPos();
		pBuf->BeginDraw();
	}
	int size=Particle.size();
	for(int i=size-1;i>=0;i--)
	{
/*		nParticle& p=Particle[i];
		if(p.key<0)continue;
		if (NeedPlume)
			PutPlumeParticle(dtime_global, Bound, texture, plume_texture, 
									mat, p,	pBuf, pos, dtime );
		const cTextureAviScale::RECT& rt = texture?
					((cTextureAviScale*)texture)->GetFramePos(p.time_summary) :
					cTextureAviScale::RECT::ID;	
		p.PutToBuf(dtime_global, keys[p.key], keys[p.key+1],
					Bound, rt, mat, pBuf, this, pos, dtime, p.time, 1.0f);
*/

		nParticle& p=Particle[i];
		if(p.key<0)continue;
		Vect3f pos;
		float angle;
		sColor4f fcolor;
		float psize;

		float dtime=dtime_global*p.inv_life_time;
		KeyParticleInt& k0=keys[p.key];
		KeyParticleInt& k1=keys[p.key+1];
		
		float& t=p.time;
		float ts=t*k0.inv_dtime;
		if (calc_pos)
			pos = p.pos0+p.vdir*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g*((p.gvel0+t*k0.gravity*0.5f)*t);
		else pos = p.pos0; ///temp
		Bound.AddBound(pos);
		angle = p.angle0+ p.angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
		fcolor = k0.color+(k1.color-k0.color)*ts;
		psize = k0.size+(k1.size-k0.size)*ts;

		//Добавить в массив
		Vect3f sx,sy;
		Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
		rot*=psize*=p.begin_size;
		mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
		mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);

		sColor4c color(round(fcolor.r*p.begin_color.r),round(fcolor.g*p.begin_color.g),
					   round(fcolor.b*p.begin_color.b),round(fcolor.a));
		const cTextureAviScale::RECT& rt = texture?
						((cTextureAviScale*)texture)->GetFramePos(p.time_summary>=1? 0.99f : p.time_summary) :
						cTextureAviScale::RECT::ID;	
		if (chPlume)
		{
			if (PutToBuf(p,pos,dtime,pBuf,color,CameraPos,psize, rt, false))
				continue;
		}
		else 
		{
			sVertexXYZDT1 *v=pBuf->Get();

			v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//	(0,0);
			v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//	(0,1);
			v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//  (1,0);
			v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//  (1,1);
			#ifdef  NEED_TREANGLE_COUNT
				parent->AddCountTriangle(2);
				parent->AddSquareTriangle(psize*psize);
			#endif
		}
		ProcessTime(p,dtime,i,pos);
	}

	pBuf->EndDraw();
	Particle.Compress();

	old_time=time;
}
/*
inline void cEmitterInt::PutPlumeParticle(const float& dtime_global, sBox6f& Bound, 
						 cTextureAviScale*& texture, cTextureAviScale*& plume_texture, 
						 const MatXf& mat, nParticle& p,
						 cQuadBuffer<sVertexXYZDT1>*& pBuf,
						 Vect3f& pos, float& dtime)
{																										
	float tt = p.time;																		
	int key = p.key;																		
	float timesummary = p.time_summary;														
	float interval = PlumeInterval;															
	xassert(PlumeInterval>0.0f && PlumeInterval<1.0f);										
	float tr_size = 1.0f;																	
	for(int j=0;j<TraceCount;j++)															
	{																						
		tt-=interval;																		
		timesummary-=interval;																
		while(tt<0)																			
		{																					
//			if (key==0)
				return;			
			float dt = keys[key-1].dtime;
			tt += keys[--key].dtime;														
		}																					
		const cTextureAviScale::RECT* rt;													
		if(plume_texture)																	
			rt = &plume_texture->GetFramePos((float)j/TraceCount);							
		else if (texture)																	
			rt = &texture->GetFramePos(timesummary);										
		else rt = &cTextureAviScale::RECT::ID;												
		tr_size*=PlumeSizeScaling;															
		p.PutToBuf(dtime_global, keys[key], keys[key+1],									
					Bound, *rt, mat, pBuf, this, pos, dtime, tt, tr_size);					
		interval*=PlumeTimeScaling;															
	}																						
}																								

inline void cEmitterInt::nParticle::PutToBuf(const float& dtime_global, const KeyParticleInt& k0, 
								const KeyParticleInt& k1, sBox6f& Bound, 
								const cTextureAviScale::RECT& rt,
								const MatXf& mat, cQuadBuffer<sVertexXYZDT1>*& pBuf, 
								const cEmitterInt* emi, Vect3f& pos, float& dtime, 
								const float& t, const float trace_size)
{
	float angle;
	sColor4f fcolor;
	float psize;
	dtime=dtime_global*inv_life_time; 

//	float& t=time;
	float ts=t*k0.inv_dtime;
	pos = pos0+vdir*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+emi->g*((gvel0+t*k0.gravity*0.5f)*t);
	Bound.AddBound(pos);
	angle = angle0+ angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
	fcolor = k0.color+(k1.color-k0.color)*ts;
	psize = k0.size+(k1.size-k0.size)*ts;

	//Добавить в массив
	Vect3f sx,sy;
	Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
	rot*=psize*begin_size*trace_size;
	mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
	mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);

	sColor4c color(round(fcolor.r*begin_color.r),round(fcolor.g*begin_color.g),
				round(fcolor.b*begin_color.b),round(fcolor.a));

	sVertexXYZDT1 *v=pBuf->Get();

	v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//	(0,0);
	v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//	(0,1);
	v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//  (1,0);
	v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//  (1,1);

	xassert(emi->parent->AddToCountTriangle(1));
}
*/
void cEmitterInt::ProcessTime(nParticle& p,float dt,int i,Vect3f& cur_pos)
{
	float& t=p.time;
	KeyParticleInt* k0=&keys[p.key];
	KeyParticleInt* k1=&keys[p.key+1];

	if(p.key_begin_time>=0)
	{
		EffectBeginSpeedMatrix* s=&begin_speed[p.key_begin_time];
		
		while(s->time_begin<=p.time_summary)
		{
			Vect3f v=CalcVelocity(*s,p,1);
			
			float& t=p.time;
			float ts=t*k0->inv_dtime;
			if (calc_pos)p.pos0 -= v*(t*(k0->vel+ts*0.5f*(k1->vel-k0->vel)));
			p.vdir += v;

			p.key_begin_time++;
			if(p.key_begin_time>=begin_speed.size())
			{
				p.key_begin_time=-1;
				break;
			}else
				s=&begin_speed[p.key_begin_time];
		}
	}

	while(t+dt>k0->dtime)
	{
		//t==1
		float tprev=t;
		t=k0->dtime;
		float ts=t*k0->inv_dtime;
		if (calc_pos)p.pos0 = p.pos0+p.vdir*(t*(k0->vel+ts*0.5f*(k1->vel-k0->vel)))+g*((p.gvel0+t*k0->gravity*0.5f)*t);
		p.angle0 = p.angle0+ p.angle_dir*(k0->angle_vel*t+t*ts*0.5f*(k1->angle_vel-k0->angle_vel));
		p.gvel0+=t*k0->gravity;

		dt-=k0->dtime-tprev;
		t=0;
		p.key++;
		if(p.key>=keys.size()-1)
		{
			Particle.SetFree(i);
			break;
		}
		
		k0=k1;
		k1=&keys[p.key+1];
	}

	t+=dt;
	p.time_summary+=dt;
}

void cEmitterInt::DummyQuant()
{
	float dtime_global=(time-old_time);
	int size=Particle.size();
	Vect3f pos;
	Bound.SetInvalidBox();
	
	for(int i=0;i<size;i++)
	{
		nParticle& p=Particle[i];
		if(p.key<0)continue;
		float dtime=dtime_global*p.inv_life_time;

		KeyParticleInt& k0=keys[p.key];
		KeyParticleInt& k1=keys[p.key+1];
		float& t=p.time;
		float ts=t*k0.inv_dtime;
		pos = p.pos0+p.vdir*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g*((p.gvel0+t*k0.gravity*0.5f)*t);
		Bound.AddBound(pos);
		
		ProcessTime(p,dtime,i,pos);
	}

	old_time=time;
}

void cEmitterInt::EmitInstantly(float tmin,float tmax)
{
	CKey::iterator it;
	FOR_EACH(num_particle,it)
	{
		KeyFloat pos=*it;
		if(pos.time>=tmin && pos.time<tmax)
		{
			int num=pos.f*parent->GetParticleRateReal();
			int prevsize=Particle.size();
			Particle.resize(prevsize+num);
			for(int i=0;i<num;i++)
			{
//				nParticle& cur=Particle[i+prevsize];
//				EmitOne(cur,0);
				EmitOne(i+prevsize,0);
			}
		}
	}
}

void cEmitterInt::EmitProlonged(float dt)
{
	int n=CalcProlongedNum(dt);
	if(n<=0)return;

	float finterpolate=1.0f/n;
	float delta=dt*finterpolate;
	BeginInterpolateGlobalMatrix();
	for(int i=0;i<n;i++)
	{
		float nt=n-1-i;
		InterpolateGlobalMatrix(finterpolate*nt);
//		nParticle& cur=Particle.GetFree();
//		EmitOne(cur,delta*nt);
		EmitOne(Particle.GetIndexFree(),0); 
	}

	EndInterpolateGlobalMatrix();
}

bool cEmitterInt::GetRndPos(Vect3f& pos, Vect3f* norm)
{
	if (Particle.is_empty())
	{
		cur_one_pos = -1;
		pos.set(0,0,0);
		if (norm)
			norm->set(0,0,0);
		return false;
	}
	else
	{
		int size;
		int i = rnd(size = Particle.size());
		while(Particle[i].key==-1) 
			i = rnd(size);
		cur_one_pos = i;
		nParticle& p = Particle[i];
//		float dtime=dtime_global*p.inv_life_time;
		KeyParticleInt& k0=keys[p.key];
		KeyParticleInt& k1=keys[p.key+1];
		float t=p.time;
		float ts=t*k0.inv_dtime;
		pos = p.pos0+p.vdir*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g*((p.gvel0+t*k0.gravity*0.5f)*t);

		if (norm)
			norm->set(Particle[i].normal);
		return true;
	}
}
Vect3f cEmitterInt::GetVdir(int i)
{
	if (i==-1)
		return Vect3f::ZERO;
	xassert((UINT)i<Particle.size());
	nParticle& p = Particle[i];
	KeyParticleInt& k0=keys[p.key];
	KeyParticleInt& k1=keys[p.key+1];
	float t=p.time;
	float ts=t*k0.inv_dtime;
	return p.vdir*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g*((p.gvel0+t*k0.gravity*0.5f)*t);
}

void cEmitterInt::EmitOne(int ix_cur/*nParticle& cur*/,float begin_time)
{
	xassert((UINT)ix_cur<Particle.size());
	nParticle& cur = Particle[ix_cur];
	float t=time*inv_emitter_life_time;
	float inv_life=inv_life_time.Get(t);
	float dlife=life_time_delta.Get(t);
	cur.key=0;
	cur.gvel0=0;
	cur.inv_life_time=inv_particle_life_time*inv_life*(1+dlife*(rnd.frand()-0.5f));
	cur.time_summary=cur.time=cur.inv_life_time*begin_time;
	cur.key_begin_time=0;

	float dsz=begin_size_delta.Get(t);
	cur.begin_size=begin_size.Get(t)*(1+dsz*(rnd.frand()-0.5f));
	float dv=1+velocity_delta.Get(t)*(rnd.frand()-0.5f);
	cur.angle0=0;

	switch(rotation_direction)
	{
	case ETRD_CW:
		cur.angle_dir=1;
		break;
	case ETRD_CCW:
		cur.angle_dir=-1;
		break;
	default:
		cur.angle_dir=(rnd()&1)?-1:1;
		break;
	}
	if (calc_pos==false)
		cur.pos0.set(0,0,0);
	else if(OnePos(ix_cur,cur.pos0,&cur.normal))
	{
		cur.pos0=relative ? cur.pos0 : GetGlobalMatrix()*cur.pos0;
		cur.normal=GetGlobalMatrix().rot()*cur.normal;
		FastNormalize(cur.normal);
	}
	{
		cur.vdir.set(0,0,0);
		int i;
		for(i=0;i<begin_speed.size();i++)
		{
			if(begin_speed[i].time_begin<1e-3f)
			{
				cur.vdir+=CalcVelocity(begin_speed[i],cur,dv);
			}else
			{
				break;
			}
		}
		cur.key_begin_time=i;
		if(cur.key_begin_time>=begin_speed.size())
			cur.key_begin_time=-1;
	}
	if (other/*&&begin_speed[0].velocity==EMV_INVARIABLY*/)
	{
		cur.vdir.add(other->GetVdir(other->cur_one_pos));
		FastNormalize(cur.vdir);
	}

	CalcColor(cur);
	if (chPlume)
	{	
		cur.plume_pos.resize(TraceCount);
		vector<Vect3f>::iterator it;
		Vect3f& bpos = /*GetGlobalMatrix()*/cur.pos0;
		FOR_EACH(cur.plume_pos,it)
			*it = bpos;
	}
}

Vect3f cEmitterInt::CalcVelocity(const EffectBeginSpeedMatrix& s,const nParticle& cur,float mul)
{
	Vect3f v;
	mul*=s.mul;

	switch(s.velocity)
	{
	default:
	case EMV_BOX:
		v=mul*RndBox(1,1,1);
		break;
	case EMV_SPHERE:
		v=mul*RndSpere(1);
		break;
	case EMV_CYLINDER:
		v=mul*RndCylinder(s.esp.pos.x,s.esp.dpos.x,s.esp.pos.z);
		break;
	case EMV_LINE:
		v.x=0;
		v.y=0;
		v.z=rnd.frand()*mul;
		break;
	case EMV_NORMAL_OUT:
		v=cur.pos0-GetGlobalMatrix().trans();
		v.Normalize();
		v*=mul;
		break;
	case EMV_NORMAL_IN:
		v=cur.pos0-GetGlobalMatrix().trans();
		v.Normalize();
		v*=-mul;
		break;
	case EMV_NORMAL_IN_OUT:
		v=cur.pos0-GetGlobalMatrix().trans();
		v.Normalize();
		if(rnd()&1)
			v*=mul;
		else
			v*=-mul;
		break;
	case EMV_Z:
		v.set(0,0,mul);
		break;
	case EMV_NORMAL_3D_MODEL:
		v  = mul*(cur.normal);
		break;
	case EMV_INVARIABLY:
		v.set(0,0,0);
		break;
	}

	v= relative ? s.rotation*v : GetGlobalMatrix().rot()*s.rotation*v;
	return v;
}

void cEmitterInt::SetKeys(vector<KeyParticleInt>& ks)
{
	keys.resize(ks.size());
	for(int i=0;i<ks.size();i++)
	{
		KeyParticleInt& k=keys[i];
		k=ks[i];
		keys[i].inv_dtime=1/max(keys[i].dtime,0.001f);
/*
		k.color.r*=255.0f;
		k.color.g*=255.0f;
		k.color.b*=255.0f;
*/
		k.color.a*=255.0f;

	}
}
void cEmitterInt::SetEmitterKey(EmitterKeyInt& k,cEmitter3dObject* models)
{
	cEmitterBase::SetEmitterKey(k,models);

	velocity_delta=k.velocity_delta;

	begin_speed.resize(k.begin_speed.size());
	for(int i=0;i<begin_speed.size();i++)
	{
		EffectBeginSpeed& in=k.begin_speed[i];
		EffectBeginSpeedMatrix& out=begin_speed[i];
		out.velocity=in.velocity;
		out.mul=in.mul;
		out.rotation.set(in.rotation);
		out.time_begin=in.time;
		out.esp=in.esp;
	}

	sort(begin_speed.begin(),begin_speed.end());

	SetKeys(k.GetKey());
	use_light=k.use_light;

	if(particle_position.type==EMP_3DMODEL && models && use_light)
	{
		models->GetEmitterMaterial(material);
	}
}

void cEmitterInt::CalcColor(nParticle& cur)
{
	if(cur_one_pos<0 || !use_light)
	{
		//cur.begin_color.set(rnd(255),rnd(255),rnd(255),255);
		cur.begin_color.set(255,255,255,255);
		return;
	}

	Vect3f LightDirection;
	parent->GetScene()->GetLighting(&LightDirection);
	/*Vect3f norm,norm1,norm2;///temp
	if (particle_position.type==EMP_3DMODEL_INSIDE)   
		norm1 = norm = normal_position[cur_one_pos];
	else 
		norm2 = norm = parent->GetNorm()[cur_one_pos];*/
	const Vect3f& norm = (particle_position.type==EMP_3DMODEL_INSIDE)? normal_position[cur_one_pos] :
															   parent->GetNorm()[cur_one_pos];			
	Vect3f word_n=Normalize(GetGlobalMatrix().rot()*norm);
	float lit=-LightDirection.dot(word_n);
	if(lit<0)
		lit=0;
	sColor4f c=material.Diffuse*lit+material.Ambient;

	c.r=clamp(c.r,0.0f,1.0f);
	c.g=clamp(c.g,0.0f,1.0f);
	c.b=clamp(c.b,0.0f,1.0f);
	
	cur.begin_color=c;
}

////////////////////////cEmitterSpl//////////////////////

void HermitCoef(float p0,float p1,float p2,float p3,float& t0,float& t1,float& t2,float& t3)
{
	t3=(1.5f*(p1-p2)+(-p0+p3)*0.5f);
	t2=(2.0f*p2+p0-p3*0.5f-2.5f*p1);
	t1=(p2-p0)*0.5f;
	t0=p1;
}

void HermitCoef(const Vect3f& p0,const Vect3f& p1,const Vect3f& p2,const Vect3f& p3,
				Vect3f& t0,Vect3f& t1,Vect3f& t2,Vect3f& t3)
{
	HermitCoef(p0.x,p1.x,p2.x,p3.x, t0.x,t1.x,t2.x,t3.x);
	HermitCoef(p0.y,p1.y,p2.y,p3.y, t0.y,t1.y,t2.y,t3.y);
	HermitCoef(p0.z,p1.z,p2.z,p3.z, t0.z,t1.z,t2.z,t3.z);
}

CKeyPosHermit::CKeyPosHermit()
{
	//cbegin=cend=T_CLOSE;
	cbegin=cend=T_FREE;
}

Vect3f CKeyPosHermit::Clamp(int i)
{
	VISASSERT(size()>=2);
	if(i<0)
	{
		switch(cbegin)
		{
		case T_CLOSE:
			return front().pos;
		case T_FREE:
			{
				Vect3f p0=front().pos,p1=(*this)[1].pos;
				return p0-(p1-p0);
			}
		case T_CYCLE:
			return back().pos;
		}
	}

	if(i>=size())
	{
		switch(cend)
		{
		case T_CLOSE:
			return back().pos;
		case T_FREE:
			{
				Vect3f p0=back().pos,p1=(*this)[size()-2].pos;
				return p0-(p1-p0);
			}
		case T_CYCLE:
			return front().pos;
		}
	}

	return (*this)[i].pos;
}

Vect3f CKeyPosHermit::Get(float t)
{
	if(empty())return KeyPos::none;
	if(size()==1)return (*this)[0].Val();

	if(t<(*this)[0].time)
		return (*this)[0].Val();

	for(int i=1;i<size();i++)
	if(t<(*this)[i].time)
	{
		Vect3f fm=Clamp(i-2);
		KeyPos& f0=(*this)[i-1];
		KeyPos& f1=(*this)[i];
		Vect3f fp=Clamp(i+1);
		float dx=f1.time-f0.time;
		xassert(dx>0);
		xassert(t>=f0.time);
		float tx=(t-f0.time)/max(dx,1e-3f);

		return HermitSpline(tx,fm,f0.pos,f1.pos,fp);
	}

	return back().Val();
}

cEmitterSpl::cEmitterSpl()
{
}

cEmitterSpl::~cEmitterSpl()
{
}
/*
inline void cEmitterSpl::nParticle::PutToBuf(const float& dtime_global, HeritKey& k, const KeyParticleSpl& k0, 
								const KeyParticleSpl& k1, sBox6f& Bound, 
								const cTextureAviScale::RECT& rt,
								const MatXf& mat, cQuadBuffer<sVertexXYZDT1>*& pBuf, 
								cEmitterSpl* emi, Vect3f& pos, float& dtime, 
								const float& t, const float& trace_size, const float& htime)
{
	float angle;
	sColor4f fcolor;
	float psize;
	dtime=dtime_global*inv_life_time;

	{
//		HeritKey& k=emi->hkeys[hkey];
		float ts=htime*k.inv_dtime;
		k.Get(pos,ts);
		this->pos.xformPoint(pos);
		Bound.AddBound(pos);
	}
	{
//		float& t=p.time;
//		KeyParticleSpl& k0=keys[p.key];
//		KeyParticleSpl& k1=keys[p.key+1];
		
		float ts=t*k0.inv_dtime;
		angle = angle0+ angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
		fcolor = k0.color+(k1.color-k0.color)*ts;
		psize = k0.size+(k1.size-k0.size)*ts;
	}
	//Добавить в массив
	Vect3f sx,sy;
	Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
	rot*=psize*begin_size*trace_size;
	mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
	mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);

	sColor4c color(round(fcolor.r),round(fcolor.g),round(fcolor.b),round(fcolor.a));
		
	sVertexXYZDT1 *v=pBuf->Get();
	v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//set(0,0);
	v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//set(0,1);
	v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//set(1,0);
	v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//set(1,1);

	xassert(emi->parent->AddToCountTriangle(1));
}
inline void cEmitterSpl::PutPlumeParticle(const float& dtime_global, sBox6f& Bound, 
						 cTextureAviScale*& texture, cTextureAviScale*& plume_texture, 
						 const MatXf& mat, nParticle& p,
						 cQuadBuffer<sVertexXYZDT1>*& pBuf,
						 Vect3f& pos, float& dtime)
{																										
	float tt = p.time;																		
	int key = p.key;																		
	float timesummary = p.time_summary;		
	float htime = p.htime;
	float interval = PlumeInterval;															
	xassert(PlumeInterval>0.0f && PlumeInterval<1.0f);										
	float tr_size = 1.0f;																	
	for(int j=0;j<TraceCount;j++)															
	{																						
		tt-=interval;																		
		timesummary-=interval;																
		htime-=interval;
		while(tt<0)																			
		{																					
//			if (key==0)																	
				return;																		
			tt += keys[--key].dtime;														
			htime+=hkeys[key].dtime;
		}																					
		const cTextureAviScale::RECT* rt;													
		if(plume_texture)																	
			rt = &plume_texture->GetFramePos((float)j/TraceCount);							
		else if (texture)																	
			rt = &texture->GetFramePos(timesummary);										
		else rt = &cTextureAviScale::RECT::ID;												
		tr_size*=PlumeSizeScaling;															
		p.PutToBuf(dtime_global, hkeys[key], keys[key], keys[key+1],									
					Bound, *rt, mat, pBuf, this, pos, dtime, tt, tr_size, htime);					
		interval*=PlumeTimeScaling;															
	}																						
}																								
*/
void cEmitterSpl::Draw(cCamera *pCamera)
{
	if(no_show_obj_editor)
		return;
	
	float dtime_global=(time-old_time);

	MatXf mat=pCamera->GetMatrix();
	Bound.SetInvalidBox();
	cURenderDevice* rd=pCamera->GetRenderDevice();

	cTextureAviScale* texture = NULL;
//	cTextureAviScale* plume_texture = NULL;
	if (GetTexture(0) && GetTexture(0)->IsAviScaleTexture())
		texture = (cTextureAviScale*)GetTexture(0);

	cQuadBuffer<sVertexXYZDT1>* pBuf=rd->GetQuadBufferXYZDT1();
/*
	Vect3f pos;
	float dtime;
	bool NeedPlume = chPlume;
	if (NeedPlume&&GetTexture(1))
	{
		if (GetTexture(1)->IsAviScaleTexture())
			plume_texture = (cTextureAviScale*)GetTexture(1);
		rd->SetNoMaterial(sprite_blend, 0, GetTexture(1));
		rd->SetRenderState( RS_CULLMODE, D3DCULL_NONE );
		pBuf->BeginDraw();
		int size=Particle.size();
		for(int i=size-1;i>=0;i--)
		{
			nParticle& p=Particle[i];
			if(p.key<0)continue;
			PutPlumeParticle(dtime_global, Bound, texture, plume_texture, 
									mat, p,	pBuf, pos, dtime );
		}
		pBuf->EndDraw();
		NeedPlume = false;
	}
*/
	rd->SetNoMaterial(sprite_blend, 0, GetTexture(0));
	rd->SetRenderState( RS_CULLMODE, D3DCULL_NONE );
	Vect3f CameraPos;
	if (relative)
	{
		if (chPlume)
		{
			MatXf invGlobMatrix(GetGlobalMatrix());
			invGlobMatrix.invert();
			CameraPos = invGlobMatrix*pCamera->GetPos();
		}
		pBuf->BeginDraw(GetGlobalMatrix());
	}else
	{
		if (chPlume) CameraPos = pCamera->GetPos();
		pBuf->BeginDraw();
	}
	int size=Particle.size();
	for(int i=size-1;i>=0;i--)
	{
/*		nParticle& p=Particle[i];
		if(p.key<0)continue;
		if (NeedPlume)
			PutPlumeParticle(dtime_global, Bound, texture, plume_texture, 
									mat, p,	pBuf, pos, dtime );
		const cTextureAviScale::RECT& rt = texture?
					((cTextureAviScale*)texture)->GetFramePos(p.time_summary) :
					cTextureAviScale::RECT::ID;	
		p.PutToBuf(dtime_global, hkeys[p.key], keys[p.key], keys[p.key+1],
					Bound, rt, mat, pBuf, this, pos, dtime, p.time, 1.0f, p.htime);
		ProcessTime(p,dtime,i);
*/
		nParticle& p=Particle[i];
		if(p.key<0)continue;
		Vect3f pos;
		float angle;
		sColor4f fcolor;
		float psize;

		float dtime=dtime_global*p.inv_life_time;

		{
			HeritKey& k=hkeys[p.hkey];
			float ts=p.htime*k.inv_dtime;
			k.Get(pos,ts);
			p.pos.xformPoint(pos);
			Bound.AddBound(pos);
		}

		{
			float& t=p.time;
			KeyParticleSpl& k0=keys[p.key];
			KeyParticleSpl& k1=keys[p.key+1];
			
			float ts=t*k0.inv_dtime;
			angle = p.angle0+ p.angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
			fcolor = k0.color+(k1.color-k0.color)*ts;
			psize = k0.size+(k1.size-k0.size)*ts;
		}

		//Добавить в массив
		Vect3f sx,sy;
		Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
		rot*=psize*=p.begin_size;
		mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
		mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);

		sColor4c color(round(fcolor.r),round(fcolor.g),round(fcolor.b),round(fcolor.a));
		const cTextureAviScale::RECT& rt = texture ?
			((cTextureAviScale*)texture)->GetFramePos(p.time_summary>=1? 0.99f : p.time_summary) :
						cTextureAviScale::RECT::ID;	
		if (chPlume)
		{
			if (PutToBuf(p,pos,dtime,pBuf,color,CameraPos,psize, rt, false))
				continue;
		}
		else 
		{
			sVertexXYZDT1 *v=pBuf->Get();
			v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//set(0,0);
			v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//set(0,1);
			v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//set(1,0);
			v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//set(1,1);
			#ifdef  NEED_TREANGLE_COUNT
				parent->AddCountTriangle(2);
				parent->AddSquareTriangle(psize*psize);
			#endif
		}
		ProcessTime(p,dtime,i);
	}

	pBuf->EndDraw();
/*
	cQuadBuffer<sVertexXYZDT1>* pBuf=pCamera->GetRenderDevice()->GetQuadBufferXYZDT1();
	pCamera->GetRenderDevice()->SetNoMaterial(sprite_blend,0,texture);
	pCamera->GetRenderDevice()->SetRenderState( RS_CULLMODE, D3DCULL_NONE );
	pBuf->BeginDraw();
	bool isAviTexture = texture&&texture->IsAviScaleTexture();
	int size=Particle.size();
	for(int i=size-1;i>=0;i--)
	{
		nParticle& p=Particle[i];
		if(p.key<0)continue;
		Vect3f pos;
		float angle;
		sColor4f fcolor;
		float psize;

		float dtime=dtime_global*p.inv_life_time;

		{
			HeritKey& k=hkeys[p.hkey];
			float ts=p.htime*k.inv_dtime;
			k.Get(pos,ts);
			p.pos.xformPoint(pos);
			Bound.AddBound(pos);
		}

		{
			float& t=p.time;
			KeyParticleSpl& k0=keys[p.key];
			KeyParticleSpl& k1=keys[p.key+1];
			
			float ts=t*k0.inv_dtime;
			angle = p.angle0+ p.angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
			fcolor = k0.color+(k1.color-k0.color)*ts;
			psize = k0.size+(k1.size-k0.size)*ts;
		}

		//Добавить в массив
		Vect3f sx,sy;
		Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
		rot*=psize*p.begin_size;
		mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
		mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);

		sColor4c color(round(fcolor.r),round(fcolor.g),round(fcolor.b),round(fcolor.a));
		
		sVertexXYZDT1 *v=pBuf->Get();
		const cTextureAviScale::RECT& rt = isAviTexture ?
					((cTextureAviScale*)texture)->GetFramePos(p.time) :
					cTextureAviScale::RECT::ID;	

		v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//set(0,0);
		v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//set(0,1);
		v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//set(1,0);
		v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//set(1,1);

		ProcessTime(p,dtime,i);
	}

	pBuf->EndDraw();
*/
	Particle.Compress();
	old_time=time;
}

void cEmitterSpl::SetEmitterKey(EmitterKeySpl& k,cEmitter3dObject* models)
{
	cEmitterBase::SetEmitterKey(k,models);
	SetKeys(k.GetKey());

	direction=k.direction;
	if(k.p_position.size()==0)return;

	hkeys.resize(k.p_position.size()-1);
	float dt=1/(float)(hkeys.size());
	int i;
	for(i=0;i<hkeys.size();i++)
	{
		HeritKey& hk=hkeys[i];
		if(k.p_position_auto_time)
			hk.dtime=dt;
		else
			hk.dtime=k.p_position[i+1].time-k.p_position[i].time;

		hk.inv_dtime=1/hk.dtime;
		Vect3f p0,p1,p2,p3;
		p0=k.p_position.Clamp(i-1);
		p1=k.p_position.Clamp(i);
		p2=k.p_position.Clamp(i+1);
		p3=k.p_position.Clamp(i+2);

		HermitCoef(p0,p1,p2,p3,	hk.H0,hk.H1,hk.H2,hk.H3);
#ifdef EMITTER_TCONTROL
		hk.t0=0;hk.t1=1;hk.t2=hk.t3=0;
#endif
	}

	if(!k.p_position_auto_time)
		return;

	vector<float> klen;
	float sum=0;
	klen.resize(hkeys.size());
	for(i=0;i<hkeys.size();i++)
	{
		HeritKey& hk=hkeys[i];
		Vect3f p0,p1;
		hk.Get(p0,0);
		const int mx=16;
		float len=0,dt=1/(float)mx;
		for(int j=0;j<mx;j++)
		{
			float t=(j+1)*dt;
			hk.Get(p1,t);
			len+=p0.distance(p1);
			p0=p1;
		}

		klen[i]=len;
		sum+=len;
	}
#ifdef EMITTER_TCONTROL
	for(i=0;i<hkeys.size();i++)
	{
		HeritKey& hk=hkeys[i];
		float d0,d1,d2;
		hk.dtime=klen[i]/sum;
		hk.inv_dtime=1/hk.dtime;

		d0=klen[(i-1>0)?i-1:0]/sum;
		d1=klen[i]/sum;
		d2=klen[(i+1<hkeys.size())?(i+1):(hkeys.size()-1)]/sum;

		float p0=-d0/d1,p1=0,p2=1,p3=(d1+d2)/d1;

		HermitCoef(p0,p1,p2,p3, hk.t0,hk.t1,hk.t2,hk.t3);
		//hk.t0=0;hk.t1=1;hk.t2=hk.t3=0;
	}
#else
	for(i=0;i<hkeys.size();i++)
	{
		HeritKey& hk=hkeys[i];
		hk.dtime=klen[i]/sum;
		hk.inv_dtime=1/hk.dtime;
	}
#endif
}

void cEmitterSpl::SetKeys(vector<KeyParticleSpl>& ks)
{
	keys.resize(ks.size());
	for(int i=0;i<ks.size();i++)
	{
		KeyParticleSpl& k=keys[i];
		k=ks[i];
		keys[i].inv_dtime=1/max(keys[i].dtime,0.001f);
		k.color.r*=255.0f;
		k.color.g*=255.0f;
		k.color.b*=255.0f;
		k.color.a*=255.0f;
	}
}

void cEmitterSpl::EmitInstantly(float tmin,float tmax)
{
	CKey::iterator it;
	FOR_EACH(num_particle,it)
	{
		KeyFloat pos=*it;
		if(pos.time>=tmin && pos.time<tmax)
		{
			int num=pos.f;
			int prevsize=Particle.size();
			Particle.resize(prevsize+num);
			for(int i=0;i<num;i++)
			{
//				nParticle& cur=Particle[i+prevsize];
//				EmitOne(cur,0);
				EmitOne(i+prevsize, 0);
			}
		}
	}
}

void cEmitterSpl::EmitProlonged(float dt)
{
	int n=CalcProlongedNum(dt);
	if(n<=0)return;

	float finterpolate=1.0f/n;
	float delta=dt*finterpolate;
	BeginInterpolateGlobalMatrix();
	for(int i=0;i<n;i++)
	{
		float nt=n-1-i;
		InterpolateGlobalMatrix(finterpolate*nt);
//		nParticle& cur=Particle.GetFree();
//		EmitOne(cur,delta*nt);
		EmitOne(Particle.GetIndexFree(),delta*nt);
	}
	EndInterpolateGlobalMatrix();
}

bool cEmitterSpl::GetRndPos(Vect3f& pos, Vect3f* norm)
{
	if (Particle.is_empty())
	{
		cur_one_pos = -1;
		pos.set(0,0,0);
		if (norm)
			norm->set(0,0,0);
		return false;
	}
	else
	{
		int size;
		int i = rnd(size = Particle.size());
		while(Particle[i].key==-1) 
			i = rnd(size);
		cur_one_pos = i;
		nParticle& p = Particle[i];
		HeritKey& k = hkeys[p.hkey];
		k.Get(pos,p.htime*k.inv_dtime);
		p.pos.xformPoint(pos);
		if (norm)
			norm->set(0,0,0);
		return true;
	}
}
Vect3f cEmitterSpl::GetVdir(int i)
{
	if (i==-1)
		return Vect3f::ZERO;
	xassert((UINT)i<Particle.size());
	nParticle& p = Particle[i];
	HeritKey& k = hkeys[p.hkey];
	Vect3f pos;
	k.Get(pos,p.htime*k.inv_dtime);
	pos = p.pos.rot().xform(pos); 
	FastNormalize(pos);
	return pos;
}

void cEmitterSpl::EmitOne(int ix_cur/*nParticle& cur*/,float begin_time)
{
	xassert((UINT)ix_cur<Particle.size());
	nParticle& cur = Particle[ix_cur];
	float t=time*inv_emitter_life_time;
	float inv_life=inv_life_time.Get(t);
	float dlife=life_time_delta.Get(t);
	cur.key=0;
	cur.hkey=0;
	cur.inv_life_time=inv_particle_life_time*inv_life*(1+dlife*(rnd.frand()-0.5f));
	cur.time_summary=cur.htime=cur.time=cur.inv_life_time*begin_time;
	

	float dsz=begin_size_delta.Get(t);
	cur.begin_size=begin_size.Get(t)*(1+dsz*(rnd.frand()-0.5f));
	cur.angle0=0;
	switch(rotation_direction)
	{
	case ETRD_CW:
		cur.angle_dir=1;
		break;
	case ETRD_CCW:
		cur.angle_dir=-1;
		break;
	default:
		cur.angle_dir=(rnd()&1)?-1:1;
		break;
	}

	Vect3f pos;
	bool need_transform;
	switch(direction)
	{
	case ETDS_ID:
		need_transform = OnePos(ix_cur,pos);
		cur.pos.rot()=Mat3f::ID;
		break;
	case ETDS_ROTATEZ:
		need_transform = OnePos(ix_cur,pos);
		cur.pos.rot().set(Vect3f(0,0,1), rnd.frand()*2*M_PI,1);
		break;
	case ETDS_BURST1:
		{
			Vect3f norm;
			need_transform = OnePos(ix_cur,pos, &norm);
			norm.Normalize();
			Vect3f z(0,0,1);
			Vect3f p_norm=norm%z;
			if(p_norm.norm2()<1e-5f)
				p_norm.set(1,0,0);
			p_norm.Normalize();
			Vect3f pp_norm=norm%p_norm;
			cur.pos.rot()= Mat3f(
						p_norm.x, pp_norm.x, norm.x,
						p_norm.y, pp_norm.y, norm.y,
						p_norm.z, pp_norm.z, norm.z	
								);
		}
		break;
	case ETDS_BURST2:
		{
			Vect3f norm;
			need_transform = OnePos(ix_cur,pos, &norm);
			cur.pos.rot().set(Vect3f(0,0,1), norm.psi(),1);
		}
		break;
	}
	cur.pos.trans()=pos;
	if (need_transform) 
		cur.pos= relative ? cur.pos : GetGlobalMatrix()*cur.pos;
	if (chPlume)
	{	
		cur.plume_pos.resize(TraceCount);
		vector<Vect3f>::iterator it;
		Vect3f bpos = /*GetGlobalMatrix()*/cur.pos.trans();
		FOR_EACH(cur.plume_pos,it)
			*it = bpos;
	}
}

void cEmitterSpl::ProcessTime(nParticle& p,float delta_time,int i)
{

	p.time_summary+=delta_time;
	{
		float& t=p.time;
		KeyParticleSpl* k0=&keys[p.key];
		KeyParticleSpl* k1=&keys[p.key+1];

		float dt=delta_time;
		while(t+dt>k0->dtime)
		{
			//t==1
			float tprev=t;
			t=k0->dtime;
			float ts=t*k0->inv_dtime;
			p.angle0 = p.angle0+ p.angle_dir*(k0->angle_vel*t+t*ts*0.5f*(k1->angle_vel-k0->angle_vel));

			dt-=k0->dtime-tprev;
			t=0;
			p.key++;

			if(p.key>=keys.size()-1)
			{
				Particle.SetFree(i);
				return;
			}
			
			k0=k1;
			k1=&keys[p.key+1];
		}

		t+=dt;
	}

	{
		float& t=p.htime;
		HeritKey* k0=&hkeys[p.hkey];

		float dt=delta_time;
		while(t+dt>k0->dtime)
		{
			//t==1
			dt-=k0->dtime-t;
			t=0;
			p.hkey++;

			if(p.hkey>=hkeys.size())
			{
				Particle.SetFree(i);
				break;
			}
			
			k0=&hkeys[p.hkey];
		}
		t+=dt;
	}
}

void cEmitterSpl::DummyQuant()
{
	float dtime_global=(time-old_time);
	int size=Particle.size();
	for(int i=0;i<size;i++)
	{
		nParticle& p=Particle[i];
		if(p.key<0)continue;
		Vect3f pos;

		float dtime=dtime_global*p.inv_life_time;

		{
			HeritKey& k=hkeys[p.hkey];
			float ts=p.htime*k.inv_dtime;
			k.Get(pos,ts);
			p.pos.xformPoint(pos);
			Bound.AddBound(pos);
		}

		ProcessTime(p,dtime,i);
	}
}

////////////////////////cEffect//////////////////////////
cEffect::cEffect()
:cIUnkObjScale(NULL)
{
	link.SetParent(this);
	link3dx.SetParent(this);
	time=0;
	auto_delete_after_life=false;
	particle_rate=1;
	func_getz=NULL;
}

cEffect::~cEffect()
{
	Clear();
	RELEASE(func_getz);
}

void cEffect::SetFunctorGetZ(FunctorGetZ* func)
{
	RELEASE(func_getz);
	func_getz=func;
	func_getz->IncRef();
	func_getz->UpdateInterface(this);

	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
		(*it)->SetFunctorGetZ(func_getz);

}

void cEffect::SetAutoDeleteAfterLife(bool auto_delete_after_life_)
{
	auto_delete_after_life=auto_delete_after_life_;
}

bool cEffect::IsLive()
{
	bool live=false;
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	{
		live=live || (*it)->IsLive();
	}

	return live;
}

void cEffect::Add(cEmitterInterface* p)
{
	emitters.push_back(p);
	p->SetPause(p->GetStartTime()>0);
}

void cEffect::Clear()
{
	time=0;
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
		(*it)->Release();
	emitters.clear();
}

void cEffect::PreDraw(cCamera *pCamera)
{
	bool visible=false;
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	if((*it)->IsVisible(pCamera))
	{
		visible=true;
		break;
	}

	if(visible)
		pCamera->Attach(SCENENODE_OBJECTSORT,this);
}

void cEffect::Draw(cCamera *pCamera)
{
#ifdef  NEED_TREANGLE_COUNT
	count_triangle = 0;
	square_triangle = 0;
#endif	
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
		(*it)->Draw(pCamera);
}

void cEffect::SetCycled(bool cycled)
{
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
		(*it)->SetCycled(cycled);
}

bool cEffect::IsCycled()const
{
	vector<cEmitterInterface*>::const_iterator it;
	FOR_EACH(emitters,it)
	if((*it)->IsCycled())
		return true;
	return false;
}

void cEffect::Animate(float dt)
{
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	{
		cEmitterInterface* p=*it;
		bool b=time<p->GetStartTime();
		p->SetPause(b);
		p->Animate(dt);
	}

	time+=dt*1e-3f;
	if(auto_delete_after_life)
	{
		if(!IsLive())
			Release();
	}
}

void cEffect::Init(EffectKey& effect_key_,cEmitter3dObject* models,float scale)
{
	EffectKey* el=&effect_key_;
#ifdef _DEBUG
	debug_effect_key=el;
	if(stricmp(el->name.c_str(),"effect")==0)
	{
		int k=0;
	}
#endif _DEBUG

	EffectKey scaled_effect;
	if(fabsf(scale-1.0f)>FLT_EPS)
	{
		scaled_effect=effect_key_;
		scaled_effect.RelativeScale(scale);
		el=&scaled_effect;
	}

	Clear();
	
	vector<EmitterKeyInterface*>::iterator it;
	if(models)
	{
		bool need_pos=false;
		bool need_normal=false;
		FOR_EACH(el->key,it)
		{
			EmitterKeyInterface* p=*it;
			EmitterKeyBase* pi=dynamic_cast<EmitterKeyBase*>(p);
			if(pi)
			{
				need_pos |= pi->particle_position.type==EMP_3DMODEL;
			}
//			if (!need_normal&&need_pos)
			if(p->GetType()==EMC_INTEGRAL)
			{
				EmitterKeyInt* pi=(EmitterKeyInt*)p;
				if(pi->particle_position.type==EMP_3DMODEL)  
				{
					need_normal |= pi->use_light;
					for(int i=pi->begin_speed.size()-1;i>=0; --i)
						need_normal |= pi->begin_speed[i].velocity == EMV_NORMAL_3D_MODEL;
					
				}
			}else if (p->GetType()==EMC_SPLINE)
			{
				EmitterKeySpl* pi=(EmitterKeySpl*)p;
				switch(((EmitterKeySpl*)p)->direction)
				{
				case ETDS_BURST1: case ETDS_BURST2:
					need_normal = true;
					break;
				}
			}
		}

		if(need_pos)
			models->GetAllPoints(begin_position);
		if(need_normal)
		{
			models->GetAllNormals(normal_position);
			VISASSERT(begin_position.size()==normal_position.size());
		}
	}

	FOR_EACH(el->key,it)
	{
		EmitterKeyInterface* p=*it;

		if(p->GetType()==EMC_INTEGRAL)
		{
			cEmitterInt* pEmitter=new cEmitterInt;

			EmitterKeyInt* pi=(EmitterKeyInt*)p;
			pEmitter->SetEmitterKey(*pi,models);
			pEmitter->SetParent(this);
			Add(pEmitter);
		}else
		if(p->GetType()==EMC_INTEGRAL_Z)
		{
			cEmitterZ* pEmitter=new cEmitterZ;

			EmitterKeyZ* pi=(EmitterKeyZ*)p;
			pEmitter->SetParent(this);
			if(func_getz)pEmitter->SetFunctorGetZ(func_getz);
			pEmitter->SetEmitterKey(*pi,models);
			Add(pEmitter);
		}else
		if(p->GetType()==EMC_SPLINE)
		{
			cEmitterSpl* pEmitter=new cEmitterSpl;

			EmitterKeySpl* pi=(EmitterKeySpl*)p;
			pEmitter->SetEmitterKey(*pi,models);
			pEmitter->SetParent(this);
			Add(pEmitter);
		}else
		if(p->GetType()==EMC_LIGHT)
		{
			cEmitterLight* pEmitter=new cEmitterLight;

			EmitterKeyLight* pi=(EmitterKeyLight*)p;
			pEmitter->SetEmitterKey(*pi);
			pEmitter->SetParent(this);
			Add(pEmitter);
		}
	}
	for(int i=0;i<emitters.size();++i)
		if (el->key[i]->GetType()!=EMC_LIGHT && ((EmitterKeyBase*)el->key[i])->other!="")
		{
			int ix=0;
			FOR_EACH(el->key,it)
			{
				if (i!=ix && ((EmitterKeyBase*)el->key[i])->other == (*it)->name)
				{	
					xassert(dynamic_cast<cEmitterBase*>(emitters[ix]));
					((cEmitterBase*)emitters[i])->other = (cEmitterBase*)emitters[ix];	
					break;
				}
				++ix;
			}
		}
}

float cEffect::GetSummaryTime()
{
	float time=0;
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	{
		float t=(*it)->GetStartTime()+(*it)->GetLiveTime();
		time=max(time,t);
	}
	return time;
}

void cEffect::ShowEmitter(EmitterKeyBase* emitter_id,bool show)
{
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	{
		cEmitterInterface* p=*it;
		if(p->GetUnicalID()==emitter_id)
			p->SetShowObjEditor(show);
	}
}

void cEffect::ShowAllEmitter()
{
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	{
		(*it)->SetShowObjEditor(true);
	}
}

void cEffect::HideAllEmitter()
{
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	{
		(*it)->SetShowObjEditor(false);
	}
}

void cEffect::SetPosition(const MatXf& Matrix)
{
	MatXf m=Matrix;
	float nx=m.rot().xcol().norm();
	float ny=m.rot().ycol().norm();
	float nz=m.rot().zcol().norm();
	if(nx<1e-3f || ny<1e-3f || nz<1e-3f)
	{
		m.rot()=Mat3f::ID;
	}else
	{
		m.rot().setXcol(m.rot().xcol()/nx);
		m.rot().setYcol(m.rot().ycol()/ny);
		m.rot().setZcol(m.rot().zcol()/nz);
	}

	cIUnkObjScale::SetPosition(m);
}

void cEffect::SetTime(float t)
{
	time = t;
}
void cEffect::MoveToTime(float t)
{
	const float dt=0.05f;
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
		(*it)->SetDummyTime(dt);
	while(time<t)
	{
		Animate(dt);
		FOR_EACH(emitters,it)
		{
			(*it)->Animate(dt);
		}
	}
	FOR_EACH(emitters,it)
		(*it)->SetDummyTime(0.2f);
}

void cEffect::LinkToNode(class cObjectNode* node)
{
	link.Link(node);
	if(node)
		SetPosition(node->GetGlobalPosition());
}

void cEffect::LinkToNode(class cObject3dx* object,int inode)
{
	link3dx.Link(object,inode);
	if(object)
		link3dx.Update();
}

const MatXf& cEffect::GetCenter3DModel()
{
	if(link3dx.IsInitialized())
		return link3dx.GetRootMatrix();

	if(link.GetNode())
		return link.GetNode()->GetRootNode()->GetLocalMatrix();
	return GetGlobalMatrix();
}

void cEffect::EffectObserverLink::Link(class cObjectNode* node_)
{
	if(observer)observer->BreakLink(this);
	node=node_;

	if(node)
		node->GetRootNode()->AddLink(this);

}

void cEffect::EffectObserverLink::Update()
{
	effect->SetPosition(node->GetGlobalPosition());
}

void cEffect::EffectObserverLink3dx::Link(class cObject3dx* object_,int inode)
{
	if(observer)observer->BreakLink(this);
	object=object_;
	node=inode;

	if(object)
		object->AddLink(this);
}

const MatXf& cEffect::EffectObserverLink3dx::GetRootMatrix()
{
	return object->GetPosition();
}

void cEffect::EffectObserverLink3dx::Update()
{
	const MatXf& mat=object->GetNodePosition(node);
	effect->SetPosition(mat);
}

void cEffect::StopAndReleaseAfterEnd()
{
	SetAutoDeleteAfterLife(true);
	vector<cEmitterInterface*>::iterator it;
	FOR_EACH(emitters,it)
	{
		cEmitterInterface* p=*it;
		p->SetCycled(false);
		p->DisableEmitProlonged();
	}
}

inline void cEffect::AddCountTriangle(int count)
{
#ifdef  NEED_TREANGLE_COUNT
	count_triangle+=count;
#endif
}
inline void cEffect::AddSquareTriangle(float sq)
{
#ifdef  NEED_TREANGLE_COUNT
	square_triangle+=sq;
#endif
}
int cEffect::GetTriangleCount()
{
#ifdef  NEED_TREANGLE_COUNT
	return count_triangle;
#else
	return 0;
#endif
}
double cEffect::GetSquareTriangle()
{
#ifdef  NEED_TREANGLE_COUNT
	return square_triangle;
#else 
	return 0;
#endif
}

/////////////////////misc///////////////////////
void CVectVect3f::Save(CSaver& s,int id)
{
	s.push(id);
	s<<(int)size();
	iterator it;
	FOR_EACH(*this,it)
		s<<*it;
	s.pop();
}
void CVectVect3f::Load(CLoadIterator rd)
{
	int sz;
	rd>>sz;
	resize(sz);
	iterator it;
	FOR_EACH(*this,it)
		rd>>*it;
}

void CKey::Save(CSaver& s,int id)
{
	s.push(id);
	s<<(int)size();
	CKey::iterator it;
	FOR_EACH(*this,it)
	{
		s<<it->time;
		s<<it->f;
	}
	s.pop();
}

void CKey::Load(CLoadIterator rd)
{
	int sz;
	rd>>sz;
	resize(sz);
	for(int i=0;i<sz;i++)
	{
		KeyFloat& p=(*this)[i];
		rd>>p.time;
		rd>>p.f;
	}
}

void EmitterType::Save(CSaver& s,int id)
{
	s.push(id);
	s<<(int)type;
	s<<size;
	s<<alpha_min;
	s<<alpha_max;
	s<<teta_min;
	s<<teta_max;

	s<<fix_pos;
	s<<num.x;
	s<<num.y;
	s<<num.z;
	s.pop();
}

void EmitterType::Load(CLoadIterator rd)
{
	int itmp;
	rd>>itmp;type=(EMITTER_TYPE_POSITION)itmp;
	rd>>size;
	rd>>alpha_min;
	rd>>alpha_max;
	rd>>teta_min;
	rd>>teta_max;

	rd>>fix_pos;
	rd>>num.x;
	rd>>num.y;
	rd>>num.z;
}


void EffectBeginSpeed::Save(CSaver& s)
{
	s.push(IDS_BUILDKEY_BEGIN_SPEED);
	s<<name;
	s<<(DWORD)velocity;
	s<<mul;
	s<<rotation.s();
	s<<rotation.x();
	s<<rotation.y();
	s<<rotation.z();
	s<<time;
	s<<esp.pos;
	s<<esp.dpos;
	s.pop();
}

void EffectBeginSpeed::Load(CLoadIterator rd)
{
	DWORD itemp;
	rd>>name;
	rd>>itemp;velocity=(EMITTER_TYPE_VELOCITY)itemp;
	rd>>mul;
	rd>>rotation.s();
	rd>>rotation.x();
	rd>>rotation.y();
	rd>>rotation.z();
	rd>>time;
	rd>>esp.pos;
	rd>>esp.dpos;
}

void CKeyRotate::Save(CSaver& s,int id)
{
	s.push(id);
	s<<(int)size();
	iterator it;
	FOR_EACH(*this,it)
	{
		s<<it->time;
		s<<it->pos.s();
		s<<it->pos.x();
		s<<it->pos.y();
		s<<it->pos.z();
	}
	s.pop();
}

void CKeyRotate::Load(CLoadIterator rd)
{
	int sz;
	rd>>sz;
	resize(sz);
	for(int i=0;i<sz;i++)
	{
		KeyRotate& p=(*this)[i];
		rd>>p.time;
		rd>>p.pos.s();
		rd>>p.pos.x();
		rd>>p.pos.y();
		rd>>p.pos.z();
	}
}

void CKeyPos::SaveInternal(CSaver& s)
{
	s<<(int)size();
	iterator it;
	FOR_EACH(*this,it)
	{
		s<<it->time;
		s<<it->pos.x;
		s<<it->pos.y;
		s<<it->pos.z;
	}
}

void CKeyPos::Save(CSaver& s,int id)
{
	s.push(id);
	SaveInternal(s);
	s.pop();
}

void CKeyPos::Load(CLoadIterator rd)
{
	int sz;
	rd>>sz;
	resize(sz);
	for(int i=0;i<sz;i++)
	{
		KeyPos& p=(*this)[i];
		rd>>p.time;
		rd>>p.pos.x;
		rd>>p.pos.y;
		rd>>p.pos.z;
	}
}

void CKeyPosHermit::Save(CSaver& s,int id)
{
	s.push(id);
	CKeyPos::SaveInternal(s);
	s<<(BYTE)cbegin;
	s<<(BYTE)cend;
	s.pop();
}

void CKeyPosHermit::Load(CLoadIterator rd)
{
	CKeyPos::Load(rd);
	BYTE b=T_FREE;
	rd>>b;
	cbegin=(CLOSING)b;
	rd>>b;
	cend=(CLOSING)b;
}

///////////////////////CKeyColor///////////////////////////////

void CKeyColor::Save(CSaver& s,int id)
{
	s.push(id);
	s<<(int)size();
	CKeyColor::iterator it;
	FOR_EACH(*this,it)
	{
		s<<it->time;
		s<<it->r;
		s<<it->g;
		s<<it->b;
		s<<it->a;
	}
	s.pop();
}

void CKeyColor::Load(CLoadIterator rd)
{
	int sz;
	rd>>sz;
	resize(sz);
	for(int i=0;i<sz;i++)
	{
		KeyColor& p=(*this)[i];
		rd>>p.time;
		rd>>p.r;
		rd>>p.g;
		rd>>p.b;
		rd>>p.a;
	}
}

/////////////////////////////EmitterKeyInterface/////////////////////////
EmitterKeyInterface::EmitterKeyInterface()
{
	texture_name="";
	emitter_create_time=0;
	emitter_life_time=1;
	cycled=false;
}

float EmitterKeyInterface::LocalTime(float t)
{
	return (emitter_life_time>KeyGeneral::time_delta)?(t-emitter_create_time)/emitter_life_time:0;
}

float EmitterKeyInterface::GlobalTime(float t)
{
	return t*emitter_life_time+emitter_create_time;
}

/////////////////////////////EmitterKeyLight/////////////////////////
EmitterKeyLight::EmitterKeyLight()
{
	KeyPos pos;
	pos.pos.set(0,0,0);
	pos.time=0;
	emitter_position.push_back(pos);

	KeyFloat size;
	size.f=0;
	size.time=0;
	emitter_size.push_back(size);
	size.f=100;
	size.time=0.2f;
	emitter_size.push_back(size);
	size.f=100;
	size.time=0.5f;
	emitter_size.push_back(size);
	size.f=30;
	size.time=1.0f;
	emitter_size.push_back(size);

	KeyColor color;
	color.set(0.5f,0.5f,0.5f,1);
	color.time=0;
	emitter_color.push_back(color);
	color.set(0.5f,0.5f,0.5f,1);
	color.time=0.2f;
	emitter_color.push_back(color);
	color.set(243.0f/510,191.0f/510,51.0f/510,1);
	color.time=0.5f;
	emitter_color.push_back(color);
	color.set(0,0,0,1);
	color.time=1.0f;
	emitter_color.push_back(color);
}


void EmitterKeyLight::Save(CSaver& s)
{
	s.push(IDS_BUILDKEY_LIGHT);
		s.push(IDS_BUILDKEY_LIGHT_HEAD);
		s<<emitter_create_time;
		s<<emitter_life_time;
		s<<cycled;
		s<<texture_name;
		s<<name;
		s.pop();

		emitter_position.Save(s,IDS_BUILDKEY_POSITION);
		emitter_size.Save(s,IDS_BUILDKEY_SIZE);
		emitter_color.Save(s,IDS_BUILDKEY_COLOR);
	s.pop();
	#ifdef EFFECTTOOL
		sdat.Save(s, IDS_BUILDKEY_TEMPLATE_DATA);
	#endif
}

void EmitterKeyLight::Load(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case IDS_BUILDKEY_LIGHT_HEAD:
		{
			CLoadIterator rd(ld);
			rd>>emitter_create_time;
			rd>>emitter_life_time;
			rd>>cycled;
			rd>>texture_name;
			rd>>name;
		}
		break;
	case IDS_BUILDKEY_TEMPLATE_DATA:
		#ifdef EFFECTTOOL
			sdat.Load(ld);
		#endif
		break;
	case IDS_BUILDKEY_POSITION:
		emitter_position.Load(ld);
		break;
	case IDS_BUILDKEY_SIZE:
		emitter_size.Load(ld);
		break;
	case IDS_BUILDKEY_COLOR:
		emitter_color.Load(ld);
		break;
	}

	BuildKey();
}

void EmitterKeyLight::BuildKey()
{
}

/////////////////////////////EmitterKeyBase/////////////////////////
EmitterKeyBase::EmitterKeyBase()
{
	sprite_blend=EMITTER_BLEND_MULTIPLY;
	generate_prolonged=false;
	particle_life_time=1;

	rotation_direction=ETRD_CW;

	KeyPos pos0;
	pos0.pos.set(0,0,0);
	pos0.time=0;
	emitter_position.push_back(pos0);
	KeyRotate rot0;
	rot0.pos=QuatF::ID;
	rot0.time=0;
	emitter_rotation.push_back(rot0);

	///Общие параметры частиц
	life_time.push_back(KeyFloat(0,1));
	life_time_delta.push_back(KeyFloat(0,0));
	begin_size.push_back(KeyFloat(0,1));
	begin_size_delta.push_back(KeyFloat(0,0));
	num_particle.push_back(KeyFloat(0,1000));

	relative = false;
	chFill = false;
	chPlume = false;
	TraceCount = 1;
	PlumeInterval = 0.1f;
	smooth = false;
//	PlumeTimeScaling = 1.0f;
//	PlumeSizeScaling = 1.0f;

	//Параметры отдельной частицы
	p_size.push_back(KeyFloat(0,0.5f));
	KeyColor c;
	c.r=c.g=c.b=c.a=1;
	c.time=0;
	p_color.push_back(c);
	p_alpha.push_back(c);
	c.r=c.g=c.b=c.a=1;
	c.time=1;
	p_color.push_back(c);
	p_alpha.push_back(c);

	p_angle_velocity.push_back(KeyFloat(0,0));
//	ix_other = -1;
}

EmitterKeyBase::~EmitterKeyBase()
{
}


void EmitterKeyBase::add_sort(vector<float>& xsort,CKey& c)
{
	CKey::iterator it;
	FOR_EACH(c,it)
		xsort.push_back(it->time);
}

void EmitterKeyBase::add_sort(vector<float>& xsort,CKeyColor& c)
{
	CKeyColor::iterator it;
	FOR_EACH(c,it)
		xsort.push_back(it->time);
}

void EmitterKeyBase::end_sort(vector<float>& xsort)
{
	sort(xsort.begin(),xsort.end());
	for(int i=1;i<xsort.size();i++)
	{
		if(xsort[i]-xsort[i-1]<1e-3)
		{
			xsort.erase(xsort.begin()+i);
			i--;
		}
	}
}

void EmitterKeyBase::GetParticleLifeTime(float t,float& mid_t,float& min_t,float& max_t)
{
	CKey inv_life_time;
	inv_life_time.resize(life_time.size());
	for(int i=0;i<life_time.size();i++)
	{
		inv_life_time[i].time=life_time[i].time;
		float l=max(life_time[i].f,1e-2f);
		inv_life_time[i].f=1/l;
	}
	//см. EmitOne
	float inv_particle_life_time=1/max(particle_life_time,1e-3f);
	float inv_life=inv_life_time.Get(t);
	float dlife=life_time_delta.Get(t);

	float inv_mid=max(inv_particle_life_time*inv_life*(1+dlife*(0.5f-0.5f)),0.1f);
	float inv_min=max(inv_particle_life_time*inv_life*(1+dlife*(0.0f-0.5f)),0.1f);
	float inv_max=max(inv_particle_life_time*inv_life*(1+dlife*(1.0f-0.5f)),0.1f);

	mid_t=1/inv_mid;
	min_t=1/inv_min;
	max_t=1/inv_max;
}

KeyPos* EmitterKeyBase::GetOrCreatePosKey(float t,bool* create)
{
	return emitter_position.GetOrCreateKey(t,emitter_life_time,emitter_create_time,create);
}

KeyRotate* EmitterKeyBase::GetOrCreateRotateKey(float t,bool* create)
{
	return emitter_rotation.GetOrCreateKey(t,emitter_life_time,emitter_create_time,create);
}

void EmitterKeyBase::GetPosition(float t,MatXf& m)
{
	m.trans()=emitter_position.Get(t);
	m.rot()=emitter_rotation.Get(t);
}

void EmitterKeyBase::Load3DModelPos(cEmitter3dObject* model)
{
	VISASSERT(model); 
	begin_position.clear();
	normal_position.clear();
	if (particle_position.type==EMP_3DMODEL_INSIDE)
	{
		model->GetAllPoints(begin_position);
		model->GetAllNormals(normal_position);
		VISASSERT(begin_position.size()==normal_position.size());
	}
}

void EmitterKeyBase::SaveInternal(CSaver& s)
{
	s.push(IDS_BUILDKEY_HEADER);
	s<<name;
	s<<(char)sprite_blend;
	s<<generate_prolonged;
	s<<texture_name;
	s<<emitter_create_time;
	s<<emitter_life_time;
	s<<particle_life_time;
	s<<cycled;
	s<<(DWORD)rotation_direction;
	s<<chFill;
	s<<relative;
	s.pop();
	if (chPlume)
	{
		s.push(IDS_BUILDKEY_PLUME);
		s<<TraceCount;
		s<<PlumeInterval;
		s<<smooth;
		s.pop();
	}
	if (other!="")
	{
		s.push(IDS_BUILDKEY_OTHER_NAME);
		s<<other;
		s.pop();
	}

	emitter_position.Save(s,IDS_BUILDKEY_POSITION);
	emitter_rotation.Save(s,IDS_BUILDKEY_ROTATION);
	particle_position.Save(s,IDS_BUILDKEY_PARTICLE_POS);

	life_time.Save(s,IDS_BUILDKEY_LIFE_TIME);
	life_time_delta.Save(s,IDS_BUILDKEY_LIFE_TIME_DELTA);
	begin_size.Save(s,IDS_BUILDKEY_BEGIN_SIZE);
	begin_size_delta.Save(s,IDS_BUILDKEY_BEGIN_SIZE_DELTA);
	num_particle.Save(s,IDS_BUILDKEY_NUM_PARTICLE);
	if (particle_position.type == EMP_3DMODEL_INSIDE)
	{
		if(!begin_position.empty())
			begin_position.Save(s,IDS_BUILDKEY_BEGIN_POSITION);
		bool need_normals = s.GetData()!=EXPORT_TO_GAME;
		switch(GetType())
		{
		case EMC_INTEGRAL:
			{
				need_normals |=  ((EmitterKeyInt*)this)->use_light;
				vector<EffectBeginSpeed>::iterator it;
				FOR_EACH(((EmitterKeyInt*)this)->begin_speed, it)
					need_normals |= it->velocity == EMV_NORMAL_3D_MODEL;
			}
			break;
		case EMC_SPLINE:
			switch(((EmitterKeySpl*)this)->direction)
			{
			case ETDS_BURST1: case ETDS_BURST2:
				need_normals = true; 
				break;
			}
			break;
		}
		if (need_normals && !normal_position.empty())
			normal_position.Save(s,IDS_BUILDKEY_NORMAL_POSITION);
	}
	p_size.Save(s,IDS_BUILDKEY_SIZE);
	p_color.Save(s,IDS_BUILDKEY_COLOR);
	p_alpha.Save(s,IDS_BUILDKEY_ALPHA);
	p_angle_velocity.Save(s,IDS_BUILDKEY_ANGLE_VELOCITY);
	#ifdef EFFECTTOOL
		sdat.Save(s, IDS_BUILDKEY_TEMPLATE_DATA);
	#endif
}

void EmitterKeyBase::LoadInternal(CLoadData* ld)
{
	switch(ld->id)
	{
	case IDS_BUILDKEY_HEADER:
		{
			CLoadIterator rd(ld);
			rd>>name;
			char blend;
			rd>>blend;
			sprite_blend=(EMITTER_BLEND)blend;
			rd>>generate_prolonged;
			rd>>texture_name;
			rd>>emitter_create_time;
			rd>>emitter_life_time;
			rd>>particle_life_time;
			rd>>cycled;

			DWORD d=rotation_direction;
			rd>>d;
			rotation_direction=(EMITTER_TYPE_ROTATION_DIRECTION)d;
			rd>>chFill;
			rd>>relative;
		}
		break;
	case IDS_BUILDKEY_TEMPLATE_DATA:
		#ifdef EFFECTTOOL
			sdat.Load(ld);
		#endif
		break;
	case IDS_BUILDKEY_OTHER_NAME:
		{
			CLoadIterator rd(ld);
			rd>>other;
		}
		break;
	case IDS_BUILDKEY_PLUME:
		{
			CLoadIterator rd(ld);
			chPlume = true;
			rd>>TraceCount;
			rd>>PlumeInterval;
			rd>>smooth;
		}
		break;
	case IDS_BUILDKEY_POSITION:
		emitter_position.Load(ld);
		break;
	case IDS_BUILDKEY_ROTATION:
		emitter_rotation.Load(ld);
		break;
	case IDS_BUILDKEY_PARTICLE_POS:
		particle_position.Load(ld);
		break;
	case IDS_BUILDKEY_LIFE_TIME:
		life_time.Load(ld);
		break;
	case IDS_BUILDKEY_LIFE_TIME_DELTA:
		life_time_delta.Load(ld);
		break;
	case IDS_BUILDKEY_BEGIN_SIZE:
		begin_size.Load(ld);
		break;
	case IDS_BUILDKEY_BEGIN_SIZE_DELTA:
		begin_size_delta.Load(ld);
		break;
	case IDS_BUILDKEY_NUM_PARTICLE:
		num_particle.Load(ld);
		break;
	case IDS_BUILDKEY_BEGIN_POSITION:
		begin_position.Load(ld);
		break;
	case IDS_BUILDKEY_NORMAL_POSITION:
		normal_position.Load(ld);
		break;
	case IDS_BUILDKEY_SIZE:
		p_size.Load(ld);
		break;
	case IDS_BUILDKEY_COLOR:
		p_color.Load(ld);
		break;
	case IDS_BUILDKEY_ALPHA:
		p_alpha.Load(ld);
		break;
	case IDS_BUILDKEY_ANGLE_VELOCITY:
		p_angle_velocity.Load(ld);
		break;
	}
}

//////////////////////////////EmitterKey////////////////////////////
EmitterKeyInt::EmitterKeyInt()
{
	p_velocity.push_back(KeyFloat(0,1));
	p_gravity.push_back(KeyFloat(0,0));

	velocity_delta.push_back(KeyFloat(0,0));
	use_light=false;
}

EmitterKeyInt::~EmitterKeyInt()
{
}

void EmitterKeyInt::BuildKey()
{
	vector<float> xtime;
	xtime.push_back(0);
	xtime.push_back(1);
	add_sort(xtime,p_velocity);
	add_sort(xtime,p_size);
	add_sort(xtime,p_color);
	add_sort(xtime,p_alpha);
	add_sort(xtime,p_angle_velocity);
	add_sort(xtime,p_gravity);
	end_sort(xtime);

	key.clear();
	KeyParticleInt k;
	k.angle_vel=0;
	k.color.set(1,1,1,1);

	for(int i=0;i<xtime.size();i++)
	{
		if(i<xtime.size()-1)
		{
			k.dtime=xtime[i+1]-xtime[i];
		}else
		{
			k.dtime=0;
		}

		float t=xtime[i];
		k.vel=p_velocity.Get(t)*200;
		k.size=p_size.Get(t)*20;
		k.color=p_color.Get(t);
		k.color.a=p_alpha.Get(t).a;
		k.angle_vel=p_angle_velocity.Get(t)*5;
		k.gravity=p_gravity.Get(t)*100;

		key.push_back(k);
	}
}

void EmitterKeyInt::SaveInternal(CSaver& s)
{
	EmitterKeyBase::SaveInternal(s);

	velocity_delta.Save(s,IDS_BUILDKEY_VELOCITY_DELTA);

	p_velocity.Save(s,IDS_BUILDKEY_VELOCITY);
	p_gravity.Save(s,IDS_BUILDKEY_GRAVITY);

	for(int i=0;i<begin_speed.size();i++)
		begin_speed[i].Save(s);
/*	if (particle_position.type==EMP_3DMODEL_INSIDE && !normal_position.empty()&&
		(s.GetData()==EXPORT_TO_GAME ? use_light : true))
		normal_position.Save(s,IDS_BUILDKEY_NORMAL_POSITION);
*/
	s.push(IDS_BUILDKEY_INT_LIGHT);
	s<<use_light;
	s.pop();
}

void EmitterKeyInt::Save(CSaver& s)
{
	s.push(IDS_BUILDKEY_INT);
	SaveInternal(s);
	s.pop();
}

void EmitterKeyInt::Load(CLoadDirectory rd)
{
	begin_speed.clear();
	while(CLoadData* ld=rd.next())
	{
		LoadInternal(ld);
		switch(ld->id)
		{
		case IDS_BUILDKEY_VELOCITY_DELTA:
			velocity_delta.Load(ld);
			break;
		case IDS_BUILDKEY_VELOCITY:
			p_velocity.Load(ld);
			break;
		case IDS_BUILDKEY_GRAVITY:
			p_gravity.Load(ld);
			break;
		case IDS_BUILDKEY_BEGIN_SPEED:
			{
				EffectBeginSpeed ps;
				ps.Load(ld);
				begin_speed.push_back(ps);
			}
			break;
		case IDS_BUILDKEY_INT_LIGHT:
			CLoadIterator rd(ld);
			rd>>use_light;
			break;
		}
	}

	BuildKey();
}

///////////////////////EmitterKeySpl//////////////////
EmitterKeySpl::EmitterKeySpl()
{
	emitter_life_time=particle_life_time=2.0f;
	cycled=true;
	generate_prolonged=true;
	begin_size[0]=KeyFloat(0,0.2f);
	num_particle[0]=KeyFloat(0,1000);


//	particle_position.size.set(0.2f,0.2f,0.2f);
	direction=ETDS_ROTATEZ;//ETDS_ID;//
	p_position_auto_time=false;
	KeyPos pos0;
	pos0.time=0;
/*
	pos0.pos.set(0,0,0);
	p_position.push_back(pos0);

	pos0.pos.set(0,0,50);
	p_position.push_back(pos0);

	pos0.pos.set(0,50,100);
	p_position.push_back(pos0);

	pos0.pos.set(0,100,50);
	p_position.push_back(pos0);

	pos0.pos.set(0,0,0);
	p_position.push_back(pos0);
/*/
	pos0.pos.set(0,0,0);
	p_position.push_back(pos0);

	pos0.pos.set(0,0,100);
	pos0.time = 1.0;
	p_position.push_back(pos0);

//	pos0.pos.set(0,0,30);
//	p_position.push_back(pos0);
//	pos0.pos.set(0,60,60);
//	p_position.push_back(pos0);
/*
	const mx=12;
	for(float t=0;t<mx;t+=0.5f)
	{
		float r=25*exp(-t*0.1f);
		pos0.time=1+(t-mx+1)*0.05;
		pos0.pos.set(t*3,25-r*cos(t),50+r*sin(t));
		p_position.push_back(pos0);
	}
*/

/**/
}

EmitterKeySpl::~EmitterKeySpl()
{
}

void EmitterKeySpl::Save(CSaver& s)
{
	s.push(IDS_BUILDKEY_SPL);
		EmitterKeyBase::SaveInternal(s);

		s.push(IDS_BUILDKEY_SPL_HEADER);
		s<<p_position_auto_time;
		s.pop();

		s.push(IDS_BUILDKEY_SPL_DIRECTION);
		s<<(DWORD)direction;
		s.pop();

		p_position.Save(s,IDS_BUILDKEY_SPL_POSITION);
	s.pop();

}

void EmitterKeySpl::Load(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	{
		EmitterKeyBase::LoadInternal(ld);
		switch(ld->id)
		{
		case IDS_BUILDKEY_SPL_HEADER:
			{
				CLoadIterator rd(ld);
				rd>>p_position_auto_time;
			}
			break;
		case IDS_BUILDKEY_SPL_DIRECTION:
			{
				CLoadIterator rd(ld);
				DWORD d;
				rd>>d;
				direction=(EMITTER_TYPE_DIRECTION_SPL)d;
			}
			break;
		case IDS_BUILDKEY_SPL_POSITION:
			p_position.Load(ld);
			break;
		}
	}

	BuildKey();
}

void EmitterKeySpl::BuildKey()
{
	vector<float> xtime;
	xtime.push_back(0);
	xtime.push_back(1);
	add_sort(xtime,p_size);
	add_sort(xtime,p_color);
	add_sort(xtime,p_alpha);
	add_sort(xtime,p_angle_velocity);
	end_sort(xtime);

	key.clear();
	KeyParticleSpl k;
	k.angle_vel=0;
	k.color.set(1,1,1,1);

	for(int i=0;i<xtime.size();i++)
	{
		if(i<xtime.size()-1)
		{
			k.dtime=xtime[i+1]-xtime[i];
		}else
		{
			k.dtime=0;
		}

		float t=xtime[i];
		k.size=p_size.Get(t)*20;
		k.color=p_color.Get(t);
		k.color.a=p_alpha.Get(t).a;
		k.angle_vel=p_angle_velocity.Get(t)*5;

		key.push_back(k);
	}
}

/////////////////////EffectKey////////////////////////
EffectKey::EffectKey()
{
}

EffectKey::~EffectKey()
{
	Clear();
}

void EffectKey::Clear()
{
	//В effect maker этот список в деструкторе должен быть всегда пустым
	vector<EmitterKeyInterface*>::iterator it;
	FOR_EACH(key,it)
		delete *it;
	key.clear();
}


void EffectKey::operator= (const EffectKey& effect_key)
{
	name=effect_key.name;
	Clear();
	vector<EmitterKeyInterface*>::const_iterator it;

	FOR_EACH(effect_key.key,it)
	{
		EmitterKeyInterface* p=(*it)->Clone();
		key.push_back(p);
	}
}

void EffectKey::Save(CSaver& s)
{
	s.push(IDS_EFFECTKEY);
		s.push(IDS_EFFECTKEY_HEADER);
		s<<name;
		s.pop();

		vector<EmitterKeyInterface*>::iterator it;
		FOR_EACH(key,it)
		{
			EmitterKeyInterface* p=*it;
			p->Save(s);
		}
	s.pop();
}

void EffectKey::Load(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case IDS_EFFECTKEY_HEADER:
		{
			CLoadIterator rd(ld);
			rd>>name;
		}
		break;
	case IDS_BUILDKEY_INT:
		{
			EmitterKeyInt* p=new EmitterKeyInt;
			key.push_back(p);
			p->Load(ld);
		}
		break;
	case IDS_BUILDKEY_SPL:
		{
			EmitterKeySpl* p=new EmitterKeySpl;
			key.push_back(p);
			p->Load(ld);
		}
		break;
	case IDS_BUILDKEY_Z:
		{
			EmitterKeyZ* p=new EmitterKeyZ;
			key.push_back(p);
			p->Load(ld);
		}
		break;
	case IDS_BUILDKEY_LIGHT:
		{
			EmitterKeyLight* p=new EmitterKeyLight;
			key.push_back(p);
			p->Load(ld);
		}
		break;
	}
}

void EffectKey::RelativeScale(float scale)
{
	vector<EmitterKeyInterface*>::iterator it;
	FOR_EACH(key,it)
		(*it)->RelativeScale(scale);
}

////////////////////////////EffectLibrary//////////////////////////////
EffectLibrary::EffectLibrary()
{
}

EffectLibrary::~EffectLibrary()
{
	iterator it;
	FOR_EACH(lst,it)
	{
		delete *it;
	}
}

void EffectLibrary::ClearNoDelete()
{
	lst.clear();
}

EffectKey* EffectLibrary::Get(const char* name) const
{
	vector<EffectKey*>::const_iterator it;
	FOR_EACH(lst,it)
	{
		if(stricmp((*it)->name.c_str(),name)==0)
			return *it;
	}

	return NULL;
}

bool EffectLibrary::Load(const char* fname,const char* texture_path)
{
	CLoadDirectoryFile s;
	if(!s.Load(fname))
		return false;
	filename=fname;

	string path;
	if(texture_path && strlen(texture_path)>0)
	{
		char c=texture_path[strlen(texture_path)-1];
		path=texture_path;
		if(c!='\\' && c!='/')
		{
			path+='\\';
		}
	}

	while(CLoadData* ld=s.next())
	if(ld->id==IDS_EFFECTKEY)
	{
		EffectKey* ek=new EffectKey;
		lst.push_back(ek);
		ek->Load(ld);

		if(texture_path)
		{
			char path_buffer[_MAX_PATH];
			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];
			
			vector<EmitterKeyInterface*>::iterator it;
			FOR_EACH(ek->key,it)
			{
				string& t=(*it)->texture_name;
				_splitpath(t.c_str(),drive,dir,fname,ext);
				sprintf(path_buffer,"%s%s%s",path.c_str(),fname,ext);
				t=path_buffer;
			}
		}

	}

	return true;
}

///////////////////////////EmitterKeyBase///////////////////////////
void EmitterKeyBase::RelativeScale(float scale)
{
	{
		CKeyPos::iterator it;
		FOR_EACH(emitter_position,it)
		{
			Vect3f& pos=it->pos;
			pos*=scale;
		}
	}

	particle_position.size*=scale;
	CKey::iterator it;
	FOR_EACH(begin_size,it)
		it->f*=scale;
}

void EmitterKeyLight::RelativeScale(float scale)
{
	{
		CKeyPos::iterator it;
		FOR_EACH(emitter_position,it)
		{
			Vect3f& pos=it->pos;
			pos*=scale;
		}
	}

	CKey::iterator it;
	FOR_EACH(emitter_size,it)
		it->f*=scale;
}

void EmitterKeyInt::RelativeScale(float scale)
{
	EmitterKeyBase::RelativeScale(scale);

	CKey::iterator it;
	FOR_EACH(p_velocity,it)
		it->f*=scale;
	FOR_EACH(p_gravity,it)
		it->f*=scale;
}

void EmitterKeySpl::RelativeScale(float scale)
{
	EmitterKeyBase::RelativeScale(scale);

	CKeyPosHermit::iterator it;
	FOR_EACH(p_position,it)
	{
		Vect3f& pos=it->pos;
		pos*=scale;
	}
}


bool EffectKey::IsNeed3DModel()
{
	vector<EmitterKeyInterface*>::iterator it;
	FOR_EACH(key,it)
	{
		EmitterKeyBase* p=dynamic_cast<EmitterKeyBase*>(*it);
		if(p)
		if(p->particle_position.type==EMP_3DMODEL)
			return true;
	}

	return false;
}

bool EffectKey::IsCycled()
{
	vector<EmitterKeyInterface*>::iterator it;
	FOR_EACH(key,it)
	if((*it)->cycled)
		return true;
	return false;
}

///////////////////////////////cEmitterZ///////////////////////////
class FunctorMapZ:public FunctorGetZ
{
	class TerraInterface* terra;
	int terra_x,terra_y;
public:
	FunctorMapZ()
	{
		terra=NULL;
		terra_x=terra_y=0;
	}

	void UpdateInterface(cEffect* parent)
	{
		cTileMap* pTileMap=parent->GetScene()->GetTileMap();
		VISASSERT(pTileMap);
		if(pTileMap)
		{
			terra=pTileMap->GetTerra();
			terra_x=terra->SizeX();
			terra_y=terra->SizeY();
		}
	}

	float GetZ(float pos_x,float pos_y)
	{
		float out_z;
		int x=round(pos_x),y=round(pos_y);
		if(x>=0 && x<terra_x && y>=0 && y<terra_y && terra)
			out_z=terra->GetZ(x,y);
		else
			out_z=0;
		return out_z;
	}
};

class FuncrorFieldZ:public FunctorMapZ
{
public:
	float GetZ(float pos_x,float pos_y)
	{
		float out_z=FunctorMapZ::GetZ(pos_x,pos_y);
		if(field_dispatcher)
		{
			float z=field_dispatcher->getGraphZ(pos_x,pos_y);
			if(out_z<z)
				out_z=z;
		}
		return out_z;
	}
};

cEmitterZ::cEmitterZ()
{
	add_z=20;
	planar=true;
	angle_by_center=true;
	base_angle=M_PI/2;

	use_force_field=false;
	func_getz=NULL;
}

cEmitterZ::~cEmitterZ()
{
	RELEASE(func_getz);
}


void cEmitterZ::SetParent(cEffect* parent_)
{
	cEmitterInt::SetParent(parent_);
}

void cEmitterZ::ProcessTime(nParticle& p,float dt,int i,Vect3f& cur_pos)
{
	float& t=p.time;
	KeyParticleInt* k0=&keys[p.key];
	KeyParticleInt* k1=&keys[p.key+1];

	if(p.key_begin_time>=0)
	{
		EffectBeginSpeedMatrix* s=&begin_speed[p.key_begin_time];
		while(s->time_begin<=p.time_summary)
		{
			Vect3f v=CalcVelocity(*s,p,1);
			
			float& t=p.time;
			float ts=t*k0->inv_dtime;
			p.pos0.x -= v.x*(t*(k0->vel+ts*0.5f*(k1->vel-k0->vel)));
			p.pos0.y -= v.y*(t*(k0->vel+ts*0.5f*(k1->vel-k0->vel)));
			p.vdir += v;

			p.key_begin_time++;
			if(p.key_begin_time>=begin_speed.size())
			{
				p.key_begin_time=-1;
				break;
			}else
				s=&begin_speed[p.key_begin_time];
		}

	}

//	p.pos0.z=CalcZ(cur_pos.x,cur_pos.y);

	while(t+dt>k0->dtime)
	{
		//t==1
		float tprev=t;
		t=k0->dtime;
		float ts=t*k0->inv_dtime;
		p.pos0.x = p.pos0.x+p.vdir.x*(t*(k0->vel+ts*0.5f*(k1->vel-k0->vel)))+g.x*((p.gvel0+t*k0->gravity*0.5f)*t);
		p.pos0.y = p.pos0.y+p.vdir.y*(t*(k0->vel+ts*0.5f*(k1->vel-k0->vel)))+g.y*((p.gvel0+t*k0->gravity*0.5f)*t);
		p.angle0 = p.angle0+ p.angle_dir*(k0->angle_vel*t+t*ts*0.5f*(k1->angle_vel-k0->angle_vel));
		p.gvel0+=t*k0->gravity;

		dt-=k0->dtime-tprev;
		t=0;
		p.key++;

		if(p.key>=keys.size()-1)
		{
			Particle.SetFree(i);
			break;
		}
		
		k0=k1;
		k1=&keys[p.key+1];
	}

	t+=dt;
	p.time_summary+=dt;
}
/*
inline void cEmitterInt::nParticle::ZPutToBuf(const float& dtime_global, const KeyParticleInt& k0, 
								const KeyParticleInt& k1, sBox6f& Bound, 
								const cTextureAviScale::RECT& rt,
								const MatXf& mat, cQuadBuffer<sVertexXYZDT1>*& pBuf, 
								cEmitterZ* emi, Vect3f& pos, float& dtime, 
								const float& t, const float trace_size)
{
	float angle;
	sColor4f fcolor;
	float psize;

	/*float * /dtime=dtime_global*inv_life_time;

//	KeyParticleInt& k0=keys[p.key];
//	KeyParticleInt& k1=keys[p.key+1];

//	float& t=p.time;
	float ts=t*k0.inv_dtime;
	pos.x = pos0.x+vdir.x*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+emi->g.x*((gvel0+t*k0.gravity*0.5f)*t);
	pos.y = pos0.y+vdir.y*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+emi->g.y*((gvel0+t*k0.gravity*0.5f)*t);
	pos.z = emi->CalcZ(pos.x,pos.y);//pos0.z;
	Bound.AddBound(pos);
	angle = angle0+ angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
	fcolor = k0.color+(k1.color-k0.color)*ts;
	psize = k0.size+(k1.size-k0.size)*ts;

	//Добавить в массив
	Vect3f sx,sy;
	Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
	rot*=psize*begin_size*trace_size;
	if(emi->planar)
	{
		sx.x=+rot.x;
		sx.y=-rot.y;
		sx.z=0;
		sy.x=+rot.y;
		sy.y=+rot.x;
		sy.z=0;
	}else
	{
		mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
		mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);
	}

	sColor4c color(round(fcolor.r),round(fcolor.g),round(fcolor.b),round(fcolor.a));

	sVertexXYZDT1 *v=pBuf->Get();

	v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//set(0,0);
	v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//set(0,1);
	v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//set(1,0);
	v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//set(1,1);

	xassert(emi->parent->AddToCountTriangle(1));
}


inline void cEmitterInt::ZPutPlumeParticle(const float& dtime_global, sBox6f& Bound, 
						 cTextureAviScale*& texture, cTextureAviScale*& plume_texture, 
						 const MatXf& mat, nParticle& p,
						 cQuadBuffer<sVertexXYZDT1>*& pBuf,
						 Vect3f& pos, float& dtime )
{
	float tt = p.time;																		
	int key = p.key;																		
	float timesummary = p.time_summary;														
	float interval = PlumeInterval;															
	xassert(PlumeInterval>0.0f && PlumeInterval<1.0f);										
	float tr_size = 1.0f;																	
	for(int j=0;j<TraceCount;j++)															
	{																						
		tt-=interval;																		
		timesummary-=interval;																
		while(tt<0)																			
		{																					
//			if (key==0)
				return;																		
			tt += keys[--key].dtime;														
		}																					
		const cTextureAviScale::RECT* rt;													
		if(plume_texture)																	
			rt = &plume_texture->GetFramePos((float)j/TraceCount);							
		else if (texture)																	
			rt = &texture->GetFramePos(timesummary);										
		else rt = &cTextureAviScale::RECT::ID;												
		tr_size*=PlumeSizeScaling;															
		p.ZPutToBuf(dtime_global, keys[key], keys[key+1],									
					Bound, *rt, mat, pBuf, (cEmitterZ*)this, pos, dtime, tt, tr_size);					
		interval*=PlumeTimeScaling;															
	}																						
}
*/
void cEmitterZ::Draw(cCamera *pCamera)
{
	if(no_show_obj_editor)
		return;
	cURenderDevice* rd=pCamera->GetRenderDevice();
	if(false)
	{
		vector<Vect3f>::iterator it;
		vector<Vect3f>& begin_position=parent->GetPos();
		FOR_EACH(begin_position,it)
		{
			Vect3f p=*it;

			rd->DrawLine(p,p+Vect3f(2,2,2),sColor4c(255,255,0,255));
		}
		rd->FlushPrimitive3D();
	}

	
	float dtime_global=(time-old_time);

	MatXf mat=pCamera->GetMatrix();
	Bound.SetInvalidBox();

	cTextureAviScale* texture = NULL;
//	cTextureAviScale* plume_texture = NULL;
	if (GetTexture(0) && GetTexture(0)->IsAviScaleTexture())
		texture = (cTextureAviScale*)GetTexture(0);

	cQuadBuffer<sVertexXYZDT1>* pBuf=rd->GetQuadBufferXYZDT1();
/*
	Vect3f pos;
	float dtime;
	bool NeedPlume = chPlume;
	if (NeedPlume&&GetTexture(1))
	{
		if (GetTexture(1)->IsAviScaleTexture())
			plume_texture = (cTextureAviScale*)GetTexture(1);
		rd->SetNoMaterial(sprite_blend, 0, GetTexture(1));
		pBuf->BeginDraw();
		int size=Particle.size();
		for(int i=size-1;i>=0;i--)
		{
			nParticle& p=Particle[i];
			if(p.key<0)continue;
			ZPutPlumeParticle(dtime_global, Bound, texture, plume_texture, 
									mat, p,	pBuf, pos, dtime );
		}
		pBuf->EndDraw();
		NeedPlume = false;
	}
*/
	MatXf GM;
	MatXf iGM;
	Vect3f CameraPos;
	UCHAR mode;
	if (chPlume)
	{
		CameraPos = relative ? iGM*pCamera->GetPos() : pCamera->GetPos();
		mode = (UCHAR)planar + (smooth? 0:2);
	}
	rd->SetNoMaterial(sprite_blend, 0, GetTexture(0));
	if (relative)
	{
		GM = GetGlobalMatrix();
		iGM = GM;
		iGM.invert();
		pBuf->BeginDraw(GM);
	}
	else pBuf->BeginDraw();
	int size=Particle.size();
	for(int i=size-1;i>=0;i--)
	{
/*		nParticle& p=Particle[i];
		if(p.key<0)continue;
		if (NeedPlume)
			ZPutPlumeParticle(dtime_global, Bound, texture, plume_texture, 
									mat, p,	pBuf, pos, dtime );
		const cTextureAviScale::RECT& rt = texture?
					((cTextureAviScale*)texture)->GetFramePos(p.time_summary) :
					cTextureAviScale::RECT::ID;	
		p.ZPutToBuf(dtime_global, keys[p.key], keys[p.key+1],
					Bound, rt, mat, pBuf, this, pos, dtime, p.time, 1.0f);
		ProcessTime(p,dtime,i,pos);
*/
		nParticle& p=Particle[i];
		if(p.key<0)continue;
		Vect3f pos;
		float angle;
		sColor4f fcolor;
		float psize;

		float dtime=dtime_global*p.inv_life_time;
		
		KeyParticleInt& k0=keys[p.key];
		KeyParticleInt& k1=keys[p.key+1];
		
		float& t=p.time;
		float ts=t*k0.inv_dtime;
		pos.x = p.pos0.x+p.vdir.x*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g.x*((p.gvel0+t*k0.gravity*0.5f)*t);
		pos.y = p.pos0.y+p.vdir.y*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g.y*((p.gvel0+t*k0.gravity*0.5f)*t);
/*		if (relative)
		{
			Vect3f pp; //word coordinate
			pos.z=0;
			pp = GM*pos;
			pp.z = CalcZ(pp.x,pp.y); 
			pos = iGM*pp;
		}
		else*/ 
			pos.z = CalcZ(pos.x,pos.y); 
//		pp.x = (GM.rot().xrow().x*pos.x + GM.rot().xrow().y*pos.y)  +  GM.trans().x;
//		pp.y = (GM.rot().yrow().x*pos.x + GM.rot().yrow().y*pos.y)  +  GM.trans().y;
//		pp.z = CalcZ(pp.x,pp.y); 
//		pos.z = (iGM.rot().zrow().x*pp.x + iGM.rot().zrow().y*pp.y + iGM.rot().zrow().z*pp.z) + iGM.trans().z;
		Bound.AddBound(pos);
		angle = p.angle0+ p.angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
		fcolor = k0.color+(k1.color-k0.color)*ts;
		psize = k0.size+(k1.size-k0.size)*ts;

		//Добавить в массив
		Vect3f sx,sy;
		Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
		rot*=psize*=p.begin_size;
		if(planar)
		{
			sx.x=+rot.x;
			sx.y=-rot.y;
			sx.z=0;
			sy.x=+rot.y;
			sy.y=+rot.x;
			sy.z=0;
		}else
		{
			mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
			mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);
		}

		sColor4c color(round(fcolor.r),round(fcolor.g),round(fcolor.b),round(fcolor.a));
		const cTextureAviScale::RECT& rt = texture ?
						((cTextureAviScale*)texture)->GetFramePos(p.time_summary>=1? 0.99f : p.time_summary) :
						cTextureAviScale::RECT::ID;	
		if (chPlume)
		{
			if (PutToBuf(p,pos,dtime,pBuf,color,CameraPos,psize, rt, mode, &iGM))
				continue;
		}
		else 
		{
			sVertexXYZDT1 *v=pBuf->Get();

			v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//set(0,0);
			v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//set(0,1);
			v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//set(1,0);
			v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//set(1,1);
			#ifdef  NEED_TREANGLE_COUNT
				parent->AddCountTriangle(2);
				parent->AddSquareTriangle(psize*psize);
			#endif

		}
		ProcessTime(p,dtime,i,pos);
	}

	pBuf->EndDraw();

/*
	cQuadBuffer<sVertexXYZDT1>* pBuf=rd->GetQuadBufferXYZDT1();
	rd->SetNoMaterial(sprite_blend,0,texture);
	pBuf->BeginDraw();

	bool isAviTexture = texture&&texture->IsAviScaleTexture();
	int size=Particle.size();
	for(int i=size-1;i>=0;i--)
	{
		nParticle& p=Particle[i];
		if(p.key<0)continue;
		Vect3f pos;
		float angle;
		sColor4f fcolor;
		float psize;

		float dtime=dtime_global*p.inv_life_time;
		
		KeyParticleInt& k0=keys[p.key];
		KeyParticleInt& k1=keys[p.key+1];
		
		float& t=p.time;
		float ts=t*k0.inv_dtime;
		pos.x = p.pos0.x+p.vdir.x*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g.x*((p.gvel0+t*k0.gravity*0.5f)*t);
		pos.y = p.pos0.y+p.vdir.y*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g.y*((p.gvel0+t*k0.gravity*0.5f)*t);
		pos.z = p.pos0.z;
		Bound.AddBound(pos);
		angle = p.angle0+ p.angle_dir*(k0.angle_vel*t+t*ts*0.5f*(k1.angle_vel-k0.angle_vel));
		fcolor = k0.color+(k1.color-k0.color)*ts;
		psize = k0.size+(k1.size-k0.size)*ts;

		//Добавить в массив
		Vect3f sx,sy;
		Vect2f rot=rotate_angle[round(angle*rotate_angle_size)&rotate_angle_mask];
		rot*=psize*p.begin_size;
		if(planar)
		{
			sx.x=+rot.x;
			sx.y=-rot.y;
			sx.z=0;
			sy.x=+rot.y;
			sy.y=+rot.x;
			sy.z=0;
		}else
		{
			mat.invXformVect(Vect3f(+rot.x,-rot.y,0),sx);
			mat.invXformVect(Vect3f(+rot.y,+rot.x,0),sy);
		}

		sColor4c color(round(fcolor.r),round(fcolor.g),round(fcolor.b),round(fcolor.a));
		
		sVertexXYZDT1 *v=pBuf->Get();
		const cTextureAviScale::RECT& rt = isAviTexture ?
					((cTextureAviScale*)texture)->GetFramePos(p.time) :
					cTextureAviScale::RECT::ID;	

		v[0].pos=pos-sx-sy; v[0].diffuse=color; v[0].GetTexel().set(rt.left, rt.top);	//set(0,0);
		v[1].pos=pos-sx+sy; v[1].diffuse=color; v[1].GetTexel().set(rt.left, rt.bottom);//set(0,1);
		v[2].pos=pos+sx-sy; v[2].diffuse=color; v[2].GetTexel().set(rt.right,rt.top);	//set(1,0);
		v[3].pos=pos+sx+sy; v[3].diffuse=color; v[3].GetTexel().set(rt.right,rt.bottom);//set(1,1);

		ProcessTime(p,dtime,i,pos);
	}

	pBuf->EndDraw();
*/
	Particle.Compress();

	old_time=time;
}

bool cEmitterZ::GetRndPos(Vect3f& pos, Vect3f* norm)
{
	if (Particle.is_empty())
	{
		cur_one_pos = -1;
		pos.set(0,0,0);
		if (norm)
			norm->set(0,0,0);
		return false;
	}
	else
	{
		int size;
		int i = rnd(size = Particle.size());
		while(Particle[i].key==-1) 
			i = rnd(size);
		cur_one_pos = i;
		nParticle& p = Particle[i];
		KeyParticleInt& k0=keys[p.key];
		KeyParticleInt& k1=keys[p.key+1];
		
		float& t=p.time;
		float ts=t*k0.inv_dtime;
		pos.x = p.pos0.x+p.vdir.x*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g.x*((p.gvel0+t*k0.gravity*0.5f)*t);
		pos.y = p.pos0.y+p.vdir.y*(t*(k0.vel+ts*0.5f*(k1.vel-k0.vel)))+g.y*((p.gvel0+t*k0.gravity*0.5f)*t);
		pos.z = p.pos0.z;
		if (norm)
			norm->set(Particle[i].normal);
		return true;
	}
}

void cEmitterZ::EmitOne(int ix_cur/*nParticle& cur*/,float begin_time)
{
	xassert((UINT)ix_cur<Particle.size());
	nParticle& cur = Particle[ix_cur];
	cEmitterInt::EmitOne(ix_cur,begin_time);
	if(angle_by_center)
	{
		Vect3f& v=cur.pos0/*-GetGlobalMatrix().trans()*/;
		cur.angle0=(atan2(v.x,v.y)-base_angle)*INV_2_PI;
	}

//	cur.pos0.z=CalcZ(cur.pos0.x,cur.pos0.y);
	if (chPlume)
	{
		float z;
		if(relative)
		{
			MatXf GM  = GetGlobalMatrix(); 
			Vect3f p; //word coord
			p.x = (GM.rot().xrow().x*cur.pos0.x + GM.rot().xrow().y*cur.pos0.y)  +  GM.trans().x;
			p.y = (GM.rot().yrow().x*cur.pos0.x + GM.rot().yrow().y*cur.pos0.y)  +  GM.trans().y;
			p.z = func_getz->GetZ(p.x,p.y)+add_z; 
			p.sub(GM.trans());
			z = GM.rot().zrow().x*p.x + GM.rot().zrow().y*p.y + GM.rot().zrow().z*p.z;
/*
			Vect3f pp;
			MatXf iGM(GetGlobalMatrix());
			iGM.invert();
			const MatXf GM = GetGlobalMatrix();
			pp.x = (GM.rot().xrow().x*cur.pos0.x + GM.rot().xrow().y*cur.pos0.y)  +  GM.trans().x;
			pp.y = (GM.rot().yrow().x*cur.pos0.x + GM.rot().yrow().y*cur.pos0.y)  +  GM.trans().y;
			pp.z = CalcZ(pp.x,pp.y); 
			z = (iGM.rot().zrow().x*pp.x + iGM.rot().zrow().y*pp.y + iGM.rot().zrow().z*pp.z) + iGM.trans().z;
*/
		}else
			z = CalcZ(cur.pos0.x,cur.pos0.y);
		vector<Vect3f>::iterator it;
		FOR_EACH(cur.plume_pos,it)
			it->z = z;
	}
}

float cEmitterZ::CalcZ(float pos_x,float pos_y)
{
/*
	float out_z;
	int x=round(pos_x),y=round(pos_y);
	if(x>=0 && x<terra_x && y>=0 && y<terra_y)
		out_z=terra->GetZ(x,y);
	else
		out_z=0;

	if(field_dispatcher && use_force_field)
	{
		float z=field_dispatcher->getGraphZ(pos_x,pos_y);
		if(out_z<z)
			out_z=z;
	}

	return out_z+add_z;
*/
	if (relative)
	{
		MatXf GM  = GetGlobalMatrix(); 
		Vect3f p; //word coord
		p.x = (GM.rot().xrow().x*pos_x + GM.rot().xrow().y*pos_y)  +  GM.trans().x;
		p.y = (GM.rot().yrow().x*pos_x + GM.rot().yrow().y*pos_y)  +  GM.trans().y;
		p.z = func_getz->GetZ(p.x,p.y)+add_z; 
		p.sub(GM.trans());
		return GM.rot().zrow().x*p.x + GM.rot().zrow().y*p.y + GM.rot().zrow().z*p.z;
//		return GM.invXformPoint(Vect3f(pos_x, pos_y, gz)).z;
	}
	else 
		return func_getz->GetZ(pos_x,pos_y)+add_z;
}


EmitterKeyZ::EmitterKeyZ()
{
	add_z=20;
	planar=true;
	angle_by_center=true;
	base_angle=M_PI/2;
	use_force_field=false;
}

void EmitterKeyZ::Save(CSaver& s)
{
	s.push(IDS_BUILDKEY_Z);
	SaveInternal(s);
		s.push(IDS_BUILDKEY_ZEMITTER);
		s<<add_z;
		s<<planar;
		s<<angle_by_center;
		s<<base_angle;
		s<<use_force_field;
		s.pop();
	s.pop();
}

void EmitterKeyZ::LoadInternal(CLoadData* ld)
{
	EmitterKeyInt::LoadInternal(ld);
	switch(ld->id)
	{
	case IDS_BUILDKEY_ZEMITTER:
		{
			CLoadIterator rd(ld);
			rd>>add_z;
			rd>>planar;
			rd>>angle_by_center;
			rd>>base_angle;
			rd>>use_force_field;
		}
		break;
	}
}

void EmitterKeyZ::RelativeScale(float scale)
{
	EmitterKeyInt::RelativeScale(scale);
	add_z*=scale;
}

void cEmitterZ::SetEmitterKey(EmitterKeyZ& k,cEmitter3dObject* models)
{
	cEmitterInt::SetEmitterKey(k,models);
	planar=k.planar;
	add_z=k.add_z;
	angle_by_center=k.angle_by_center;
	base_angle=k.base_angle;
	use_force_field=k.use_force_field;
	smooth = k.smooth;

	for(int i=0;i<keys.size();i++)
	{
		KeyParticleInt& k=keys[i];

		k.color.r*=255.0f;
		k.color.g*=255.0f;
		k.color.b*=255.0f;

	}

	if(!func_getz)
	{
		if(use_force_field)
			func_getz=new FuncrorFieldZ;
		else
			func_getz=new FunctorMapZ;
		func_getz->UpdateInterface(parent);
	}
}

///////////////////////////////////cEmitterLight/////////////////////
cEmitterLight::cEmitterLight()
{
	light=NULL;
}

cEmitterLight::~cEmitterLight()
{
	RELEASE(light);
}

void cEmitterLight::SetEmitterKey(EmitterKeyLight& k)
{
	SetTexture(0,GetTexLibrary()->GetElement(k.texture_name.c_str()));
	unical_id=&k;
	cycled=k.cycled;

	start_time=k.emitter_create_time;
	emitter_life_time=k.emitter_life_time;

	emitter_position=k.emitter_position;
	emitter_size=k.emitter_size;
	emitter_color=k.emitter_color;
}

void cEmitterLight::Animate(float dt)
{
	if(b_pause)return;
	dt*=1e-3f;
	if(dt>0.1f)
		dt=0.1f;

	float t=time/emitter_life_time;

	if(t<1)
	{
		if(!light)
		{
			cTexture* pTexture=GetTexture(0);
			if(pTexture)pTexture->IncRef();
			light=parent->GetScene()->CreateLight(ATTRLIGHT_SPHERICAL,pTexture);
		}
		LocalMatrix.trans() = emitter_position.Get(t);
		LocalMatrix.rot()=Mat3f::ID;
		GlobalMatrix=parent->GetGlobalMatrix()*GetLocalMatrix();
		float size=emitter_size.Get(t);
		light->SetScale(Vect3f(size,size,size));
		sColor4f color=emitter_color.Get(t);
		light->SetColor(NULL,&color);
		light->SetPosition(GlobalMatrix);
	}else
	{
		if(cycled)
			time-=emitter_life_time;
		else
			RELEASE(light);
	}

	time+=dt;
}


//////////////////
EmitterKeyInterface* EmitterKeyInt::Clone()
{
	EmitterKeyInt* p=new EmitterKeyInt;
	*p=*this;
	return p;
}

EmitterKeyInterface* EmitterKeySpl::Clone()
{
	EmitterKeySpl* p=new EmitterKeySpl;
	*p=*this;
	return p;
}

EmitterKeyInterface* EmitterKeyZ::Clone()
{
	EmitterKeyZ* p=new EmitterKeyZ;
	*p=*this;
	return p;
}

EmitterKeyInterface* EmitterKeyLight::Clone()
{
	EmitterKeyLight* p=new EmitterKeyLight;
	*p=*this;
	return p;
}

