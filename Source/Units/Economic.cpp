#include "StdAfx.h"

#include "Runtime.h"
#include "Universe.h"

#include "UniverseInterface.h"

#include "IronExplosion.h"

#include "PerimeterSound.h"
#include "Config.h"

#include "CameraManager.h"
#include "GameShell.h"
#include "IronClusterUnit.h"
#include "Scripts\ForceField.hi"
#include "EnergyConsumer.h"

void terPlayer::EconomicQuant()
{
//	watch_i(clusters_.size(), this);
//	watch_i(active_generators_.size(), this);
	CalcBuildingBlock();
	UpdateEconomicStructure();
	updateFieldQuant();
}

void terPlayer::destroyLinkEconomic()
{
	bool update = false;
	ClusterList::iterator ci;
	for(ci=clusters_.begin();ci!=clusters_.end();)
	{
		if(ci->destroyLinks()) {
			ci = clusters_.erase(ci);
			update = true;
		}else
		{
			ci++;
		}
	}
	if(update)
		updateField();
/*
	ClusterList::iterator ci = remove_if(clusters_.begin(), clusters_.end(), mem_fun_ref(Cluster::destroyLinks));
	if(ci != clusters_.end()){
		clusters_.erase(ci,	clusters_.end());
		updateField();
	}
*/
	
	for(int i = 0;i < UNIT_ATTRIBUTE_STRUCTURE_MAX;i++)
		removeNotAlive(BuildingList[i]);
}

void terPlayer::UpdateStructureAccessible()
{	
	start_timer_auto(UpdateStructureAccessible, 2);
	
	for(int i = 0;i < UNIT_ATTRIBUTE_STRUCTURE_MAX;i++)
		GetEvolutionBuildingData((terUnitAttributeID)i).clear();

	if(frame()){
		for(int i = 0;i < UNIT_ATTRIBUTE_STRUCTURE_MAX;i++){
			terUnitAttributeID evolution_id = (terUnitAttributeID)i;
			const AttributeBuilding& attr = *safe_cast<const AttributeBuilding*>(unitAttribute(evolution_id));
			EnableData& evolution = GetEvolutionBuildingData(evolution_id);
			bool enabled = true;
			// Если уже есть построенная структура, то не требуется иметь downgrade
			terUnitAttributeID downgrade_id = countUnits(evolution_id) ? attr.downgrade() : UNIT_ATTRIBUTE_NONE;
			for(int j = 0; j < attr.EnableStructure.size(); j++){
				terUnitAttributeID id = attr.EnableStructure[j];
				if(id != downgrade_id && !GetEvolutionBuildingData(id).Worked){
					enabled = false;
					break;
				}
			}
			if(enabled){
				evolution.Enabled = 1;
				terBuildingList::iterator bi;
				int worked = 0;
				FOR_EACH(BuildingList[i], bi)
					if((*bi)->isBuildingEnable()){
						worked = 1;
						if(!(*bi)->isUpgrading())
							evolution.Worked = 1;
						break;
					}
				for(int j = 0; j < attr.Downgrades.size(); j++){
					EnableData& downgrade = GetEvolutionBuildingData(attr.Downgrades[j]);
					downgrade.Enabled = 1;
					downgrade.Worked |= worked;
				}
			}
			
			// Устанавливаем Requested и Construction независимо от Enable
			terBuildingList::iterator bi;
			int downgrades_constructed = 0;
			FOR_EACH(BuildingList[i], bi){
				evolution.Requested++;
				if((*bi)->isConstructed())
					evolution.Constructed++;
				if((*bi)->isConstructed() || (*bi)->isUpgrading())
					downgrades_constructed++;
			}
			for(int j = 0; j < attr.Downgrades.size(); j++){
				EnableData& downgrade = GetEvolutionBuildingData(attr.Downgrades[j]);
				downgrade.Requested += evolution.Requested;
				downgrade.Constructed += downgrades_constructed;
			}
		}
	}

	for(int i = UNIT_ATTRIBUTE_SOLDIER + MUTATION_ATOM_MAX;i < UNIT_ATTRIBUTE_LEGIONARY_MAX; i++){
		EnableData& mutation = GetMutationElement((terUnitAttributeID)i);
		const AttributeLegionary& attr = *safe_cast<const AttributeLegionary*>(unitAttribute((terUnitAttributeID)i));
		mutation.Enabled = 1;
		for(int j = 0;j < attr.EnableStructure.size();j++)
			if(!GetEvolutionBuildingData(attr.EnableStructure[j]).Worked){
				mutation.Enabled = 0;
				break;
				}
		}
}

