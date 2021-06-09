#include "StdAfx.h"

#include "Umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"

#include "Runtime.h"
#include "Region.h"

#include "CameraManager.h"

#include "terra.h"

#include "GenericControls.h"
#include "Universe.h"

#include "TrustMap.h"
const int stats_full=2,stats_border=1;

inline bool IsNoHardness(unsigned short p)
{
	return (p & GRIDAT_MASK_HARDNESS) == GRIDAT_MASK_HARDNESS;
}

void SharedObject::Kill()
{
	alive = false;
}

//-----------------------------------------------------

terTerraformGeneral::terTerraformGeneral(int id,int x,int y,terPlayer* player)
{
	PositionX = x;
	PositionY = y;

	Player = player;
	ID = id;
	UnitPoint = NULL;
	Status = TERRAFORM_STATUS_NONE;
	CollisionCount = 0;
}

terTerraformGeneral::~terTerraformGeneral()
{
	MTL();
	xassert(!inserted());
}

void terTerraformGeneral::Quant(terTerraformDispatcher* dispatcher,bool first)
{
	xassert(Alive());
	int prepared_zero,resource_zero,dig_layer,fill_layer,ground_zero,cell,zero_present;

	int last_status = Status;
	bool last_df=IsDigFill();

	GetClusterAttribute(PositionX,PositionY,TERRAFORM_ELEMENT_SIDE,TERRAFORM_ELEMENT_SIDE,prepared_zero,resource_zero,dig_layer,fill_layer,ground_zero,cell,zero_present);

	Status = TERRAFORM_STATUS_NONE;
	if(dig_layer)
		Status |= TERRAFORM_STATUS_DIG;
	if(fill_layer)
		Status |= TERRAFORM_STATUS_FILL;

	if(prepared_zero == cell){
		if(resource_zero == cell)
			Status |= TERRAFORM_STATUS_RESOURCE;
		else
			Status |= TERRAFORM_STATUS_PREPARED;
	}

	if(zero_present == 0)
		Status |= TERRAFORM_STATUS_CLEAN;

	if((Status & TERRAFORM_STATUS_RESOURCE) && !(last_status & TERRAFORM_STATUS_RESOURCE))
		Player->TerrainWorkingComplete(PositionX - TERRAFORM_ELEMENT_SIDE,PositionY - TERRAFORM_ELEMENT_SIDE,TERRAFORM_ELEMENT_SIDE,TERRAFORM_ELEMENT_SIDE);

	bool cur_df=IsDigFill();
	if(first || last_df!=cur_df)
	{
		ShareHandle<terTerraformGeneral> p=this;
		if(!first)
		{
			xassert((*it_self)()==this);
			if(last_df)
				dispatcher->TerraformsDigFill.erase(it_self);
			else
				dispatcher->TerraformsOther.erase(it_self);
		}

		if(cur_df)
		{
			dispatcher->TerraformsDigFill.push_front(this);
			it_self=dispatcher->TerraformsDigFill.begin();
		}else
		{
			dispatcher->TerraformsOther.push_front(this);
			it_self=dispatcher->TerraformsOther.begin();
		}

		terTerraformGeneral* self=*it_self;
		xassert(self==this);
	}

	ChangeStatus(dispatcher,last_status,Status);
}

void terTerraformGeneral::ChangeStatus(terTerraformDispatcher* dispatcher,int begin_status,int end_status)
{
	bool old_prepared=(begin_status&TERRAFORM_STATUS_PREPARED)?true:false;
	bool new_prepared=(end_status&TERRAFORM_STATUS_PREPARED)?true:false;
	bool old_dig=(begin_status&TERRAFORM_STATUS_DIG)?true:false;
	bool new_dig=(end_status&TERRAFORM_STATUS_DIG)?true:false;
	switch(Type)
	{
	case TERRAFORM_TYPE_FULL:
		if(old_prepared!=new_prepared)
			dispatcher->zero_complete+=new_prepared?stats_full:-stats_full;
		break;

	case TERRAFORM_TYPE_BORDER:
		if(old_prepared!=new_prepared)
			dispatcher->zero_complete+=new_prepared?stats_border:-stats_border;
		break;

	case TERRAFORM_TYPE_ABYSS_FULL:
		if(old_dig!=new_dig)
			dispatcher->abyss_complete+=new_dig?-stats_full:stats_full;
		break;

	case TERRAFORM_TYPE_ABYSS_BORDER:
		if(old_dig!=new_dig)
			dispatcher->abyss_complete+=new_dig?-stats_border:stats_border;
		break;

	case TERRAFORM_TYPE_GARBAGE:
		break;

	default:
		xassert(0);
	}
}


void terTerraformGeneral::ShowRect(sColor4c c,bool line1,bool line2)
{
	Vect3f v1,v2,v3,v4;
	int d=TERRAFORM_ELEMENT_SIDE-1;

	v1=vMap.getVect3f(PositionX-d, PositionY-d);
	v2=vMap.getVect3f(PositionX+d, PositionY-d);
	v3=vMap.getVect3f(PositionX+d, PositionY+d);
	v4=vMap.getVect3f(PositionX-d, PositionY+d);
	terRenderDevice->DrawLine(v1,v2,c);
	terRenderDevice->DrawLine(v2,v3,c);
	terRenderDevice->DrawLine(v3,v4,c);
	terRenderDevice->DrawLine(v4,v1,c);

	if(line1)
		terRenderDevice->DrawLine(v1,v3,c);
	if(line2)
		terRenderDevice->DrawLine(v2,v4,c);
}

//-------------------------------------------------
terTerraformGarbage::terTerraformGarbage(int id,int x,int y, terPlayer* player)
: terTerraformGeneral(id,x,y,player)
{
	Type = TERRAFORM_TYPE_GARBAGE; 
}

void terTerraformGarbage::Show()
{
	ShowRect(sColor4c(255,255,255),false,false);
}

