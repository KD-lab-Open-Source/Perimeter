#include "StdAfx.h"

#include "Runtime.h"
#include "CameraManager.h"

#include "Universe.h"
#include "IronFrame.h"
#include "IronBuilding.h"

#include "UniverseInterface.h"

#include "PerimeterSound.h"
#include "Config.h"
#include "ScanPoly.h"
#include "AIPrm.h"
#include "Triggers.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

REGISTER_CLASS(AttributeBase, AttributeBuilding, "Здание");
REGISTER_CLASS(AttributeBase, AttributeSquad, "Сквад");

AttributeBuilding::AttributeBuilding() 
{
	Upgrade = UNIT_ATTRIBUTE_NONE;
	constructionPriority = 0;
	disconnectAnimation = 0;

	InstallBound = true;
	IgnoreTargetTrace = true;

	UnitClass = UNIT_CLASS_STRUCTURE;
	CollisionGroup = COLLISION_GROUP_ENEMY | COLLISION_GROUP_REAL;

	considerInFieldPathFind = 1;
	
	rigidBodyPrm.setKey("Building");
}

terBuilding::terBuilding(const UnitTemplate& data) 
:	terUnitReal(data),
	connection_icon_(terModelBuildingNoConnection),
	energy_icon_(terModelBuildingLowEnergy)
{
	setBuildingStatus(BUILDING_STATUS_PLUGGED_IN);
	cancelConstructionTime_ = cancelConstructionTime;
	visible_ = true;
}

terBuilding::~terBuilding()
{
}

void terBuilding::Kill()
{
	terUnitReal::Kill();
	uninstallBasement();
}

void terBuilding::Start()
{
	terUnitReal::Start();

	setRealModel(0, 1);

	if(!isConstructed())
		CreateHologram();
}

void terBuilding::setPose(const Se3f& poseIn, bool initPose)
{
	Se3f poseZ = poseIn;
	poseZ.trans().z = vMap.hZeroPlast - BodyPoint->boxMin().z;
	terUnitReal::setPose(poseZ, false);
}

SaveUnitData* terBuilding::universalSave(SaveUnitData* baseData)
{
	SaveUnitBuildingData* data = castOrCreate<SaveUnitBuildingData>(baseData);
	terUnitReal::universalSave(data);

	data->buildingStatusBV = buildingStatus();
	data->visible = visible_;

	return data;
}

void terBuilding::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitBuildingData* data = safe_cast<const SaveUnitBuildingData*>(baseData);
	terUnitReal::universalLoad(data);

	setBuildingStatus(data->buildingStatusBV);
	visible_ = data->visible;
}

void terBuilding::CreateHologram()
{
	terBuildingHologram* p = safe_cast<terBuildingHologram*>(Player->buildUnit(UNIT_ATTRIBUTE_BUILDING_HOLOGRAM));
	p->SetBuildingPoint(this);
	p->Start();
}

void terBuilding::setRealModel(int modelIndex, float scale)
{
	uninstallBasement();

	__super::setRealModel(isConstructed() ? 0 : 1, 1);

	if(attr().hasAnimationSetup()){
		if(!isConstructed()){
			if(attr().MakingChainNum != -1)
				realAvatar()->setChain(CHAIN_BUILD1);
		}
	}

	installBasement();

	AvatarQuant();
	avatar()->Start();
}

void terBuilding::WayPointStart()
{
	terUnitReal::WayPointStart();
	BodyPoint->makeStatic();
}

bool terBuilding::isBuildingEnable() const
{ 
	int flags = BUILDING_STATUS_CONNECTED | BUILDING_STATUS_POWERED | BUILDING_STATUS_ENABLED | BUILDING_STATUS_MOUNTED;
	return (buildingStatus() & flags) == flags && (buildingStatus() & (BUILDING_STATUS_CONSTRUCTED | BUILDING_STATUS_UPGRADING));
}