void terPlayer::CalcBuildingBlock()
{
	buildingBlockRequest_ = false;

	UnitList::iterator it;
	FOR_EACH(Units,it){
		if((*it)->needBuildingRepair()){
			terUnitGeneric* p = safe_cast<terUnitGeneric*>(*it);
			if(p->repairRequest()){
				buildingBlockRequest_ = true;
				break;
			}
		}
	}
}

int terPlayer::calcBaseUnits() const
{
	int units = 0;
	SquadList::const_iterator si;
	FOR_EACH(squads, si)
		if(*si)
			units += (*si)->squadMolecula().elementCount(DAMAGE_FILTER_BASE);

	return units;
}

terBuilding* terPlayer::BuildStructure(terUnitAttributeID build_index, const Vect3f& pos)
{
	terBuildingList::iterator bi;
	FOR_EACH(buildingList(build_index), bi){
		if(!(*bi)->isConstructed())
			return 0;
	}

	terBuildingInstaller installer;
	installer.InitObject(unitAttribute(build_index));
	installer.SetBuildPosition(To3D(pos), pos.z, 0);
	if(!installer.valid())
		return 0;
	installer.Clear();

	terBuilding* n = safe_cast<terBuilding*>(buildUnit(build_index));
	n->setPose(Se3f(QuatF(pos.z, Vect3f::K), pos), true);
	n->constructionStart();
	n->Start();
	return n;
}

void terPlayer::UpdateEconomicStructure()
{
	start_timer_auto(UpdateEconomicStructure, 2);

	UpdateEnergyStructure();

	CalcStructureRegion();
	CalcEnergyRegion();

	TrustMap->GetWorkAreaStats(FrameStatData.ZeroSquareRequest,FrameStatData.ZeroSquareComplete,FrameStatData.AbyssSquareRequest,FrameStatData.AbyssSquareComplete);

	UpdateStructureAccessible();
	UpdatePowering();

	structure_column_.setUnchanged();
	energyColumn().setUnchanged();
}

void terPlayer::attachEnergyUser(EnergyConsumer& energyConsumer, int priority)
{
	energyConsumers.insert(EnergyConsumerMap::value_type(priority, &energyConsumer));
}

void terPlayer::detachEnergyUser(EnergyConsumer& energyConsumer)
{
	EnergyConsumerMap::iterator ei;
	FOR_EACH(energyConsumers, ei)
		if(ei->second == &energyConsumer){
			energyConsumers.erase(ei);
			break;
		}
}