//-------------------------------------------------

terTerraformFull::terTerraformFull(int id,int x,int y, terPlayer* player)
: terTerraformGeneral(id,x,y,player)
{
	Type = TERRAFORM_TYPE_FULL; 
	Height = vMap.hZeroPlast;
}

void terTerraformFull::Show()
{
	sColor4c c(0,0,0);
/*
	if(IsDigFill())
		c=sColor4c(255,255,255);
/*/
	if(Status & TERRAFORM_STATUS_DIG)
		c.r += 127;
	if(Status & TERRAFORM_STATUS_FILL)
		c.g += 127;
/**/
	ShowRect(c,Status&TERRAFORM_STATUS_RESOURCE,Status&TERRAFORM_STATUS_PREPARED);
}

int terTerraformFull::GetDigPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);

	int lx = -1;
	int ly = -1;
	x=(x0+x1)/2;
	y=(y0+y1)/2;
	unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];

	if(!GRIDTST_LEVELED(p) && GRIDTST_TALLER_HZEROPLAST(p) && IsNoHardness(p)){
		lx = vMap.m2wHalf(x);
		ly = vMap.m2wHalf(y);
		if(lx != cx || ly != cy){
			cx = lx;
			cy = ly;
			return 1;
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];

			if(!GRIDTST_LEVELED(p) && GRIDTST_TALLER_HZEROPLAST(p) && IsNoHardness(p)){
				lx = vMap.m2wHalf(x);
				ly = vMap.m2wHalf(y);
				if(lx != cx || ly != cy){
					cx = lx;
					cy = ly;
					return 1;
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformFull::GetFillPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	int lx = -1;
	int ly = -1;
	x=(x0+x1)/2;
	y=(y0+y1)/2;
	unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];

	if((!GRIDTST_LEVELED(p) && !GRIDTST_TALLER_HZEROPLAST(p)) || (GRIDTST_LEVELED(p) && (p & GRIDAT_BASE_OF_BUILDING_CORRUPT))){
		lx = vMap.m2wHalf(x);
		ly = vMap.m2wHalf(y);
		if(lx != cx || ly != cy){
			cx = lx;
			cy = ly;
			return 1;
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];

			if((!GRIDTST_LEVELED(p) && !GRIDTST_TALLER_HZEROPLAST(p)) || (GRIDTST_LEVELED(p) && (p & GRIDAT_BASE_OF_BUILDING_CORRUPT))){
				lx = vMap.m2wHalf(x);
				ly = vMap.m2wHalf(y);
				if(lx != cx || ly != cy){
					cx = lx;
					cy = ly;
					return 1;
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformFull::GetZeroPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	int lx = -1;
	int ly = -1;
	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];
			if(!GRIDTST_ZEROPLAST(p) && GRIDTST_LEVELED(p)){
				lx = vMap.m2wHalf(x);
				ly = vMap.m2wHalf(y);
				if(lx != cx || ly != cy){
					cx = lx;
					cy = ly;
					return 1;
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformFull::GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present)
{
	int i,x;
	int cnt;

	prepared_zero = 0;
	resource_zero = 0;
	dig_layer = 0;
	fill_layer = 0;
	ground_zero = 0;
	cell = 0;
	zero_present = 0;

	int x0 = vMap.w2mClampX(cx - sx);
	int y0 = vMap.w2mClampY(cy - sy);
	int x1 = vMap.w2mClampX(cx + sx);
	int y1 = vMap.w2mClampY(cy + sy);
	
	cnt = 0;
	for(i = y0;i < y1;i++){
		for(x = x0;x < x1;x++){
			unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, i)];

			if(GRIDTST_LEVELED(p)){
				if(p & GRIDAT_BASE_OF_BUILDING_CORRUPT)
					fill_layer++;
				else
					prepared_zero++;
				cell++;
			}else{
				if(GRIDTST_TALLER_HZEROPLAST(p))
				//if(vMap.GVBuf[vMap.offsetGBuf(x,i)]>vMap.hZeroPlast)
				{
					if(IsNoHardness(p)){
						dig_layer++;
						cell++;
					}
				}else{
					fill_layer++;
					cell++;
				}					
			}
		}
	}
	return cnt;
}

int terTerraformFull::CheckDigPosition(int cx,int cy)
{
	unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(cx, cy)];
	if(!GRIDTST_LEVELED(p) && GRIDTST_TALLER_HZEROPLAST(p) && IsNoHardness(p))
		return 1;
	return 0;
}

int terTerraformFull::CheckFillPosition(int cx,int cy)
{
	unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(cx, cy)];
	if(!GRIDTST_LEVELED(p) && !GRIDTST_TALLER_HZEROPLAST(p))
		return 1;
	return 0;
}

int terTerraformFull::CheckZeroPosition(int cx,int cy)
{
	unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(cx, cy)];
	if(!GRIDTST_ZEROPLAST(p) && GRIDTST_LEVELED(p))
		return 1;
	return 0;
}

//--------------------------------------------------
terTerraformBorder::terTerraformBorder(int id,int x,int y, terPlayer* player)
: terTerraformFull(id,x,y,player)
{
	Type = TERRAFORM_TYPE_BORDER; 
}


