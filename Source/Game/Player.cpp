#include "StdAfx.h"

#include "umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"
#include "IUnkObj.h"
#include "ForceField.h"

#include "Runtime.h"

#include "terra.h"

#include "Region.h"

#include "CameraManager.h"

#include "GenericControls.h"

#include "Universe.h"
#include "Config.h"

#include "RigidBody.h"

#include "TrustMap.h"
#include "GenericUnit.h"

#include "RealUnit.h"
#include "RealInterpolation.h"
#include "IronLegion.h"
#include "IronBullet.h"
#include "IronExplosion.h"
#include "IronClusterUnit.h"

#include "IronDigger.h"

#include "Nature.h"

#include "Triggers.h"

#include "UniverseInterface.h"

#include "AIPrm.h"

#include "Mutation.h"
#include "ClusterFind.h"
#include "Squad.h"
#include "IronPort.h"

#include "PerimeterSound.h"

#include "TerrainMaster.h"
#include "BuildingBlock.h"
#include "BuildMaster.h"
#include "IronBuilding.h"

#include "FrameLegion.h"
#include "FramePlant.h"
#include "SecondLegion.h"
#include "WarBuilding.h"
#include "FrameCore.h"
#include "FrameField.h"

#include "GenericFilth.h"
#include "AIMain.h"
#include "GameShell.h"

#include "GeoControl.h"
#include "ExternalShow.h"

#include "CorpseDynamic.h"

#include "FilthAnts.h"
#include "FilthWasp.h"
#include "FilthDragon.h"
#include "FilthGhost.h"
#include "FilthEye.h"
#include "FilthCrow.h"
#include "FilthDaemon.h"
#include "FilthRat.h"
#include "FilthWorm.h"
#include "FilthShark.h"
#include "FilthVolcano.h"
#include "..\ht\ht.h"

//-------------------------------------
terPlayer::terPlayer(const PlayerData& playerData) 
: structure_column_(vMap.V_SIZE), 
energy_region_(vMap.V_SIZE),
core_column_(vMap.V_SIZE),
field_region_(vMap.V_SIZE),
defenceMap_(vMap.H_SIZE, vMap.V_SIZE),
playerStrategyIndex_(0)
{
	MTINIT(lock_burn_zeroplast);
	playerID_ = playerData.playerID;
	clan_ = playerData.clan != -1 ? playerData.clan : playerID_;
	setDifficulty(DIFFICULTY_HARD);
	isWorld_ = playerData.realPlayerType == REAL_PLAYER_TYPE_WORLD;
	name_ = playerData.name();
	compAndUserID_ = playerData.compAndUserID;
	handicap_ = playerData.handicap/100.;

	controlEnabled_ = true;
	ignoreIntfCommands = false;

	UnitCount = 0;

	setColorIndex(playerData.colorIndex);
	belligerent_ = playerData.belligerent;
	
	const AttributeBase* coreAttr = unitAttribute(UNIT_ATTRIBUTE_CORE);
	EnergyData.setEnergyPerArea(coreAttr->MakeEnergy/(10*sqr(coreAttr->ZeroLayerRadius)*M_PI));
	
	TrustMap = new terTerraformDispatcher(this);

	RegionPoint = new RegionMetaDispatcher(2,vMap.V_SIZE);

	ZeroRegionPoint = (*RegionPoint)[0];
	AbyssRegionPoint = (*RegionPoint)[1];

	{
		MetaRegionLock lock(RegionPoint);
		AbyssRegionPoint->setToolzerRadius(toolzer_radius_fixed);
	}

	frame_ = 0;
	
	buildingBlockRequest_ = false;
	
	HologramPoint = terVisGeneric->CreateTexture(terTextureHologram);

	RegionSize = 0;
	RegionCount = 0;

	PrevBrigadierWorking = 0;
	BrigadierWorking = 0;

	totalDefenceMode_ = false;

	active_ = false;
	isAI_ = false;

#ifndef _FINAL_VERSION_
	if(active()){
		netLog.close();
		XBuffer name;
		name.SetRadix(16);
		name < "Client" <= playerID();
		netLog.open(name, XS_OUT);
	}
#endif

	pTextureUnitSelection = terVisGeneric->CreateTexture(terTextureUnitSelection);
	rasterize_region_on_next_quant = false;

	enemyPlayer_ = observedPlayer_ = chooseEnemyPlayer_ = 0;
	chooseEnemyDistance_ = FLT_INF;
	chooseEnemyIndex_ = 0;

	defenceMapPlayer_ = 0;
	defenceMapGunIndex_ = -1;

	lastFramePosition_ = Vect2f::ZERO;

	voiceDispatcher_.setOwner(this);
}

terPlayer::~terPlayer()
{
	MTDONE(lock_burn_zeroplast);
	RELEASE(pTextureUnitSelection);
	clusters_.clear();

	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		(*ui)->Kill();
	
	removeUnits();
	
	delete RegionPoint;
	delete TrustMap;

	HologramPoint->Release();

	LightList::iterator i_light;
	FOR_EACH(Lights,i_light)
		(*i_light)->Release();
}

void terPlayer::setColorIndex(int colorIndex)
{
	const PlayerColor& pc = playerColors[colorIndex_ = colorIndex];
	unitColor_ = sColor4f(pc.unitColor);
	fieldColor_ = sColor4f(pc.fieldColor);
	zeroLayerColor_ = sColor4f(pc.zeroLayerColor);
}

void terPlayer::setTriggerChains(const SavePlayerManualData& data)
{
	if(!data.triggerChainNames.empty()){
		SavePlayerManualData::TriggerChainNames::const_iterator i;
		FOR_EACH(data.triggerChainNames, i){
			triggerChains_.push_back(TriggerChain());
			triggerChains_.back().load(*i);
			triggerChains_.back().initializeCameraTrigger(playerID());
		}
	}
	else
		triggerChains_.push_back(data.triggerChainOld);
}

void terPlayer::refreshCameraTrigger(const char* triggerName)
{
	if(triggerChains_.empty())
		return;
	TriggerChain& strategy = triggerChains_.front();
	Trigger* trigger = strategy.find(triggerName);
	if(!trigger->action)
		trigger->action = new ActionSetCamera;
	XBuffer name;
	name < triggerName <= playerStrategyIndex();
	safe_cast<ActionSetCamera*>(trigger->action())->cameraSplineName = name;
	trigger->setState(Trigger::CHECKING);
}