void terPlayer::UpdatePowering()
{
	if(!frame())
		return;

	float accumulatedPrev = energyData().accumulated();
	float dischargePrev = energyData().requested() - energyData().produced();

	terBuildingList accums = buildingList(UNIT_ATTRIBUTE_COLLECTOR);

	// Вся энергия - на зарядку
	float chargeEnergy = EnergyData.produced() + EnergyData.returned();
	EnergyData.clearReturned();
	if(chargeEnergy > 0){
		chargeEnergy -= frame()->addAccumulatedEnergy(chargeEnergy);

		if(!accums.empty()){
			float delta = chargeEnergy/accums.size();
			terBuildingList::iterator bi;
			FOR_EACH(accums, bi)
				if((*bi)->isConstructed())
					chargeEnergy -= (*bi)->addAccumulatedEnergy(delta);

			FOR_EACH(accums, bi)
				if((*bi)->isConstructed())
					chargeEnergy -= (*bi)->addAccumulatedEnergy(chargeEnergy);
		}
	}

	// Перерасчет запасов
	EnergyData.clearCapacity();
	EnergyData.clearAccumulated();
	EnergyData.addCapacity(frame()->attr().energyCapacity);
	EnergyData.addAccumulated(frame()->accumulatedEnergy());
	terBuildingList::iterator bi;
	FOR_EACH(accums, bi){
		if((*bi)->isConstructed()){
			EnergyData.addAccumulated((*bi)->accumulatedEnergy());
			EnergyData.addCapacity((*bi)->attr().energyCapacity);
		}
	}

//#ifndef _FINAL_VERSION_
//	XBuffer energyUsing(256, 1);
//	energyUsing < "\n";
//#endif

	// Использование по приоритетам
	EnergyData.clearUsed();
	EnergyConsumerMap::iterator ei;
	FOR_EACH(energyConsumers, ei){
		float delta = ei->second->requestEnergy(energyData());
//		#ifndef _FINAL_VERSION_
//		if(delta >= 0)
//		{
//			EnergyConsumer* e = ei->second;
//			energyUsing <= ei->first < "] " < e->name() < ": " <= delta < "\n";
//		}
//		#endif
	}

//#ifndef _FINAL_VERSION_
//	if(debug_show_energy_consumption)
//		watch_i(energyUsing, playerID());
//#endif

	// Удаление заряженных потребителей
	for(;;){
		bool run = false;
		EnergyConsumerMap::iterator ei;
		FOR_EACH(energyConsumers, ei)
			if(ei->second->finished()){
				ei->second->setDetached();
				energyConsumers.erase(ei);
				run = true;
				break;
			}
		if(!run)
			break;
	}

	// Вычитаем запросы из аккумуляторов
	float dischargeEnergy = -EnergyData.used();
	if(dischargeEnergy < 0){
		if(!accums.empty()){
			float delta = dischargeEnergy/accums.size();
			terBuildingList::iterator bi;
			FOR_EACH(accums, bi)
				if((*bi)->isConstructed())
					dischargeEnergy -= (*bi)->addAccumulatedEnergy(delta);

			FOR_EACH(accums, bi)
				if((*bi)->isConstructed())
					dischargeEnergy -= (*bi)->addAccumulatedEnergy(dischargeEnergy);
		}

		dischargeEnergy -= frame()->addAccumulatedEnergy(dischargeEnergy);
	}
								 
	if(accumulatedPrev > soundEventsPrm.collectorEmptyThreshold && energyData().accumulated() < soundEventsPrm.collectorEmptyThreshold)
		soundEvent(SOUND_VOICE_ENERGY_COLLECTOR_EMPTY);

	if(energyData().capacity() - accumulatedPrev > 0.1 && energyData().capacity() - energyData().accumulated() < 0.1)
		soundEvent(SOUND_VOICE_ENERGY_COLLECTOR_FULL);
	
	float dischargeRatio = energyData().requested()/(energyData().produced() + 0.1);
	if(dischargeRatio > soundEventsPrm.energyLosingThreshold)
		soundEvent(SOUND_VOICE_ENERGY_LOSING);

	if(energyData().accumulated() < soundEventsPrm.energyNotEnoughThreshold && dischargeRatio > soundEventsPrm.energyNotEnoughDischargeThreshold)
		soundEvent(SOUND_VOICE_ENERGY_NOT_ENOUGH);

	if(dischargePrev > 0 && energyData().requested() < energyData().produced())
		soundEvent(SOUND_VOICE_ENERGY_RESUPPLY);

	//b.setBuildingStatus(b.buildingStatus() | BUILDING_STATUS_ENABLED | BUILDING_STATUS_POWERED);
}

