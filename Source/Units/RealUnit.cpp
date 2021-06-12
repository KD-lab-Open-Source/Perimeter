#include "StdAfx.h"

#include "Runtime.h"
#include "Universe.h"
#include "Player.h"
#include "RealUnit.h"
#include "SecondGun.h"
#include "GridTools.h"

#include "Config.h"
#include "ForceField.h"
#include "AITileMap.h"
#include "ScanPoly.h"
#include "GameShell.h"
#include "CameraManager.h"
#include "../HT/ht.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

REGISTER_CLASS(AttributeBase, AttributeReal, "AttributeReal");

int terUnitReal::zeroLayerCounterCurrent_;

AttributeReal::AttributeReal() 
{
	SelectAble = true;

	FallDelay = 20;
	FallSpeed = -2;
	FallAcceleration = 0;
}

terUnitReal::terUnitReal(const UnitTemplate& data) : terUnitGeneric(data),
	weapon_(NULL),
	ownerUnit_(NULL),
	toolzerController_(data.attribute()->toolzerSetup)
{
	BodyPoint = new RigidBody;
	
	Scale = 1.0f;

	SightFactor = 1.0f;

	HotCount = 0;
	FreezeFactor = 0;
	HealFactor = 0;
	HotFactor = 0;

	targetUnit_ = 0;

	dockMode_ = DOCK_MODE_NONE;
	DockPoint = 0;
	DockSlot = 0;
	DockPhase.startPhase(1.0f,0,0);
	DockAnglePhase.startPhase(1.0f,0,0);
	AngleDockingEnable = false;

	basementInstalled_ = false;

	pathFindSucceeded_ = false;
	pathFindTarget_ = Vect2i::ZERO;

	LogicObjectPoint = 0;

	if(attr().weaponSetup.enabled())
		weapon_ = terWeapon::create(attr().weaponSetup.weaponType,this);

	accumulatedEnergy_ = 0;
	checkFieldCluster_ = 0;

	zeroLayerCounter_ = 0;
}

terUnitReal::~terUnitReal()
{
	if(LogicObjectPoint){
		LogicObjectPoint->Release();
		LogicObjectPoint = 0;
	}

	terWeapon::release(weapon_);

	xassert(!basementInstalled_);

	delete BodyPoint;
}

void terUnitReal::setRealModel(int modelIndex, float scale)
{
	if(modelIndex == -1)
		modelIndex = terLogicRND(1 + attr().additionalModelsData.size());
	else if(modelIndex >= attr().additionalModelsData.size())
		modelIndex = 0;

	const ModelData& modelData = !modelIndex ? attr().modelData : attr().additionalModelsData[modelIndex - 1];
	sBox6f LogicObjectBound;
	if(!modelIndex){ 
		Scale = attr().modelScale*scale;
		LogicObjectBound = attr().logicObjectBound; 
		setRadius(attr().boundRadius);
	}
	else{
		GeometryAttribute geometryAttribute;
		geometryAttribute.initGeometryAttribute(modelData, attr());
		Scale = geometryAttribute.modelScale*scale;
		LogicObjectBound = geometryAttribute.logicObjectBound; 
		setRadius(geometryAttribute.boundRadius);
	}
	LogicObjectBound.min *= scale;
	LogicObjectBound.max *= scale;
	
	if(LogicObjectPoint){
		LogicObjectPoint->Release();
		LogicObjectPoint = 0;
	}

	if(modelData.logicName){
		LogicObjectPoint = terLogicGeneric->GetElement(modelData.logicName);
		LogicObjectPoint->SetPosition(MatXf(Mat3f::ID,Vect3f::ZERO));
		LogicObjectPoint->SetScale(Vect3f(1,1,1));
		//LogicObjectPoint->SetChannel("main",0);
		LogicObjectPoint->SetScale(Vect3f(Scale,Scale,Scale));
		
		DockList.clear();
		if(!attr().dockingSlots.empty()){ 
			DockList.reserve(attr().dockingSlots.size());
			for(int i = 0; i < attr().dockingSlots.size(); i++){
				DockList.push_back(terDockingSlot());
				DockList.back().init(LogicObjectPoint, attr().dockingSlots[i]);
			}
		}
	}

	avatar()->setLogicObject(LogicObjectPoint);
	avatar()->SetModel(modelData.modelName, Scale);
	BodyPoint->build(*attr().rigidBodyPrm, avatar()->GetModelPoint(), LogicObjectBound.min, LogicObjectBound.max);
	if(attr().forwardVelocity)
		BodyPoint->setForwardVelocity(attr().forwardVelocity);
	if(attr().flyingHeight)
		BodyPoint->setFlyingHeight(attr().flyingHeight);

	if(weapon_){
		if(isConstructed()){
			weapon_->init();
			weapon_->enable();
		}
		else
			weapon_->disable();
	}
}

