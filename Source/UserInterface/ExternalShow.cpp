#include "StdAfx.h"

#include "Region.h"
#include "Runtime.h"
#include "GameShell.h"
#include "Universe.h"
#include "Config.h"

#include "CameraManager.h"
#include "ExternalShow.h"
#include "../HT/ht.h"
#include "StdAfxRD.h"
#include "DrawBuffer.h"

#define ZFIX 2.0f

#define GAME_SHELL_SHOW_REGION_UP_DELTA		32.0f
#define GAME_SHELL_SHOW_REGION_DOWN_DELTA	-16.0f

#define GAME_SHELL_SHOW_REGION_V_STEP		0.1f
#define GAME_SHELL_SHOW_REGION_U_STEP		0.01f
#define GAME_SHELL_SHOW_REGION_U_SPEED		0.5f

#define GAME_SHELL_SHOW_REGION_TRIANGLESTRIP_LOCK_SIZE 16

float terExternalEnergyTextureStart = 0;
float terExternalEnergyTextureEnd = GAME_SHELL_SHOW_REGION_U_STEP;
//--------------------------------------------------
void terExternalQuant()
{
    if (gameShell->BuildingInstallerInited()) {
        gameShell->BuildingInstaller->ShowCircle();
    }
	

	terExternalEnergyTextureStart = xm::fmod(terExternalEnergyTextureStart + (float)scale_time.delta()*0.001f*GAME_SHELL_SHOW_REGION_U_SPEED + 1.0f,1.0f);
	terExternalEnergyTextureEnd = terExternalEnergyTextureStart + GAME_SHELL_SHOW_REGION_U_STEP;

	gbCircleShow->Quant();
}


cCircleShow* gbCircleShow=NULL;

void terCircleShowNormal()
{
	gbCircleShow->Show(0);
}

void terCircleShowDotted()
{
	gbCircleShow->Show(1);
}

void terCircleShowTriangle()
{
	gbCircleShow->Show(2);
}

void terCircleShowCross()
{
	gbCircleShow->Show(3);
}

cCircleShow::cCircleShow()
{
	in_lock=false;
	no_interpolation=false;
	MTINIT(lock);
	types.resize(4);
	types[0].external_show=terScene->CreateExternalObj(terCircleShowNormal,sRegionTextureCircle);
	types[1].external_show=terScene->CreateExternalObj(terCircleShowDotted,sRegionTextureCircleDotted);
	types[2].external_show=terScene->CreateExternalObj(terCircleShowTriangle,sRegionTextureTriangle);
	types[3].external_show=terScene->CreateExternalObj(terCircleShowCross,sRegionTextureCircle);
	u_begin=0;
}

cCircleShow::~cCircleShow()
{
	std::vector<sCircleType>::iterator it;
	FOR_EACH(types,it)
	{
		(*it).external_show->Release();
	}
	MTDONE(lock);
}

void cCircleShow::Lock()
{
	MTENTER(lock);
	in_lock=true;
}

void cCircleShow::Unlock()
{
	in_lock=false;
	MTLEAVE(lock);
}

void terCircleShowGraph(const Vect3f& pos,float r, const CircleColor& circleColor)
{
	gbCircleShow->Circle(pos,r,circleColor);
}

void terCircleShow(const Vect3f& pos0,const Vect3f& pos1,float r, const struct CircleColor& circleColor)
{
	gbCircleShow->Circle(pos0,pos1,r,r,circleColor);
}

void terCircleShow(const Vect3f& pos0,const Vect3f& pos1,float r0, float r1, const struct CircleColor& circleColor)
{
	gbCircleShow->Circle(pos0,pos1,r0,r1,circleColor);
}

void cCircleShow::Circle(const Vect3f& pos0,const Vect3f& pos1,float r0,float r1, const CircleColor& attr)
{
	if(no_interpolation)
	{
		Circle(pos0,r0,attr);
		return;
	}

	xassert(in_lock);
	sCircle c;
	c.pos[0]=pos0;
	c.pos[1]=pos1;
	c.r[0]=r0;
	c.r[1]=r1;
	c.attr=attr;
	types[attr.dotted].circles.push_back(c);
}