void terPlayer::setAI(bool isAI)
{
	if(isWorld())
		return;
	isAI_ = isAI;
}

int terPlayer::registerUnitID(int unitID) 
{ 
	UnitCount = max(unitID, UnitCount); 
	UnitList::iterator ui;
	FOR_EACH(Units, ui)
		if(unitID == (*ui)->unitID())
			return ++UnitCount;
	return unitID;
}

const AttributeBase* terPlayer::unitAttribute(terUnitAttributeID id) const 
{ 
	const AttributeBase* attr = attributeLibrary().find(AttributeIDBelligerent(id, belligerent())); 
	if(!attr)
		attr = attributeLibrary().find(AttributeIDBelligerent(id, BELLIGERENT_NONE)); 
	xassert(attr);
	return attr;
}

void terPlayer::Quant()
{
	start_timer_auto(PlayerQuant, 2);
	//EconomicQuant();
	MTL();
	stats.update(GetFrameStats(), energyData());
	if(rasterize_region_on_next_quant){
		RasterizeRegion();
		rasterize_region_on_next_quant=false;
	}

	voiceDispatcher_.quant();

	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		if((*ui)->alive()){
			log_var("Quant");
			log_var(getEnumName((*ui)->attr().ID));
			(*ui)->Quant();
			log_var(vMap.getChAreasInformationCRC());
			log_var(terLogicRNDfrnd());
			log_var((*ui)->position());
		}

	rebuildDefenceMapQuant();
	chooseEnemyQuant();
	
	if(frame())
		lastFramePosition_ = frame()->position2D();
}

void terPlayer::DestroyLink()
{
	MTL();
	destroyLinkEconomic();
	
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		(*ui)->DestroyLink();

//	TrustMap->EraseQuant();

	if(frame() && !frame()->alive())
		clearFrame();
}

void terPlayer::DeleteQuant()
{
	MTAuto lock(HTManager::instance()->GetLockDeleteUnit());
	UnitList::iterator ui;
	ui = Units.begin();
	while(ui != Units.end()){
		if((*ui)->alive())
			ui++;
		else{
			terUnitBase* unit = *ui;
			if(!unit->dead()){
				unit->makeDead();
				ui++;
			}
			else{
				CUNITS_LOCK(this);
				ui = Units.erase(ui);
				removeUnit(unit);

				unit->DeleteInterpolator();
				HTManager::instance()->DeleteUnit(unit);
			}
		}			
	}
}

void terPlayer::MoveQuant()
{
	MTL();
	if(PrevBrigadierWorking != BrigadierWorking){
		PrevBrigadierWorking = BrigadierWorking;
		if(active()){
			if(BrigadierWorking)
				soundEvent(SOUND_VOICE_TERRAFORMING_STARTED);
			else
				soundEvent(SOUND_VOICE_TERRAFORMING_FINISHED);
		}
	}
	BrigadierWorking = 0;

	UnitList::iterator ui;
	FOR_EACH(Units,ui)
	if((*ui)->alive())
	{
		log_var("MoveQuant");
		log_var(getEnumName((*ui)->attr().ID));
		(*ui)->MoveQuant();
		log_var(vMap.getChAreasInformationCRC());
		log_var(terLogicRNDfrnd());
	}
}

void terPlayer::AvatarQuant()
{
	MTL();
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
	if((*ui)->alive())
	{
		(*ui)->AvatarQuant();
		(*ui)->AvatarInterpolation();
	}
}

void terPlayer::PrepareQuant()
{
	marked_ = false;
}

void terPlayer::ShowInfo()
{
	MTG();
	CUNITS_LOCK(this);
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		if((*ui)->alive())
			(*ui)->ShowInfo();

	quantZeroplast();
//	if(ActivePlayerFlag)
//		TrustMap->Show();
}

//----------------------------------

terUnitBase* terPlayer::buildUnit(terUnitAttributeID id)
{
	terUnitBase* p = createUnit(UnitTemplate(unitAttribute(id), this));
	p->setRealModel(0, 1);
	addUnit(p);
	return p;
}

void terPlayer::addUnit(terUnitBase* unit)
{
	unit->Player = this;
	(terUnitID&)*unit = terUnitID(++UnitCount, playerID());


	CUNITS_LOCK(this);
	Units.push_back(unit);

	if(unit->attr().isBuilding()){// && unit->isBuilding()
		BuildingList[unit->attr().ID].push_back(safe_cast<terBuilding*>(unit));
	}

//	if(active() && (unit->attr().ID == UNIT_ATTRIBUTE_TERRAIN_MASTER ||
//		unit->attr().ID == UNIT_ATTRIBUTE_BUILD_MASTER)){
//			universe()->DeselectAll();
//			universe()->SelectUnit(unit);
//	}			

	if(unit->attr().ID == UNIT_ATTRIBUTE_FRAME && !frame_)
		frame_ = safe_cast<terFrame*>(unit);

	universe()->checkEvent(EventUnitPlayer(Event::CREATE_OBJECT, unit, this));
}

void terPlayer::removeUnit(terUnitBase* unit)
{
	universe()->checkEvent(EventUnitPlayer(Event::DESTROY_OBJECT, unit, this));

	if(unit->attr().isBuilding()){
		terBuildingList& list = BuildingList[unit->attr().ID];
		list.erase(remove(list.begin(), list.end(), safe_cast<terBuilding*>(unit)), list.end());

		if(totalDefenceMode_ && unit->attr().ID == UNIT_ATTRIBUTE_CORE && list.empty())
			totalDefenceMode_ = false;
	}

	CUNITS_LOCK(this);
	Units.erase(remove(Units.begin(), Units.end(), unit), Units.end());

	if(frame_ == unit)
		clearFrame();
}

void terPlayer::clearFrame() 
{ 
	if(!frame())
		return;

	frame_ = 0; 

	if(gameShell->CurrentMission.isMultiPlayer()){
		if(active())
			_pShellDispatcher->OnInterfaceMessage(UNIVERSE_INTERFACE_MESSAGE_GAME_DEFEAT, false); 
		else{
			bool victory = true;
			PlayerVect::iterator pi;
			FOR_EACH(universe()->Players, pi)
				if((*pi)->frame() && (*pi)->clan() != clan()){
					victory = false;
					break;
				}
			if(victory)
				_pShellDispatcher->OnInterfaceMessage(UNIVERSE_INTERFACE_MESSAGE_GAME_VICTORY, false); 
		}
	}
}