class terScanBasementReadyLineOp
{
	int cnt, max;

public:
	terScanBasementReadyLineOp(){ cnt = max = 0; }
	void operator()(int x1,int x2,int y)
	{
		max += x2 - x1 + 1;
		unsigned short* buf = vMap.GABuf + vMap.offsetGBufC(0,y);
		while(x1 <= x2){
			unsigned short p = *(buf + vMap.XCYCLG(x1));
			if((p & GRIDAT_LEVELED) && !(p & (GRIDAT_BASE_OF_BUILDING_CORRUPT | GRIDAT_BUILDING)))
				cnt++;
			x1++;
		}
	}
	bool check() const { return cnt == max; }
};

class terScanBasementDamageLineOp
{
	int cnt, max;

public:
	terScanBasementDamageLineOp(){ cnt = max = 0; }
	void operator()(int x1,int x2,int y)
	{
		max += x2 - x1 + 1;
		unsigned short* buf = vMap.GABuf + vMap.offsetGBufC(0,y);
		while(x1 <= x2){
			unsigned short p = *(buf + vMap.XCYCLG(x1));
			if(!(p & GRIDAT_BASE_OF_BUILDING_CORRUPT))
				cnt++;
			x1++;
		}
	}
	float ratio() const { return max ? (float)cnt/(float)max : 0; }
};


bool terUnitReal::basementReady()
{
	if(attr().InstallBound){
		MatX2f mx2(Mat2f(BodyPoint->angleZ()), BodyPoint->position());
		vector<Vect2i> points(attr().BasementPoints.size());
		for(int i = 0; i < points.size(); i++){
			Vect2i& v = points[i];
			v = mx2*attr().BasementPoints[i];
			v >>= kmGrid;
		}

		terScanBasementReadyLineOp line_op;
		scanPolyByLineOp(&points[0], points.size(), line_op);
		return line_op.check();
	}

	return false;
}

float terUnitReal::basementDamage()
{
	if(basementInstalled()){
		MatX2f mx2(Mat2f(BodyPoint->angleZ()), BodyPoint->position());
		vector<Vect2i> points(attr().BasementPoints.size());
		for(int i = 0; i < points.size(); i++){
			Vect2i& v = points[i];
			v = mx2*attr().BasementPoints[i];
			v >>= kmGrid;
		}
		
		terScanBasementDamageLineOp line_op;
		scanPolyByLineOp(&points[0], points.size(), line_op);
		return line_op.ratio();
	}
	
	return 1;
}


class terScanInstallLineOp
{
public:
	void operator()(int x1,int x2,int y)
	{
		unsigned short* buf = vMap.GABuf + vMap.offsetGBufC(0, y);
		while(x1 <= x2)
			*(buf + vMap.XCYCLG(x1++)) |= GRIDAT_BUILDING;
	}
};

void terUnitReal::installBasement()
{
	if(!basementInstalled_ && attr().InstallBound){
		basementInstalled_ = true;
		MatX2f mx2(Mat2f(BodyPoint->angleZ()), BodyPoint->position());
		vector<Vect2i> points(attr().BasementPoints.size());
		for(int i = 0; i < points.size(); i++){
			Vect2i& v = points[i];
			v = mx2*attr().BasementPoints[i];
			v >>= kmGrid;
		}
        terScanInstallLineOp op = terScanInstallLineOp();
		scanPolyByLineOp(&points[0], points.size(), op);
	}
}


class terScanUninstallLineOp
{
public:
	void operator()(int x1,int x2,int y)
	{
		unsigned short* buf = vMap.GABuf + vMap.offsetGBufC(0,y);
		while(x1 <= x2)
			*(buf + vMap.XCYCLG(x1++)) &= ~GRIDAT_BUILDING;
	}
};

void terUnitReal::uninstallBasement()
{
	if(basementInstalled_ && attr().InstallBound){
		basementInstalled_ = false;
		MatX2f mx2(Mat2f(BodyPoint->angleZ()), BodyPoint->position());
		vector<Vect2i> points(attr().BasementPoints.size());
		for(int i = 0; i < points.size(); i++){
			Vect2i& v = points[i];
			v = mx2*attr().BasementPoints[i];
			v >>= kmGrid;
		}
        terScanUninstallLineOp op = terScanUninstallLineOp();
		scanPolyByLineOp(&points[0], points.size(), op);
	}
}

void terUnitReal::Start()
{
	terUnitGeneric::Start();

	WayPointStart();

	AvatarQuant();
	avatar()->Start();

	for(int i = 0; i < attr().effectsData.effects.size(); i++){
		terEffectID id=attr().effectsData.effects[i].effectID;
		switch(id){
		case EFFECT_ID_UNIT_SMOKE:
		case EFFECT_ID_UNIT_MOVE:
			if(EffectKey* key = attr().getEffect(attr().effectsData.effects[i].effectName)){
				cEffect* effect = terScene->CreateScaledEffect(*key,avatar()->GetModelPoint());
				effect->SetPosition(avatar()->matrix());
				if(id==EFFECT_ID_UNIT_SMOKE)
					effect->SetParticleRate(0);
				effectControllers_.push_back(terEffectController(&attr().effectsData.effects[i],effect));
			}
			break;
		case EFFECT_ID_LASER_HIT:
			if(EffectKey* key = attr().getEffect(attr().effectsData.effects[i].effectID))
				effectControllers_.push_back(terEffectController(&attr().effectsData.effects[i],NULL));
            break;
        default:
            break;
		}
	}

	WayPointController();
}