void terPlayer::CalcStructureRegion()
{
	float area = 0;
	if(frame())
		if(frame()->attached()){
			const AttributeBase& attr = frame()->attr();
			area = sqr(attr.ZeroLayerRadius)*M_PI;
			frame()->placeZeroLayer(false);
		}
		else
			frame()->freeZeroLayer();

	terBuildingList::iterator bi;
	FOR_EACH(BuildingList[UNIT_ATTRIBUTE_CORE], bi){
		terBuilding& b = **bi;
		if(b.isBuildingEnable()){
			const AttributeBase& attr = b.attr();
			area += sqr(attr.ZeroLayerRadius)*M_PI;
			b.placeZeroLayer(false);
		}
		else
			b.freeZeroLayer();
	}
	EnergyData.setAreaIdeal(area);
}

void terPlayer::CalcEnergyRegion()
{
	start_timer_auto(CalcEnergyRegion, STATISTICS_GROUP_AI);

	if(structureColumn().changed() || universe()->clusterColumn().changed()){
		MTAuto lock(universe()->EnergyRegionLocker());
		energyColumn().intersect(structureColumn(), universe()->clusterColumn());
		if(energyColumn().changed()){
			energy_region_.vectorize(0, false);
			FrameStatData.EnergyArea = energyColumn().area();
			EnergyData.setArea(FrameStatData.EnergyArea);
		}
	}
}

//--------------------------------------

typedef pair<Vect3f, Vect3f> EnergyLine;
class EnergyLineList : public vector<EnergyLine> {};

class ConnectCoreOp
{
	int playerID_;
	terPlayer* player_;
	terUnitGridType& UnitGrid;
	EnergyLineList& EnergyLines;

	typedef vector<terUnitBase*> List;
	List CurrentList;
	List NewList;

	bool powered_;
	bool restorePosition_;

public:
	ConnectCoreOp(terUnitBase* core, terUnitGridType& unit_grid, EnergyLineList& energy_lines) :
	  UnitGrid(unit_grid), EnergyLines(energy_lines), 
		  powered_(false), restorePosition_(true)
	{
		player_ = core->Player;
		playerID_ = core->playerID();
		CurrentList.push_back(core);
	}

	void operator()(terUnitBase* p)
	{
		if(!p->alive() || !p->attr().ConnectionRadius)
			return;

		if(p->attr().ID != UNIT_ATTRIBUTE_FRAME){ // Ядра и трансмиттеры
			if(p->playerID() == playerID_){
				if(p->includingCluster() && FieldCluster::get_player_id(p->includingCluster()) != playerID_)
					return; // Мои под вражеским полем
			}
			else if(!p->Player->isWorld())
				if(!p->includingCluster() || FieldCluster::get_player_id(p->includingCluster()) != playerID_)
					return; // Чужие не под моим

			terBuilding* b = safe_cast<terBuilding*>(p);
			if(b->buildingStatus() & (BUILDING_STATUS_CONSTRUCTED | BUILDING_STATUS_UPGRADING) && !b->isConnected()){
				terUnitBase* b_min = closestBuilding(b);
				if(b_min){
					if(b->attr().ID == UNIT_ATTRIBUTE_CORE && b_min->attr().ID == UNIT_ATTRIBUTE_FRAME){
						powered_ = true;
						if(b->position2D().distance2(b_min->position2D()) < sqr(b_min->attr().ConnectionRadius - safe_cast<terFrame*>(b_min)->attr().oneStepMovement))
							restorePosition_ = false;
					}

					if(b->Player != player_)
						universe()->changeOwner(b, player_);

					b->setBuildingStatus(b->buildingStatus() | BUILDING_STATUS_CONNECTED | BUILDING_STATUS_ENABLED);
					if(b->attr().MakeEnergy > 0)
						b->setBuildingStatus(b->buildingStatus() | BUILDING_STATUS_POWERED);

					if(b->readyToConnect() && b_min->readyToConnect()){
						EnergyLines.push_back(EnergyLine(
							safe_cast<terInterpolationConnection*>(b_min->avatar())->GetConnectionPosition(b->position()), 
							safe_cast<terInterpolationConnection*>(b->avatar())->GetConnectionPosition(b_min->position())));
						NewList.push_back(b);
					}
				}
			}
		}
		else if(p->attr().ID == UNIT_ATTRIBUTE_FRAME && player_->frame() != p){
			terFrame* frame = safe_cast<terFrame*>(p);
			terUnitBase* b_min = closestBuilding(p);
			if(b_min){
				if(frame->Player != player_){
					if(frame->frameStatus() == terFrame::ISOLATED){
						frame->setPowered(true);
						universe()->changeOwner(frame, player_);
					}
				}
				else{
					frame->setPowered(true);
					if(frame->readyToConnect() && b_min->readyToConnect()){
						EnergyLines.push_back(EnergyLine(
							safe_cast<terInterpolationConnection*>(b_min->avatar())->GetConnectionPosition(p->position()), 
							safe_cast<terInterpolationConnection*>(p->avatar())->GetConnectionPosition(b_min->position())));
					}
				}
			}
		}
	}

