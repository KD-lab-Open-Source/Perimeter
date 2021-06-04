#ifndef __UNIVERSE_H__
#define __UNIVERSE_H__

#include "GenericControls.h"
#include "SafeMath.h"
#include "HyperSpace.h"
#include "TrustMap.h"
#include "Region.h"
#include "Player.h"
#include "MonkManager.h"

class terPlayer;
struct TriggerDispatcher;
typedef void(*PROGRESSCALLBACK)(float);

//-------------------------------------

typedef Grid2D<terUnitGeneric, 5, GridVector<terUnitGeneric, 8> > terUnitGridType;

///////////////////////////////////////
//		Игровая вселенная
///////////////////////////////////////
class terUniverse : public terHyperSpace
{
public:
	terUniverse(PNetCenter* net_client, MissionDescription& mission, SavePrm& data,  PROGRESSCALLBACK loadProgressUpdate);

	~terUniverse();

	void Quant();
	void AvatarQuant();
	void PrepareQuant();
	void triggerQuant();

	bool universalSave(const MissionDescription& mission, bool userSave);
	void relaxLoading();

	void addLinkToResolve(const SaveUnitLink* link) { saveUnitLinks_.push_back(link); }

	terPlayer* activePlayer() { return active_player_; }
	terBelligerent activeBelligerent() const { xassert(active_player_); return active_player_->belligerent(); }

	void SetActivePlayer(int id);

	terPlayer* findPlayer(int playerID) { xassert(playerID >= 0 && playerID < Players.size()); return Players[playerID]; }
	terPlayer* worldPlayer() { xassert(!Players.empty() && Players.back()->isWorld()); return Players.back(); }

	terUnitBase* findUnit(const terUnitID& unitID);
	terUnitBase* findUnit(terUnitAttributeID id);
	terUnitBase* findUnitByLabel(const char* label);
	terUnitBase* findCorridor() const;

	bool soundEvent(SoundEventID event_id);

	void checkEvent(const class Event& event);

	void ShowInfo();

	void makeCommand(CommandID id, int data);
	void makeCommand2D(CommandID command_id, const Vect3f& position, CommandSelectionMode mode);
	void makeCommandSubtle(CommandID command_id, const Vect3f& position, CommandSelectionMode mode);
	void makeCommand(CommandID command_id, const Vect3f& position, CommandSelectionMode mode);
	void makeCommand(CommandID command_id, terUnitBase* actionObject, CommandSelectionMode mode);
	void toggleHold();

	void setShouldIgnoreIntfCommands(bool shouldIgnoreIntfCommands) {
		PlayerVect::iterator pi;
		FOR_EACH (Players, pi) {
			(*pi)->setShouldIgnoreIntfCommands(shouldIgnoreIntfCommands);
		}
	}

	void receiveCommand(const netCommand4G_UnitCommand& command);
	void receiveCommand(const netCommand4G_Region& reg);
	bool forcedDefeat(int playerID);

	void MakeGenericList(const Vect2f& pos, UnitList& unit_list);
	int SelectUnit(terUnitBase* p);
	void DeselectAll();
	int SelectSquad(terUnitBase* p);

	terUnitBase* selectedObject();
	void DeleteSelectedObjects();

	void showDebugInfo();
	void WriteDebugInfo();

	terPlayer* addPlayer(const PlayerData& playerData);

	void UpdateSkinColor();

	void changeOwner(terUnitBase* unit, terPlayer* player);

	void updateClusterColumn(const struct sRect& rect);
	const Column& clusterColumn() const { return cluster_column_; }

	int quantCounter() const { return quant_counter_; }

	void switchFieldTransparency();
	bool fieldTransparent() const { return fieldTransparent_; }

	void RefreshAttribute();
	terUnitBase* createDebugObject(terUnitAttributeID id, const Vect3f& pos, float angle = 0);

	RegionMetaDispatcher* activeRegionDispatcher() const { return activeRegionDispatcher_; }

	MultiBodyDispatcher& multiBodyDispatcher() { return multibody_dispatcher; }

	PlayerVect Players;
	