int terTerraformBorder::GetDigPosition(int& cx,int& cy)
{
	int x,y;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	MetaRegionLock lock(Player->RegionPoint);
	RegionDispatcher* region = Player->ZeroRegionPoint.data();

	int lx = -1;
	int ly = -1;

	x=(x0+x1)/2;
	y=(y0+y1)/2;
	if(region->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)), vMap.YCYCL(vMap.m2wHalf(y))))){
		unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];
		if(!GRIDTST_LEVELED(p) && GRIDTST_TALLER_HZEROPLAST(p) && IsNoHardness(p)){
			lx = vMap.m2wHalf(x);
			ly = vMap.m2wHalf(y);
			if(lx != cx || ly != cy){
				cx = lx;
				cy = ly;
				return 1;
			}
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			if(region->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)), vMap.YCYCL(vMap.m2wHalf(y))))){
				unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];
				if(!GRIDTST_LEVELED(p) && GRIDTST_TALLER_HZEROPLAST(p) && IsNoHardness(p)){
					lx = vMap.m2wHalf(x);
					ly = vMap.m2wHalf(y);
					if(lx != cx || ly != cy){
						cx = lx;
						cy = ly;
						return 1;
					}
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformBorder::GetFillPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);

	MetaRegionLock lock(Player->RegionPoint);
	RegionDispatcher* region = Player->ZeroRegionPoint.data();

	int lx = -1;
	int ly = -1;
	x=(x0+x1)/2;
	y=(y0+y1)/2;
	if(region->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)), vMap.YCYCL(vMap.m2wHalf(y))))){
		unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];
		if((!GRIDTST_LEVELED(p) && !GRIDTST_TALLER_HZEROPLAST(p)) || (GRIDTST_LEVELED(p) && (p & GRIDAT_BASE_OF_BUILDING_CORRUPT))){
			lx = vMap.m2wHalf(x);
			ly = vMap.m2wHalf(y);
			if(lx != cx || ly != cy){
				cx = lx;
				cy = ly;
				return 1;
			}
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			if(region->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)), vMap.YCYCL(vMap.m2wHalf(y))))){
				unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];
				if((!GRIDTST_LEVELED(p) && !GRIDTST_TALLER_HZEROPLAST(p)) || (GRIDTST_LEVELED(p) && (p & GRIDAT_BASE_OF_BUILDING_CORRUPT))){
					lx = vMap.m2wHalf(x);
					ly = vMap.m2wHalf(y);
					if(lx != cx || ly != cy){
						cx = lx;
						cy = ly;
						return 1;
					}
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformBorder::GetZeroPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	int lx = -1;
	int ly = -1;

	MetaRegionLock lock(Player->RegionPoint);
	RegionDispatcher* region = Player->ZeroRegionPoint.data();

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			if(region->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)), vMap.YCYCL(vMap.m2wHalf(y))))){
				unsigned short p = vMap.GABuf[vMap.offsetGBuf(x, y)];
				if(!GRIDTST_ZEROPLAST(p) && GRIDTST_LEVELED(p)){
					lx = vMap.m2wHalf(x);
					ly = vMap.m2wHalf(y);
					if(lx != cx || ly != cy){
						cx = lx;
						cy = ly;
						return 1;
					}
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformBorder::GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present)
{
	int i,j;
	int cnt;

	prepared_zero = 0;
	resource_zero = 0;
	dig_layer = 0;
	fill_layer = 0;
	ground_zero = 0;
	cell = 0;

	zero_present = 0;

	int x0 = vMap.w2mClampX(cx - sx);
	int y0 = vMap.w2mClampY(cy - sy);
	int x1 = vMap.w2mClampX(cx + sx);
	int y1 = vMap.w2mClampY(cy + sy);
	
	cnt = 0;

	MetaRegionLock lock(Player->RegionPoint);
	RegionDispatcher* region = Player->ZeroRegionPoint.data();

	for(i = y0;i < y1;i++){
		for(j = x0;j < x1;j++){
			if(region->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(j)), vMap.YCYCL(vMap.m2wHalf(i))))){
				unsigned short p = vMap.GABuf[vMap.offsetGBuf(j, i)];

				if(GRIDTST_LEVELED(p)){
					if(p & GRIDAT_BASE_OF_BUILDING_CORRUPT)
						fill_layer++;
					else
						prepared_zero++;

					cell++;
				}else{
					if(GRIDTST_TALLER_HZEROPLAST(p))
					//if(vMap.GVBuf[vMap.offsetGBuf(j,i)]>vMap.hZeroPlast)
					{
						if(IsNoHardness(p)){
							dig_layer++;
							cell++;
						}
					}else{
						fill_layer++;
						cell++;
					}
				}
			}
		}
	}
	return cnt;
}

int terTerraformBorder::CheckDigPosition(int cx,int cy)
{
	if(Player->ZeroRegionPoint->filledRasterized(Vect2i(cx,cy))){
		unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(cx, cy)];
		if(!GRIDTST_LEVELED(p) && GRIDTST_TALLER_HZEROPLAST(p) && IsNoHardness(p))
			return 1;
	}
	return 0;
}

int terTerraformBorder::CheckFillPosition(int cx,int cy)
{
	if(Player->ZeroRegionPoint->filledRasterized(Vect2i(cx,cy))){
		unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(cx, cy)];
		if(!GRIDTST_LEVELED(p) && !GRIDTST_TALLER_HZEROPLAST(p))
			return 1;
	}
	return 0;
}

int terTerraformBorder::CheckZeroPosition(int cx,int cy)
{
	if(Player->ZeroRegionPoint->filledRasterized(Vect2i(cx,cy))){
		unsigned short p = vMap.GABuf[vMap.offsetGBufWorldC(cx, cy)];
		if(!GRIDTST_ZEROPLAST(p) && GRIDTST_LEVELED(p))
			return 1;
	}
	return 0;
}

//--------------------------------------------------

terTerraformDispatcher::terTerraformDispatcher(terPlayer* player)
: TrustGrid(vMap.H_SIZE, vMap.V_SIZE)
{
	Count = 0;
	Player = player;	
	zero_request = 0;
	zero_complete = 0;
	abyss_request = 0;
	abyss_complete = 0;
}

terTerraformDispatcher::~terTerraformDispatcher()
{
	TerraformList::iterator ti;
	FOR_EACH(TerraformsDigFill, ti)
	{
		TrustGrid.Remove(**ti);
	}
	TerraformsDigFill.clear();
	FOR_EACH(TerraformsOther, ti)
	{
		TrustGrid.Remove(**ti);
	}
	TerraformsOther.clear();
}

//---------------------------------------------