void terUnitReal::MoveQuant()
{
	terUnitGeneric::MoveQuant();

	BodyPoint->evolve(terGlobalTimePeriodSeconds);

	setPose(BodyPoint->pose(), false);
	//avatar()->setPose(BodyPoint->poseSe3f());

	WayPointController();

	weaponQuant();
}

void terUnitReal::Quant()
{
	terUnitGeneric::Quant();

	if(dockMode() != DOCK_MODE_LOCK){
		if(!Player->active() && universe()->activePlayer()->clan() != Player->clan()){
			if(!isUnseen() && (!includingCluster() || universe()->fieldTransparent())){
				average(SightFactor, 1.0f, 0.3f);
				DockingSlotList::iterator i_dock;
				FOR_EACH(DockList,i_dock){
					if(i_dock->UnitPoint)
						average(i_dock->UnitPoint->SightFactor, 1.0f, 0.3f);
				}
			}
			else {
				average(SightFactor, 0, 0.3f);
				DockingSlotList::iterator i_dock;
				FOR_EACH(DockList,i_dock){
					if(i_dock->UnitPoint)
						average(i_dock->UnitPoint->SightFactor, 0, 0.3f);
				}
			}
		}
		else {
			if(!isUnseen()){
				average(SightFactor, 1.0f, 0.3f);
				DockingSlotList::iterator i_dock;
				FOR_EACH(DockList,i_dock){
					if(i_dock->UnitPoint)
						average(i_dock->UnitPoint->SightFactor, 1.0f, 0.3f);
				}
			}
			else {
				average(SightFactor, terInvisibilityCoeff, 0.3f);
				DockingSlotList::iterator i_dock;
				FOR_EACH(DockList,i_dock){
					if(i_dock->UnitPoint)
						average(i_dock->UnitPoint->SightFactor, terInvisibilityCoeff, 0.3f);
				}
			}
		}
	}

	terEffectControllerList::iterator it;
	FOR_EACH(effectControllers_,it)
		it->quant(this);

	if(attr().hasAnimationSetup()){
		ChainID request = chainRequest();
		if(request != CHAIN_NONE && realAvatar()->chainID() != request)
			realAvatar()->requestChain(request);
	}

	if(!freezeTimer_())
		average(FreezeFactor,0, 0.3f);
	else
		FreezeFactor = 1.0f;

	if(!healTimer_())
		average(HealFactor,0, 0.3f);
	else
		HealFactor = 1.0f;

	if(HotCount > 0){
		HotCount--;
		HotFactor = 1.0f;
	}
	else
		average(HotFactor,0, 0.3f);

	if(weapon_)
		weapon_->quant();

	if(BodyPoint->diggingMode() || isMoving())
		toolzerController_.quant(position().xi(),position().yi(),Player->zerolayer(position2D()));

	if(attr().environmentalDamage.enabled() && !BodyPoint->underMutation()){
		float damage_ratio = 0.0f;

		switch(attr().environmentalDamage.damageType){
		case ENV_DAMAGE_CHAOS:
			damage_ratio = BodyPoint->chaosCollidingFactor();
			break;
		case ENV_DAMAGE_BASEMENT:
			damage_ratio = 1.0f - basementDamage();
			break;
        default:
            break;
		}

		if(damage_ratio >= attr().environmentalDamage.damageRatioMin){
			if(attr().environmentalDamage.period){
				if(envDamageTimer_.get_start_time()){
					int delta = round(float(attr().environmentalDamage.periodDelta) * (1.0f - damage_ratio));

					if(envDamageTimer_() >= delta){
						DamageData damage = attr().environmentalDamage.damage(damage_ratio);

						if(damage.width == -1)
							damage.width = damageMolecula().atomCount();

						setDamage(damage);

						envDamageTimer_.start(attr().environmentalDamage.period);
					}
				}
				else
					envDamageTimer_.start(attr().environmentalDamage.period);
			}
			else {
				DamageData damage = attr().environmentalDamage.damage(damage_ratio);

				if(damage.width == -1)
					damage.width = damageMolecula().atomCount();

				setDamage(damage);
			}
		}
	}
}

void terUnitReal::DestroyLink()
{
	DockingSlotList::iterator i_dock;

	terUnitGeneric::DestroyLink();

	if(DockPoint && !(DockPoint->alive())){
		DockPoint = 0;
		DockSlot = 0;
	}

	FOR_EACH(DockList,i_dock)
		i_dock->DestroyLink();

	if(targetUnit() && !targetUnit()->alive())
		targetUnit_ = 0;

	if(ownerUnit_ && !ownerUnit_->alive())
		ownerUnit_ = 0;

	if(weapon_)
		weapon_->destroyLink();
}