void terPlayer::killAllUnits()
{
	MTL();
	UnitList::iterator ui;
	FOR_EACH(Units, ui)
		(*ui)->Kill();
}

void terPlayer::removeUnits()
{
	clusters_.clear();

	while(!Units.empty()){
		terUnitBase* unit = Units.front();
		removeUnit(unit);
		delete unit;
	}
}

terUnitBase* terPlayer::createUnit(const UnitTemplate& data)
{
	switch(data.attribute()->ClassID){
		case UNIT_CLASS_ID_NONE:
			return 0;

		case UNIT_CLASS_ID_PROJECTILE_DEBRIS:
			return (new terProjectileDebris(data));
		case UNIT_CLASS_ID_PROJECTILE_DEBRIS_CRATER:
			return (new terProjectileDebrisCrater(data));

		case UNIT_CLASS_ID_SCUM_STORM:
			return (new terProjectileScumStorm(data));

		case UNIT_CLASS_ID_IRON_DESTRUCTION_CRATER:
			return (new terDestructionCraterType(data));
		case UNIT_CLASS_ID_IRON_DEBRIS_CRATER:
			return (new terDebrisCraterType(data));
		case UNIT_CLASS_ID_CRATER:
			return (new terCrater(data));

		case UNIT_CLASS_ID_MONK:
			xassert(0);
			return NULL;

		case UNIT_CLASS_ID_TRUCK:
			return (new terUnitTruck(data));

		case UNIT_CLASS_ID_NATURE_MOUNTAIN:
			return (new terNatureMountain(data));
		case UNIT_CLASS_ID_NATURE_WORM:
			return (new terNatureWorm(data));
		case UNIT_CLASS_ID_NATURE_RIFT:
			return (new terNatureRift(data));
		case UNIT_CLASS_ID_NATURE_CLEFT:
			return (new terNatureCleft(data));
		case UNIT_CLASS_ID_NATURE_FACE:
			return (new terNatureFace(data));

		case UNIT_CLASS_ID_STATIC_NATURE:
			return (new terNatureObject(data));

		case UNIT_CLASS_ID_PROJECTILE_BULLET:
			return (new terProjectileBullet(data));

		case UNIT_CLASS_ID_PROJECTILE_MISSILE:
			return (new terProjectileMissile(data));
		case UNIT_CLASS_ID_PROJECTILE_UNDERGROUND:
			return (new terProjectileUnderground(data));

		case UNIT_CLASS_ID_SQUAD:
			return (new terUnitSquad(data));

		case UNIT_CLASS_ID_FALL_TREE:
			return (new terNatureFallTree(data));
		case UNIT_CLASS_ID_FALL_STRUCTURE:
			return (new terFallStructure(data));

		case UNIT_CLASS_ID_UNIT_CORPSE:
			return (new terUnitCorpse(data));

		case UNIT_CLASS_ID_CORRIDOR_ALPHA:
			return (new terCorridorAlpha(data));
		case UNIT_CLASS_ID_CORRIDOR_OMEGA:
			return (new terCorridorOmega(data));

		case UNIT_CLASS_ID_FRAME:
			return (new terFrame(data));
		case UNIT_CLASS_ID_TERRAIN_MASTER:
			return (new terUnitTerrainMaster(data));
		case UNIT_CLASS_ID_BUILD_MASTER:
			return (new terUnitBuildMaster(data));
		case UNIT_CLASS_ID_GENERIC_BUILDING:
			return (new terBuilding(data));
		case UNIT_CLASS_ID_BUILDING_ENVIRONMENT: 
			return (new terBuildingEnvironment(data));
		case UNIT_CLASS_ID_BUILDING_BLOCK:
			return (new terUnitBuildingBlock(data));

		case UNIT_CLASS_ID_CORE:
			return (new terProtector(data));
		case UNIT_CLASS_ID_COMMANDER:
			return (new terBuildingCommandCenter(data));

		case UNIT_CLASS_ID_LEGIONARY:
			return (new terUnitLegionary(data));

		case UNIT_CLASS_ID_PLANT:
			return (new terBuildingPlant(data));

		case UNIT_CLASS_ID_BUILDING_ENERGY:
			return (new terBuildingEnergy(data));

		case UNIT_CLASS_ID_BUILDING_MILITARY:
			return (new terBuildingMilitary(data));

		case UNIT_CLASS_ID_BUILDING_POWERED:
			return (new terBuildingPowered(data));

		case UNIT_CLASS_ID_NATURE_TORPEDO:
			return (new terNatureTorpedo(data));
		case UNIT_CLASS_ID_NATURE_FAULT:
			return (new terNatureFault(data));

		case UNIT_CLASS_ID_FILTH_SPOT:
			return new terFilthSpot(data);
		case UNIT_CLASS_ID_FILTH_ANTS:
			return new terFilthAnt(data);
		case UNIT_CLASS_ID_FILTH_WASP:
			return new terFilthWasp(data);
		case UNIT_CLASS_ID_FILTH_GHOST:
			return new terFilthGhost(data);
		case UNIT_CLASS_ID_FILTH_EYE:
			return new terFilthEye(data);
		case UNIT_CLASS_ID_FILTH_CROW:
			return new terFilthCrow(data);
		case UNIT_CLASS_ID_FILTH_DAEMON:
			return new terFilthDaemon(data);
		case UNIT_CLASS_ID_FILTH_DRAGON_HEAD:
			return new terFilthDragonHead(data);
		case UNIT_CLASS_ID_FILTH_DRAGON_BODY:
			return new terFilthDragonBody(data);
		case UNIT_CLASS_ID_FILTH_RAT:
			return new terFilthRat(data);
		case UNIT_CLASS_ID_FILTH_WORM:
			return new terFilthWorm(data);
		case UNIT_CLASS_ID_FILTH_SHARK:
			return new terFilthShark(data);
		case UNIT_CLASS_ID_FILTH_VOLCANO:
			return new terFilthVolcano(data);

		case UNIT_CLASS_ID_ALPHA_POTENTIAL:
			return new terUnitAplhaPotential(data);

		case UNIT_CLASS_ID_BUILDING_HOLOGRAM:
			return new terBuildingHologram(data);

		case UNIT_CLASS_ID_BUILDING_UNINSTALL:
			return new terBuildingUninstall(data);

		case UNIT_CLASS_GEO_INFLUENCE:
			return new terGeoInfluence(data);
		case UNIT_CLASS_GEO_BREAK:
			return new terGeoBreak(data);
		case UNIT_CLASS_GEO_FAULT:
			return new terGeoFault(data);
		case UNIT_CLASS_GEO_HEAD:
			return new terGeoHead(data);
		case UNIT_CLASS_CORPSE_DYNAMIC:
			return new terCorpseDynamic(data);
		default:
			xassert(0 && "terCreateUnitData : Can't create Unit Data");
			break;
	}
	return 0;
}