terTerraformGeneral* terTerraformDispatcher::AddElement(terTerraformType type, int x, int y, int id)
{
	MTL();
	Count++;

	terTerraformGeneral* element;
	switch(type)
	{
	case TERRAFORM_TYPE_FULL:
		element = new terTerraformFull(id,x,y,Player);
		zero_request+=stats_full;
		break;

	case TERRAFORM_TYPE_BORDER:
		element = new terTerraformBorder(id,x,y,Player);
		zero_request+=stats_border;
		break;

	case TERRAFORM_TYPE_ABYSS_FULL:
		element = new terTerraformAbyssFull(id,x,y,Player);
		abyss_request+=stats_full;
		break;

	case TERRAFORM_TYPE_ABYSS_BORDER:
		element = new terTerraformAbyssBorder(id,x,y,Player);
		abyss_request+=stats_border;
		break;

	case TERRAFORM_TYPE_GARBAGE:
		element = new terTerraformGarbage(id,x,y,Player);
		break;

	default:
		xassert(0);
	}

	element->Quant(this,true);
	TrustGrid.Insert(*element, x, y, TERRAFORM_ELEMENT_SIDE);
	return element;
}


//------------------------------------------------

void terTerraformDispatcher::Clear()
{
	MTL();
	TerraformList::iterator ti;
	for(ti=TerraformsDigFill.begin();ti!=TerraformsDigFill.end();)
	{
		switch((*ti)->Type) 
		{
		case TERRAFORM_TYPE_FULL:
		case TERRAFORM_TYPE_BORDER:
		case TERRAFORM_TYPE_ABYSS_FULL:
		case TERRAFORM_TYPE_ABYSS_BORDER:
			ti = DeleteElement(ti);
			break;
		default:
			ti++;
			break;
		}
	}

	for(ti=TerraformsOther.begin();ti!=TerraformsOther.end();)
	{
		switch((*ti)->Type) 
		{
		case TERRAFORM_TYPE_FULL:
		case TERRAFORM_TYPE_BORDER:
		case TERRAFORM_TYPE_ABYSS_FULL:
		case TERRAFORM_TYPE_ABYSS_BORDER:
			ti = DeleteElement(ti);
			break;
		default:
			ti++;
			break;
		}
	}
}

void terTerraformDispatcher::Show()
{
	TerraformList::iterator ti;
	FOR_EACH(TerraformsDigFill, ti)
		(*ti)->Show();
	FOR_EACH(TerraformsOther, ti)
		(*ti)->Show();
}

TerraformList::iterator terTerraformDispatcher::DeleteElement(TerraformList::iterator ti)
{
	TrustGrid.Remove(**ti);
	(*ti)->Kill();

	switch((*ti)->Type)
	{
	case TERRAFORM_TYPE_FULL:
		zero_request-=stats_full;
		break;

	case TERRAFORM_TYPE_BORDER:
		zero_request-=stats_border;
		break;

	case TERRAFORM_TYPE_ABYSS_FULL:
		abyss_request-=stats_full;
		break;

	case TERRAFORM_TYPE_ABYSS_BORDER:
		abyss_request-=stats_border;
		break;

	case TERRAFORM_TYPE_GARBAGE:
		break;

	default:
		xassert(0);
	}
	(*ti)->ChangeStatus(this,(*ti)->Status,TERRAFORM_STATUS_DIG);

	bool is=(*ti)->IsDigFill();
	if(is)
		return TerraformsDigFill.erase(ti);

	return TerraformsOther.erase(ti);
}

void terTerraformDispatcher::DeleteElement(terTerraformGeneral* element)
{
	DeleteElement(element->it_self);
}

void terTerraformDispatcher::ScanTrustMap(int& request_digger,int& request_filler)
{
	request_digger = 0;
	request_filler = 0;

	TerraformList::iterator ti;
	FOR_EACH(TerraformsDigFill, ti)
	{
		terTerraformGeneral& terraform = **ti;
		if(terraform.UnitPoint)
			continue;
		switch(terraform.Type)
		{
		case TERRAFORM_TYPE_FULL:
		case TERRAFORM_TYPE_BORDER:
			if(terraform.Status & TERRAFORM_STATUS_DIG)
				request_digger++;
			else if(terraform.Status & TERRAFORM_STATUS_FILL)
				request_filler++;
			break;

		case TERRAFORM_TYPE_ABYSS_FULL:
		case TERRAFORM_TYPE_ABYSS_BORDER: 
			if(terraform.Status & TERRAFORM_STATUS_DIG)
				request_digger++;
			break;
		}
	}
}

void terTerraformDispatcher::ScanTrustMap(int& request_digger,int& request_filler,int id)
{
	request_digger = 0;
	request_filler = 0;

	TerraformList::iterator ti;
	FOR_EACH(TerraformsDigFill, ti){
		terTerraformGeneral& terraform = **ti;
		if(terraform.UnitPoint || terraform.ID != id)
			continue;
		switch(terraform.Type)
		{
		case TERRAFORM_TYPE_FULL:
		case TERRAFORM_TYPE_BORDER:
			if(terraform.Status & TERRAFORM_STATUS_DIG)
				request_digger++;
			else if(terraform.Status & TERRAFORM_STATUS_FILL)
				request_filler++;
			break;

		case TERRAFORM_TYPE_ABYSS_FULL:
		case TERRAFORM_TYPE_ABYSS_BORDER: 
			if(terraform.Status & TERRAFORM_STATUS_DIG)
				request_digger++;
			break;
		}
	}
}