void terUnitReal::setPose(const Se3f& pose, bool initPose)
{
	if(initPose)
		BodyPoint->initPose(pose, !attr().lockInitialPosition);
	else
		BodyPoint->setPose(pose);

	if(DockPoint && DockSlot){
		DockPoint->avatar()->updateLogicObject();
		if(dockMode() == DOCK_MODE_LOCK){
			BodyPoint->docking(DockSlot->position(), 1, 1);
		}
		else if(dockMode() != DOCK_MODE_NONE){
			if(AngleDockingEnable)
				BodyPoint->docking(DockSlot->position(), DockPhase.phase(), DockAnglePhase.phase());
			else
				BodyPoint->docking(DockSlot->position(), DockPhase.phase(), DockPhase.phase());
		}
	}

	terUnitGeneric::setPose(BodyPoint->pose(), initPose);

	checkField();
}

SaveUnitData* terUnitReal::universalSave(SaveUnitData* baseData)
{
	SaveUnitRealData* data = castOrCreate<SaveUnitRealData>(baseData);
	terUnitBase::universalSave(data);
	
	data->basementInstalled = basementInstalled();
	if(attr().energyCapacity)
		data->accumulatedEnergy = accumulatedEnergy()/attr().energyCapacity;
	data->zeroLayerCounter = zeroLayerCounter_;

	data->wayPoints = wayPoints_;

	if(weapon_){
		data->weaponChargeLevel = weapon_->chargeLevel();
		data->weapon = weapon_->universalSave(NULL);
	}

	//realAvatar()->universalSave(data->interpolationReal);
	
	return data;
}

void terUnitReal::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitRealData* data = safe_cast<const SaveUnitRealData*>(baseData);
	terUnitBase::universalLoad(data);

	uninstallBasement();
	if(data->basementInstalled)
		installBasement();

	accumulatedEnergy_ = clamp(data->accumulatedEnergy*Player->handicap(), 0, 1)*attr().energyCapacity;
	zeroLayerCounter_ = data->zeroLayerCounter;

	wayPoints_ = data->wayPoints;

	if(weapon_){
		weapon_->setChargeLevel(data->weaponChargeLevel);
		weapon_->universalLoad(data->weapon);
		//weapon_->enable();
		//weapon_->moveQuant();
	}

	//realAvatar()->universalLoad(data->interpolationReal);
}

void terUnitReal::UpdateLogicPoint()
{
	if(LogicObjectPoint){
		LogicObjectPoint->SetScale(Vect3f(Scale,Scale,Scale));
		LogicObjectPoint->SetPosition(BodyPoint->matrix());
	}
}

void terUnitReal::showDebugInfo()
{
	terUnitGeneric::showDebugInfo();

	BodyPoint->show();

	if(showDebugUnitReal.chain && avatar()->GetModelPoint()){
		char name[256];
		avatar()->GetModelPoint()->GetCurrentChannel(0, name, 0);
		float phase=avatar()->GetModelPoint()->GetPhase();
		XBuffer msg;
		msg.SetDigits(2);
		msg < name < ", " <= phase;
		show_text(position(), msg, BLUE);
	}

	if(showDebugUnitReal.chainID)
		realAvatar()->showDebugInfo();

	if(showDebugUnitReal.docking){
		DockingSlotList::iterator di;
		FOR_EACH(DockList, di){
			MatXf d=di->position();
			Vect3f target=d*Vect3f(0,10,0);
			show_line(d.trans(), target, MAGENTA);
			show_vector(d.trans(), 2, MAGENTA);
			show_vector(target, 1, MAGENTA);
		}
	}

	if(showDebugUnitReal.chaosCollidingFactor){
		XBuffer msg;
		msg.SetDigits(2);
		msg < "Chaos: " <= BodyPoint->chaosCollidingFactor();
		show_text(position(), msg, GREEN);
	}

	if(showDebugUnitReal.positionValue){
		XBuffer msg;
		msg.SetDigits(6);
		msg <= position();
		show_text(position(), msg, GREEN);
	}
	
	if(showDebugUnitReal.toolzer)
		toolzerController_.showDebugInfo(position());

	if(showDebugUnitReal.unitID){
		XBuffer msg;
		msg <= unitID();
		show_text(position(), msg, MAGENTA);
	}
}

void terUnitReal::RefreshAttribute()
{
	if(BodyPoint)
		BodyPoint->reload_parameters();
}

//----------------------------------------------
void terUnitReal::SetDockingPoint(terUnitReal* unit, terDockingSlot* slot, terDockMode mode) 
{ 
	xassert(slot);
	dockMode_ = mode; 
	DockPoint = unit; 
	DockSlot = slot; 
	DockSlot->UnitPoint = this;
	setPose(pose(), true);
}

void terUnitReal::clearDockingPoint()
{ 
	dockMode_ = DOCK_MODE_NONE; 
	DockPoint = 0; 
	if(DockSlot){
		DockSlot->UnitPoint = 0;
		DockSlot = 0; 
	}
}


void terUnitReal::Kill()
{
	terUnitGeneric::Kill();

	terEffectControllerList::iterator it;
	FOR_EACH(effectControllers_,it)
		it->kill();

	if(weapon_)
		weapon_->kill();

	clearDockingPoint();
}

