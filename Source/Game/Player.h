#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "NetPlayer.h"
#include "RealUnit.h"
#include "IronBuilding.h"
#include "FrameField.h"
#include "DefenceMap.h"
#include "Save.h"
#include "SelectManager.h"
#include "PerimeterSound.h"

class terFrame;
class terBuilding;
class terUnitSquad;
class terTerraformDispatcher;
class EnergyConsumer;

typedef vector<terBuilding*> terBuildingList;
typedef vector<terUnitSquad*> SquadList;
typedef vector<terPlayer*> PlayerVect;

class terEnergyDataType
{
public:
	terEnergyDataType() { produced_ = used_ = requested_ = capacity_ = efficiency_ = returned_ = accumulated_ = 0; area_ = areaIdeal_ = 1; energyPerArea_ = 0; }
	
	void setEnergyPerArea(float energyPerArea) { energyPerArea_ = energyPerArea; }
	void setArea(float area) { area_ = area; Update(); }
	void setAreaIdeal(float area) { areaIdeal_ = area; Update(); }
	float efficiency() const { return efficiency_; }
	float produced() const { return produced_; }

	void clearCapacity(){ capacity_ = 0; }
	void addCapacity(float capacity){ capacity_ += capacity; }
	float capacity() const { return capacity_; }
	
	void clearAccumulated(){ accumulated_ = 0; }
	void addAccumulated(float accumulated){ accumulated_ += accumulated; }
	float accumulated() const { return accumulated_; }

	void clearUsed(){ used_ = requested_ = 0; }
	float addUsed(float requested) {
		requested_ += requested;
		float returned = clamp(requested, 0, accumulated() - used());
		used_ += returned;
		return returned;
	}
	float used() const { return used_; }
	float requested() const { return requested_; }

	void clearReturned() { returned_ = 0; }
	void addReturned(float returned) { returned_ += returned; }
	float returned() const { return returned_; }

private:
	float produced_;
	float used_;
	float requested_;
	float accumulated_;
	float capacity_;
	float area_;
	float areaIdeal_;
	float energyPerArea_;
	float efficiency_;
	float returned_;
	
	void Update() { 
		efficiency_ = clamp((float)(area_)/(FLT_EPS + areaIdeal_), 0, 1); 
		produced_ = energyPerArea_*area_;
	}
};

struct terFrameStatisticsType
{
	int ZeroSquareRequest;
	int ZeroSquareComplete;
	int AbyssSquareRequest;
	int AbyssSquareComplete;

	int EnergyArea;
	int ProtectedArea;

	terFrameStatisticsType()
	{
		ZeroSquareRequest = 0;
		ZeroSquareComplete = 0;
		AbyssSquareRequest = 0;
		AbyssSquareComplete = 0;

		EnergyArea = 0;
		ProtectedArea = 0;
	}
};

class Cluster {
public:
	Cluster();
	~Cluster();

	void setCluster(terPlayer* player, Region* region, const ProtectorList& generators);

	Region* region() const { return region_; }
	void setRegion(Region* region) { region_ = region; }

	FieldCluster* fieldCluster() const { return fieldCluster_; }
	void setFieldCluster(FieldCluster* fieldCluster) { fieldCluster_ = fieldCluster; }
	
	bool destroyLinks(); // true - is to be removed
	void startField();
	void stopField(bool restartAfterStop);

	const ProtectorList& generators() const { return generators_; }
	int fieldState() const;

	bool fieldStarted() const { return started_ || startedTimer_(); }

	float velocityFactor() const;

private:
	terPlayer* player_;
	Region* region_;
	FieldCluster* fieldCluster_;
	ProtectorList generators_;
	MonkList monks_;
	float length_;
	float monksPosition_;
	float monksVelocity_;
	bool fieldStopping_;

	bool started_;
	DurationTimer startedTimer_;
	bool restartAfterStop_;

	EnergyConsumer fieldConsumer_;
};