	terUnitBase* closestBuilding(terUnitBase* b)
	{
		float d_min = FLT_INF;
		terUnitBase* b_min = 0;
		List::iterator i;
		FOR_EACH(CurrentList, i){
			if(b->attr().ID == UNIT_ATTRIBUTE_RELAY && (*i)->attr().ID == UNIT_ATTRIBUTE_FRAME)
				continue;
			float d = (*i)->position2D().distance2(b->position2D());
			if(d_min > d && d < sqr((*i)->attr().ConnectionRadius)){
				d_min = d;
				b_min = *i;
			}
		}
		return b_min;
	}

	void scan() 
	{ 
		MTL();
		do {
			List::iterator i;
			FOR_EACH(CurrentList, i){
				Vect2f pos = (*i)->position2D();
				UnitGrid.Scan(pos.x, pos.y, (*i)->attr().ConnectionRadius, *this); 
			}

			swap(NewList, CurrentList);
			NewList.clear();
		} while(!CurrentList.empty());
	}

	bool powered() const { return powered_; }
	bool restorePosition() const { return restorePosition_; }
};

void terPlayer::UpdateEnergyStructure()
{
	start_timer_auto(AnalyzeEnergyStructure, 2);
	MTL();

	// Сбросить статус подключенности.
	for(int i = UNIT_ATTRIBUTE_CORE; i <= UNIT_ATTRIBUTE_RELAY; i++){
		terBuildingList::iterator bi;
		int flags = BUILDING_STATUS_CONNECTED | BUILDING_STATUS_ENABLED;
		if(i != UNIT_ATTRIBUTE_RELAY)
			flags |= BUILDING_STATUS_POWERED;
		FOR_EACH(BuildingList[i], bi)
			(*bi)->setBuildingStatus((*bi)->buildingStatus() & ~flags);
	}

	// Рекурсивно просканировать, начиная с фрейма, размер сканирования 2*ConnectionRadiusMax
	EnergyLineList energy_lines;
	if(frame()){
		ConnectCoreOp op(frame(), universe()->UnitGrid, energy_lines);
		op.scan();
		if(!frame()->underTeleportation()){
			frame()->setPowered(op.powered());
			if(op.restorePosition())
				frame()->restorePosition();
		}
		else
			frame()->setPowered(true);
	}

	UpdateEnergyLines(energy_lines);
}