terDockingSlot* terUnitReal::requestDockingSlot()
{
	DockingSlotList::iterator i_dock;
	FOR_EACH(DockList,i_dock){
		if(!i_dock->UnitPoint)
			return &*i_dock;
	}
	return 0;
}

terDockingSlot* terUnitReal::requestDockingSlot(int offset)
{
	int count = 0;
	terDockingSlot* slot = NULL;

	DockingSlotList::iterator i_dock;
	FOR_EACH(DockList,i_dock){
		if(!i_dock->UnitPoint){
			slot = &*i_dock;

			if(++count >= offset)
				break;
		}
	}

	return slot;
}

bool terUnitReal::isDockReady(int index) const 
{ 
	xassert(index >= 0 && index < DockList.size());
	if(DockList[index].UnitPoint && DockList[index].UnitPoint->dockMode() == DOCK_MODE_LOCK)
		return true;
	
	return false;
}

void terDockingSlot::init(cLogicObject* logicObject, const char* position_control)
{
	UnitPoint = 0;
	PositionControl = logicObject->FindObject(position_control);
	xassert_s(PositionControl, position_control);
}

void terDockingSlot::DestroyLink()
{
	if(UnitPoint && !(UnitPoint->alive()))
		UnitPoint = 0;
}

//-------------------------------------------------
void terUnitReal::executeCommand(const UnitCommand& command)
{
	terUnitBase::executeCommand(command);

	switch(command.commandID()){
	case COMMAND_ID_POINT:
		if(command.selectionMode() == COMMAND_SELECTED_MODE_SINGLE){
			wayPoints_.clear();
			soundEvent(SOUND_VOICE_UNIT_MOVE);
		}
		wayPoints_.push_back(command.position());
		break;
	case COMMAND_ID_OBJECT:
		if(command.selectionMode() == COMMAND_SELECTED_MODE_SINGLE)
			wayPoints_.clear();
		targetUnit_ = safe_cast<terUnitBase*>(command.unit());
		break;
	case COMMAND_ID_STOP: 
		wayPoints_.clear();
		targetUnit_ = 0;
		break;
    default:
        break;
	}
}

void terUnitReal::moveToPoint(const Vect3f& v)
{
	if(!attr().enablePathFind){
		BodyPoint->way_points.push_back(v);
		return;
	}

	//3f -> 2f -> 2i
	Vect2i target = static_cast<Vect2i>(static_cast<Vect2f>(v));
	if(pathFindTarget_ != target){
		pathFindSucceeded_ = false;
		pathFindTarget_ = target;
		recalcPathTimer_.stop();
	}
	if(!pathFindSucceeded_ && !recalcPathTimer_){
		pathFindSucceeded_ = ai_tile_map->findPath(Vect2i(position2D()), pathFindTarget_, pathFindList_, AITileMap::PATH_NORMAL);
		recalcPathTimer_.start(1000);
	}
	
	if(pathFindSucceeded_){
		if(!pathFindList_.empty()){
			vector<Vect2i>::iterator pi;
			for(pi=pathFindList_.begin();pi!=pathFindList_.end();){
				if(!BodyPoint->is_point_reached(*pi))
				{
					BodyPoint->way_points.push_back(To3D(*pi));
					pi++;
				}else
				{
					pi = pathFindList_.erase(pi);
				}
			}
		}
		else{
			BodyPoint->way_points.push_back(v);
		}
	}
}

//-----------------------------------------------------
class CalcPathFindFieldOp
{
public:
	CalcPathFindFieldOp(terUnitBase* unit)
	{
		unit_ = unit;
		position_ = unit->position2D();
		direction_ = Vect2f::ZERO;
	}
	
	void operator()(terUnitBase* p)
	{
		if(unit_ != p && p->attr().considerInFieldPathFind){
			Vect2f delta = position_ - p->position2D();
			direction_ += delta/(delta.norm2() + 1);
		}
	}
	
	const Vect2f& direction() const { return direction_; }

private:
	terUnitBase* unit_;
	Vect2f position_;
	Vect2f direction_;
};

//-------------------------------------
const Vect3f& terUnitReal::interpolatedPosition() 
{
	if(!realAvatar())
	{
		xassert(realAvatar());
		return Vect3f::ID;
	}

	cObjectNodeRoot* model=realAvatar()->GetModelPoint();
	if(model)
		return model->GetPosition().trans();
	xassert(model);
	return Vect3f::ID;
}

void terUnitReal::showPath(const vector<Vect3f>& wayPoints) {
	if(!(avatar())) 
		return;

	if (selected()) {
		if(attr().showPath && !wayPoints.empty()){
			Vect3f posPrev = interpolatedPosition();
			vector<Vect3f>::const_iterator i;
			FOR_EACH(wayPoints, i){
				terRenderDevice->DrawLine(posPrev, *i, pathColor);
				posPrev = *i;
			}
			Vect3f w, e;
			terCamera->GetCamera()->ConvertorWorldToViewPort(&posPrev,&w,&e);
			if(e.z < 1.0f)
				terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,pathColor,0);
		}
	}
}