void cCircleShow::Circle(const Vect3f& pos,float r,const CircleColor& attr)
{
	MTG();
	sCircleGraph c;
	c.pos=pos;
	c.r=r;
	c.attr=attr;
	types[attr.dotted].circles_graph.push_back(c);
}


void cCircleShow::Show(int dotted)
{
	Lock();
	xassert(dotted>=0 && dotted<types.size());
	sCircleType& tp=types[dotted];

	{
		float t=HTManager::instance()->interpolationFactor();
		float t_=1-t;

		std::vector<sCircle>::iterator it;
		FOR_EACH(tp.circles,it)
		{
			sCircle& c=*it;
			CircleShow(c.pos[0]*t_+c.pos[1]*t,c.r[0]*t_+c.r[1]*t,c.attr);
		}
	}

	{
		std::vector<sCircleGraph>::iterator it;
		FOR_EACH(tp.circles_graph,it)
		{
			sCircleGraph& c=*it;
			CircleShow(c.pos,c.r,c.attr);
		}
		tp.circles_graph.clear();
	}
	Unlock();
}

void cCircleShow::Clear()
{
	std::vector<sCircleType>::iterator it;
	FOR_EACH(types,it)
	{
		it->circles.clear();
	}
}


void cCircleShow::Quant()
{
	u_begin=xm::fmod(u_begin + (float)scale_time.delta()*0.001f*RegionUSpeedDotted + 1.0f,1.0f);
}

void cCircleShow::CircleShow(const Vect3f& pos,float r, const CircleColor& circleColor)
{
	sColor4c diffuse(circleColor.color);
	float width = circleColor.width;
    indices_t* ib = nullptr;
    sVertexXYZDT1* vb = nullptr;
    
	if (circleColor.dotted != 3) {

		Vect2f tp,dn;

		int num_du = xm::round((2.0f * r * XM_PI) / circleColor.length);
		if(num_du<2)
			return;
		int num_da = xm::round((2.0f * r * XM_PI) / region_show_spline_space);
		if(num_da<2)
			return;


		//Кривовато, т.к. только для определённой сцены
		if(width<0)
		{
			float dist = pos.distance(terCamera->GetCamera()->GetPos());
			width =  -dist * width;
			float alpha = dist * float(diffuse.a) / maxAlphaCircleColorHeight;
			diffuse.a = (alpha > 255) ? 255.0f : alpha;
		}

		float da = XM_PI * 2.0f / (float)(num_da);
		float du=num_du/ (float)(num_da);
		float a=0, u=0;

	//	if(circleColor.dotted == 1 || circleColor.dotted == 2)
		if(circleColor.dotted == 1)
			u=u_begin;

		bool selection = (circleColor.dotted == 4);

        gb_RenderDevice->SetWorldMat4f(nullptr);
        DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);
        

		float v_pos = 0;

        uint32_t diffusev = gb_RenderDevice->ConvertColor(diffuse);

		for(int i = 0;i <= num_da;i++)
		{
			dn.x = xm::cos(a);
			dn.y = xm::sin(a);

			tp.x = pos.x + dn.x*r;
			tp.y = pos.y + dn.y*r;

			dn.x *= width;
			dn.y *= width;

			float z0 = ZFIX+(float)(vMap.GetAlt(vMap.XCYCL(xm::round(tp.x)), vMap.YCYCL(xm::round(tp.y))) >> VX_FRACTION);
            
            db->AutoLockTriangleStripStep(GAME_SHELL_SHOW_REGION_TRIANGLESTRIP_LOCK_SIZE, 1, vb, ib);

            vb[0].pos.set(tp.x-dn.x,tp.y-dn.y,z0);
            vb[0].u1() = selection ? v_pos : u;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;
		
			vb[1].pos.set(tp.x+dn.x,tp.y+dn.y,z0);
            vb[1].u1() = selection ? v_pos : vb[0].u1();
            
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;
            
            vb[0].diffuse = vb[1].diffuse = diffusev;
            vb[0].v1() = selection ? terExternalEnergyTextureEnd : 1.0f;
            vb[1].v1() = selection ? terExternalEnergyTextureStart : 0.05f;

			a += da;
			u+=du;
		}

        db->AutoUnlock();
        db->EndTriangleStrip();
	} else {
		int num = xm::round(2.0f * r / region_show_spline_space);
		if (num < 2) {
			return;
		}

		float v_pos = 0;

		uint32_t diffusev = gb_RenderDevice->ConvertColor(diffuse);

        gb_RenderDevice->SetWorldMat4f(nullptr);
        DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);

		float x1 = pos.x - width / 2.0f;
		float x2 = x1 + width;
		float y = pos.y - r;
		for (int i = 0; i <= num; i++) {
            db->AutoLockTriangleStripStep(GAME_SHELL_SHOW_REGION_TRIANGLESTRIP_LOCK_SIZE, 1, vb, ib);

            float z0 = ZFIX+(float)(vMap.GetAlt(vMap.XCYCL(xm::round(pos.x)), vMap.YCYCL(xm::round(y))) >> VX_FRACTION);
            
            vb[0].pos.set(x1, y, z0);
            vb[0].u1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;

            vb[1].pos.set(x2, y, z0);
            vb[1].u1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;


            vb[0].diffuse = vb[1].diffuse = diffusev;
            vb[0].v1() = 1.0f;
            vb[1].v1() = 0.05f;

			y += region_show_spline_space;
		}

        db->AutoUnlock();
        db->EndTriangleStrip();
        ib = nullptr;
        vb = nullptr;

		float y1 = pos.y - width / 2.0f;
		float y2 = y1 + width;
		float x = pos.x - r;
		for (int i = 0; i <= num; i++) {
            db->AutoLockTriangleStripStep(GAME_SHELL_SHOW_REGION_TRIANGLESTRIP_LOCK_SIZE, 1, vb, ib);

			float z0 = ZFIX+(float)(vMap.GetAlt(vMap.XCYCL(xm::round(x)), vMap.YCYCL(xm::round(pos.y))) >> VX_FRACTION);
			vb[0].pos.set(x, y1, z0);
			vb[0].u1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;
		
			vb[1].pos.set(x, y2, z0);
			vb[1].u1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;


            vb[0].diffuse = vb[1].diffuse = diffusev;
            vb[0].v1() = 1.0f;
            vb[1].v1() = 0.05f;

			x += region_show_spline_space;
		}

        db->AutoUnlock();
        db->EndTriangleStrip();
	}
}