void terPlayer::UpdateEnergyLines(EnergyLineList& energy_lines)
{
	MTL();
	// Обновить энергетические связи
	LightList::iterator current_light = Lights.begin();
	EnergyLineList::iterator li;
	FOR_EACH(energy_lines, li){
		cLine3d* p;
		if(current_light != Lights.end()){
			p = (*current_light);
			++current_light;
		}
		else{
			p = terScene->CreateLine3d(terTextureEnergyLinkTrail);
			p->SetAttr(ATTRUNKOBJ_ADDBLEND | ATTRUNKOBJ_REFLECTION);
			p->SetAnimation(8.0f,0);

			Lights.push_back(p);
			current_light = Lights.end();
		}

		sVertexLine line[2];
		line[0].pos=li->first;
		line[1].pos=li->second;
		line[0].color=line[1].color=sColor4c(255,255,255,255);
		line[0].v=0;
		line[1].v=line[0].pos.distance(line[1].pos)/64.0f;
		line[0].width=line[1].width=2.0f;

		p->UpdateVertex(2, line);
		p->ClearAttr(ATTRUNKOBJ_IGNORE);
	}

	while(current_light != Lights.end()){
		(*current_light)->SetAttr(ATTRUNKOBJ_IGNORE);
		++current_light;
	}
}

//-----------------------------------------------
void terPlayer::chargeAllCores()
{
	terBuildingList::iterator bi;
	FOR_EACH(BuildingList[UNIT_ATTRIBUTE_CORE], bi)
		(*bi)->executeCommand(UnitCommand(COMMAND_ID_START_CHARGING, 0));
}

void terPlayer::dischargeAllCores()
{
	terBuildingList::iterator bi;
	FOR_EACH(BuildingList[UNIT_ATTRIBUTE_CORE], bi)
		(*bi)->executeCommand(UnitCommand(COMMAND_ID_STOP_CHARGING, 0));
}

void terPlayer::startField()
{
	totalDefenceMode_ = true;
	terBuildingList::iterator bi;
	FOR_EACH(BuildingList[UNIT_ATTRIBUTE_CORE],bi){
		terProtector* generator = safe_cast<terProtector*>(*bi);
		if(generator->canStartField() || generator->enableCharge())
			generator->startField();
	}
}

void terPlayer::stopField()
{
	totalDefenceMode_ = false;
	terBuildingList::iterator bi;
	FOR_EACH(BuildingList[UNIT_ATTRIBUTE_CORE],bi)
		safe_cast<terProtector*>(*bi)->stopField();
}

void terPlayer::updateFieldQuant()
{
	terBuildingList& cores = buildingList(UNIT_ATTRIBUTE_CORE);

	bool toStart = false;
	terBuildingList::iterator ci;
	FOR_EACH(cores, ci)
		if(safe_cast<terProtector*>(*ci)->fieldState() == terProtector::FIELD_TO_START)
			toStart = true;

	if(toStart)
		updateField();
}