class PlayerStats : public SavePlayerStats
{
	public:
		PlayerStats() {
			reset();
		}
		void reset() {
			maxLeveledArea = 0;
			maxZeroedArea = 0;
			medEfficiency = 0;
			effQuants = 0;
			energy = 0;
			scourgeKilled = 0;

			unitCount = 0;
			unitKilled = 0;
			unitLost = 0;

			buildings = 0;
			buildingRazed = 0;
			buildingCaptured = 0;
			buildingLost = 0;
		}
		void checkEvent(const Event& event, int playerID);
		void update(const terFrameStatisticsType& frameStats, terEnergyDataType& energyData);

		int getTotalScore(int totalTimeMillis) const;
		int getGeneralTotalScore(int totalTimeMillis) const;
		int getUnitsTotalScore() const;
		int getBuildingsTotalScore() const;

		int getMaxZeroedArea() const {
			return maxZeroedArea;
		}

		int getMaxLeveledArea() const {
			return maxLeveledArea;
		}
		float getMedEfficiency() const {
			return medEfficiency / float(effQuants);
		}
		int getScourgeKilled() const {
			return scourgeKilled;
		}
		float getEnergy() const {
			return energy;
		}

		int getUnitCount() const {
			return unitCount;
		}
		int getUnitKilled() const {
			return unitKilled;
		}
		int getUnitLost() const {
			return unitLost;
		}

		int getBuildingCount() const {
			return buildings;
		}
		int getBuildingRazed() const {
			return buildingRazed;
		}
		int getBuildingCaptured() const {
			return buildingCaptured;
		}
		int getBuildingLost() const {
			return buildingLost;
		}
		static bool isBuilding(int unitClass) {
			return unitClass & (UNIT_CLASS_STRUCTURE | UNIT_CLASS_STRUCTURE_GUN | UNIT_CLASS_STRUCTURE_SPECIAL | UNIT_CLASS_STRUCTURE_CORE);
		}
};

class terVoiceDispatcher
{
public:
	terVoiceDispatcher() : owner_(NULL) { }
	~terVoiceDispatcher(){ };

	bool startVoice(const SoundEventSetup& voice_setup,bool ignore_delay = false);

	void quant();

	void setOwner(terPlayer* owner){ owner_ = owner; }

private:

	class VoiceTimer
	{
	public:
		VoiceTimer(const SoundEventSetup& voice_setup,int time) : voiceSetup_(voice_setup) { if(time) timer_.start(time); }

		bool operator == (const SoundEventSetup& voice_setup) const { return (&voiceSetup_ == &voice_setup); }

		bool end() const { return timer_(); }

		const SoundEventSetup& voiceSetup() const { return voiceSetup_; }

	private:
		DelayTimer timer_;
		const SoundEventSetup& voiceSetup_;
	};

	typedef list<VoiceTimer> VoiceTimerList;

	VoiceTimerList voiceQueue_;
	VoiceTimerList delayedVoices_;
	VoiceTimerList disabledVoices_;

	SND2DSound voice_;

	terPlayer* owner_;

	bool isVoiceDisabled(const SoundEventSetup& voice_setup) const { return (find(disabledVoices_.begin(),disabledVoices_.end(),voice_setup) != disabledVoices_.end()); }
	bool isVoicePlaying() const { return voice_.IsPlayed(); }
	bool play(const SoundEventSetup& voice_setup);
};


struct EnableData
{
	bool Enabled; // разрешена для строительства, разрешена мутация для юнитов
	int Requested; // кол-во запрошенных и построенных
	int Constructed; // кол-во построенных
	int Worked; // работает

	EnableData() {
		clear();
	}

	void clear() { Enabled = Worked = Constructed = Requested = 0; }
};


///////////////////////////////////////
//			Игрок
///////////////////////////////////////
class terPlayer
{
public:
	terPlayer(const PlayerData& playerData);
	virtual ~terPlayer();

	//-----------------------------
	void setTriggerChains(const SavePlayerManualData& names);
	void setPlayerStrategyIndex(int playerStrategyIndex) { playerStrategyIndex_ = playerStrategyIndex; }
	int playerStrategyIndex() const { return playerStrategyIndex_; }
	void refreshCameraTrigger(const char* triggerName);

