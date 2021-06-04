#include "StdAfx.h"

#include "umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"
#include "PrmEdit.h"
#include "Terra.h"
#include "Region.h"
#include "CameraManager.h"
#include "Runtime.h"
#include "GameShell.h"
#include "GenericControls.h"
#include "Universe.h"
#include "Config.h"

#include "IUnkObj.h"
#include "RigidBody.h"
#include "ScanPoly.h"
#include "UniverseInterface.h"

terBuildingInstaller::terBuildingInstaller(bool a_light_show)
{
	Attribute = 0;
	ObjectPoint = 0;
	BaseBuff = 0;
	BaseBuffSX = BaseBuffSY = 0;
	pTexture = 0;
	plane = 0;

	light_show = a_light_show;
	connection_icon_ = new terIconBuilding(terModelBuildingNoConnection);
}

terBuildingInstaller::~terBuildingInstaller()
{
	Clear();
	delete connection_icon_;
}

void terBuildingInstaller::Clear()
{
	CancelObject();

	if(BaseBuff)
		delete BaseBuff;
	BaseBuff = 0;
	BaseBuffSX = BaseBuffSY = 0;

	if(plane)
		plane->Release();
	plane=0;

	RELEASE(pTexture);
}

void terBuildingInstaller::InitObject(const AttributeBase* attr)
{
	xassert(attr); 

	Clear();
	
	Attribute = attr; 
	valid_ = 0;
	buildingInArea_ = 0;

	ObjectPoint = createObject(attr->modelData.modelName, attr->belligerent);
	ObjectPoint->SetScale(Vect3f::ID*attr->modelScale);

	Angle = 0;
	Position = Vect3f::ZERO;

	ObjectPoint->SetAttr(ATTRUNKOBJ_IGNORE);

	pos_set = Vect2f::ZERO;
	angle_set = 0;

	OffsetX = OffsetY = 0;
	visible_ = 0;
	ObjectPoint->SetChannel(attr->UpgradeChainName,0);

	if(attr->MilitaryUnit && attr->isBuilding() && attr->ShowCircles){
		FireRadius = attr->fireRadius();
		UmbrellaRadius = attr->fireRadiusMin();
	}
}

void terBuildingInstaller::ConstructObject(terPlayer* player)
{
	if(player && valid() && Attribute->ID != UNIT_ATTRIBUTE_FRAME){
		if(player->frame())
			player->frame()->commandOutcoming(UnitCommand(COMMAND_ID_BUILDING_START, Vect3f(Position.x,Position.y,Angle), Attribute->ID, COMMAND_SELECTED_MODE_NEGATIVE));
		Clear();
	}
}

void terBuildingInstaller::CancelObject()
{
	Attribute = 0;

	if(ObjectPoint){
		ObjectPoint->Release();
		ObjectPoint = 0;
	}

	valid_ = 0;
	visible_ = 0;

	connection_icon_->quant();
}

class terScanGroundLineBuffOp
{
	int cnt, max;
	int x0_, y0_, sx_, sy_;
	char* buffer_;
	bool building_;

public:
	terScanGroundLineBuffOp(int x0,int y0,int sx,int sy, char* buffer)
	{
		x0_ = x0;
		y0_ = y0;
		sx_ = sx;
		sy_ = sy;
		buffer_ = buffer;
		cnt = max = 0;
		building_ = false;
	}
	void operator()(int x1,int x2,int y)
	{
		xassert((x1 - x0_) >= 0 && (x2 - x0_) < sx_ && (y - y0_) >= 0 && (y - y0_) < sy_);

		max += x2 - x1 + 1;
		unsigned short* buf = vMap.GABuf + vMap.offsetGBufWorldC(0, y);
		char* pd = buffer_ + (y - y0_)*sx_ + x1 - x0_;
		while(x1 <= x2){
			unsigned short p = *(buf + vMap.XCYCLG(x1 >> kmGrid));
			if((p & GRIDAT_LEVELED) && !(p & (GRIDAT_MASK_CLUSTERID | GRIDAT_BUILDING | GRIDAT_BASE_OF_BUILDING_CORRUPT))){
				cnt++;
				(*pd) |= 2;
			}
			else if(p & GRIDAT_BUILDING)
				building_ = true;

			(*pd) |= 1;
			x1++;
			pd++;
		}
	}
	bool valid() 
	{ 
		xassert(cnt <= max && "Bad Max");
		return cnt == max; 
	}
	bool building() const { return building_; }
};