void terExternalRegionShowUniform(Region* region,uint32_t color)
{
	float dt;
	float t;
	float t_max;
	Vect2f tp,dn,pp1,pp2;
	float z1,z2,z;

    DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);
	if (!region->spline().empty()) {

		dt = region->spline().suggest_dt(region_show_spline_space);

		const float ulen=48;
		const int addup=0,adddown=20;
		const float kdn=adddown/32.0f;

		float umap=0,umap_old;
		Vect3f o1,o2,begin1,begin2;

		t = 0;
		t_max = region->spline().t_max() - dt/2;

		int nums=0;
		do{
			dn = region->spline().inward_normal(t)*0.3f;
			umap+=dn.norm()/ulen;
			nums++;
		}while((t += dt) < t_max);

		std::vector<int> smoothz(nums);
		int i=0;
		t = 0;
		do{
			tp = region->spline()(t);
			dn = region->spline().inward_normal(t)*0.3f;

			pp1=tp;//+dn;
			pp2=tp-dn*kdn;

			z1 = (float)(vMap.GetAlt(vMap.XCYCL(xm::round(pp1.x)), vMap.YCYCL(xm::round(pp1.y))) >> VX_FRACTION);
			z2 = (float)(vMap.GetAlt(vMap.XCYCL(xm::round(pp2.x)), vMap.YCYCL(xm::round(pp2.y))) >> VX_FRACTION);
			z=max(z1,z2);
			smoothz[i++]=z;

		}while((t += dt) < t_max);

		const int maxdz=16;
		int prevz=smoothz[nums-1];
		for(i=0;i<nums;i++)
		{
			int& curz=smoothz[i];
			if(curz<prevz-maxdz)
				curz=prevz-maxdz;
			prevz=curz;
		}

		prevz=smoothz[0];
		for(i=nums-1;i>=0;i--)
		{
			int& curz=smoothz[i];
			if(curz<prevz-maxdz)
				curz=prevz-maxdz;
			prevz=curz;
		}

		t = 0;
		t_max = region->spline().t_max() - dt/2;

		if(umap<1)umap=1;

		float kumap= xm::round(umap) / umap;
		kumap/=ulen;

		bool first=true;
		umap=0;
		i=0;

        indices_t* indices = nullptr;
        sVertexXYZDT1* vertex_data = nullptr;
		do{
			tp = region->spline()(t);
			dn = region->spline().inward_normal(t)*0.3f;

			pp1=tp;//+dn;
			pp2=tp-dn*kdn;
			z=smoothz[i++];

			z1=z + addup;
			z2=z + adddown;

			if(!first)
			{
                db->AutoLockQuad(10, 1, vertex_data, indices);
				vertex_data[0].pos=o1;
				vertex_data[0].diffuse=color;
				vertex_data[0].u1() = umap_old;
				vertex_data[0].v1() = 1.0f;

				vertex_data[1].pos=o2;
				vertex_data[1].diffuse=color;
				vertex_data[1].u1() = umap_old;
				vertex_data[1].v1() = 0.0f;
				
				vertex_data[2].pos.set(pp1.x,pp1.y,z1);
				vertex_data[2].diffuse=color;
				vertex_data[2].u1() = umap;
				vertex_data[2].v1() = 1.0f;

				vertex_data[3].pos.set(pp2.x,pp2.y,z2);
				vertex_data[3].diffuse=color;
				vertex_data[3].u1() = umap;
				vertex_data[3].v1() = 0.0f;

			}else
			{
				begin1.set(pp1.x,pp1.y,z1);
				begin2.set(pp2.x,pp2.y,z2);
				first=false;
			}

			if(umap>3)
				umap-=3;

			umap_old=umap;
			o1.set(pp1.x,pp1.y,z1);
			o2.set(pp2.x,pp2.y,z2);

			umap+=kumap*dn.norm();
		}while((t += dt) < t_max);

		{
            db->AutoLockQuad(10, 1, vertex_data, indices);
			vertex_data[0].pos=o1;
			vertex_data[0].diffuse=color;
			vertex_data[0].u1() = umap_old;
			vertex_data[0].v1() = 1.0f;

			vertex_data[1].pos=o2;
			vertex_data[1].diffuse=color;
			vertex_data[1].u1() = umap_old;
			vertex_data[1].v1() = 0.0f;
			
			vertex_data[2].pos=begin1;
			vertex_data[2].diffuse=color;
			vertex_data[2].u1() = umap;
			vertex_data[2].v1() = 1.0f;

			vertex_data[3].pos=begin2;
			vertex_data[3].diffuse=color;
			vertex_data[3].u1() = umap;
			vertex_data[3].v1() = 0.0f;
		}

        db->AutoUnlock();
	}

	Region::iterator i;
	FOR_EACH(*region, i)
		terExternalRegionShowUniform(*i,color);
}