//---------------------------------------------------------

void terPlayer::incomingCommandRegion(const netCommand4G_Region& reg)
{
	//Region Buffer можно сделать глобальным
	XBuffer RegionBuffer(5000, 1);
	RegionBuffer.init();
	RegionBuffer.write(reg.pData_, reg.dataSize_);
	RegionBuffer.set(0);
	MetaRegionLock lock(RegionPoint);
	RegionPoint->load(RegionBuffer);
	if(MT_IS_LOGIC())
		RasterizeRegion();
	else
		RasterizeRegionOnNextLogicQuant();
}

terUnitBase* terPlayer::traceUnit(const Vect2f& pos)
{
	CUNITS_LOCK(this);
	Vect3f v0,v1;
	terCamera->calcRayIntersection(pos.x, pos.y, v0, v1);
	Vect3f v01 = v1 - v0;
	
	float dist, distMin = FLT_INF;
	terUnitBase* unitMin = 0;
	
	UnitList::iterator i_unit;
	FOR_EACH(Units, i_unit){
		terUnitBase* unit = *i_unit;
		if(unit->alive() && unit->selectAble() && unit->attr().ID != UNIT_ATTRIBUTE_SQUAD){
			if(unit->avatar() && unit->avatar()->GetModelPoint()){
				if(safe_cast<cObjectNode*>(unit->avatar()->GetModelPoint())->Intersect(v0,v1) &&
					distMin > (dist = unit->position().distance2(v0))){
					distMin = dist;
					unitMin = unit;
				}
			}
			else{
				Vect3f v0x = unit->position() - v0;
				Vect3f v_normal, v_tangent;
				decomposition(v01, v0x, v_normal, v_tangent);
				if(v_tangent.norm2() < sqr(unit->radius()) && distMin > (dist = v_normal.norm2())){
					distMin = dist;
					unitMin = unit;
				}
			}
		}
	}
	
	return unitMin;
}

void terPlayer::ChangeRegion(XBuffer& out)
{
	universe()->sendCommand(netCommand4G_Region(playerID(), out));
}

//-------------------------------------
void terPlayer::RasterizeRegion()
{
	TrustMap->Clear();
	MetaRegionLock lock(RegionPoint);
	ScanRegion(ZeroRegionPoint.data());
	ScanAbyssRegion(AbyssRegionPoint.data());
}

void terPlayer::ScanRegion(RegionDispatcher* region)
{
	terTerraformAddOp op(*TrustMap, TERRAFORM_TYPE_FULL, region->ID());
	terTerraformAddOp opb(*TrustMap, TERRAFORM_TYPE_BORDER, region->ID());
	region->scanRecursive(opb, op,TERRAFORM_ELEMENT_SIZE);
}

void terPlayer::ScanAbyssRegion(RegionDispatcher* region)
{
	terTerraformAddOp op(*TrustMap, TERRAFORM_TYPE_ABYSS_FULL, region->ID());
	terTerraformAddOp opb(*TrustMap, TERRAFORM_TYPE_ABYSS_BORDER, region->ID());
	region->scanRecursive(opb, op,TERRAFORM_ELEMENT_SIZE);
}

//-------------------------------------------
terUnitBase* terPlayer::findUnit(terUnitAttributeID id)
{
	MTL();
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		if((*ui)->attr().ID == id)
			return (*ui);
		
	return 0;
}

terUnitBase* terPlayer::findUnit(terUnitAttributeID id, const Vect2f& nearPosition, float distanceMin)
{
	MTL();
	terUnitBase* bestUnit = 0;
	float dist, bestDist = FLT_INF;
	UnitList::iterator ui;
	if(id != UNIT_ATTRIBUTE_ANY){
		FOR_EACH(Units,ui)
			if((*ui)->attr().ID == id && bestDist > (dist = nearPosition.distance2((*ui)->position2D())) && dist > sqr(distanceMin)){
				bestDist = dist;
				bestUnit = *ui;
			}
	}
	else{
		FOR_EACH(Units,ui)
			if((*ui)->attr().ID < UNIT_ATTRIBUTE_LEGIONARY_MAX && bestDist > (dist = nearPosition.distance2((*ui)->position2D())) && dist > sqr(distanceMin)){
				bestDist = dist;
				bestUnit = *ui;
			}
	}
	return bestUnit;
}

terUnitBase* terPlayer::findUnitByUnitClass(int unitClass, const Vect2f& nearPosition, float distanceMin)
{
	MTL();
	terUnitBase* bestUnit = 0;
	float dist, bestDist = FLT_INF;
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		if(((*ui)->unitClass() & unitClass) && bestDist > (dist = nearPosition.distance2((*ui)->position2D())) 
		  && ((*ui)->isConstructed() || (*ui)->isUpgrading()) && dist > sqr(distanceMin)){
			bestDist = dist;
			bestUnit = *ui;
		}
	return bestUnit;
}

terUnitBase* terPlayer::findUnit(unsigned int unit_id)
{
	MTL();
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		if((*ui)->unitID() == unit_id)
			return (*ui);

	return 0;
}

terUnitBase* terPlayer::findUnitByLabel(const char* label)
{
	MTAuto lock(UnitsLock());
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		if(!strcmp((*ui)->label(), label))
			return (*ui);
		
		return 0;
}


//----------------------------------------------

void terPlayer::RefreshAttribute()
{
	MTL();
	
	setColorIndex(colorIndex_);
	terMapPoint->SetZeroplastColor(playerID(), zeroLayerColor_);

	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		(*ui)->RefreshAttribute();
}



//-----------------------------------------------
class ShowOp
{
public:
	ShowOp(sColor4c color) : color_(color){}
	void operator()(int x, int y){ show_vector(Vect3f(x, y, vMap.hZeroPlast), color_); }
private:
	sColor4c color_;
};