int terBuilding::repairRequest() const
{
	if(!(buildingStatus() & BUILDING_STATUS_HOLD_CONSTRUCTION)){
		int element_count = damageMolecula().deadElementCount();
		if(element_count){
			element_count = round(float(element_count) / attr().constructionSpeedCoeff + 0.5f) - repairRequested();
			return (element_count > 0) ? element_count : 0;
		}
	}

	return 0;
}

void terBuilding::Quant()
{
	terUnitReal::Quant();

	checkConnection();

	if(isConstructed()){
		if(realAvatar()->chainID() != CHAIN_NONE){
			if(isConnected() || attr().disconnectAnimation)
				realAvatar()->setAnimationSpeed(1.0f);
			else 
				realAvatar()->setAnimationSpeed(0.0f);
		}
	}
	else{
		if(buildingStatus() & BUILDING_STATUS_HOLD_CONSTRUCTION)
			cancelConstructionTimer_.start(cancelConstructionTime_);
		if(cancelConstructionTimer_()){
			if(!downgrade())
				Kill();
			return;
		}
	}
	
//	if(Player->GetEvolutionBuildingData(Attribute.ID).Enabled)
		setBuildingStatus(buildingStatus() | BUILDING_STATUS_ENABLED);
//	else
//		setBuildingStatus(buildingStatus() & ~BUILDING_STATUS_ENABLED);

	if(buildingStatus() & BUILDING_STATUS_PLUGGED_IN)
		setBuildingStatus(buildingStatus() | BUILDING_STATUS_POWERED);
	else
		setBuildingStatus(buildingStatus() & ~BUILDING_STATUS_POWERED);

	int flags = BUILDING_STATUS_CONNECTED | BUILDING_STATUS_POWERED | BUILDING_STATUS_ENABLED;
	if((buildingStatus() & flags) == flags && buildingStatus() & (BUILDING_STATUS_CONSTRUCTED | BUILDING_STATUS_UPGRADING) 
	  && BuildingEnableRequest()){
		setBuildingStatus(buildingStatus() | BUILDING_STATUS_MOUNTED);
	}
	else
		setBuildingStatus(buildingStatus() & ~BUILDING_STATUS_MOUNTED);

	bool condition = ((buildingStatus() & BUILDING_STATUS_CONSTRUCTED) || !attr().ConnectionRadius)
		&& !(buildingStatus() & BUILDING_STATUS_CONNECTED) && !Player->isWorld();
	if(changePlayerTimer_(condition, globalAttr().changePlayerDelay))
		universe()->changeOwner(this, universe()->worldPlayer());
}

ChainID terBuilding::chainRequest() const
{
	if(isConstructed()){
		if(isConnected()){
			ChainID id = terUnitReal::chainRequest();
			if(id != CHAIN_NONE) 
				return id;
		}
		
		return CHAIN_SWITCHED_ON;
	}
	else {
		if(!attr().isUpgrade && attr().MakingChainNum != -1){
			int chain_index = floor(damageMolecula().phase() * float(attr().MakingChainNum));
			return ChainID(CHAIN_BUILD1 + chain_index);
		}
		else
			return CHAIN_NONE;
	}
}

terBuilding* terBuilding::upgrade()
{
	if(!isConstructed())
		return 0;

	terUnitAttributeID upID = attr().Upgrade;
	if(upID != UNIT_ATTRIBUTE_NONE){
		Kill();
		terBuilding* building = safe_cast<terBuilding*>(Player->buildUnit(upID));
		building->setPose(pose(), true);
		building->setBuildingStatus((buildingStatus() | BUILDING_STATUS_UPGRADING) & ~BUILDING_STATUS_HOLD_CONSTRUCTION);
		building->constructionStart();
		if(upID != UNIT_ATTRIBUTE_CORRIDOR_OMEGA)
			building->terUnitGeneric::damageMoleculaRepair(damageMolecula().aliveElementCount());

		building->Start();
//		universe()->select.unitToSelection(building, COMMAND_SELECTED_MODE_SINGLE);
		universe()->select.grade(this, building);
		return building;
	}
	return 0;
}