void terExternalRegionShowLine(Region* region,uint32_t diffuse)
{
	if(!(region->spline().empty()))
	{
		Vect2f tp,dn;
        gb_RenderDevice->SetWorldMat4f(nullptr);
        DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);

		float v_pos = 0;
		float dt = region->spline().suggest_dt(region_show_spline_space);
		float t = 0;
		float t_max = region->spline().t_max() + dt/2;

        indices_t* ib = nullptr;
		sVertexXYZDT1* vb = nullptr;

		do{
            db->AutoLockTriangleStripStep(GAME_SHELL_SHOW_REGION_TRIANGLESTRIP_LOCK_SIZE, 1, vb, ib);
            
			tp = region->spline()(t);
			dn = region->spline().inward_normal(t)*0.02f;

			float z0 = ZFIX+(float)(vMap.GetAlt(vMap.XCYCL(xm::round(tp.x)), vMap.YCYCL(xm::round(tp.y))) >> VX_FRACTION);

			vb[0].pos.set(tp.x+dn.x,tp.y+dn.y,z0);
			vb[0].v1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;

			vb[1].pos.set(tp.x-dn.x,tp.y-dn.y,z0);
			vb[1].v1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;

            vb[0].diffuse = diffuse;
            vb[0].u1() = terExternalEnergyTextureEnd;
            vb[1].diffuse = diffuse;
            vb[1].u1() = terExternalEnergyTextureStart;
		} while((t += dt) < t_max);

        db->AutoUnlock();
        db->EndTriangleStrip();
	}

	Region::iterator i;
	FOR_EACH(*region, i)
		terExternalRegionShowLine(*i,diffuse);
}