void terBuildingInstaller::SetBuildPosition(const Vect3f& position,float angle, terPlayer* player)
{
	valid_ = 1;
	visible_ = 0;
	old_build_position=position;
	old_build_angle=angle;
	old_build_player=player;
	if(ObjectPoint){
		visible_ = 1;

		Position = to3D(position, vMap.hZeroPlast - Attribute->logicObjectBound.min.z);
		Angle = angle;

		MatX2f mx2(Mat2f(Angle), Position);
		vector<Vect2i> points(Attribute->BasementPoints.size());
		int x0 = INT_INF, y0 = INT_INF; 
		int x1 = -INT_INF, y1 = -INT_INF;
		for(int i = 0; i < Attribute->BasementPoints.size(); i++){
			Vect2i& v = points[i];
			v = mx2*Attribute->BasementPoints[i];
			if(x0 > v.x)
				x0 = v.x;
			if(y0 > v.y)
				y0 = v.y;
			if(x1 < v.x)
				x1 = v.x;
			if(y1 < v.y)
				y1 = v.y;
		}
		OffsetX = x0;
		OffsetY = y0;

		x0 = x0 - 1;
		x1 = x1 + 1;
		y0 = y0 - 1;
		y1 = y1 + 1;

		if(!BaseBuff || BaseBuffSX < (x1 - x0) || BaseBuffSY < (y1 - y0)){
			if(BaseBuff)
				delete BaseBuff;
			BaseBuffSX = x1 - x0;
			BaseBuffSY = y1 - y0;
			BaseBuff = new char[BaseBuffSX * BaseBuffSY];
			InitTexture();
		}
		memset(BaseBuff,0,BaseBuffSX * BaseBuffSY);

		terScanGroundLineBuffOp line_op(OffsetX, OffsetY, BaseBuffSX, BaseBuffSY, BaseBuff);
		scanPolyByLineOp(&points[0], points.size(), line_op);
		valid_ = line_op.valid() && checkScriptInstructions();
		buildingInArea_ = line_op.building();
		
		connection_icon_->quant();

		if(Attribute->ID != UNIT_ATTRIBUTE_FRAME && player){
			bool connected = false;
			if(!Attribute->ConnectionRadius){
				MTAuto lock(universe()->EnergyRegionLocker());
				GenShapeLineOp op;
				scanPolyByLineOp(&points[0], points.size(), op);
				connected = player->energyColumn().intersected(op.shape());
				//if(!connected)
				//	valid_ = 0;
			}
			else{
				CUNITS_LOCK(player);
				const UnitList& unit_list=player->units();
				UnitList::const_iterator ui;
				FOR_EACH(unit_list, ui)
					if((*ui)->attr().ConnectionRadius && ((*ui)->isBuildingEnable() || (*ui)->attr().ID == UNIT_ATTRIBUTE_FRAME) &&
					  (*ui)->position2D().distance2(position) < sqr((*ui)->attr().ConnectionRadius))
						connected = true;
			}

			if(!connected){
				valid_ = 0;
				Vect3f pos = position;
				pos.z += Attribute->boundRadius*Attribute->iconDistanceFactor;
				connection_icon_->show(pos);
			}
		}
	}
	else
		valid_ = 0;
}

void terBuildingInstaller::InitTexture()
{
	RELEASE(pTexture);

	int dx = 1 << (BitSR(BaseBuffSX) + 1);
	int dy = 1 << (BitSR(BaseBuffSY) + 1);
	dx = dy = max(dx,dy);
	pTexture = terVisGeneric->CreateTexture(dx,dy,true);
	if(pTexture == 0)return;

	int Pitch;
	BYTE* buf = (BYTE*)pTexture->LockTexture(Pitch);
	for(int y=0;y<dy;y++)
	{
		DWORD* c = (DWORD*)(buf+y*Pitch);
		for(int x = 0; x < dx; x++,c++)
			*c = 0;
	}
	pTexture->UnlockTexture();
}

void terBuildingInstaller::SetBuildPosition(const Vect2f& mousePos, terPlayer* player)
{
	valid_ = 1;
	visible_ = 0;
	if(ObjectPoint){
		Vect3f v;
		Vect3f pos,dir;
		terCamera->GetCamera()->GetWorldRay(pos_set = mousePos, pos, dir);
		if(dir.z < 0){
			v = pos + dir*((pos.z - vMap.hZeroPlast)/-dir.z);
			float radius = Attribute->boundRadius;
			v.x = clamp(v.x, radius, vMap.H_SIZE - radius);
			v.y = clamp(v.y, radius, vMap.V_SIZE - radius);
			visible_ = 1;

			SetBuildPosition(v, round(cycle(angle_set, 2*M_PI)/(M_PI/4))*(M_PI/4), player);

			ObjectPoint->ClearAttr(ATTRUNKOBJ_IGNORE);

			ObjectPoint->SetPosition(Se3f(QuatF(Angle, Vect3f::K), Position));
			if(valid())
				ObjectPoint->SetColor(0,&sColor4f(0,1.0f,0,0.5f),&sColor4f(0,1.0f,0,0.5f));
			else
				ObjectPoint->SetColor(0,&sColor4f(1.0f,0,0,0.5f),&sColor4f(1.0f,0,0,0.5f));
			return;
		}
		ObjectPoint->SetAttr(ATTRUNKOBJ_IGNORE);
	}
	valid_ = 0;
}