	//-----------------------------
	bool active() const { return active_; }
	
	bool controlEnabled() const { return controlEnabled_; }
	void setControlEnabled(bool controlEnabled) { controlEnabled_ = controlEnabled; }

	terFrame* frame() const { return frame_; }
	void clearFrame();

	int playerID() const { return playerID_; }
	bool isWorld() const { return isWorld_;	}
	terBelligerent belligerent() const { return belligerent_; }
	const char* name() const { return name_.c_str(); }
	int colorIndex() const { return colorIndex_; }

	int clan() const { return clan_; }
	void setClan(int clan) { clan_ = clan; }

	float handicap() const { return handicap_; }

	const DifficultyPrm& difficultyPrm() const { return difficultyPrmArray[difficulty_]; }
	void setDifficulty(Difficulty difficulty) { difficulty_ = difficulty; }
	Difficulty difficulty() const { return difficulty_; }
	
	terPlayer* enemyPlayer() const { return enemyPlayer_; }

	const AttributeBase* unitAttribute(terUnitAttributeID id) const;
	
	void EconomicQuant();
	void destroyLinkEconomic();
	
	void CalcBuildingBlock();
	int calcBaseUnits() const;
	void UpdateStructureAccessible();
	void UpdatePowering();
	
	void UpdateEconomicStructure();
	void UpdateEnergyStructure();
	void UpdateEnergyLines(class EnergyLineList& energy_lines);
	
	Column& structureColumn() { return structure_column_; }
	Column& energyColumn() { EnergyRegionLockAssert(); return energy_region_.getEditColumn(); }
	RegionDispatcher& energyRegion() { EnergyRegionLockAssert(); return energy_region_; }
	terEnergyDataType& energyData(){ return EnergyData; };

	const terFrameStatisticsType& GetFrameStats(){ return FrameStatData; }
	const Column& fieldColumn() const { return field_region_.getEditColumn(); }

	void attachEnergyUser(EnergyConsumer& energyConsumer, int priority);
	void detachEnergyUser(EnergyConsumer& energyConsumer);
	
	void CalcStructureRegion();
	void CalcEnergyRegion();
	
	terBuildingList& buildingList(int type){ MTL();xassert(type >= 0 && type < UNIT_ATTRIBUTE_STRUCTURE_MAX); return BuildingList[type]; }
	const terBuildingList& buildingList(int type) const { MTL();xassert(type >= 0 && type < UNIT_ATTRIBUTE_STRUCTURE_MAX); return BuildingList[type]; }

	SquadList& squadList() { return squads; }
	terUnitSquad* squad(int index) { MTL(); return index < squads.size() ? squads[index] : 0; }

	bool buildingBlockRequest() const { return buildingBlockRequest_; }
	
	terBuilding* BuildStructure(terUnitAttributeID build_index, const Vect3f& pos);
	
	EnableData& GetEvolutionBuildingData(terUnitAttributeID id) { return buildingsState_[id]; }
	EnableData& GetMutationElement(terUnitAttributeID id) { return legionaryState_[id - UNIT_ATTRIBUTE_SOLDIER]; }
	
	//-----------------------------
	
	int countUnits(terUnitAttributeID id) const;
	int countBuildingsConstructed(terUnitAttributeID id) const;
	int countBuildingsPowered(terUnitAttributeID id) const;

	//-----------------------------
	void chargeAllCores();
	void dischargeAllCores();

	void startField();
	void stopField();

	bool canStartField() const;
	bool isFieldOn() const { return !clusters_.empty(); }
	
	void updateFieldQuant();
	void updateField();
	
	//-----------------------------
	virtual void Quant();
	virtual void triggerQuant() {}
	virtual void checkEvent(const Event& event) {
		stats.checkEvent(event, playerID());
	}

	void DestroyLink();
	void DeleteQuant();
	void MoveQuant();
	void CollisionQuant();