void terExternalRegionShowLineZeroplast(Region* region,uint32_t diffuse)
{
	if(!(region->spline().empty()))
	{
		Vect2f tp,dn;
        gb_RenderDevice->SetWorldMat4f(nullptr);
        DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);

        float v_pos = 0;
		float dt = region->spline().suggest_dt(region_show_spline_space);
		float t = 0;
		float t_max = region->spline().t_max() + dt/2;

        indices_t* ib = nullptr;
		sVertexXYZDT1* vb = nullptr;

		do{
			tp = region->spline()(t);
			dn = region->spline().inward_normal(t)*0.02f;

			float z0 = (float)(vMap.GetAlt(vMap.XCYCL(xm::round(tp.x)), vMap.YCYCL(xm::round(tp.y))) >> VX_FRACTION);
			if(z0<vMap.hZeroPlast)
				z0=vMap.hZeroPlast;
			z0+=ZFIX;

            db->AutoLockTriangleStripStep(GAME_SHELL_SHOW_REGION_TRIANGLESTRIP_LOCK_SIZE, 1, vb, ib);

            vb[0].pos.set(tp.x+dn.x,tp.y+dn.y,z0);
            vb[0].v1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;

			vb[1].pos.set(tp.x-dn.x,tp.y-dn.y,z0);
			vb[1].v1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;

            vb[0].diffuse = diffuse;
            vb[0].u1() = terExternalEnergyTextureEnd;
            vb[1].diffuse = diffuse;
            vb[1].u1() = terExternalEnergyTextureStart;
        } while((t += dt) < t_max);

        db->AutoUnlock();
        db->EndTriangleStrip();
	}

	Region::iterator i;
	FOR_EACH(*region, i)
		terExternalRegionShowLineZeroplast(*i,diffuse);
}

void terExternalRegionShowLineZeroplastVertical(Region* region,uint32_t diffuse)
{
	terRenderDevice->SetNoMaterial(ALPHA_BLEND);
	if(!(region->spline().empty()))
	{
		Vect2f tp;
        gb_RenderDevice->SetWorldMat4f(nullptr);
        DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLESTRIP);

		float v_pos = 0;
		float dt = region->spline().suggest_dt(region_show_spline_space);
		float t = 0;
		float t_max = region->spline().t_max() + dt/2;

        indices_t* ib = nullptr;
        sVertexXYZDT1* vb = nullptr;
		float zero=vMap.hZeroPlast;
		float zero_fix=zero-ZFIX;

        
		do{
            db->AutoLockTriangleStripStep(GAME_SHELL_SHOW_REGION_TRIANGLESTRIP_LOCK_SIZE, 1, vb, ib);
            
			tp = region->spline()(t);

			float z0 = (float)(vMap.GetAlt(vMap.XCYCL(xm::round(tp.x)), vMap.YCYCL(xm::round(tp.y))) >> VX_FRACTION);
			if(z0>zero) z0=zero;

            vb[0].pos.set(tp.x,tp.y,zero_fix);
            vb[0].v1() = v_pos; 
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;

            vb[1].pos.set(tp.x,tp.y,z0);
            vb[1].v1() = v_pos;
			v_pos += GAME_SHELL_SHOW_REGION_V_STEP;

            vb[0].diffuse = diffuse;
            vb[0].u1() = terExternalEnergyTextureEnd;
            vb[1].diffuse = diffuse;
            vb[1].u1() = terExternalEnergyTextureStart;
        } while((t += dt) < t_max);

        db->AutoUnlock();
        db->EndTriangleStrip();
	}

	Region::iterator i;
	FOR_EACH(*region, i)
		terExternalRegionShowLineZeroplastVertical(*i,diffuse);
}