terBuilding* terBuilding::downgrade()
{
	terUnitAttributeID downID = attr().downgrade();
	if(downID != UNIT_ATTRIBUTE_NONE){
		Kill();
		terBuilding* building = safe_cast<terBuilding*>(Player->buildUnit(downID));
		building->setPose(pose(), true);
		building->setBuildingStatus((buildingStatus() | BUILDING_STATUS_CONSTRUCTED) & ~(BUILDING_STATUS_UPGRADING | BUILDING_STATUS_HOLD_CONSTRUCTION));
		building->Start();
//		universe()->select.unitToSelection(building, COMMAND_SELECTED_MODE_SINGLE);
		universe()->select.grade(this, building);
		return building;
	}
	return 0;
}

float terBuilding::upgradePhase() const
{
	if(int alive_elements = damageMolecula().aliveElementCount()){
		terUnitAttributeID down_id = attr().downgrade();
		if(down_id != UNIT_ATTRIBUTE_NONE){
			const AttributeBase* attr = Player->unitAttribute(down_id);
			xassert(attr);

			int downgrade_elements = attr->damageMolecula.elementCount();
			if(alive_elements <= downgrade_elements) return 0.0f;

			return float(alive_elements - downgrade_elements) / float(damageMolecula().elementCount() - downgrade_elements);
		}
	}

	return 0.0f;
}

void terBuilding::MapUpdateHit(float x0,float y0,float x1,float y1)
{
//	SetMaxPower(Attribute.GenericPower*clamp(
//		(basementDamage() - Attribute.BasementDamageFactor)/(1.0f - Attribute.BasementDamageFactor), 0, 1));
}

int terBuilding::damageMoleculaRepair(int element_count)
{
	element_count = round(float(element_count) * attr().constructionSpeedCoeff);
	int ret = terUnitGeneric::damageMoleculaRepair(element_count);

	if(!isConstructed()){
		if(!damageMolecula().deadElementCount()){
			setBuildingStatus((buildingStatus() | BUILDING_STATUS_CONSTRUCTED) & ~BUILDING_STATUS_UPGRADING);
			setRealModel(0, 1);

			if(!attr().ConnectionRadius && isConnected())
				Player->burnZeroplast();

			universe()->checkEvent(EventUnitPlayer(Event::COMPLETE_BUILDING, this, Player));

			clearRepairRequest();

			if(attr().isUpgrade)
				soundEvent(SOUND_VOICE_BUILDING_UPGRADE_FINISHED);
			else
				soundEvent(SOUND_VOICE_BUILDING_READY);

			soundEvent(SOUND_EVENT_BUILDING_READY);
		}
	}

	return ret;
}

void terBuilding::executeCommand(const UnitCommand& command)
{
	terUnitReal::executeCommand(command);

	switch(command.commandID()){
		case COMMAND_ID_UNINSTALL:
			uninstall();
			break;
		case COMMAND_ID_STRUCTURE_UPGRADE:
			upgrade();
			break;
		case COMMAND_ID_CANCEL_UPGRADE:
			if(!isConstructed())
				downgrade();
			break;
		case COMMAND_ID_POWER_ON:
			if(isConstructed())
				setBuildingStatus(buildingStatus() | BUILDING_STATUS_PLUGGED_IN);
			break;
		case COMMAND_ID_POWER_OFF:
			if(isConstructed())
				setBuildingStatus(buildingStatus() & ~BUILDING_STATUS_PLUGGED_IN);
			break;

		case COMMAND_ID_HOLD_CONSTRUCTION:
			if(!(buildingStatus_ & BUILDING_STATUS_HOLD_CONSTRUCTION))
				soundEvent(SOUND_EVENT_BUILDING_HOLD_CONSTRUCTION);

			setBuildingStatus(buildingStatus() | BUILDING_STATUS_HOLD_CONSTRUCTION);
			break;
		case COMMAND_ID_CONTINUE_CONSTRUCTION:
			setBuildingStatus(buildingStatus() & ~BUILDING_STATUS_HOLD_CONSTRUCTION);
			break;
	}
}