terTerraformGeneral* terTerraformDispatcher::FindNear(int TerraformTypes, terTerraformStatus status, int x,int y, int id)
{
//	int size_df=TerraformsDigFill.size();
//	int size_oter=TerraformsOther.size();
	xassert(status==TERRAFORM_STATUS_DIG || status==TERRAFORM_STATUS_FILL);

	terTerraformGeneral* p = NULL;
	int md = 0;
	TerraformList::iterator ti;
	FOR_EACH(TerraformsDigFill, ti){
		terTerraformGeneral& t = **ti;
		if( (t.Type | TerraformTypes) && 
			(id == -1 || t.ID == id) && 
			!t.UnitPoint && (t.Status & status)
		  )
		{
			int d = sqr(x - t.PositionX) + sqr(y - t.PositionY);			
			if(d < md || !p){
				md = d;
				p = &t;
			}
		}
	}
	if(p) 
		return p;

	return 0;
}

terTerraformGeneral* terTerraformDispatcher::FindNearDigger(int x,int y)
{
	int types = 
		TERRAFORM_TYPE_FULL | 
		TERRAFORM_TYPE_BORDER |
		TERRAFORM_TYPE_ABYSS_FULL |
		TERRAFORM_TYPE_ABYSS_BORDER ;

	return FindNear( types, TERRAFORM_STATUS_DIG, x, y);
}

terTerraformGeneral* terTerraformDispatcher::FindNearFiller(int x,int y)
{
	int types = 
		TERRAFORM_TYPE_FULL |
		TERRAFORM_TYPE_BORDER;

	return FindNear(types, TERRAFORM_STATUS_FILL, x, y);
}

//--------------

terTerraformGeneral* terTerraformDispatcher::FindNearDigger(int x,int y,int id)
{
	int types = 
		TERRAFORM_TYPE_FULL |
		TERRAFORM_TYPE_BORDER |
		TERRAFORM_TYPE_ABYSS_FULL |
		TERRAFORM_TYPE_ABYSS_BORDER;

	return FindNear( types, TERRAFORM_STATUS_DIG, x, y, id);
}

terTerraformGeneral* terTerraformDispatcher::FindNearFiller(int x,int y,int id)
{
	int types = 
		TERRAFORM_TYPE_FULL |
		TERRAFORM_TYPE_BORDER;

	return FindNear( types, TERRAFORM_STATUS_FILL, x, y, id);
}

//-----------------------------------Dig Garbage----------------------------------

int terDigGarbageCheckFull(int cx,int cy,class RegionMetaDispatcher* region)
{
	int i,j;

	int x0 = vMap.w2mClampX(cx - TRUST_MAP_GARBAGE_SIZE);
	int y0 = vMap.w2mClampY(cy - TRUST_MAP_GARBAGE_SIZE);
	int x1 = vMap.w2mClampX(cx + TRUST_MAP_GARBAGE_SIZE);
	int y1 = vMap.w2mClampY(cy + TRUST_MAP_GARBAGE_SIZE);

	for(i = y0;i <= y1;i++){
		for(j = x0;j <= x1;j++){
			unsigned short p = vMap.GABuf[vMap.offsetGBuf(j,i)];
			if(GRIDTST_LEVELED(p) || !IsNoHardness(p) || (vMap.GVBuf[vMap.offsetGBuf(j,i)] == 0 /*PERIMETER_GARBAGE_MIN_HEIGH*/))
				return 0;
			if(region->filled(vMap.m2w(Vect2i(j, i))))
				return 0;
		}
	}
	return 1;
}

inline int terDigGarbageMapTest(int x,int y)
{
	int offset = vMap.offsetGBufWorldC(x,y);
	unsigned short p = vMap.GABuf[offset];
	return GRIDTST_LEVELED(p) || !IsNoHardness(p) || vMap.GVBuf[offset] == 0;
}

struct terMapGridGarbageCheckOperator
{
	terPlayer* Player;

	terMapGridGarbageCheckOperator(terPlayer* player)
	{ 
		Player = player; 
	}

	int operator()(const terTerraformGeneral* p)
	{
		xassert(p->Player == Player);
		return 0;
/*
		if(p->Player == Player)
			return 0;
		return 1;
*/
	}
};

int terDigGarbageTest(int x,int y,class RegionMetaDispatcher* region,terPlayer* player)
{
	MetaRegionLock lock(region);
//	terMapGridGarbageCheckOperator op(player);

	if(terDigGarbageMapTest(x,y) || 
	   terDigGarbageMapTest(x - TRUST_MAP_GARBAGE_SIZE,y - TRUST_MAP_GARBAGE_SIZE) || 
	   terDigGarbageMapTest(x + TRUST_MAP_GARBAGE_SIZE,y - TRUST_MAP_GARBAGE_SIZE) || 
	   terDigGarbageMapTest(x - TRUST_MAP_GARBAGE_SIZE,y + TRUST_MAP_GARBAGE_SIZE) || 
	   terDigGarbageMapTest(x + TRUST_MAP_GARBAGE_SIZE,y + TRUST_MAP_GARBAGE_SIZE))
		return 0;

	if(region->filled(Vect2i(x,y)) || 
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))))
		return 0;

//	if(!TrustGrid.ConditionScan(x, y, TRUST_MAP_GARBAGE_SIZE,&op))
//		return 0;

	return terDigGarbageCheckFull(x,y,region);
}

int terDigScanGarbageCheck(int x, int y, RegionMetaDispatcher* region, terPlayer* player)
{
	MetaRegionLock lock(region);
	terMapGridGarbageCheckOperator op(player);

	if(region->filled(Vect2i(x,y)) || 
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))))
		return 0;

	if(!player->TrustMap->TrustGrid.ConditionScan(x, y, TRUST_MAP_GARBAGE_SIZE, op))
		return 0;

	return terDigGarbageCheckFull(x,y,region);
}

const int TRUST_MAP_NOISE_SIDE = 128;
const int TRUST_MAP_NOISE_SIZE = TRUST_MAP_NOISE_SIDE * 2;