/*
void terExternalRegionShowColumn(Column* column,sColor4c color)
{//Для дебага
	terRenderDevice->SetNoMaterial(ALPHA_BLEND);
    DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);

	float z=vMap.hZeroPlast-ZFIX;
    indices_t* i;
    sVertexXYZDT1* p;
	for (CellLine& cell : *column) {
		for (Cell& c : cell) {
            db->AutoLockQuad<sVertexXYZDT1>(50, 1, p, i);
			p[0].pos.x=c.xl;
			p[0].pos.y=c.y;
			p[0].pos.z=z;
			p[0].diffuse=color;
			p[0].u1()=p[0].v1()=0;

			p[1].pos.x=c.xr;
			p[1].pos.y=c.y;
			p[1].pos.z=z;
			p[1].diffuse=color;
			p[1].u1()=p[0].v1()=0;

			p[2].pos.x=c.xl;
			p[2].pos.y=c.y+1;
			p[2].pos.z=z;
			p[2].diffuse=color;
			p[2].u1()=p[0].v1()=0;

			p[3].pos.x=c.xr;
			p[3].pos.y=c.y+1;
			p[3].pos.z=z;
			p[3].diffuse=color;
			p[3].u1()=p[0].v1()=0;
            break;
		}
        break;
	}
    db->AutoUnlock();
}
/*/

void terExternalRegionShowColumn(Column* column,uint32_t color) {
	terRenderDevice->SetNoMaterial(ALPHA_BLEND);

    DrawBuffer* db = gb_RenderDevice->GetDrawBuffer(sVertexXYZDT1::fmt, PT_TRIANGLES);

	float z=vMap.hZeroPlast-ZFIX;

    const size_t locked = 100;
    const size_t v_len = sizeof(sVertexXYZDT1) * 3;

    sVertexXYZDT1* vp = nullptr;
    indices_t* ip = nullptr;
    std::vector<CellLine>::iterator it_line;
	FOR_EACH(*column, it_line) {
		CellLine* cell=&*it_line;
		CellLine* next_cell=NULL;
		CellLine* prev_cell=NULL;

		{
			std::vector<CellLine>::iterator it_line_next=it_line;
			it_line_next++;
			if(it_line_next!=column->end())
			{
				next_cell=&*it_line_next;
			}
		}

		{
			std::vector<CellLine>::iterator it_line_prev=it_line;
			if(it_line_prev!=column->begin())
			{
				it_line_prev--;
				prev_cell=&*it_line_prev;
			}
		}

		std::list<Cell>::iterator it,it_next,it_prev;
		if(next_cell)
			it_next=next_cell->begin();
		if(prev_cell)
			it_prev=prev_cell->begin();

		for (const Cell& c : *cell) {
			//Emulate/Эмулируем triangle fan
			sVertexXYZDT1 p[3];
            p[0].u1()=0, p[0].v1()=0;
            p[1].u1()=0, p[1].v1()=0;
            p[2].u1()=0, p[2].v1()=0;
			p[0].diffuse=p[1].diffuse=p[2].diffuse=color;

			p[0].pos.set(c.xl,c.y,z);
			p[2].pos.set(c.xl,c.y+1,z);

			if(next_cell) {
                for (; it_next != next_cell->end(); it_next++) {
                    //Добавляем доп. точки из нижней линии.
                    //Add extra point from the bottom line
                    Cell& cn = *it_next;
                    if (cn.xr <= c.xl)
                        continue;
                    if (cn.xl >= c.xr)
                        break;
                    xassert(c.y + 1 == cn.y);

                    if (cn.xl > c.xl && cn.xl < c.xr) {
                        p[1] = p[2];
                        p[2].pos.set(cn.xl, cn.y, z);
                        db->AutoLockTriangle(locked, 1, vp, ip);
                        memcpy(vp, p, v_len);
                    }

                    if (cn.xr > c.xl && cn.xr < c.xr) {
                        p[1] = p[2];
                        p[2].pos.set(cn.xr, cn.y, z);
                        db->AutoLockTriangle(locked, 1, vp, ip);
                        memcpy(vp, p, v_len);
                    }
                }
            }

			p[1]=p[2];
			p[2].pos.set(c.xr,c.y+1,z);
            db->AutoLockTriangle(locked, 1, vp, ip);
            memcpy(vp, p, v_len);

			bool first=true;
            if (prev_cell) {
                for (; it_prev != prev_cell->end(); it_prev++) {
                    //Добавляем доп. точки из верхней линии.
                    //Add extra point from the top line
                    Cell& cn = *it_prev;
                    if (cn.xr <= c.xl)
                        continue;
                    if (cn.xl >= c.xr)
                        break;
                    xassert(c.y - 1 == cn.y);

                    if (cn.xl > c.xl && cn.xl < c.xr) {
                        if (first)
                            first = false;
                        else
                            p[0] = p[1];
                        p[1].pos.set(cn.xl, c.y, z);
                        db->AutoLockTriangle(locked, 1, vp, ip);
                        memcpy(vp, p, v_len);
                    }

                    if (cn.xr > c.xl && cn.xr < c.xr) {
                        if (first)
                            first = false;
                        else
                            p[0] = p[1];
                        p[1].pos.set(cn.xr, c.y, z);
                        db->AutoLockTriangle(locked, 1, vp, ip);
                        memcpy(vp, p, v_len);
                    }
                }
            }

			if(first) {
				p[1]=p[2];
				p[2].pos.set(c.xr,c.y,z);
			} else {
				p[0]=p[1];
				p[1].pos.set(c.xr,c.y,z);
			}

            db->AutoLockTriangle(locked, 1, vp, ip);
            memcpy(vp, p, v_len);
		}
	}

    db->AutoUnlock();
}
//*/