	void RefreshAttribute();

	void AvatarQuant();
	void PrepareQuant();

	void incomingCommandRegion(const netCommand4G_Region& reg);

	terUnitBase* buildUnit(terUnitAttributeID id);
	virtual void addUnit(terUnitBase* p);
	virtual void removeUnit(terUnitBase* p);
	
	void killAllUnits();
	void removeUnits();

	void CommandRegionHeaderEvent(int size);
	void CommandRegionDataEvent(char* data);

	terUnitBase* traceUnit(const Vect2f& pos);

	bool marked() const {
		return marked_;
	}
	virtual void mark() {
		marked_ = true;
	}
	virtual void unmark() {
		marked_ = false;
	}

//LocalCommands
	void ChangeRegion(XBuffer& out);
	void RasterizeRegion();

	void ShowInfo();

	void universalLoad(const SavePlayerData& data);
	void universalSave(SavePlayerData& data, bool userSave);

	void loadWorld(const SavePrm& data);
	void saveWorld(SavePrm& data);


	void ScanRegion(RegionDispatcher* region);
	void ScanAbyssRegion(RegionDispatcher* region);

	terUnitBase* findUnit(unsigned int unit_id);
	terUnitBase* findUnit(terUnitAttributeID id);
	terUnitBase* findUnit(terUnitAttributeID id, const Vect2f& nearPosition, float distanceMin = 0);
	terUnitBase* findUnitByLabel(const char* label);
	terUnitBase* findUnitByUnitClass(int unitClass, const Vect2f& nearPosition, float distanceMin = 0);

	bool findPathToPoint(DefenceMap& defenceMap, const Vect2i& from_w, const Vect2i& to_w, vector<Vect2i>& out_path);
	terUnitBase* findPathToTarget(DefenceMap& defenceMap, terUnitAttributeID id, terUnitBase* ignoreUnit, const Vect2f& nearPosition, Vect2iVect& path);
	terUnitBase* findPathToTarget(DefenceMap& defenceMap, int unitClass, terUnitBase* ignoreUnit, const Vect2f& nearPosition, Vect2iVect& path);
	DefenceMap& defenceMap() { return defenceMap_; } // Ищем цель у врага, подставляя свою карту защищенности

	bool soundEvent(SoundEventID event_id);
	bool soundEvent(const SoundEventSetup* ev);
	bool startSound(const SoundEventSetup* ev) const;

//------------------------------------------------------------

	const sColor4f& fieldColor() const { return fieldColor_; }
	const sColor4f& unitColor() const { return unitColor_; }
	const sColor4f& zeroLayerColor() const { return zeroLayerColor_; }
	
	void setColorIndex(int colorIndex);

//------------------------------------------------------------

	void SetActivePlayer();
	void SetDeactivePlayer();

	void UpdateSkinColor();

	void ClusterPick(CommandID id, unsigned int data);
	void ClusterActionPoint(Vect3f v, CommandSelectionMode mode, CommandID command_id, unsigned int data);

	void showFireCircles(terUnitAttributeID attribute_id, const Vect2f& position);
	void showConnectionCircle(const Vect2f& point, bool includeFrame);

	bool totalDefenceMode() const { return totalDefenceMode_; }

	bool zerolayer(const Vect2i& point) const;

	int registerUnitID(int unitID);
		
	// ----- AI Interface ---------
	bool isAI() const { return isAI_; }
	virtual void setAI(bool isAI);
	
	virtual void TerrainWorkingComplete(int x,int y,int sx,int sy){}

	//-----------------------------------------
	virtual void showDebugInfo();
	virtual void WriteDebugInfo(XBuffer& buf);

	//-----------------------------------------
	TriggerChain* getStrategyToEdit();

	void rebuildDefenceMapQuant();
	void chooseEnemyQuant();

	Vect2f lastFramePosition() const { return lastFramePosition_; }
	
	//-----------------------------------------
	terTerraformDispatcher* TrustMap;
	