void terBuilding::uninstall()
{
	Player->energyData().addReturned(attr().buildEnergy()*globalAttr().sellBuildingEfficiency*life());

	terBuildingUninstall* p = safe_cast<terBuildingUninstall*>(Player->buildUnit(UNIT_ATTRIBUTE_BUILDING_UNINSTALL));
	p->setParent(this);
	p->Start();

	soundEvent(SOUND_VOICE_BUILDING_DISASSEMBLED);

	Kill();
}

void terBuilding::AvatarInterpolation()
{
	terUnitReal::AvatarInterpolation();
}

void terBuilding::AvatarQuant()
{
	terUnitReal::AvatarQuant();

	if(SightFactor < 0.001f){
		avatar()->Hide();
	}
	else{
		avatar()->Show();
		UpdateAvatar();
	}

	if(visible_)
		avatar()->Show();
	else
		avatar()->Hide();
}

void terBuilding::UpdateAvatar()
{
	realAvatar()->setSight(SightFactor);
	avatar()->setPose(BodyPoint->pose());

	if(isConstructed()){
		realAvatar()->setTexture(0);
		realAvatar()->setBuild(1.0f);
		realAvatar()->setHologram(0);
		realAvatar()->setLife(damageMolecula().phase());
	}
}

void terBuilding::showDebugInfo()
{
	terUnitReal::showDebugInfo();
	
	if(show_building_status){
		string str;
		if(buildingStatus() & BUILDING_STATUS_CONSTRUCTED)
			str += "Cs ";
		if(buildingStatus() & BUILDING_STATUS_PLUGGED_IN)
			str += "Pg ";
		if(buildingStatus() & BUILDING_STATUS_CONNECTED)
			str += "Cn ";
		if(buildingStatus() & BUILDING_STATUS_POWERED)
			str += "Pw ";
		if(buildingStatus() & BUILDING_STATUS_ENABLED)
			str += "En ";
		if(buildingStatus() & BUILDING_STATUS_UPGRADING)
			str += "Up ";
		if(buildingStatus() & BUILDING_STATUS_MOUNTED)
			str += "Mo ";
		if(buildingStatus() & BUILDING_STATUS_HOLD_CONSTRUCTION)
			str += "Hl ";
		
		show_text(position(), str.c_str(), CYAN);
	}
}

int terBuilding::GetProductionProgress()
{
	if(!isConstructed()){
		if(damageMolecula().elementCount())
			return damageMolecula().aliveElementCount() * 100 / damageMolecula().elementCount();
	}

	return 100;
}

int terBuilding::CanBuildingEnable()
{ 
	int flags = BUILDING_STATUS_CONNECTED | BUILDING_STATUS_ENABLED; 
	return (buildingStatus() & flags) == flags && buildingStatus() & (BUILDING_STATUS_CONSTRUCTED | BUILDING_STATUS_UPGRADING); 
}

//-------------------------------

terIconBuilding::terIconBuilding(const ModelProperty& model_prop)
{
	model_ = terScene->CreateObject(model_prop.name,NULL);
	float scale=model_->GetBoundRadius();
	scale = model_prop.size/scale;
	model_->SetScale(Vect3f(scale, scale, scale));
	model_->SetAttr(ATTRUNKOBJ_IGNORE | ATTRUNKOBJ_NOLIGHT);
	phase_ = 0;
}

terIconBuilding::~terIconBuilding()
{
	model_->SetAttr(ATTRUNKOBJ_IGNORE);
	model_->Release();
}

void terIconBuilding::quant()
{
	model_->SetAttr(ATTRUNKOBJ_IGNORE);
	phase_ = fmod(phase_ + (float)(frame_time.delta())/terModelBuildingSpeed, 1.0f);
}

void terIconBuilding::show(const Vect3f& pos)
{
	xassert(pos);
	Mat3f mat;
	mat.xpose(terCamera->GetCamera()->GetMatrix().rot());

	model_->SetPosition(MatXf(mat*Mat3f(M_PI, Z_AXIS), pos));
	
	model_->ClearAttr(ATTRUNKOBJ_IGNORE);
	model_->SetPhase(phase_);
}