void terUnitReal::ShowInfo()
{
	terUnitBase::ShowInfo();

	if(!(avatar())) 
		return;

	if (isUnseen() && !Player->active() && universe()->activePlayer()->clan() != Player->clan()) {
		return;
	}
/*
	if(selected()){
		if(attr().showPath && !BodyPoint->way_points.empty()){
			Vect3f posPrev = interpolatedPosition();
			Vect3fList::const_iterator i;
			FOR_EACH(BodyPoint->way_points, i){
				terRenderDevice->DrawLine(posPrev, *i, pathColor);
				posPrev = *i;
			}
			Vect3f w, e;
			terCamera->GetCamera()->ConvertorWorldToViewPort(&posPrev,&w,&e);
			if(e.z < 1.0f)
				terRenderDevice->DrawRectangle(round(e.x) - 2,round(e.y) - 2,4,4,pathColor,0);
		}

//		if(targetUnit()) targetUnit()->SelectEnemy();

	}
*/
	if ( (marked() || Player->marked()) && !(unitClass() & UNIT_CLASS_BASE) && alive() && selectAble() ) {
		Vect3f e;
		Vect3f pv;

		Vect3f pos = interpolatedPosition();

//		terCamera->GetCamera()->ConvertorWorldToViewPort(&(avatar()->matrix().trans()),&pv,&e);
		terCamera->GetCamera()->ConvertorWorldToViewPort(&pos,&pv,&e);
		float radiusFactor = terCamera->GetCamera()->GetFocusViewPort().x / pv.z;
		int r = 0;
		if (radius()) {
			r = round( radius() * radiusFactor );
		}

		if (pv.z > 0) {
			drawMark2d(terRenderDevice, e.xi(), e.yi() + r * attr().SelectionDistance, life(), maxHealth(), radiusFactor);
		}
/*
		DrawSelect2d(terRenderDevice,Player->pTextureUnitSelection,Player->unitColor(),
			e.xi() - r * attr().SelectionSize,
			e.yi() + r * attr().SelectionDistance,
			r*2 * attr().SelectionSize,
			r*2 * attr().SelectionSize,0, life());
*/
	}
}

void terUnitReal::AvatarInterpolation()
{
	terUnitGeneric::AvatarInterpolation();

	terEffectControllerList::iterator it;
	FOR_EACH(effectControllers_,it)
		it->avatarInterpolation();

	if(weapon_)
		weapon_->avatarInterpolation();
}

void terUnitReal::AvatarQuant()
{
	terUnitGeneric::AvatarQuant();

	terEffectControllerList::iterator it;
	FOR_EACH(effectControllers_,it)
		it->avatarQuant(this);

	if(weapon_)
		weapon_->avatarQuant();
}

bool terUnitReal::fireRequest(const Vect3f* targetPosition, terUnitBase* targetUnit, int& fire_status)
{
	if(weapon_ && weapon_->isEnabled()){
		bool log = weapon_->fireRequest(targetPosition,targetUnit,fire_status);

		if(showDebugUnitReal.fireResponse)
		{
			XBuffer buf;
			if(fire_status & LEGION_FIRE_STATUS_GROUND_OBSTACLE)
				buf < "G ";
			if(fire_status & LEGION_FIRE_STATUS_FRIENDLY_FIRE)
				buf < "ff ";
			if(fire_status & LEGION_FIRE_STATUS_RELOAD_AMMO)
				buf < "R ";
			if(fire_status & LEGION_FIRE_STATUS_DISTANCE)
				buf < "D ";
			if(fire_status & LEGION_FIRE_STATUS_FIELD_OBSTACLE)
				buf < "F ";
			if(fire_status & LEGION_FIRE_STATUS_HORIZONTAL_ANGLE)
				buf < "H ";
			if(fire_status & LEGION_FIRE_STATUS_VERTICAL_ANGLE)
				buf < "V ";
			if(fire_status & LEGION_FIRE_STATUS_BORDER_ANGLE)
				buf < "B ";
			show_text(terUnitReal::position(), buf, RED);
		}
		if(showDebugUnitReal.target)
			show_line(position(), targetUnit ? targetUnit->position() : *targetPosition, RED);

		return log;
	}

	return false;
}

bool terUnitReal::fireCheck(const Vect3f& position,terUnitBase* target) const
{
	if(weapon_){
		int fire_status;
		return weapon_->fireTest(position,target,fire_status);
	}

	return false;
}

bool terUnitReal::fireDistanceCheck(const Vect3f& position) const
{
	if(weapon_)
		return weapon_->fireDistanceCheck(position);

	return false;
}

bool terUnitReal::isWeaponLoaded() const 
{
	if(weapon_)
		return weapon_->isLoaded();

	return false;
}

bool terUnitReal::isWeaponReady() const 
{
	if(weapon_)
		return weapon_->isReady();

	return false;
}

bool terUnitReal::weaponReload()
{
	if(weapon_){
		weapon_->reloadStart();
		return true;
	}

	return false;
}

void terUnitReal::weaponQuant()
{
	if(weapon_){
		if(needWeaponDisable())
			weapon_->disable();
		else
			weapon_->enable();

		weapon_->moveQuant();
	}
}