void terPlayer::updateField()
{
	terBuildingList& cores = buildingList(UNIT_ATTRIBUTE_CORE);
	ProtectorList freeGenerators;

	// Построить новый регион
	core_column_.clear();
	terBuildingList::const_iterator gi;
	FOR_EACH(cores, gi){
		terProtector* core = safe_cast<terProtector*>(*gi);
		if(core->canStartOrJoinToField()){
			core_column_.operateByCircle(core->position2D(), core->attr().FieldRadius + core->radius(), 1);
			freeGenerators.push_back(core);
		}
	}

	field_region_.clear();
	for(int i = 0;i < UNIT_ATTRIBUTE_STRUCTURE_MAX;i++){
		terBuildingList::iterator bi;
		FOR_EACH(BuildingList[i],bi){
			Vect2f pos = (*bi)->position2D();
			if((*bi)->attr().ID == UNIT_ATTRIBUTE_CORE && safe_cast<terProtector*>(*bi)->canStartOrJoinToField())
				field_region_.getEditColumn().operateByCircle(pos,(*bi)->attr().FieldRadius, 1);
			else if(core_column_.filled(pos.xi(),pos.yi()))
				field_region_.getEditColumn().operateByCircle(pos,(*bi)->radius(), 1);
		}
	}

	if(frame()){
		Vect2f pos = frame()->position2D();
		if(core_column_.filled(pos.xi(),pos.yi()))
			field_region_.getEditColumn().operateByCircle(frame()->position2D(),frame()->radius(),1);
	}

	field_region_.getEditColumn().intersect(universe()->clusterColumn());
	PlayerVect::iterator pli;
	FOR_EACH(universe()->Players, pli)
		if(*pli != this)
			field_region_.getEditColumn().sub((*pli)->field_region_.getEditColumn());

	//field_region_.vectorize(minimal_region_size, false);
	field_region_.postOperateAnalyze();
	FrameStatData.ProtectedArea = field_region_.getEditColumn().area();

	// Оставить только стационарные кластера 
	int cluster_id = 0;
	ClusterList::iterator ci;
	FOR_EACH(clusters_, ci){
		if(ci->fieldCluster() && cluster_id < ci->fieldCluster()->get_cluster_id())
			cluster_id = ci->fieldCluster()->get_cluster_id();

		ci->setRegion(0);
		Region::iterator ri;
		FOR_EACH(field_region_, ri){
			if((*ri)->positive()){
				ProtectorList::const_iterator gi;
				FOR_EACH(ci->generators(), gi){
					if(field_region_.locateEdit((*gi)->position2D()) == *ri){
						ci->setRegion(*ri);
						break;
					}
				}
			}
			if(ci->region())
				break;
		}
		
		if(ci->region() && ci->fieldCluster()){
			bool stop = false;
			ProtectorList::iterator gi;
			FOR_EACH(freeGenerators, gi){
				if(field_region_.locateEdit((*gi)->position2D()) == ci->region()){
					if(find(ci->generators().begin(), ci->generators().end(), *gi) == ci->generators().end())
						stop = true;
					gi = freeGenerators.erase(gi);
					--gi;
				}
			}

			if(stop)
				ci->stopField(true);
		}
		else{
			ci = clusters_.erase(ci);
			--ci;
		}
	}

	Region::iterator ri;
	FOR_EACH(field_region_, ri)
		if((*ri)->positive()){
			bool canStart = false;
			ProtectorList generators;
			ProtectorList::iterator gi;
			FOR_EACH(freeGenerators, gi)
				if(field_region_.locateEdit((*gi)->position2D()) == *ri){
					generators.push_back(*gi);
					if((*gi)->fieldState() == terProtector::FIELD_TO_START)
						canStart = true;
				}

			if(!generators.empty() && canStart){
				clusters_.push_back(Cluster());
				Cluster& cluster = clusters_.back();
				cluster.setCluster(this, *ri, generators);
				cluster.setFieldCluster(field_dispatcher->createCluster(playerID(), 10*playerID() + ++cluster_id));
				cluster.fieldCluster()->SetColor(fieldColor());
				cluster.startField();
			}
		}
}

bool terPlayer::canStartField() const 
{
	terBuildingList::const_iterator bi;
	FOR_EACH(BuildingList[UNIT_ATTRIBUTE_CORE],bi)
		if((*bi)->canStartField())
			return 1;

	return 0;
}

//------------------------------------------
Cluster::Cluster()
{
	player_ = 0;
	region_ = 0;
	fieldCluster_ = 0;
	fieldStopping_ = false;
	started_ = false;
	restartAfterStop_ = false;
}

void Cluster::setCluster(terPlayer* player, Region* region, const ProtectorList& generators)
{
	player_ = player;
	region_ = region;
	generators_ = generators;
}

Cluster::~Cluster() 
{
	if(fieldCluster_){
		ProtectorList::iterator gi;
		FOR_EACH(generators_, gi)
		{
			(*gi)->monksGoHome();
		}
		field_dispatcher->removeCluster(fieldCluster_);
	}
}