static void debugInfoBorderCall(void* data,Vect2f& p)
{
	show_vector(to3D(p, vMap.hZeroPlast), BLUE);
}

void terPlayer::showDebugInfo()
{
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		(*ui)->showDebugInfo();

	if(showDebugPlayer.field_regions)
		field_region_.getEditColumn().show(CYAN);

	if(showDebugPlayer.field_region_border){
		ClusterList::const_iterator ci;
		FOR_EACH(clusters_, ci){
			if(!ci->fieldCluster())
				continue;
			const Vect2sVect& border = ci->fieldCluster()->border();
			Vect2sVect::const_iterator pi;
			FOR_EACH(border, pi)
				show_vector(to3D(*pi, FieldCluster::ZeroGround), vMap.leveled(vMap.offsetBuf(pi->x, pi->y)) ? BLUE : RED);
		}
	}

	if(showDebugPlayer.energyColumn)
		energyColumn().show(CYAN);

	if(showDebugPlayer.coresColumn)
		structure_column_.show(GREEN);

	if(showDebugPlayer.zeroColumn == 1)
		ZeroRegionPoint->getEditColumn().show(GREEN);
	if(showDebugPlayer.zeroColumn == 2)
		ZeroRegionPoint->getRasterizeColumn().show(BLUE);
	if(showDebugPlayer.zeroColumn == 3)
		ZeroRegionPoint->getBorder(debugInfoBorderCall,NULL, true);
	if(showDebugPlayer.zeroColumn == 4)
		ZeroRegionPoint->scanRecursive(ShowOp(GREEN), ShowOp(RED), TERRAFORM_ELEMENT_SIZE);
	if(showDebugPlayer.zeroColumn == 5)
		ZeroRegionPoint->scanRecursive(ShowOp(GREEN), ShowOp(RED), 1);

	if(showDebugPlayer.trustMap)
		TrustMap->Show();

	if(showDebugPlayer.defenceMap && active())
		defenceMap_.showDebugInfo();

	if(showDebugPlayer.enemyPlayer && frame() && enemyPlayer() && enemyPlayer()->frame()){
		float t = 0.8f;
		show_line(frame()->position(), frame()->position()*(1 - t) + enemyPlayer()->frame()->position()*t, unitColor());
		XBuffer buf;
		buf < "ID: " <= playerID() < ", clan: " <= clan();
		show_text(frame()->position(), buf, unitColor());
	}
}

void terPlayer::WriteDebugInfo(XBuffer& buf)
{
	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		(*ui)->WriteDebugInfo(buf);
}

void terPlayer::SetActivePlayer()
{
	active_ = 1;

	ClusterList::iterator ci;
	FOR_EACH(clusters_, ci)
		if(ci->fieldCluster())
			ci->fieldCluster()->setTransparent(true);
}

void terPlayer::SetDeactivePlayer()
{
	active_ = 0;

	ClusterList::iterator ci;
	FOR_EACH(clusters_, ci)
		if(ci->fieldCluster())
			ci->fieldCluster()->setTransparent(false);
}

//--------------------------------

void terPlayer::UpdateSkinColor()
{
	MTAuto lock(UnitsLock());

	UnitList::iterator ui;
	FOR_EACH(Units,ui)
		(*ui)->UpdateSkinColor();
}

//-------------------------------------------------------------
void terPlayer::loadWorld(const SavePrm& data)
{
	SaveUnitDataList::const_iterator i;
	FOR_EACH(data.environment.objects, i){
		terUnitBase* unit = buildUnit((*i)->attributeID);
		unit->universalLoad(*i);
		unit->Start();
	}

	FOR_EACH(data.filth.objects, i){
		terUnitBase* unit = buildUnit((*i)->attributeID);
		unit->universalLoad(*i);
		unit->Start();
	}

	FOR_EACH(data.nobodysBuildings.objects, i){
		terUnitBase* unit = buildUnit((*i)->attributeID);
		unit->universalLoad(*i);
		unit->Start();
	}

	FOR_EACH(data.worldObjects.alphaPotentials, i){
		terUnitBase* unit = buildUnit((*i)->attributeID);
		unit->universalLoad(*i);
		unit->Start();
	}
}

void terPlayer::saveWorld(SavePrm& data)
{
	MTL();
	UnitList::const_iterator ui;
	FOR_EACH(Units,ui){
		switch((*ui)->attr().ID){
		case UNIT_ATTRIBUTE_STATIC_NATURE:
			data.environment.objects.push_back((*ui)->universalSave(0));
			break;
		case UNIT_ATTRIBUTE_FILTH_SPOT:
		case UNIT_ATTRIBUTE_GEO_INFLUENCE:
		case UNIT_ATTRIBUTE_GEO_BREAK:
		case UNIT_ATTRIBUTE_GEO_FAULT:
		case UNIT_ATTRIBUTE_GEO_HEAD:
			data.filth.objects.push_back((*ui)->universalSave(0));
			break;
		case UNIT_ATTRIBUTE_ALPHA_POTENTIAL:
			data.worldObjects.alphaPotentials.push_back((*ui)->universalSave(0));
			break;
		default:
			if((*ui)->attr().isBuilding())
				data.nobodysBuildings.objects.push_back((*ui)->universalSave(0));
		}
	}
}

void terPlayer::universalLoad(const SavePlayerData& data)
{
	if(data.frame){
		terUnitBase* unit = buildUnit(data.frame->attributeID);
		unit->universalLoad(data.frame);
		unit->Start();
	}

	SaveUnitDataList::const_iterator i;
	FOR_EACH(data.buildings, i){
		terUnitBase* building = buildUnit((*i)->attributeID);
		building->universalLoad(*i);
		building->Start();
	}

	FOR_EACH(data.commonObjects, i){
		terUnitBase* unit = buildUnit((*i)->attributeID);
		unit->universalLoad(*i);
		unit->Start();
	}

	FOR_EACH(data.catchedFrames, i){
		terUnitBase* frame;
		terBelligerent frameBelligerent = safe_cast<const SaveUnitFrameData*>(&**i)->belligerent;
		if(frameBelligerent != belligerent()){
			//attributes_.set(frameBelligerent, unitAttributeDataTable);
			frame = buildUnit(UNIT_ATTRIBUTE_FRAME);
			//attributes_.set(belligerent(), unitAttributeDataTable);
		}
		else
			frame = buildUnit(UNIT_ATTRIBUTE_FRAME);
		frame->universalLoad(*i);
		frame->Start();
	}

	(SavePlayerStats&)stats = data.playerStats;

	if(!data.currentTriggerChains.empty()){ // userSave
		triggerChains_ = data.currentTriggerChains;
		//TriggerChains::iterator tci;
		//FOR_EACH(triggerChains_, tci)
		//	tci->initializeCameraTrigger(playerID());
	}
}