//-------------------------------
void terBuilding::ShowInfo()
{
	terUnitReal::ShowInfo();
	
	connection_icon_.quant();
	energy_icon_.quant();
	
	if(isConstructed() && attr().iconDistanceFactor && visible_){
		if(buildingStatus() & BUILDING_STATUS_CONNECTED){
			int flag = BUILDING_STATUS_POWERED | BUILDING_STATUS_ENABLED;
			if((buildingStatus() & flag) != flag){
				MatXf m = avatar()->matrix();
				m.trans().z += radius()*attr().iconDistanceFactor;
				energy_icon_.show(m.trans());
			}
		}
		else{
			MatXf m = avatar()->matrix();
			m.trans().z += radius()*attr().iconDistanceFactor;
			connection_icon_.show(m.trans());
		}
	}
}

void terBuilding::explode()
{
	if(isConstructed() || attr().isUpgrade)
		terUnitReal::explode();
}

void terBuilding::constructionStart()
{
	damageMoleculaZeroize();
	clearRepairRequest();

	soundEvent(SOUND_VOICE_UNIT_PRODUCTION_STARTED);

	setBuildingStatus(buildingStatus() & ~(BUILDING_STATUS_MOUNTED | BUILDING_STATUS_CONSTRUCTED));
}

void terBuilding::setCanselConstructionTime(int time)
{
	cancelConstructionTimer_.start(cancelConstructionTime_ = time);
}

int terBuilding::isPluggedIn()
{
	switch(lastCommand()){
	case COMMAND_ID_POWER_ON:
		return 1;
	case COMMAND_ID_POWER_OFF:
		return 0;
	}
	return buildingStatus() & BUILDING_STATUS_PLUGGED_IN;
}

//-------------------------------------------------

terFallStructure::terFallStructure(const UnitTemplate& data) : terUnitCorpse(data)
{
	soundFlag_ = true;
}

void terFallStructure::Quant()
{
	terUnitCorpse::Quant();

	if(alive()){
		if(time() > fallDelay_){
			if(soundFlag_){
				SND3DPlaySound("explosion_start",&position());
				soundFlag_ = false;
			}

			setPositionZ(position().z + fallSpeed_);
			fallSpeed_ += fallAcceleration_;
		}
	}
}

void terFallStructure::AvatarQuant()
{
	terUnitCorpse::AvatarQuant();
	avatar()->setPose(pose());
}

void terFallStructure::setParent(terUnitBase* p)
{
	terUnitCorpse::setParent(p);

	terUnitReal* b = safe_cast<terUnitReal*>(p);
	fallSpeed_ = b->attr().FallSpeed;
	fallDelay_ = b->attr().FallDelay;
	fallAcceleration_ = b->attr().FallAcceleration;

	setPose(p->pose(), true);
}

//-------------------------------------------

terBuildingHologram::terBuildingHologram(const UnitTemplate& data) : terUnitBase(data)
{
	MasterPoint = NULL;
	ConstructionScale = 0;
}

void terBuildingHologram::Start()
{
	terUnitBase::Start();

	if(MasterPoint){
		setPose(MasterPoint->pose(), true);

		avatar()->SetModel(MasterPoint->attr().buildModelName(), MasterPoint->GetScale()*1.1f);
		avatar()->SetChain(MasterPoint->attr().PrevChainName);

		AvatarQuant();
		avatar()->Start();
	}
}

void terBuildingHologram::Quant()
{
	terUnitBase::Quant();

	if(MasterPoint && !(MasterPoint->isConstructed())){
		ConstructionScale += 0.1f * M_PI;
	}
	else
		Kill();
}

void terBuildingHologram::AvatarQuant()
{
	terUnitBase::AvatarQuant();

//	avatar()->SetSight(PrevSightFactor,SightFactor);
	avatar()->Show();

	avatar()->setPose(pose());

	terInterpolationReal* real_avatar = safe_cast<terInterpolationReal*>(avatar());
	if(MasterPoint)
		real_avatar->setBuild(1.0f - MasterPoint->damageMolecula().phase()*0.8f);

	real_avatar->setHologram(0.5f + sinf(ConstructionScale)*0.5f);
	real_avatar->setTexture(Player->HologramPoint);

	real_avatar->setPhase(1.0f);
}