void Cluster::startField()
{
	field_dispatcher->setBorder(fieldCluster_, *region_);
	Vect2sVect& path = const_cast<Vect2sVect&>(fieldCluster_->border());

	length_ = 0;
	Vect2s p0 = path.back();
	Vect2sVect::iterator pi;
	FOR_EACH(path, pi){
		length_ += sqrtf((*pi - p0).norm2());
		p0 = *pi;
	}

	monks_.clear();
	ProtectorList::iterator gi;
	FOR_EACH(generators_, gi){
		monks_.insert(monks_.end(), (*gi)->monks().begin(), (*gi)->monks().end());
		(*gi)->setFieldState(terProtector::FIELD_STARTING);
	}

	if(!monks_.size() || length_ < FLT_EPS)
		return;

	ConsumptionData data = fieldPrm.fieldConsumption;
	data.energy *= generators_.size();
	fieldConsumer_.attach(player_, data);

	float deltaLength = length_/monks_.size();
	monksVelocity_ = fieldPrm.monksVelocity*velocityFactor();
	monksPosition_ = 0;
	float position = 0;
	MonkList::iterator mi;
	FOR_EACH(monks_, mi){
		(*mi)->setPath(path, position);
		position += deltaLength;
	}
}

float Cluster::velocityFactor() const
{
	return (fieldPrm.monksPerCore*length_)/(2*M_PI*generators().front()->attr().FieldRadius*monks_.size());
}

bool Cluster::destroyLinks()
{
	if(fieldCluster_ && !fieldStarted()){
		if(fieldStopping_){
			field_dispatcher->removeCluster(fieldCluster_);
			fieldCluster_ = 0;
			if(restartAfterStop_){
				restartAfterStop_ = false;
				ProtectorList::iterator gi;
				FOR_EACH(generators_, gi)
					if((*gi)->canStartField())
						(*gi)->setFieldState(terProtector::FIELD_TO_START);
			}
			else{
				ProtectorList::iterator gi;
				FOR_EACH(generators_, gi)
					(*gi)->setFieldState(terProtector::FIELD_TO_STOP);
					//(*gi)->stopField();
			}
			return true;
		}
		else if(fieldState() == terProtector::FIELD_STARTED){
				started_ = true;
				fieldCluster_->switchON();
				ProtectorList::iterator gi;
				FOR_EACH(generators_, gi)
					SND3DPlaySound("field_start", &(*gi)->position());
			}
	}

	if(removeNotAlive(generators_) || (fieldState() & terProtector::FIELD_TO_STOP))
		stopField(false);

	removeNotAliveMonk(monks_);
	log_var(monks_.size());

	if(monks_.size() >= (fieldPrm.monksPerCoreMin*generators_.size() + 1)){
		if(monks_.front()->monkMode() == MONK_MODE_GUARD){
			monksVelocity_ += (fieldPrm.monksVelocity*velocityFactor() - monksVelocity_)*fieldPrm.monksAccelerationFactor;
			monksPosition_ += monksVelocity_;
			float deltaLength = length_/monks_.size();
			float position = monksPosition_;
			MonkList::iterator mi;
			FOR_EACH(monks_, mi){
				(*mi)->setPathPosition(((*mi)->pathPosition() + monksVelocity_)*(1 - fieldPrm.monksPositionFactor) + position*fieldPrm.monksPositionFactor);
				position += deltaLength;
			}
			
			fieldConsumer_.requestCharge();
			if(!fieldConsumer_.charged())
				player_->stopField();
		}
	}
	else{
		stopField(false);
	}

	removeNotAliveMonk(monks_);

	return false;
}

void Cluster::stopField(bool restartAfterStop)
{
	restartAfterStop_ = restartAfterStop;
	if(fieldStopping_)
		return;

	fieldStopping_ = true;
	started_ = false;
	startedTimer_.start(fade_duration);
	fieldCluster_->switchOFF();
	SND2DPlaySound("perimeter_broken");
	
	ProtectorList::iterator gi;
	FOR_EACH(generators_, gi){
		SND3DPlaySound("field_stop", &(*gi)->position());
		(*gi)->monksGoHome();
	}
}

int Cluster::fieldState() const
{
	int state = 0;
	ProtectorList::const_iterator gi;
	FOR_EACH(generators_, gi)
		state |= (*gi)->fieldState();
	return state;
}