	//При обращении из графического потока к RegionPoint,ZeroRegionPoint,AbyssRegionPoint
	//не забывать про RegionPoint->Lock,Unlock
	RegionMetaDispatcher* RegionPoint;
	MetaLockRegionDispatcher ZeroRegionPoint;
	MetaLockRegionDispatcher AbyssRegionPoint;
	int RegionSize;
	int RegionCount;
	
	cTexture* HologramPoint;
	
	int PrevBrigadierWorking, BrigadierWorking;
	
	void burnZeroplast();

	bool shouldIgnoreIntfCommands() const {
		return ignoreIntfCommands;
	}
	void setShouldIgnoreIntfCommands(bool shouldIgnoreIntfCommands) {
		ignoreIntfCommands = shouldIgnoreIntfCommands;
	}

	const PlayerStats& getStats() const {
		return stats;
	}

	cTexture *pTextureUnitSelection;
	const UnitList& units()
	{
#ifdef _DEBUG
		if(!MT_IS_LOGIC())
		{
			xassert(units_lock.is_lock());
		}
#endif
		return Units;
	}

	MTSection* UnitsLock(){return &units_lock;}

protected:
	MTSection units_lock;
	UnitList Units;
	SquadList squads;

	list<double> begin_time_burn_zeroplast;
	MTDECLARE(lock_burn_zeroplast);
	void quantZeroplast();

	bool ignoreIntfCommands;

	PlayerStats stats;

	typedef list<TriggerChain> TriggerChains;
	TriggerChains triggerChains_;

private:
	bool active_;
	bool controlEnabled_;
	int playerID_;
	int clan_;
	Difficulty difficulty_;
	float handicap_;
	string name_;
	bool isWorld_;
	terBelligerent belligerent_;
	int colorIndex_;
	bool isAI_;
	unsigned int compAndUserID_;

	sColor4f fieldColor_;
	sColor4f unitColor_;
	sColor4f zeroLayerColor_;

	int UnitCount;
	
	terFrame* frame_;

	bool buildingBlockRequest_;
	terBuildingList BuildingList[UNIT_ATTRIBUTE_STRUCTURE_MAX];
	
	Column structure_column_; 

	//!!!! energy_region_ может изменяться только в terPlayer::CalcEnergyRegion()
	//иначе будут пороблемы с многопоточностью
	RegionDispatcher energy_region_;
	Column core_column_;
	RegionDispatcher field_region_;
	void EnergyRegionLockAssert();

	typedef list<Cluster> ClusterList; 
	ClusterList clusters_;
	bool totalDefenceMode_;
	
	typedef list<cLine3d*> LightList;
	LightList Lights;
	
	terEnergyDataType EnergyData;
	
	terFrameStatisticsType FrameStatData;

	EnableData buildingsState_[UNIT_ATTRIBUTE_STRUCTURE_MAX];
	EnableData legionaryState_[UNIT_ATTRIBUTE_LEGIONARY_MAX - UNIT_ATTRIBUTE_SOLDIER];
	
	bool marked_;

	typedef multimap<int, EnergyConsumer*> EnergyConsumerMap;
	EnergyConsumerMap energyConsumers;

	DefenceMap defenceMap_;
	int defenceMapGunIndex_;
	terPlayer* defenceMapPlayer_;

	terVoiceDispatcher voiceDispatcher_;

	terPlayer* enemyPlayer_;
	terPlayer* observedPlayer_;
	terPlayer* chooseEnemyPlayer_;
	int chooseEnemyIndex_;
	float chooseEnemyDistance_;
	PlayerVect companions_;

	bool rasterize_region_on_next_quant;

	Vect2f lastFramePosition_;

	int playerStrategyIndex_;

	//--------------------------------
	terUnitBase* createUnit(const UnitTemplate& data);
	void RasterizeRegionOnNextLogicQuant() { rasterize_region_on_next_quant = true; }
};

#define CUNITS_LOCK(player) MTAuto lock_player_units((player)->UnitsLock())

#endif //__PLAYER_H__