void terBuildingInstaller::ChangeBuildAngle(float dA, terPlayer* player)
{
	angle_set += dA;
	SetBuildPosition(pos_set, player);
}

void terBuildingInstaller::ShowCircle()
{
	if(ObjectPoint){
		if(Attribute->ZeroLayerRadius)
			terCircleShowGraph(Position, Attribute->ZeroLayerRadius, circleColors.zeroLayerRadius);
		if(Attribute->ConnectionRadius){
			universe()->activePlayer()->showConnectionCircle(Position, Attribute->ID == UNIT_ATTRIBUTE_CORE);
			if(Attribute->ID == UNIT_ATTRIBUTE_RELAY)
				terCircleShowGraph(Position, Attribute->ConnectionRadius, circleColors.connectionRadiusSelected);
		}
		if(Attribute->MilitaryUnit && Attribute->ShowCircles){
			universe()->activePlayer()->showFireCircles(Attribute->ID, Position);
			terCircleShowGraph(Position, Attribute->fireRadius(), circleColors.fireRadius);
			if(Attribute->fireRadiusMin())
				terCircleShowGraph(Position, Attribute->fireRadiusMin(), circleColors.fireRadiusMin);
		}
	}
}

void terBuildingInstaller::UpdateInfo(cCamera *DrawNode)
{
	if(plane)
		plane->SetAttr(ATTRUNKOBJ_IGNORE);

	if(!(ObjectPoint && visible_ && pTexture))
		return;

	SetBuildPosition(old_build_position,old_build_angle,old_build_player);

	if(plane==0)
		plane=terScene->CreatePlaneObj();

	if(light_show && DrawNode && DrawNode->FindCildCamera(ATTRCAMERA_SHADOW) &&
		gb_VisGeneric->GetShadowType()!=SHADOW_MAP_SELF)
	{
		plane->SetAttr(ATTRUNKOBJ_SHADOW);
		plane->SetAttr(ATTRUNKOBJ_IGNORE_NORMALCAMERA);
	}
	else{
		plane->ClearAttr(ATTRUNKOBJ_SHADOW);
		plane->ClearAttr(ATTRUNKOBJ_IGNORE_NORMALCAMERA);
	}

	int Pitch;
	BYTE* buf = (BYTE*)pTexture->LockTexture(Pitch);

	sColor4c cempty(0,0,0,0);
	sColor4c cgood = valid() ? sColor4c(0,255,0,128) : sColor4c(200,128,128,128);
	sColor4c cbad(255,0,0,128);
	char* p = BaseBuff;
	for(int i = 0;i < BaseBuffSY;i++)
	{
		DWORD* c = (DWORD*)(buf+i*Pitch);
		for(int j = 0;j < BaseBuffSX;j++)
		{
			if((*p) & 1){
				if((*p) & 2)
					*c = cgood.RGBA();
				else
					*c = cbad.RGBA();
			}else
				*c = cempty.RGBA();
			p++;
			c++;
		}
	}

	pTexture->UnlockTexture();

	if(plane)
	{
		plane->ClearAttr(ATTRUNKOBJ_IGNORE);
		pTexture->IncRef();
		plane->SetTexture(0,pTexture);
		plane->SetScale(Vect3f(BaseBuffSX,BaseBuffSY,-10));
		MatXf mat = MatXf::ID;
		mat.trans() = Vect3f(OffsetX, OffsetY, vMap.hZeroPlast+2);
		plane->SetPosition(mat);
		float du = BaseBuffSX/(float)pTexture->GetWidth();
		float dv = BaseBuffSY/(float)pTexture->GetHeight();
		plane->SetUV(0,0,du,dv);
	}
}

bool terBuildingInstaller::checkScriptInstructions()
{
	bool enable = true;
	vector<SaveBuildingInstallerInstruction>::const_iterator ii;
	FOR_EACH(gameShell->manualData().buildingInstallerInstructions, ii)
		if(ii->building == Attribute->ID){
			enable = false;
			const SaveBuildingInstallerInstruction& instruction = *ii;
			terUnitBase* unit = universe()->findUnitByLabel(instruction.label);
			if(!unit){
				xassert_s(0 && "Объект по метке не найден: ", instruction.label);
			}												
			else{
				if((unit->activity() && instruction.labeledObjectActivity || !unit->activity() && !instruction.labeledObjectActivity) &&
					unit->position2D().distance2(Position) < sqr(instruction.distance))
					return true;
			}
		}
	return enable;
}