int terDigScanGarbage(int cx,int cy,int sx,int sy,int& x,int& y,class RegionMetaDispatcher* region,terPlayer* player)
{
	int i;
	int x0,x1,y0,y1;
	int rx,ry;

	x0 = cx - sx;
	y0 = cy - sy;
	x1 = cx + sx;
	y1 = cy + sy;

	if(x0 < 0) x0 = 0;
	else if(x0 >= vMap.H_SIZE) x0 = vMap.H_SIZE - 1;

	if(x1 < 0) x1 = 0;
	else if(x1 >= vMap.H_SIZE) x1 = vMap.H_SIZE - 1;

	if(y0 < 0) y0 = 0;
	else if(y0 >= vMap.V_SIZE) y0 = vMap.V_SIZE - 1;

	if(y1 < 0) y0 = 0;
	else if(y1 >= vMap.V_SIZE) y1 = vMap.V_SIZE - 1;

	for(i = x0;i <= x1;i += (4 << kmGrid)){
		rx = vMap.XCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(y0 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		if(!terDigGarbageMapTest(rx,ry) &&  
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) &&
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) &&
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   terDigScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}

		rx = vMap.XCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(y1 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		if(!terDigGarbageMapTest(rx,ry) &&  
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   terDigScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}
	}

	for(i = y0;i <= y1;i += (4 << kmGrid)){
		rx = vMap.XCYCL(x0 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));

		if(!terDigGarbageMapTest(rx,ry) &&  
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   terDigScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}

		rx = vMap.XCYCL(x1 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		if(!terDigGarbageMapTest(rx,ry) &&  
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terDigGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   terDigScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}
	}
	return 0;
}

//-----------------------------------Fill Garbage----------------------------------

int terFillGarbageCheckFull(int cx,int cy,class RegionMetaDispatcher* region)
{
	int i,j;

	int x0 = vMap.w2mClampX(cx - TRUST_MAP_GARBAGE_SIZE);
	int y0 = vMap.w2mClampY(cy - TRUST_MAP_GARBAGE_SIZE);
	int x1 = vMap.w2mClampX(cx + TRUST_MAP_GARBAGE_SIZE);
	int y1 = vMap.w2mClampY(cy + TRUST_MAP_GARBAGE_SIZE);

	for(i = y0;i <= y1;i++){
		for(j = x0;j <= x1;j++){
			if(GRIDTST_LEVELED(vMap.GABuf[vMap.offsetGBuf(j,i)]) || (vMap.GVBuf[vMap.offsetGBuf(j,i)] == 0/*> PERIMETER_GARBAGE_MAX_HEIGH*/))
				return 0;
			if(region->filled(vMap.m2w(Vect2i(j, i))))
				return 0;
		}
	}
	return 1;
}

inline int terFillGarbageMapTest(int x,int y)
{
//	return (GRIDTST_LEVELED((*(vMap.GABuf + (vMap.YCYCLG((y >> kmGrid)) << (vMap.V_SIZE_POWER - kmGrid)) + vMap.XCYCLG((x >> kmGrid))))) || (*(vMap.GVBuf + (vMap.YCYCLG(y >> kmGrid) << (vMap.V_SIZE_POWER - kmGrid)) + vMap.XCYCLG(x >> kmGrid)) == 0/*> PERIMETER_GARBAGE_MAX_HEIGH*/));
	int offset = vMap.offsetGBufWorldC(x,y);
	unsigned short p = vMap.GABuf[offset];
	return GRIDTST_LEVELED(p) || !IsNoHardness(p) || vMap.GVBuf[offset] == 0;
}

int terFillGarbageTest(int x,int y,class RegionMetaDispatcher* region,terPlayer* player)
{
//	terMapGridGarbageCheckOperator op(player);
	MetaRegionLock lock(region);
	if(terFillGarbageMapTest(x,y) || 
	   terFillGarbageMapTest(x - TRUST_MAP_GARBAGE_SIZE,y - TRUST_MAP_GARBAGE_SIZE) || 
	   terFillGarbageMapTest(x + TRUST_MAP_GARBAGE_SIZE,y - TRUST_MAP_GARBAGE_SIZE) || 
	   terFillGarbageMapTest(x - TRUST_MAP_GARBAGE_SIZE,y + TRUST_MAP_GARBAGE_SIZE) || 
	   terFillGarbageMapTest(x + TRUST_MAP_GARBAGE_SIZE,y + TRUST_MAP_GARBAGE_SIZE))
		return 0;

	if(region->filled(Vect2i(x,y)) || 
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))))
		return 0;

//	if(!TrustGrid.ConditionScan(x, y, TRUST_MAP_GARBAGE_SIZE, &op))
//		return 0;

	return terFillGarbageCheckFull(x,y,region);
}

int terFillScanGarbageCheck(int x,int y, RegionMetaDispatcher* region, terPlayer* player)
{
	terMapGridGarbageCheckOperator op(player);

	if(region->filled(Vect2i(x,y)) || 
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y - TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))) ||
	   region->filled(Vect2i(vMap.XCYCL(x - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(y + TRUST_MAP_GARBAGE_SIZE))))
		return 0;

	if(!player->TrustMap->TrustGrid.ConditionScan(x, y, TRUST_MAP_GARBAGE_SIZE, op))
		return 0;

	return terFillGarbageCheckFull(x,y,region);
}