void terBuildingHologram::DestroyLink()
{
	terUnitBase::DestroyLink();
	if(MasterPoint && !(MasterPoint->alive()))
		MasterPoint = NULL;
}

bool terBuilding::canUpgrade() const 
{ 
	if(!isConstructed())
		return false;

	if(attr().Upgrade != UNIT_ATTRIBUTE_NONE)
		return true;
	else
		return false; 
}

void terBuilding::ChangeUnitOwner(terPlayer* player)
{
	terUnitReal::ChangeUnitOwner(player);

	setBuildingStatus(buildingStatus() & ~BUILDING_STATUS_HOLD_CONSTRUCTION);
}

//------------------------------------------
void terBuildingPowered::checkConnection()
{
	xassert(attr().InstallBound);

	MatX2f mx2(Mat2f(BodyPoint->angleZ()), BodyPoint->position());
	vector<Vect2i> points(attr().BasementPoints.size());
	for(int i = 0; i < points.size(); i++)
		points[i] = mx2*attr().BasementPoints[i];

	GenShapeLineOp op;
	scanPolyByLineOp(&points[0], points.size(), op);

	if(!Player->isWorld()){
		if(Player->energyColumn().intersected(op.shape())){
			if(isConstructed() && !(buildingStatus() & BUILDING_STATUS_CONNECTED))
				Player->burnZeroplast();
			setBuildingStatus(buildingStatus() | BUILDING_STATUS_CONNECTED);
		}
		else 
			setBuildingStatus(buildingStatus() & ~BUILDING_STATUS_CONNECTED);
	}
	else{
		PlayerVect::iterator pi;
		FOR_EACH(universe()->Players, pi)
			if(!(*pi)->isWorld() && (*pi)->energyColumn().intersected(op.shape())){
				//setBuildingStatus(buildingStatus() | BUILDING_STATUS_CONNECTED);
				universe()->changeOwner(this, (*pi));
			}
	}
}

int terBuildingPowered::repairRequest() const
{
	if(Player->isAI() && !isConnected())
		return 0;

	return terBuilding::repairRequest();
}

//-------------------------------------------

terBuildingEnvironment::terBuildingEnvironment(const UnitTemplate& data) 
: terBuildingPowered(data)
{
}

//-------------------------------------------

terBuildingUninstall::terBuildingUninstall(const UnitTemplate& data)
: terUnitBase(data)
{
	sight_ = 1.0f;
	hologram_ = 0.0f;
}

void terBuildingUninstall::setParent(terBuilding* p)
{
	cObjectNodeRoot* pModel=p->avatar()->GetModelPoint();
	avatar()->SetModelPoint(pModel);
	avatar()->setPose(p->pose());

	if(const terUnitEffectData* eff_data = p->attr().getEffectData(EFFECT_ID_BUILDING_UNINSTALL)){
		if(EffectKey* key = p->attr().getEffect(eff_data->effectName)){
			cEffect* eff = terScene->CreateScaledEffect(*key,p->avatar()->GetModelPoint(),true);

			MatXf pos = (eff_data->needOrientation) ? p->avatar()->matrix() : MatXf(Mat3f::ID,p->avatar()->matrix().trans());
			eff->SetPosition(pos);
		}
	}

	setLifeTime(attr().LifeTime);
	xassert(attr().LifeTime);
}

void terBuildingUninstall::Start()
{
	terUnitBase::Start();
	avatar()->Start();
}

void terBuildingUninstall::Quant()
{
	terUnitBase::Quant();

	sight_ = float(lifeTimer_())/float(attr().LifeTime);

	terInterpolationUninstall* p = safe_cast<terInterpolationUninstall*>(avatar());
	p->setSight(sight_);
	p->setHologram(0.5f + sinf(hologram_)*0.5f);
	p->setTexture(Player->HologramPoint);

	hologram_ += 0.1f * M_PI;

	if(!lifeTimer_())
		Kill();
}