	terUnitGridType UnitGrid;
	
	cSpriteManager* pSpriteCongregation;
	cSpriteManager* pSpriteCongregationProtection;
	cSpriteManager* pSpriteCongregationAnnihilation;

	MTSection* EnergyRegionLocker(){return &lock_energy_region;};

    MonkManager monks;

	cSelectManager select;

	friend terUniverse* universe();
    
protected:
	void clearLinkAndDelete();
	MTSection lock_energy_region;

private:
	terPlayer* active_player_;
	Column cluster_column_;
	
	bool enableEventChecking_;

	int quant_counter_;
	float interpolation_factor_;

	bool fieldTransparent_;

	struct ChangeOwnerData
	{
		terUnitBase* unit_;
		terPlayer* player_;
		
		ChangeOwnerData(terUnitBase* unit, terPlayer* player) : unit_(unit), player_(player) {}
	};
	typedef list<ChangeOwnerData> ChangeOwnerList;
	ChangeOwnerList changeOwnerList;

	RegionMetaDispatcher* activeRegionDispatcher_;

	string loadedGmpName_;

	MultiBodyDispatcher multibody_dispatcher;

	typedef vector<const SaveUnitLink*> SaveUnitLinkList;
	SaveUnitLinkList saveUnitLinks_;

	static terUniverse* universe_;

	//-------------------------------
	void loadZeroLayer();
	void resolveLinks();
};

inline terUniverse* universe() { return terUniverse::universe_; }

extern class FieldDispatcher* field_dispatcher;

extern struct CEffectManager* terEffectD;

//inline int terObjectBoxTest(const Vect3f& min, const Vect3f& max, sPlane4f* p)
//{
//	for(int i = 0;i < 5;i++){
//		if(p[i].GetDistance(min) >= 0) continue;
//		if(p[i].GetDistance(Vect3f(max.x,min.y,min.z)) >= 0) continue;
//		if(p[i].GetDistance(Vect3f(min.x,max.y,min.z)) >= 0) continue;
//		if(p[i].GetDistance(Vect3f(max.x,max.y,min.z)) >= 0) continue;
//		if(p[i].GetDistance(Vect3f(min.x,min.y,max.z)) >= 0) continue;
//		if(p[i].GetDistance(Vect3f(max.x,min.y,max.z)) >= 0) continue;
//		if(p[i].GetDistance(Vect3f(min.x,max.y,max.z)) >= 0) continue;
//		if(p[i].GetDistance(max)>=0) continue;
//		return 0;
//	};
//	return 1;
//};

inline bool terObjectBoxTest(const Vect3f& pos, sPlane4f* box)
{
	bool res = false;

	float leftDist = box[1].GetDistance(pos);
	float rightDist = box[2].GetDistance(pos);
	float topDist = box[3].GetDistance(pos);
	float bottomDist = box[4].GetDistance(pos);

	return
			(leftDist >= 0 && rightDist >= 0)
		&&	(topDist >= 0 && bottomDist >= 0);
};

extern int terShowFPS;

float GetRealHeight(float x,float y,float r);

void terCircleShowGraph(const Vect3f& pos, float r, const struct CircleColor& circleColor);
void terCircleShow(const Vect3f& pos0,const Vect3f& pos1,float r, const struct CircleColor& circleColor);
void terCircleShow(const Vect3f& pos0,const Vect3f& pos1,float r0, float r1, const struct CircleColor& circleColor);

#include "RealUnit.h"
#include "RealInterpolation.h"

#include "IronFrame.h"

class TerraInterface* GreateTerraInterface();

class cAutoUnmark
{
	terUnitBase* p;
public:
	cAutoUnmark(terUnitBase* p_)
	{
		p=p_;
	}

	~cAutoUnmark()
	{
	}

	void operator=(terUnitBase* unit)
	{
		if(p)
			p->Unmark();
		p=unit;
	}

	terUnitBase* operator ()()
	{
		return p;
	}

	terUnitBase* operator ->()
	{
		return p;
	}
};

extern cAutoUnmark _pUnitHover; // Юнит под курсором

#endif //__UNIVERSE_H__