int terFillScanGarbage(int cx,int cy,int sx,int sy,int& x,int& y,class RegionMetaDispatcher* region,terPlayer* player)
{
	int i;
	int x0,x1,y0,y1;
	int rx,ry;

	x0 = cx - sx;
	y0 = cy - sy;
	x1 = cx + sx;
	y1 = cy + sy;

	if(x0 < 0) x0 = 0;
	else if(x0 >= vMap.H_SIZE) x0 = vMap.H_SIZE - 1;

	if(x1 < 0) x1 = 0;
	else if(x1 >= vMap.H_SIZE) x1 = vMap.H_SIZE - 1;

	if(y0 < 0) y0 = 0;
	else if(y0 >= vMap.V_SIZE) y0 = vMap.V_SIZE - 1;

	if(y1 < 0) y0 = 0;
	else if(y1 >= vMap.V_SIZE) y1 = vMap.V_SIZE - 1;

	for(i = x0;i <= x1;i += (4 << kmGrid)){
		rx = vMap.XCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(y0 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		if(!terFillGarbageMapTest(rx,ry) &&  
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) &&
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) &&
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   terFillScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}

		rx = vMap.XCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(y1 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		if(!terFillGarbageMapTest(rx,ry) &&  
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   terFillScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}
	}

	for(i = y0;i <= y1;i += (4 << kmGrid)){
		rx = vMap.XCYCL(x0 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));

		if(!terFillGarbageMapTest(rx,ry) &&  
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   terFillScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}

		rx = vMap.XCYCL(x1 - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		ry = vMap.YCYCL(i - TRUST_MAP_NOISE_SIDE + terLogicRND(TRUST_MAP_NOISE_SIZE));
		if(!terFillGarbageMapTest(rx,ry) &&  
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx - TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry - TRUST_MAP_GARBAGE_SIZE)) && 
		   !terFillGarbageMapTest(vMap.XCYCL(rx + TRUST_MAP_GARBAGE_SIZE),vMap.YCYCL(ry + TRUST_MAP_GARBAGE_SIZE)) &&
		   terFillScanGarbageCheck(rx,ry,region,player)){
			x = rx;
			y = ry;
			return 1;
		}
	}
	return 0;
}

//---------------------------

void terTerraformLevelFull::Show()
{
	sColor4c c(128,128,128);

	if(Status & TERRAFORM_STATUS_DIG)
		c.r += 127;
	if(Status & TERRAFORM_STATUS_FILL)
		c.g += 127;

	ShowRect(c,false,false);
}


bool terTerraformLevelFull::IsOkDig(int x,int y)
{
	int offset=vMap.offsetGBuf(x, y);
	unsigned short p = vMap.GABuf[offset];
	if(p&GRIDAT_ZEROPLAST_IS_PRESENT)
		return false;
	return vMap.GVBuf[offset] > Height && IsNoHardness(p);
}

void terTerraformLevelFull::AddFillDig(int x,int y,int& dig_layer,int& fill_layer)
{
	int offset=vMap.offsetGBuf(x, y);
	unsigned short p = vMap.GABuf[offset];
	unsigned char a = vMap.GVBuf[offset];
	if(IsNoHardness(p) && !(p&GRIDAT_ZEROPLAST_IS_PRESENT))
	{
		if(a > Height)
			dig_layer++;
	}
	if(a < Height)
		fill_layer++;
}

int terTerraformLevelFull::GetDigPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	int lx = -1;
	int ly = -1;

	x=(x0+x1)/2;
	y=(y0+y1)/2;
	if(IsOkDig(x,y)){
		lx = vMap.m2wHalf(x);
		ly = vMap.m2wHalf(y);
		if(lx != cx || ly != cy){
			cx = lx;
			cy = ly;
			return 1;
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			if(IsOkDig(x,y)){
				lx = vMap.m2wHalf(x);
				ly = vMap.m2wHalf(y);
				if(lx != cx || ly != cy){
					cx = lx;
					cy = ly;
					return 1;
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformLevelFull::GetFillPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	int lx = -1;
	int ly = -1;
	x=(x0+x1)/2;
	y=(y0+y1)/2;
	if(vMap.GVBuf[vMap.offsetGBuf(x,y)] < Height){
		lx = vMap.m2wHalf(x);
		ly = vMap.m2wHalf(y);
		if(lx != cx || ly != cy){
			cx = lx;
			cy = ly;
			return 1;
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			if(vMap.GVBuf[vMap.offsetGBuf(x,y)] < Height){
				lx = vMap.m2wHalf(x);
				ly = vMap.m2wHalf(y);
				if(lx != cx || ly != cy){
					cx = lx;
					cy = ly;
					return 1;
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformLevelFull::CheckDigPosition(int cx,int cy)
{
	return IsOkDig(cx,cy);
}

int terTerraformLevelFull::CheckFillPosition(int cx,int cy)
{
	if(vMap.GetApproxAlt(vMap.XCYCL(cx),vMap.YCYCL(cy)) < Height)
		return 1;
	return 0;
}

int terTerraformLevelFull::GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present)
{
	int i,j;
	int cnt;

	prepared_zero = 0;
	resource_zero = 0;
	dig_layer = 0;
	fill_layer = 0;
	ground_zero = 0;
	cell = 0;
	zero_present = 0;

	int x0 = vMap.w2mClampX(cx - sx);
	int y0 = vMap.w2mClampY(cy - sy);
	int x1 = vMap.w2mClampX(cx + sx);
	int y1 = vMap.w2mClampY(cy + sy);
	
	cnt = 0;
	for(i = y0;i < y1;i++){
		for(j = x0;j < x1;j++){
			cell++;
			AddFillDig(j,i,dig_layer,fill_layer);
		}
	}
	return cnt;
}

//-------------------------------------------------------------

int terTerraformLevelBorder::GetDigPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	int lx = -1;
	int ly = -1;
	x=(x0+x1)/2;
	y=(y0+y1)/2;
	if(RegionPoint->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)),vMap.YCYCL(vMap.m2wHalf(y))))){
		if(IsOkDig(x,y)){
			lx = vMap.m2wHalf(x);
			ly = vMap.m2wHalf(y);
			if(lx != cx || ly != cy){
				cx = lx;
				cy = ly;
				return 1;
			}
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			if(RegionPoint->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)),vMap.YCYCL(vMap.m2wHalf(y))))){
				if(IsOkDig(x,y)){
					lx = vMap.m2wHalf(x);
					ly = vMap.m2wHalf(y);
					if(lx != cx || ly != cy){
						cx = lx;
						cy = ly;
						return 1;
					}
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformLevelBorder::GetFillPosition(int& cx,int& cy)
{
	int y,x;

	int x0 = vMap.w2mClampX(PositionX - TERRAFORM_ELEMENT_SIDE);
	int y0 = vMap.w2mClampY(PositionY - TERRAFORM_ELEMENT_SIDE);
	int x1 = vMap.w2mClampX(PositionX + TERRAFORM_ELEMENT_SIDE);
	int y1 = vMap.w2mClampY(PositionY + TERRAFORM_ELEMENT_SIDE);
	
	int lx = -1;
	int ly = -1;
	x=(x0+x1)/2;
	y=(y0+y1)/2;
	if(RegionPoint->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)),vMap.YCYCL(vMap.m2wHalf(y))))){
		if(vMap.GVBuf[vMap.offsetGBuf(x,y)] < Height){
			lx = vMap.m2wHalf(x);
			ly = vMap.m2wHalf(y);
			if(lx != cx || ly != cy){
				cx = lx;
				cy = ly;
				return 1;
			}
		}
	}

	for(y = y0;y <= y1;y++){
		for(x = x0;x <= x1;x++){
			if(RegionPoint->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(x)),vMap.YCYCL(vMap.m2wHalf(y))))){
				if(vMap.GVBuf[vMap.offsetGBuf(x,y)] < Height){
					lx = vMap.m2wHalf(x);
					ly = vMap.m2wHalf(y);
					if(lx != cx || ly != cy){
						cx = lx;
						cy = ly;
						return 1;
					}
				}
			}
		}
	}

	if(lx != -1 && ly != -1){
		cx = lx;
		cy = ly;
		return 1;
	}
	return 0;
}