void terPlayer::universalSave(SavePlayerData& data, bool userSave)
{
	if(frame())
		data.frame = frame()->universalSave(data.frame);
	
	for(int i = 0;i < UNIT_ATTRIBUTE_STRUCTURE_MAX;i++){
		terBuildingList::iterator bi;
		FOR_EACH(buildingList(i),bi){
			if((*bi)->dockMode() == DOCK_MODE_NONE){
				data.buildings.push_back((*bi)->universalSave(0));
				xassert(data.buildings.back());
			}
			else 
				xassert_s(0 && "Игнорируется запись здания: ", (*bi)->attr().internalName());
		}
	}
	
	UnitList::iterator ui;
	FOR_EACH(Units, ui){
		if((*ui)->attr().ID == UNIT_ATTRIBUTE_FRAME && *ui != frame()){
			data.catchedFrames.push_back((*ui)->universalSave(0));
		}
		else if((*ui)->attr().saveAsCommonObject){
			data.commonObjects.push_back((*ui)->universalSave(0));
		}
	}

	data.compAndUserID = compAndUserID_;
	data.playerStats = stats;

	if(userSave)
		data.currentTriggerChains = triggerChains_;
	else
		data.currentTriggerChains.clear();
}


//-----------------------------------------------------------------

void terPlayer::ClusterPick(CommandID id,unsigned int data)
{
	if(!frame())
		return;

	frame()->commandOutcoming(UnitCommand(id, data, COMMAND_SELECTED_MODE_NEGATIVE));
}

//--------------------------------------------------

struct terMissileTestOperator
{
	float Radius;
	Vect3f Position;

	MatXf Matrix;
 	class RigidBody* SourcePoint;
 	class RigidBody* TargetPoint;
 	class RigidBody* BodyPoint;

 	class RigidBody* HitPoint;
 	
	terMissileTestOperator(class RigidBody* object,class RigidBody* source,class RigidBody* target)
	{
		BodyPoint = object;
		SourcePoint = source;
		TargetPoint = target;

		Matrix = BodyPoint->matrix();
		Position = BodyPoint->position();
		Radius = BodyPoint->radius();

		HitPoint = NULL;
	}

	int operator()(const terUnitBase* p)
	{
		if(p->alive() && (p->collisionGroup() & COLLISION_GROUP_REAL)){
			RigidBody* b = p->GetRigidBodyPoint();
			if(b != SourcePoint && b->prm().unit_type == RigidBodyPrm::UNIT){
				MatXf X12 = b->matrix();
				if(Position.distance2(X12.trans()) < (Radius + b->radius()) * (Radius + b->radius())){
					X12.invert();
					X12.postmult(Matrix);
					if(universe()->multiBodyDispatcher().test(*BodyPoint,*b,X12,false)){
						HitPoint = b;
						if(b == TargetPoint)
							return 0;
					}
				}
			}
		}
		return 1;
	}
};

RigidBody* GetMissileTest(RigidBody* object,RigidBody* source,RigidBody* target)
{
	float x,y,r;
	x = object->position().x;
	y = object->position().y;
	r = object->radius();

	terMissileTestOperator op(object,source,target);
	universe()->UnitGrid.ConditionScan(round(x), round(y), round(r), op);
	return (op.HitPoint);
}

void terPlayer::showFireCircles(terUnitAttributeID attribute_id, const Vect2f& point)
{
	gbCircleShow->SetNoInterpolationLock();
	CUNITS_LOCK(this);
	const UnitList& unit_list=units();
	UnitList::const_iterator ui;
	FOR_EACH(unit_list,ui)
		if((*ui)->alive() && (*ui)->attr().ID == attribute_id && point.distance2((*ui)->position2D()) < sqr(2*(*ui)->attr().fireRadius()))
			(*ui)->ShowCircles();
	gbCircleShow->SetNoInterpolationUnlock();
}

void terPlayer::showConnectionCircle(const Vect2f& point, bool includeFrame)
{
	float dist, distBest = FLT_INF;
	terUnitBase* unitBest = 0;
	CUNITS_LOCK(this);
	const UnitList& unit_list=units();
	UnitList::const_iterator ui;
	FOR_EACH(unit_list,ui)
		if((*ui)->alive() && (dist = point.distance2((*ui)->position2D())) < sqr((*ui)->attr().ConnectionRadius)){
			if((*ui)->attr().ID == UNIT_ATTRIBUTE_FRAME){
				if(includeFrame){
					distBest = dist;
					unitBest = *ui;
					break;
				}
			}
			else if(safe_cast<terBuilding*>(*ui)->isConnected() && distBest > dist){
				distBest = dist;
				unitBest = *ui;
			}
		}

	if(unitBest)
		terCircleShowGraph(unitBest->position(), unitBest->attr().ConnectionRadius, circleColors.connectionRadius);
}

int terEnergyBorderCheck(int x,int y)
{
	PlayerVect::iterator pi;
	FOR_EACH(universe()->Players, pi){
		if((*pi)->energyColumn().filled(x,y))
			return 0;
	}
	return 1;
}

//-----------------------------------------------------------

void terPlayer::ClusterActionPoint(Vect3f v,CommandSelectionMode mode,CommandID command_id,unsigned int data)
{
	if(!frame())
		return;

	frame()->commandOutcoming(UnitCommand(command_id, v, data, mode));
}

int terPlayer::countUnits(terUnitAttributeID id) const
{
	MTL();
	if(id < UNIT_ATTRIBUTE_STRUCTURE_MAX)
		return buildingList(id).size();

	int count  = 0;
	UnitList::const_iterator ui;
	FOR_EACH(Units, ui)
		if((*ui)->attr().ID == id)
			++count;
	
	return count;
}

int terPlayer::countBuildingsConstructed(terUnitAttributeID id) const
{
	int count  = 0;

	terBuildingList::const_iterator i;
	const terBuildingList& lst = buildingList(id);
	FOR_EACH(lst, i)
		if((*i)->isConstructed())
			++count;
		
	return count;
}