void terUnitReal::explode()
{
	terUnitGeneric::explode();

	if(attr().isBuilding())
		soundEvent(SOUND_VOICE_BUILDING_DESTROYED);
	else
		soundEvent(SOUND_VOICE_UNIT_DESTROYED);

	splashDamage();
}

void terUnitReal::splashDamage()
{
	if(!(BodyPoint->clusterColliding()) && attr().unitDamage.splashDamageRadius){
		terUnitGridSplashDamageOperator op(this, ownerUnit_ ? ownerUnit_ : this);
		universe()->UnitGrid.Scan(round(position().x), round(position().y), attr().unitDamage.splashDamageRadius, op);
	}
}

int terUnitReal::estimatedDamage() const
{
	if(weapon_)
		return weapon_->estimatedDamage();
	else
		return 0;
}

void terUnitReal::removeMissileReference(terUnitBase* p)
{
	if(weapon_)
		weapon_->removeMissileReference();
}

float terUnitReal::addAccumulatedEnergy(float delta) 
{ 
	float energy = clamp(accumulatedEnergy() + delta, 0, attr().energyCapacity); 
	float used = energy - accumulatedEnergy();
	accumulatedEnergy_ = energy;
	return used;
}

float terUnitReal::productionProgress()
{
	float progress = productionConsumption_.progress();
	if(!productionConsumption_.attached()){
		productionConsumption_.attach(Player, attr().productionConsumption);
	}
	productionConsumption_.requestCharge();
	return progress;
}

void terUnitReal::checkField()
{
	if(BodyPoint->underGround())
		return;

	if(BodyPoint->flyingMode()){
		int cluster = field_dispatcher->getIncludingCluster(position());
		if(cluster != checkFieldCluster_ && FieldCluster::get_player_id(max(cluster, checkFieldCluster_)) != Player->playerID() && !checkFieldTimer_){
			checkFieldTimer_.start(fieldPrm.impulseDuration);
			setDamage(fieldPrm.impulseFieldDamage, 0);
		}
		checkFieldCluster_ = cluster;
	}

	Vect3f p0, dpx, dpy, box = BodyPoint->boxMax() - BodyPoint->boxMin();
  	BodyPoint->matrix().xformPoint(BodyPoint->boxMin(), p0);
	BodyPoint->rotation().xform(Vect3f(max(box.x, fieldPrm.checkFieldRadiusMin), 0, 0), dpx);
	BodyPoint->rotation().xform(Vect3f(0, max(box.y, fieldPrm.checkFieldRadiusMin), 0), dpy);

	Vect3f points[] = {
		p0, p0 + dpx, p0 + dpy, p0 + dpx + dpy
	};

	int playerID = -1;
	for(int i = 0; i < 4; i++){
		int cluster = field_dispatcher->getIncludingCluster(points[i]);
		int id = FieldCluster::get_player_id(cluster);
		if(cluster && id != Player->playerID()){
			playerID = id;
			break;
		}
	}

	if(playerID == -1)
		return;

	terPlayer* player = universe()->findPlayer(playerID);
	int counter = 0;
	for(int i = 0; i < 4; i++){
        //3f -> 2f -> 2i
        Vect2i pos = static_cast<Vect2i>(static_cast<Vect2f>(points[i]));
		if(player->fieldColumn().filled(pos.x, pos.y))
			counter++;
	}
	if(counter != 0 && counter != 4)
		setFieldDamage(fieldPrm.fieldDamage);
}

ChainID terUnitReal::chainRequest() const
{
	if(weapon_)
		return weapon_->chainRequest();

	return CHAIN_NONE;
}

float terUnitReal::chargeLevel() const
{
	if(weapon_)
		return weapon_->chargeLevel();

	return terUnitGeneric::chargeLevel();
}

bool terUnitReal::isFiring() const 
{ 
	return weapon_ ? weapon_->isFiring() : false; 
}

void terUnitReal::ChangeUnitOwner(terPlayer* player)
{
	terUnitGeneric::ChangeUnitOwner(player);
	if(weapon_)
		weapon_->changePlayer(player);

	DockingSlotList::iterator di;
	FOR_EACH(DockList, di)
		if(di->UnitPoint)
			di->UnitPoint->ChangeUnitOwner(player);
}

terUnitBase* terUnitReal::weaponTarget()
{
	if(weapon_)
		return weapon_->fireTarget();

	return NULL;
}


void terUnitReal::placeZeroLayer(bool restore)
{
	if(!restore ? !zeroLayerPlaced() : zeroLayerPlaced()){
		if(!restore)
			zeroLayerCounter_ = ++zeroLayerCounterCurrent_;
		else if(zeroLayerCounterCurrent_ < zeroLayerCounter_)
			zeroLayerCounterCurrent_ = zeroLayerCounter_;


		ColumnVect columns;
		PlayerVect::iterator pli;
		FOR_EACH(universe()->Players, pli)
			if(*pli != Player && (*pli)->structureColumn().area())
				columns.push_back(&(*pli)->structureColumn());

		Player->structureColumn().addCircleThenSub(position2D(), attr().ZeroLayerRadius, columns);

		plotCircleZL(position2D().x, position2D().y, attr().ZeroLayerRadius);
		terMapPoint->UpdateMap(position2D(), attr().ZeroLayerRadius);
	}
}