terRegionColumnMain::terRegionColumnMain()
{
}

terRegionColumnMain::~terRegionColumnMain()
{
	clear();
}

void terRegionColumnMain::clear()
{
	std::vector<cObjectNodeRoot*>::iterator it;
	FOR_EACH(object,it)
		(*it)->Release();
	object.clear();
}

bool terCheckFilthHardness(int x,int y);

void terRegionColumnMain::quant()
{
	if(!_pShellDispatcher->ShowTerraform())
	{
		clear();
		return;
	}

	sColor4f red(sColor4c(RegionMain.column_red));
	sColor4f green(sColor4c(RegionMain.column_green));

	int cur_object=0;
	int step=RegionMain.column_step;
	MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());

	for(int num_region=0;num_region<2;num_region++)
	{
		MetaLockRegionDispatcher region=(*_pShellDispatcher->regionMetaDispatcher())[num_region];
		Column& column=region->getRasterizeColumn();
		std::vector<CellLine>::iterator it_line;
		FOR_EACH(column,it_line)
		{
			CellLine& cell=*it_line;
			std::list<Cell>::iterator it;
			FOR_EACH(cell,it)
			{
				Cell& c=*it;
				if(c.y%step)
					break;
				int min_x=((c.xl+step-1)/step)*step;
				int max_x=(c.xr/step)*step;
				for(int x=min_x;x<=max_x;x+=step)
				{
					Vect3f pos=To3D(Vect3f(x,c.y,0));
					if(pos.z<=vMap.hZeroPlast)
						continue;
					bool hard=terCheckFilthHardness(x,c.y);
					
					if(object.size()<=cur_object)
					{
						object.push_back(terScene->CreateObject(RegionMain.column_model));
					}
					cObjectNodeRoot* p=object[cur_object];

					p->SetColor(0,hard?&red:&green,0);

					float s=RegionMain.column_model_size;
					p->SetScale(Vect3f(s,s,s));
					MatXf mat(Mat3f::ID,pos);
					p->SetPosition(mat);

					cur_object++;
				}
			}
		}
	}

	for(int i=cur_object;i<object.size();i++)
		object[i]->Release();
	object.resize(cur_object);
}