int terTerraformLevelBorder::CheckDigPosition(int cx,int cy)
{
	if(RegionPoint->filledRasterized(Vect2i(cx,cy))){
		if(IsOkDig(cx,cy))
			return 1;
	}
	return 0;
}

int terTerraformLevelBorder::CheckFillPosition(int cx,int cy)
{
	if(RegionPoint->filledRasterized(Vect2i(cx,cy))){
		if(vMap.GetApproxAlt(vMap.XCYCL(cx),vMap.YCYCL(cy)) < Height)
			return 1;
	}
	return 0;
}

int terTerraformLevelBorder::GetClusterAttribute(int cx,int cy,int sx,int sy,int& prepared_zero,int& resource_zero,int& dig_layer,int& fill_layer,int& ground_zero,int& cell,int& zero_present)
{
	int y,j;
	int cnt;

	prepared_zero = 0;
	resource_zero = 0;
	dig_layer = 0;
	fill_layer = 0;
	ground_zero = 0;
	cell = 0;
	zero_present = 0;

	int x0 = vMap.w2mClampX(cx - sx);
	int y0 = vMap.w2mClampY(cy - sy);
	int x1 = vMap.w2mClampX(cx + sx);
	int y1 = vMap.w2mClampY(cy + sy);
	
	cnt = 0;
	for(y = y0;y < y1;y++){
		for(j = x0;j < x1;j++){
			if(RegionPoint->filledRasterized(Vect2i(vMap.XCYCL(vMap.m2wHalf(j)),vMap.YCYCL(vMap.m2wHalf(y))))){
				cell++;
				AddFillDig(j,y,dig_layer,fill_layer);
			}
		}
	}
	return cnt;
}

terTerraformAbyssFull::terTerraformAbyssFull(int id,int x,int y,terPlayer* player)
: terTerraformLevelFull(id,x,y,player)
{
	Type = TERRAFORM_TYPE_ABYSS_FULL; 
	Status = TERRAFORM_STATUS_DIG;
	Height = 0;
}

terTerraformAbyssBorder::terTerraformAbyssBorder(int id,int x,int y,terPlayer* player)
: terTerraformLevelBorder(id,x,y,player)
{
	Type = TERRAFORM_TYPE_ABYSS_BORDER; 
	Height = 0;
	Status = TERRAFORM_STATUS_DIG;
	RegionPoint = player->AbyssRegionPoint;
}

void terTerraformDispatcher::GetWorkAreaStats(int& zero_request,int& zero_complete,int& abyss_request,int& abyss_complete)
{
#ifdef _DEBUG
	zero_request = 0;
	zero_complete = 0;
	abyss_request = 0;
	abyss_complete = 0;

	for(int i=0;i<2;i++)
	{
		TerraformList& Terraforms=i?TerraformsDigFill:TerraformsOther;
		TerraformList::iterator ti;
		FOR_EACH(Terraforms, ti)
		{
			xassert((*ti)->Alive());
			switch((*ti)->Type)
			{
			case TERRAFORM_TYPE_FULL:	
				zero_request += 2;
				if((*ti)->Status & TERRAFORM_STATUS_PREPARED)
					zero_complete += 2;
				break;
			case TERRAFORM_TYPE_BORDER:	
				zero_request++;
				if((*ti)->Status & TERRAFORM_STATUS_PREPARED)
					zero_complete++;
				break;

			case TERRAFORM_TYPE_ABYSS_FULL:
				abyss_request += 2;
				if(!((*ti)->Status & TERRAFORM_STATUS_DIG))
					abyss_complete += 2;
				break;
			case TERRAFORM_TYPE_ABYSS_BORDER:
				abyss_request++;
				if(!((*ti)->Status & TERRAFORM_STATUS_DIG))
					abyss_complete++;
				break;
			}
		}
	}

	xassert(this->zero_request==zero_request);
	xassert(this->abyss_request==abyss_request);
	xassert(this->zero_complete==zero_complete);
	xassert(this->abyss_complete==abyss_complete);

	zero_request /= 2;
	zero_complete /= 2;

	abyss_request /= 2;
	abyss_complete /= 2;
#else

	zero_request = this->zero_request/stats_full;
	zero_complete = this->zero_complete/stats_full;

	abyss_request = this->abyss_request/stats_full;
	abyss_complete = this->abyss_complete/stats_full;
#endif 
}