int terPlayer::countBuildingsPowered(terUnitAttributeID id) const
{
	int count  = 0;
	
	terBuildingList::const_iterator i;
	const terBuildingList& lst = buildingList(id);
	FOR_EACH(lst, i)
		if((*i)->isBuildingPowerOn() && (*i)->isConstructed())
			++count;
		
	return count;
}

void terPlayer::chooseEnemyQuant()
{
	if(universe()->Players.size() <= 3){
		enemyPlayer_ = universe()->Players[playerID() ? 0 : 1];
		return;
	}

	if(!enemyPlayer_){
		PlayerVect::iterator pi;
		FOR_EACH(universe()->Players, pi)
			if(!(*pi)->isWorld() && (*pi)->frame()){
				if((*pi)->clan() == clan())
					companions_.push_back(*pi);
				else if(!enemyPlayer_)
					enemyPlayer_ = observedPlayer_ = chooseEnemyPlayer_ = *pi;
			}
		if(!enemyPlayer_)
			enemyPlayer_ = observedPlayer_ = chooseEnemyPlayer_ = universe()->worldPlayer();
		return;
	}

	Vect2f position = Vect2f::ZERO;
	int index = chooseEnemyIndex_++;
	terBuildingList& cores = observedPlayer_->buildingList(UNIT_ATTRIBUTE_CORE);
	if(index < cores.size()){
		position = cores[index]->position2D();
	}
	else if((index -= cores.size()) < observedPlayer_->squadList().size()){
		terUnitSquad* squad = observedPlayer_->squadList()[index];
		if(squad)
			position = squad->position2D();
		else
			return;
	}
	else{
		bool log = 0;
		for(int i = observedPlayer_->playerID() + 1;;){
			if(i == universe()->Players.size()){
				enemyPlayer_ = chooseEnemyPlayer_;
				chooseEnemyDistance_ = FLT_INF;
				i = 0;
				if(log)
					break;
				log = true;
			}
			else{
				terPlayer* player = universe()->Players[i++];
				if(!player->isWorld() && player->clan() != clan() && player->frame()){
					observedPlayer_ = player;
					break;
				}
			}
		}
		chooseEnemyIndex_ = 0;
		return;
	}

	terUnitBase* unit = findUnitByUnitClass(UNIT_CLASS_FRAME | UNIT_CLASS_STRUCTURE | UNIT_CLASS_STRUCTURE_GUN, position);
	float dist;
	if(unit && chooseEnemyDistance_ > (dist = position.distance2(unit->position2D()))){
		chooseEnemyDistance_ = dist;
		chooseEnemyPlayer_ = observedPlayer_;
	}
}

void terPlayer::rebuildDefenceMapQuant()
{
	if(!defenceMap_.recalcMapQuant())
		return;

	PlayerVect& players = universe()->Players;

	if(defenceMapPlayer_){
		if(defenceMapGunIndex_ == -1){
			defenceMap_.analizeField(defenceMapPlayer_->playerID());
			defenceMapGunIndex_ = 0;
			return;
		}
		int index = defenceMapGunIndex_++;
		for(int id = UNIT_ATTRIBUTE_LASER_CANNON; id <= UNIT_ATTRIBUTE_GUN_SUBCHASER; id++){
			terBuildingList& guns = defenceMapPlayer_->buildingList(id);
			if(index >= guns.size())
				index -= guns.size();
			else{
				defenceMap_.addGun(guns[index]->position2D(), guns[index]->attr().fireRadius());
				return;
			}
		}
		for(int i = defenceMapPlayer_->playerID() + 1; i < players.size() - 1; i++)
			if(players[i]->clan() != clan()){
				defenceMapPlayer_ = players[i];
				defenceMapGunIndex_ = -1;
				return;
			}
		defenceMapPlayer_ = 0;
	}
	defenceMap_.startRecalcMap();
	defenceMap_.set(0);
	defenceMap_.analizeChaos();

	PlayerVect::iterator pi;
	FOR_EACH(players, pi)
		if((*pi)->clan() != clan()){
			defenceMapPlayer_ = *pi;
			defenceMapGunIndex_ = -1;
			break;
		}
}

terUnitBase* terPlayer::findPathToTarget(DefenceMap& defenceMap, terUnitAttributeID id, terUnitBase* ignoreUnit, const Vect2f& nearPosition, Vect2iVect& path)
{
	MTL();
	xassert(id != UNIT_ATTRIBUTE_ANY && "Недопустим любой юнит для цели атаки");
	UnitList targets;
	UnitList::iterator ui;
	FOR_EACH(Units, ui)
		if((*ui)->attr().ID == id && !(*ui)->includingCluster() && *ui != ignoreUnit && !(*ui)->isUnseen() && (*ui)->isConstructed())
			targets.push_back(*ui);

	return defenceMap.findPathToTarget(nearPosition, targets, path);
}

terUnitBase* terPlayer::findPathToTarget(DefenceMap& defenceMap, int unitClass, terUnitBase* ignoreUnit, const Vect2f& nearPosition, Vect2iVect& path)
{
	MTL();
	UnitList targets;
	UnitList::iterator ui;
	FOR_EACH(Units, ui)
		if(((*ui)->unitClass() & unitClass) && !(*ui)->includingCluster() && *ui != ignoreUnit && !(*ui)->isUnseen() && (*ui)->isConstructed())
			targets.push_back(*ui);

	return defenceMap.findPathToTarget(nearPosition, targets, path);
}

bool terPlayer::findPathToPoint(DefenceMap& defenceMap, const Vect2i& from_w, const Vect2i& to_w, vector<Vect2i>& out_path)
{
	return defenceMap.findPathToPoint(from_w, to_w, out_path);
}