class FreeZeroLayerOp
{
	typedef map<int, terUnitReal*> Map;

public:
	FreeZeroLayerOp(terUnitBase* unit) : unit_(unit) {}
	void operator()(terUnitBase* p)
	{		
		if(p->alive() && unit_ != p && p->attr().ZeroLayerRadius > 0
			&& unit_->position2D().distance2(p->position2D()) < sqr(unit_->attr().ZeroLayerRadius + p->attr().ZeroLayerRadius)){
				terUnitReal* unit = safe_cast<terUnitReal*>(p);
				if(unit->zeroLayerPlaced())
					map.insert(Map::value_type(unit->zeroLayerPlaced(), unit));
			}
	}

	~FreeZeroLayerOp() 
	{
		Map::iterator i;
		FOR_EACH(map, i)
			i->second->placeZeroLayer(true);
	}

private:
	terUnitBase* unit_;
	Map map;
};

void terUnitReal::freeZeroLayer()
{
	if(zeroLayerPlaced()){
		zeroLayerCounter_ = 0;

		eraseCircleZL(position2D().x, position2D().y, attr().ZeroLayerRadius);
		Player->structureColumn().operateByCircle(position2D(), attr().ZeroLayerRadius, 0);
		terMapPoint->UpdateMap(position2D(), attr().ZeroLayerRadius);

        FreeZeroLayerOp op = FreeZeroLayerOp(this);
		universe()->UnitGrid.Scan(position().xi(), position().yi(), round(attr().ZeroLayerRadius*2.5), op);
	}
}

void terUnitReal::handleLaserHit()
{
	terEffectControllerList::iterator it = find(effectControllers_.begin(),effectControllers_.end(),EFFECT_ID_LASER_HIT);
	if(it != effectControllers_.end())
		it->createEffect(this);
}

void terUnitReal::fireStop()
{
	if(weapon_ && weapon_->needCharge() && weapon_->isSwitchedOn()){
		weapon_->switchOff();
		weapon_->reloadStart();
	}
}

void terUnitReal::setDamage(const DamageData& damage,terUnitBase* p)
{
	if(isDocked()) return;
	terUnitGeneric::setDamage(damage,p);
}

//--------------------------------

terEffectController::terEffectController(const terUnitEffectData* setup,cEffect* effect) : setup_(setup),
	effect_(effect)
{
	xassert(setup_);
	effectRate_ = 0.0f;
}

terEffectController::~terEffectController()
{
}

void terEffectController::avatarQuant(terUnitReal* owner)
{
	float rate = 0.0f;

	if(owner->effectsEnabled()){
		switch(setup_->effectID){
		case EFFECT_ID_UNIT_SMOKE:
			if(1.0f - owner->damageMolecula().phase() >= setup_->startupDamage)
				rate = sqr(((1.0f - owner->damageMolecula().phase()) - setup_->startupDamage) / (1.0f - setup_->startupDamage));
			break;
		case EFFECT_ID_UNIT_MOVE:
			if(owner->isMoving())
				rate = 1.0f;
			break;
		case EFFECT_ID_LASER_HIT:
			rate = 1.0f;
			break;
		case EFFECT_ID_UNIT_SMOKE_CORPSE:
			rate = 1.0f;
			break;
		default:
			assert(0);
			break;
		}
	}

	effectRate_ = rate;
}

void terEffectController::quant(terUnitReal* owner)
{
	if(!effect_)
		return;

	Se3f pos = (setup_->needOrientation) ? owner->pose() : Se3f(QuatF::ID,owner->avatar()->matrix().trans());

	switch(setup_->effectID){
	case EFFECT_ID_UNIT_MOVE:
		if(owner->isMoving())
			pos.trans()=To3D(pos.trans());
		break;
	case EFFECT_ID_LASER_HIT:
		if(effect_ && !effect_->IsLive()){
			effect_->Release();
			effect_ = NULL;
		}
		break;
    default:
        break;
	}

	effectPose_=pos;
}

void terEffectController::avatarInterpolation()
{
	if(!effect_)
		return;
	effectRate_(effect_);
	effectPose_(effect_);
}

void terEffectController::kill()
{
	if(effect_){
		effect_->SetCycled(false);
		effect_->SetParticleRate(0.0f);
		effect_->SetAutoDeleteAfterLife(true);
		effect_ = NULL;
	}
}

bool terEffectController::createEffect(terUnitReal* owner)
{
	RELEASE(effect_);

	if(EffectKey* key = owner->attr().getEffect(setup_->effectID)){
		effect_ = terScene->CreateScaledEffect(*key,owner->avatar()->GetModelPoint());
		effect_->SetPosition(owner->avatar()->matrix());
		effect_->SetAutoDeleteAfterLife(false);
		return true;
	}

	return false;
}