void PlayerStats::checkEvent(const Event& event, int playerID) {
	switch (event.type()) {
		case Event::CREATE_BASE_UNIT:
			{
				const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
				if(eventUnit.player()->playerID() == playerID)
						unitCount++;
			}
			break;
		case Event::KILL_OBJECT:
			{
				const EventUnitMyUnitEnemy& eventUnit = safe_cast_ref<const EventUnitMyUnitEnemy&>(event);
				if (eventUnit.unitMy()->playerID() == playerID) {
					if (eventUnit.unitMy()->attr().ClassID == UNIT_CLASS_ID_LEGIONARY) {
						if (eventUnit.unitMy()->unitClass() & UNIT_CLASS_BASE) {
							unitLost++;
						} else {
							unitLost += eventUnit.unitMy()->attr().damageMolecula[0] + eventUnit.unitMy()->attr().damageMolecula[1] + eventUnit.unitMy()->attr().damageMolecula[2];
						}
					} else if ( isBuilding(eventUnit.unitMy()->unitClass()) ) {
						buildingLost++;
//					} else {
//						unitLost++;
					}
				// temp until splash id
				} else if (eventUnit.unitEnemy() && eventUnit.unitEnemy()->playerID() == playerID) {
					if (eventUnit.unitMy()->Player->isWorld()) {
						scourgeKilled++;
					} else if (eventUnit.unitMy()->attr().ClassID == UNIT_CLASS_ID_LEGIONARY) {
						if (eventUnit.unitMy()->unitClass() & UNIT_CLASS_BASE) {
							unitKilled++;
						} else {
							unitKilled += eventUnit.unitMy()->attr().damageMolecula[0] + eventUnit.unitMy()->attr().damageMolecula[1] + eventUnit.unitMy()->attr().damageMolecula[2];
						}
					} else if ( isBuilding(eventUnit.unitMy()->unitClass()) ) {
						buildingRazed++;
//					} else {
//						unitKilled++;
					}
				}
			}
			break;
		case Event::COMPLETE_BUILDING:
			{
				const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
				if (eventUnit.player()->playerID() == playerID) {
					buildings++;
				}
			}
			break;
		case Event::CAPTURE_BUILDING:
			{
				const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
				if (eventUnit.player()->playerID() == playerID) {
					buildingCaptured++;
				}
			}
			break;
	}
}

void PlayerStats::update(const terFrameStatisticsType& frameStats, terEnergyDataType& energyData) {
	maxZeroedArea = max(maxZeroedArea, frameStats.EnergyArea);
	maxLeveledArea = max(maxLeveledArea, frameStats.ZeroSquareComplete);
	medEfficiency += energyData.efficiency();
	effQuants++;
	energy += energyData.produced();
}
int PlayerStats::getTotalScore(int totalTimeMillis) const {
	return getGeneralTotalScore(totalTimeMillis) + getUnitsTotalScore() * UNITS_WEIGHT + getBuildingsTotalScore() * BUILDINGS_WEIGHT;
}
int PlayerStats::getGeneralTotalScore(int totalTimeMillis) const {
	return energy * ENERGY_WEIGHT / float(totalTimeMillis) + scourgeKilled * SCOURGE_WEIGHT;
}
int PlayerStats::getUnitsTotalScore() const {
	return unitCount + unitKilled - unitLost;
}
int PlayerStats::getBuildingsTotalScore() const {
	return buildings + buildingRazed - buildingLost;
}

void terPlayer::EnergyRegionLockAssert()
{
#ifndef _FINAL_VERSION_
	if(!MT_IS_LOGIC())
	{
		xassert(universe()->EnergyRegionLocker()->is_lock());
	}
#endif
}

bool terPlayer::soundEvent(SoundEventID event_id)
{
	if(const SoundEventSetup* ev = playerSound.findEvent(event_id)){
		return soundEvent(ev);
	}

	return false;
}

bool terPlayer::soundEvent(const SoundEventSetup* ev)
{
	if(ev->activePlayer && !active()) return false;

	if(!ev->isVoice)
		return startSound(ev);
	else
		return voiceDispatcher_.startVoice(*ev);

	return false;
}

bool terPlayer::startSound(const SoundEventSetup* ev) const
{
	if(ev->is3D)
		return SND3DPlaySound(ev->name,&Vect3f(0,0,0));
	else
		return SND2DPlaySound(ev->name);

	return false;
}

bool terPlayer::zerolayer(const Vect2i& point) const 
{ 
	if(energy_region_.getEditColumn().filled(point.x, point.y)) 
		return true;
	PlayerVect::const_iterator pi;
	FOR_EACH(companions_, pi)
		if((*pi)->energy_region_.getEditColumn().filled(point.x, point.y)) 
			return true;
	return false;
}

TriggerChain* terPlayer::getStrategyToEdit()
{
	xassert(!triggerChains_.empty());

	//if(triggerChains_.size() == 1)
	//	return &triggerChains_.front();

	vector<const char*> items;
	TriggerChains::iterator it;
	FOR_EACH(triggerChains_, it)
		items.push_back(strlen(it->name) ? (const char*)it->name : "xxx");

	int i = popupMenuIndex(items);
	if(i == -1)
		return 0;
	it = triggerChains_.begin();
	while(i--)
		++it;
	return &*it;
}

//-------------------------------------------------
void terVoiceDispatcher::quant()
{
	for(VoiceTimerList::const_iterator it = delayedVoices_.begin(); it != delayedVoices_.end(); ++it){
		if(it->end())
			startVoice(it->voiceSetup(),true);
	}

	voiceQueue_.remove_if(mem_fun_ref(&VoiceTimer::end));
	delayedVoices_.remove_if(mem_fun_ref(&VoiceTimer::end));
	disabledVoices_.remove_if(mem_fun_ref(&VoiceTimer::end));

	while(!isVoicePlaying() && !voiceQueue_.empty()){
		play(voiceQueue_.begin()->voiceSetup());
		voiceQueue_.pop_front();
	}
}

bool terVoiceDispatcher::startVoice(const SoundEventSetup& voice_setup,bool ignore_delay)
{
	if(isVoiceDisabled(voice_setup)) return false;
	if(voice_setup.isVoice && !owner_->active()) return false;

	if(!ignore_delay && voice_setup.startDelay){
		delayedVoices_.push_back(VoiceTimer(voice_setup,voice_setup.startDelay));
		return true;
	}

	if(isVoicePlaying())
		voiceQueue_.push_back(VoiceTimer(voice_setup,voice_setup.queueTime));
	else 
		play(voice_setup);

	return true;
}

bool terVoiceDispatcher::play(const SoundEventSetup& voice_setup)
{
	if(voice_.Init(voice_setup.name)){
		if(voice_.Play(false)){
			if(voice_setup.pauseTime)
				disabledVoices_.push_back(VoiceTimer(voice_setup,voice_setup.pauseTime));

			return true;
		}
	}

	return false;
}

