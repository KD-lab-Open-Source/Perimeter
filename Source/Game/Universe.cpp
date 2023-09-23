#include "StdAfx.h"

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

#include "IronFrame.h"
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
#include "MissionEdit.h"
#include "PerimeterShellUI.h"
#include "ExternalShow.h"
#include "../HT/ht.h"

#include "qd_textdb.h"

#include "XPrmArchive.h"
#include "BinaryArchive.h"

#include "BelligerentSelect.h"
#include "GameContent.h"

const int REGION_DATA_FILE_VERSION = 8383;

RandomGenerator logicRND;
int terRealCollisionCount = 0;
int terMapUpdatedCount = 0;

terUniverse* terUniverse::universe_ = NULL;

//------------------------------------------
void UpdateRegionMap(int x1,int y1,int x2,int y2)
{
	if(terMapPoint) 
		terMapPoint->UpdateMap(Vect2i(x1,y1), Vect2i(x2,y2));
}

terUniverse::terUniverse(PNetCenter* net_client, MissionDescription& mission, PROGRESSCALLBACK loadProgressUpdate) :
	terHyperSpace(net_client, mission), //mission может измениться в случае rePlay-а
	UnitGrid(vMap.H_SIZE, vMap.V_SIZE),
	cluster_column_(vMap.V_SIZE)
{
	loadProgressUpdate(0.5f);
	xassert(vMap.H_SIZE && vMap.V_SIZE);
	xassert(!universe());
	universe_ = this;

	fieldTransparent_ = true;

	ai_tile_map = new AITileMap(1 << vMap.H_SIZE_POWER, 1 << vMap.V_SIZE_POWER);

	field_dispatcher = terScene->CreateForceFieldDispatcher(vMap.H_SIZE,vMap.V_SIZE,vMap.hZeroPlast,terTextureField01,terTextureField02);

	monks.Init();

	terRealCollisionCount = 0;
	terMapUpdatedCount = 0;

	quant_counter_ = 0;

	if(terObjectReflection)
		field_dispatcher->SetAttr(ATTRUNKOBJ_REFLECTION);
	else
		field_dispatcher->ClearAttr(ATTRUNKOBJ_REFLECTION);

	active_player_ = nullptr;
	activeRegionDispatcher_ = new RegionMetaDispatcher(2,vMap.V_SIZE);

	enableEventChecking_ = false;
	pSpriteCongregation=terScene->CreateSpriteManager(terTextureCongregationUnit);
	pSpriteCongregation->SetAttr(ATTRUNKOBJ_REFLECTION);
	pSpriteCongregationProtection=terScene->CreateSpriteManager(terTextureCongregationProtection);
	pSpriteCongregationProtection->SetAttr(ATTRUNKOBJ_REFLECTION);
	pSpriteCongregationAnnihilation=terScene->CreateSpriteManager(terTextureCongregationAnnihilation);
	pSpriteCongregationAnnihilation->SetAttr(ATTRUNKOBJ_REFLECTION);
}

void terUniverse::clear() {
    enableEventChecking_ = false;
    
    clearLogList();

    stream_interpolator.ClearData();
    
    select.DeselectAll();

    for (terPlayer* p : Players) {
        p->killAllUnits();
    }
    
    for (terPlayer* p : Players) {
        p->removeUnits();
    }
    
    monks.Done();
    
    HTManager::instance()->ClearDeleteUnit(true);
    
    for (terPlayer* p : Players) {
        delete p;
    }    
    Players.clear();

    active_player_ = nullptr;

    terScene->DeleteAutoObject();
}

terUniverse::~terUniverse()
{
	DBGCHECK;

    clear();

	delete ai_tile_map;
	ai_tile_map = 0;

	RELEASE(field_dispatcher);

	delete activeRegionDispatcher_;

	if (terMapPoint) {
		terMapPoint->Release(); 
		terMapPoint = nullptr;
	}

	RELEASE(pSpriteCongregation);
	RELEASE(pSpriteCongregationProtection);
	RELEASE(pSpriteCongregationAnnihilation);

	xassert(this == universe());
	universe_ = nullptr;

	DBGCHECK;
}

//----------------------------------------

struct terMapUnitUpdateOperator
{
	float x0,y0,x1,y1;

	terMapUnitUpdateOperator(int _x0,int _y0,int _x1,int _y1)
	{
		x0 = _x0;
		x1 = _x1;
		y0 = _y0;
		y1 = _y1;
	}

	void operator()(terUnitBase* p)
	{		
		if(p->alive() && p->GetMapUpdatedCount() != terMapUpdatedCount){
			p->MapUpdateHit(x0,y0,x1,y1);
			p->SetMapUpdatedCount(terMapUpdatedCount);
		}
	}
};

struct terMapGridUpdateOperator
{
	int x0,y0,x1,y1;
	terTerraformDispatcher* dispathcer;

	terMapGridUpdateOperator(int _x0,int _y0,int _x1,int _y1)
	{
		x0 = _x0;
		x1 = _x1;
		y0 = _y0;
		y1 = _y1;
		dispathcer=NULL;
	}

	void operator()(terTerraformGeneral* p)
	{		
		if(p->CollisionCount != terRealCollisionCount && p->Type != TERRAFORM_TYPE_GARBAGE){
			MetaRegionLock lock(dispathcer->GetPlayer()->RegionPoint);
			p->Quant(dispathcer,false);
			p->CollisionCount = terRealCollisionCount;
		}
	}
};

void terUniverse::Quant()
{
	start_timer_auto(UniverseQuant,STATISTICS_GROUP_TOTAL);
	DBGCHECK;

	setLogicFp();
	///commands.execute();

	watch(global_time()/float(frame_time() + 1));

	global_time.next_frame();

	triggerQuant();

	vMap.changedAreas.clear();
	if(field_dispatcher){
		field_dispatcher->logicQuant();
		log_var_crc(field_dispatcher->height_array(), field_dispatcher->mapSizeX()*field_dispatcher->mapSizeY());
	}

	multibody_dispatcher.prepare();

	terRealCollisionCount++;
	terMapUpdatedCount++;
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->CollisionQuant();

	multibody_dispatcher.resolve();

	FOR_EACH(Players, pi)
		(*pi)->MoveQuant();

	FOR_EACH(Players, pi)
		(*pi)->Quant();
	monks.quant();

	ChangeOwnerList::iterator iChange;
	FOR_EACH(changeOwnerList,iChange)
		if(iChange->unit_->alive()){
			iChange->unit_->ChangeUnitOwner(iChange->player_);
            EventUnitPlayer ev(Event::CAPTURE_BUILDING, iChange->unit_, iChange->player_);
			checkEvent(&ev);
		}
	changeOwnerList.clear();

	std::list<sRectS>::iterator i_area;
	FOR_EACH(vMap.changedAreas,i_area){
		terMapUnitUpdateOperator unit_op((int)(i_area->x),(int)(i_area->y),(int)(i_area->x + i_area->dx),(int)(i_area->y + i_area->dy));
		UnitGrid.Scan(unit_op.x0, unit_op.y0, unit_op.x1, unit_op.y1, unit_op);

		terMapGridUpdateOperator op(i_area->x, i_area->y, i_area->x + i_area->dx, i_area->y + i_area->dy);

		PlayerVect::iterator it_player;
		FOR_EACH(Players,it_player)
		{
			op.dispathcer=(*it_player)->TrustMap;
			(*it_player)->TrustMap->TrustGrid.Scan(op.x0, op.y0, op.x1, op.y1, op);
		}
	}

	terCamera->destroyLink();

	FOR_EACH(Players, pi)
		(*pi)->EconomicQuant();

	clearLinkAndDelete();

	cluster_column_.setUnchanged();
	std::list<sRectS>::iterator rc;
	FOR_EACH(vMap.changedAreas,rc){
		ai_tile_map->UpdateRect(rc->x,rc->y,rc->dx,rc->dy);
		updateClusterColumn(*rc);
	}

	ai_tile_map->recalcPathFind();

	AvatarQuant();
	
	check_determinacy_quant(false);

	vMap.renderQuant();
}

void terUniverse::AvatarQuant()
{
	quant_counter_++;

	stream_interpolator.Lock();
	stream_interpolator.SetInAvatar(true);
	start_timer_auto(AvatarQuant, STATISTICS_GROUP_LOGIC);

	stream_interpolator.ClearData();

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->AvatarQuant();
	monks.avatarInterpolation();

	stream_interpolator.SetInAvatar(false);
	stream_interpolator.Unlock();

	select.ShowCircles();
}
 
void terUniverse::ShowInfo()
{
	terHyperSpace::ShowInfo();

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->ShowInfo();
}

void terUniverse::receiveCommand(const netCommand4G_Region& reg)
{
	terPlayer* player = findPlayer(reg.PlayerID_);
	if (player) {
		player->incomingCommandRegion(reg);
		if (player->shouldIgnoreIntfCommands() && player == activePlayer()) {
			*activeRegionDispatcher_ = *player->RegionPoint;
		}
	}
}

void terUniverse::PrepareQuant()
{

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->PrepareQuant();

}

void terUniverse::makeCommandSubtle(CommandID command_id, const Vect3f& position, CommandSelectionMode mode)
{
	select.makeCommandSubtle(command_id, position, mode);
}

void terUniverse::makeCommand(CommandID id, int data)
{
	select.makeCommand(id, data);
}

void terUniverse::makeCommand2D(CommandID command_id, const Vect3f& position, CommandSelectionMode mode)
{
	select.makeCommand2D(command_id, position, mode);
}

void terUniverse::toggleHold()
{
	select.toggleHold();
}

void terUniverse::makeCommand(CommandID command_id, const Vect3f& position, CommandSelectionMode mode)
{
	select.makeCommand(command_id, position, mode);
}
void terUniverse::makeCommand(CommandID command_id, terUnitBase* actionObject, CommandSelectionMode mode)
{
	select.makeCommand(command_id, actionObject, mode);
}

int terUniverse::SelectSquad(terUnitBase* p)
{
	return select.SelectSquad(p);
}

void terUniverse::DeselectAll()
{
	select.DeselectAll();
}

void terUniverse::DeleteSelectedObjects()
{
	select.DeleteSelectedObjects();

	clearLinkAndDelete();

//		UnitGrid.Clear();
//		FOR_EACH(Players, pi)
//			(*pi)->GridQuant();
}

terUnitBase* terUniverse::selectedObject() 
{ 
	return select.selectedObject();
}

terUnitBase* terUniverse::findUnit(const terUnitID& unitID)
{
	terPlayer* player = findPlayer(unitID.playerID());
	xassert(player);
	return player->findUnit(unitID.unitID());
}

terUnitBase* terUniverse::findUnit(terUnitAttributeID id)
{
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi){
		terUnitBase* unit = (*pi)->findUnit(id);
		if(unit)
			return unit;
	}
	return 0;
}

terUnitBase* terUniverse::findUnitByLabel(const char* label)
{
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi){
		terUnitBase* unit = (*pi)->findUnitByLabel(label);
		if(unit)
			return unit;
	}
	return 0;
}

void terUniverse::receiveCommand(const netCommand4G_UnitCommand& eventCommand)
{
	terUnitBase* owner = findUnit(eventCommand.owner());
	if(owner && owner->alive()){
		UnitCommand command = eventCommand.unitCommand();
		command.prepareAfterSend();
		owner->commandIncoming(command);
	}
}

bool terUniverse::forcedDefeat(int playerID)
{
	terPlayer* player = findPlayer(playerID);
	xassert(player);
    terFrame* frame = player ? player->frame() : nullptr;
	if (frame) {
        frame->explode();
        frame->Kill();
    }
	return true;
}

//---------------------------------------------------
terUnitBase* terUniverse::TraceUnit(const Vect2f& pos, terUnitID* unit_filter)
{
	Vect3f v0,v1;
	terCamera->calcRayIntersection(pos.x, pos.y, v0, v1);
	Vect3f v01 = v1 - v0;

	float dist, distMin = FLT_INF;
	terUnitBase* unitMin = nullptr;

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi){
        if (unit_filter && (*pi)->playerID() != unit_filter->playerID()) {
            //Not our player
            continue;
        }
		CUNITS_LOCK(*pi);
		const UnitList& unit_list=(*pi)->units();
		UnitList::const_iterator i_unit;
		FOR_EACH(unit_list, i_unit) {
			terUnitBase* unit = *i_unit;
            if (unit_filter && unit->unitID() != unit_filter->unitID()) {
                //Not our unit
                continue;
            }
			//if(unit->alive() && unit->selectAble() && (unit->collisionGroup() & (COLLISION_GROUP_ENEMY | COLLISION_GROUP_SELECTED)))
			if(unit->alive() && unit->selectAble() && unit->attr()->ID != UNIT_ATTRIBUTE_SQUAD){
				if(unit->avatar() && unit->avatar()->GetModelPoint()){
					//safe_cast<cObjectNodeRoot*>(unit->avatar()->GetModelPoint())->Update();
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
	}

	return unitMin;
}

//---------------------------------------------------
terInterpolationBase* terCreateUnitInterpolation(terInterpolationID id, terUnitBase* owner)
{
	switch(id){
		case INTERPOLATION_ID_NONE:
			return NULL;

		case INTERPOLATION_ID_REAL:
			return new terInterpolationReal(owner);

		case INTERPOLATION_ID_CONGREGATION:
			xassert(0);
			return NULL;
		case INTERPOLATION_ID_CORE:
			return new terInterpolationCore(owner);
		case INTERPOLATION_ID_DUMMY:
			return new terInterpolationPose(owner);
		case INTERPOLATION_ID_PORT:
			return new terInterpolationCorridor(owner);

		case INTERPOLATION_ID_LASER:
			return new terInterpolationLaser(owner);
		case INTERPOLATION_ID_GUN:
			return new terInterpolationGun(owner);

		case INTERPOLATION_ID_STRUCTURE:
			return new terInterpolationStructure(owner);
		case INTERPOLATION_ID_CONNECTION:
			return new terInterpolationConnection(owner);
		case INTERPOLATION_ID_LEECH:
			return new terInterpolationLeech(owner);

		case INTERPOLATION_ID_COPY:
			return new terInterpolationCopy(owner);
		case INTERPOLATION_ID_UNINSTALL:
			return new terInterpolationUninstall(owner);

		default:
			xassert(0 && "terInterpolationBase : Can't create Avatar");
			break;
	}
	return NULL;
}

//----------------------------------------------
void terUniverse::RefreshAttribute()
{
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->RefreshAttribute();
}

int terUniverse::SelectUnit(terUnitBase* p)
{
	return select.SelectUnit(p);
}

//---------------------------------------------------------

void MissionDescription::refresh() {
    //if(gameType_ != GT_PLAY_RELL){
        if (!savePathKey_.empty()) {
            savePathContent_ = resolve_mission_path(savePathKey_);
        }
        if (!savePathContent_.empty()) {
            missionDescriptionStr_ = qdTextDB::instance().getText(missionDescriptionID);
            if (missionDescriptionStr_.empty()) {
                missionDescriptionStr_ = missionDescriptionID;
            }
        }
    //}

    if (version.empty()) {
        version = currentShortVersion;
    }

    if (!worldName_.empty()) {
        worldID_ = vMap.getWorldID(worldName_.value().c_str());
    } else if (0 <= worldID_) {
        worldName_ = vMap.getWorldName(worldID_);
    }
    
    if (!originalSaveName) {
        std::string name = string_to_lower(savePathKey_.c_str());
        originalSaveName = strstr(name.c_str(), "resource");
    }
}

void MissionDescription::loadDescription() {    
    if (gameType_ == GT_PLAY_RELL) {
        init();
        //Set this again after init
        gameType_ = GT_PLAY_RELL;
        getMissionDescriptionInThePlayReelFile(playReelPath().c_str(), *this);
    } else {
        if (!savePathContent_.empty()) {
            if (getExtension(savePathContent_, true) == "spg") {
                std::string headerName = setExtension(savePathContent_, "sph");
                XPrmIArchive ia;
                if (ia.open(headerName.c_str())) {
                    ia >> WRAP_NAME(*this, "MissionDescriptionPrm");
                } else if (!ia.open(savePathContent_.c_str())) {
                    return;
                }
                ia >> WRAP_NAME(*this, "MissionDescriptionPrm");
            } else {
                BinaryIArchive bia;
                if (!bia.open(savePathContent_.c_str()))
                    return;
                bia >> WRAP_NAME(*this, "MissionDescriptionPrm");
            }
            
            //Adjust arrays
            fitPlayerArrays();

            // Установка АИ
            for (int i = 0; i < playerAmountScenarioMax; i++) {
                PlayerData& pd = playersData[i];
                pd.flag_playerStartReady = false;
                pd.flag_playerGameReady = false;
                switch (gameType_) {
                    case GT_SINGLE_PLAYER:
                        if (isCampaign()) {
                            switch (pd.realPlayerType) {
                                case REAL_PLAYER_TYPE_CLOSE:
                                case REAL_PLAYER_TYPE_WORLD:
                                case REAL_PLAYER_TYPE_OPEN:
                                    break;
                                case REAL_PLAYER_TYPE_PLAYER:
                                case REAL_PLAYER_TYPE_AI:
                                case REAL_PLAYER_TYPE_PLAYER_AI:
                                    pd.clan = i;
                                    pd.realPlayerType = i ? REAL_PLAYER_TYPE_AI : REAL_PLAYER_TYPE_PLAYER;
                                    break;
                            }
                        }
                        break;
                    case GT_SINGLE_PLAYER_ALL_AI:
                        pd.clan = i;
                        pd.realPlayerType = REAL_PLAYER_TYPE_AI;
                        break;
                    case GT_SINGLE_PLAYER_NO_AI:
                        pd.clan = i;
                        pd.realPlayerType = REAL_PLAYER_TYPE_PLAYER;
                        break;
                    case GT_MULTI_PLAYER_CREATE:
                        pd.clan = i;
                        pd.realPlayerType = REAL_PLAYER_TYPE_OPEN;
                        break;
                    case GT_MULTI_PLAYER_LOAD:
                        switch (pd.realPlayerType) {
                            case REAL_PLAYER_TYPE_OPEN:
                                pd.realPlayerType = REAL_PLAYER_TYPE_CLOSE;
                            case REAL_PLAYER_TYPE_CLOSE:
                            case REAL_PLAYER_TYPE_WORLD:
                            case REAL_PLAYER_TYPE_AI:
                                break;
                            case REAL_PLAYER_TYPE_PLAYER:
                            case REAL_PLAYER_TYPE_PLAYER_AI:
                                pd.setNameInitial(pd.name());
                                pd.realPlayerType = REAL_PLAYER_TYPE_OPEN;
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    refresh();
}

bool MissionDescription::loadMission(SavePrm& savePrm) const
{
	savePrm = SavePrm();
	MissionDescription missionDescription;
	
	if(getExtension(savePathContent_, true) == "spg"){
		XPrmIArchive ia;
		if(!ia.open(savePathContent_.c_str())) {
            return false;
        }
		ia >> WRAP_NAME(missionDescription, "MissionDescriptionPrm");
		ia >> WRAP_NAME(savePrm, "SavePrm");
	}
	else{
		BinaryIArchive bia;
		if(!bia.open(savePathContent_.c_str())) {
            return false;
        }
		bia >> WRAP_NAME(missionDescription, "MissionDescriptionPrm");
		bia >> WRAP_NAME(savePrm, "SavePrm");
	}
	return true;
}

bool MissionDescription::saveMission(const SavePrm& savePrm, bool userSave) const 
{
	MissionDescription data = *this;
    
    data.gameContent = isCampaign() ? getGameContentCampaign() : terGameContentSelect;

	if(!userSave) {
        data.playerAmountScenarioMax = static_cast<int>(savePrm.manualData.players.size());
		data.activePlayerID = 0;
        data.fitPlayerArrays();
		for (int i = 0; i < data.playersData.size(); i++) {
			data.playersData[i].playerID = i;
			data.playersData[i].colorIndex = i;
			data.playersShufflingIndices[i] = i;
		}
		std::string name = string_to_lower(savePathKey_.c_str());
		data.originalSaveName = strstr(name.c_str(), "resource");
	}

//	BinaryOArchive boa(setExtension(saveName(), "spb").c_str());
//	boa << WRAP_NAME(data, "MissionDescriptionPrm");
//	boa << WRAP_NAME(savePrm, "SavePrm");

    XPrmOArchive oa(setExtension(savePathContent(), "spg").c_str());
    oa << WRAP_NAME(data, "MissionDescriptionPrm");
    oa << WRAP_NAME(savePrm, "SavePrm");
    return oa.close();
}

void MissionDescription::loadIntoMemory() {
    XStream ff(0);
    
    //Load saveprm
    SavePrm savePrm;
    loadMission(savePrm);
    XPrmOArchive oaSavePrm;
    oaSavePrm.binary_friendly = true;
    oaSavePrm << WRAP_NAME(savePrm, "SavePrm");
    std::swap(saveData, oaSavePrm.buffer());
    
    //Load compressed binary data from file
    if (ff.open(setExtension(savePathContent(), "bin"), XS_IN) && 0 < ff.size()) {
        binaryData.alloc(ff.size());
        ff.read(binaryData.address(), ff.size());
        binaryData.set(ff.size());
    }
}

void MissionDescription::restart()
{
    if(originalSaveName) {
        setSaveName(originalSaveName);
    }
}

//---------------------------------------------------------

bool terUniverse::universalLoad(MissionDescription& missionToLoad, SavePrm& data, PROGRESSCALLBACK loadProgressUpdate) {
    MTAuto lock(HTManager::instance()->GetLockLogic());
    
    gameShell->CurrentMission = missionToLoad;
    MissionDescription& mission = gameShell->CurrentMission;

    switch (mission.gameType_) {
        case GT_MULTI_PLAYER_RESTORE_PARTIAL:
        case GT_MULTI_PLAYER_RESTORE_FULL:
            mission.gameType_ = GT_MULTI_PLAYER_LOAD;
            break;
        default:
            break;
    }
    
    setLogicFp();

    //---------------------
    //Load binary data from memory or from files

    bool binaryDataLoaded = false;
    XBuffer binarySavePrmBinary(0, true);
    XBuffer binaryMapData(0, true);
    XBuffer binaryRegionData(0, true);

    //Try compressed format (from mission or bin file)
    {
        XBuffer compressedData(0, true);
        XStream ff(0);
        if (mission.binaryData.length()) {
            std::swap(compressedData, mission.binaryData);
        } else if (mission.gameType_ != GT_MULTI_PLAYER_LOAD) {
            if (ff.open(setExtension(mission.savePathContent(), "bin"), XS_IN) && 0 < ff.size()) {
                compressedData.alloc(ff.size());
                ff.read(compressedData.address(), ff.size());
            }
        }
        if (compressedData.length()) {
            XBuffer uncompressedData(compressedData.length(), true);
            compressedData.set(0);
            if (compressedData.uncompress(uncompressedData) == 0) {
                binaryDataLoaded = true;
                uncompressedData.realloc(uncompressedData.tell());
                uncompressedData.set(0);
                uncompressedData > binarySavePrmBinary;
                binarySavePrmBinary.realloc(binarySavePrmBinary.tell());
                binarySavePrmBinary.set(0);
                uncompressedData > binaryMapData;
                binaryMapData.realloc(binaryMapData.tell());
                binaryMapData.set(0);
                uncompressedData > binaryRegionData;
                binaryRegionData.realloc(binaryRegionData.tell());
                binaryRegionData.set(0);
                //Added in 3.0.8, was not present before
                if (uncompressedData.tell() < uncompressedData.length()) {
                    XBuffer replayData(0, true);
                    uncompressedData > replayData;
                    //Do not deserialize save replay data if we are loading an replay since we already load it
                    if (!flag_rePlayReel && replayData.tell()) {
                        replayData.realloc(replayData.tell());
                        replayData.set(0);
                        deserializeGameCommands(replayData, replayData.length());
                    }
                }
            } else {
                fprintf(stderr, "Error decompressing binary save data!\n");
            }
        }
    }

    //Try legacy format (dat/gmp files)
    if (!binaryDataLoaded && gameShell->CurrentMission.gameType_ != GT_MULTI_PLAYER_LOAD) {
        XStream ff(0);
        if (ff.open(setExtension(mission.savePathContent(), "gmp"), XS_IN) && 0 < ff.size()) {
            binaryMapData.alloc(ff.size());
            ff.read(binaryMapData.address(), ff.size());
        }
        if(ff.open(setExtension(mission.savePathContent(), "dat"), XS_IN) && 0 < ff.size()) {
            binaryRegionData.alloc(ff.size());
            ff.read(binaryRegionData.address(), ff.size());
        }
    }

    if (loadProgressUpdate) loadProgressUpdate(0.6f);

    //---------------------

    SavePrmBinary savePrmBinary;

    size_t binSavePrm_len = binarySavePrmBinary.length();
    if (binSavePrm_len) {
        //Make sure is null terminated or XPrmIArchive will attempt out of bounds read
        if (binarySavePrmBinary[binSavePrm_len - 1] != 0) {
            binarySavePrmBinary.realloc(binSavePrm_len + 1);
            binarySavePrmBinary[binSavePrm_len] = 0;
        }
        XPrmIArchive ia;
        std::swap(ia.buffer(), binarySavePrmBinary);
        ia.reset();
        ia >> WRAP_NAME(savePrmBinary, "SavePrmBinary");

        //Restore random generators
        XRndSet(savePrmBinary.X_RND);
        logicRND.set(savePrmBinary.logic_RND);
        xm_random_generator.set(savePrmBinary.xm_RND);
    } else {
        XRndSet(1);
        logicRND.set(1);
        xm_random_generator.set(1);
    }

    //---------------------

    //If a campaign mission then load campaign attributes
    bool campaign = mission.isCampaign();
    loadUnitAttributes(campaign, mission.scriptsData.length() ? &mission.scriptsData : nullptr);
    initUnitAttributes();

    if (loadProgressUpdate) loadProgressUpdate(0.7f);

    //---------------------

    global_time.setTime(mission.globalTime); // Нужно установить время до загрузки spg

    //---------------------
    //Load SavePrm from memory or from file

    data = SavePrm();
    if (mission.saveData.length()) {
        //Load save data into IA and deserialize
        XPrmIArchive ia;
        std::swap(ia.buffer(), mission.saveData);
        ia.reset();
        ia >> WRAP_NAME(data, "SavePrm");
    } else {
        mission.loadMission(data);
    }

    SaveManualData& manualData = data.manualData;

    //---------------------
    // Save Prm Binary

    if (binarySavePrmBinary.length()) {
        //Restore trigger chains
        for (int i = 0; i < savePrmBinary.TriggerChains.size(); ++i) {
            if (i < data.players.size()) {
                std::swap(data.players[i].currentTriggerChains, savePrmBinary.TriggerChains[i]);
            }
        }
    }

    if (loadProgressUpdate) loadProgressUpdate(0.8f);

    //---------------------
    // Map changes
    if (binaryMapData.length()) {
        vMap.loadGameMap(binaryMapData, IniManager("Perimeter.ini").getInt("TD", "FastLoad"));
        UpdateRegionMap(0, 0, vMap.H_SIZE - 1, vMap.V_SIZE - 1);
    }
    vMap.initGrid();
    if(manualData.loadHardness) {
        vMap.loadHardness();
    }

    if (loadProgressUpdate) loadProgressUpdate(0.9f);

    //---------------------
    gameShell->changeControlState(manualData.controls, true);

    // Загрузка игроков
    PlayerData worldPlayerData;
    worldPlayerData.set("World", NETID_NONE, 0, BELLIGERENT_EXODUS0, playerWorldColorIdx, REAL_PLAYER_TYPE_WORLD);
    
    //Load each player
    std::vector<int> playerLoadIndices;
    for(int i = 0; i < mission.playersData.size(); i++){
        auto& playerData = mission.playersData[i];
        if (playerData.realPlayerType == REAL_PLAYER_TYPE_PLAYER
            || playerData.realPlayerType == REAL_PLAYER_TYPE_AI
            || playerData.realPlayerType == REAL_PLAYER_TYPE_PLAYER_AI) {
            if (mission.isCampaign()) {
                playerData.colorIndex = belligerentPropertyTable.find(playerData.belligerent).colorIndex;
            }

            int playerIndex = mission.playersShufflingIndices[i];
            terPlayer* player = addPlayer(playerData);
            if (playerIndex < manualData.players.size()) {
                player->setTriggerChains(manualData.players[playerIndex]);
            }
            player->setPlayerStrategyIndex(playerIndex);
            playerLoadIndices.push_back(playerIndex);
            if(data.players.size() > playerIndex){
                player->universalLoad(data.players[playerIndex]);
                player->setDifficulty(playerData.difficulty);
            }
            //Check if UserCamera exists, otherwise use Camera as spline name for camera initialization
            std::string cameraSplineName = "UserCamera";
            const SaveManualData* manualDataConst = &data.manualData;
            if (!manualDataConst->findCameraSpline((cameraSplineName + std::to_string(playerIndex)).c_str())) {
                cameraSplineName = "Camera";
            }
            player->initializeCameraTrigger(cameraSplineName.c_str());
            worldPlayerData.playerID++;
        }
    }

    //Load world/neutral player
    terPlayer* world_player = addPlayer(worldPlayerData);
    world_player->loadWorld(data);
    world_player->setDifficulty(mission.difficulty);

    //---------------------

    if (binaryRegionData.length()) {
        int changedCounter, version;
        binaryRegionData > version > changedCounter;
        if(version == REGION_DATA_FILE_VERSION){
            for(int i = 0; i < changedCounter; i++){
                unsigned int playerID;
                binaryRegionData > playerID;
                std::vector<int>::iterator ii = find(playerLoadIndices.begin(), playerLoadIndices.end(), playerID);
                if(ii != playerLoadIndices.end()){
                    terPlayer* player = Players[ii - playerLoadIndices.begin()];
                    MetaRegionLock lock(player->RegionPoint);
                    player->RegionPoint->loadEditing(binaryRegionData);
                    player->RasterizeRegion();
                }
                else{
                    RegionMetaDispatcher regionMetaDispatcher(2, vMap.V_SIZE);
                    {
                        MetaRegionLock lock(&regionMetaDispatcher);
                        regionMetaDispatcher.loadEditing(binaryRegionData);
                    }
                }
            }
        } else {
            fprintf(stderr, "Region data version unknown!\n");
        }
    }

    enableEventChecking_ = true;

    //---------------------
    
    SetActivePlayer(mission.activePlayerID);

    if(universe()->activePlayer()){
        const BelligerentProperty& property = belligerentPropertyTable.find(universe()->activePlayer()->belligerent());
        for (int i = 0; i < 3; i++) {
            if (manualData.soundTracks[i].empty()) {
                manualData.soundTracks[i] = property.soundTracks[i];
            }
        }
    }

    //_pShellDispatcher->RegionEndEdit();

    ai_tile_map->InitialUpdate();
    cluster_column_.setUnchanged();
    updateClusterColumn(sRectS(0, 0, vMap.H_SIZE, vMap.V_SIZE));

    //-----------------
    vMap.WorldRender();

    if (!terMapPoint) {
        terMapPoint = terScene->CreateMap(GreateTerraInterface(),
                                          universe()->Players.size());
    }

    PlayerVect::iterator pi;
    FOR_EACH(Players, pi)
        terMapPoint->SetZeroplastColor((*pi)->playerID(), (*pi)->zeroLayerColor());

    if (terMapReflection) {
        terMapPoint->SetAttr(ATTRUNKOBJ_REFLECTION);
    } else {
        terMapPoint->ClearAttr(ATTRUNKOBJ_REFLECTION);
    }

    FOR_EACH(Players, pi)
        (*pi)->chooseEnemyQuant();

    loadZeroLayer();
    resolveLinks();

    ToolzerController::resetActionOp();

    //Flush to avoid crashing when hot-loading
    stream_interpolator.ClearData();
    
    mission.clearData();
    missionToLoad.clearData();

    if (loadProgressUpdate) loadProgressUpdate(1);
    
    return true;
}

bool terUniverse::universalSave(MissionDescription& mission, bool userSave) const {
	SavePrm data;

	data.manualData = gameShell->manualData();

	if (userSave) {
		_shellIconManager.save(data.activeTasks);
		data.manualData.interfaceEnabled = true;
		
		for (int i = 0; i < data.manualData.players.size(); i++) {
            //Set camera to UserCamera for active player once save is opened, the rest use default Camera pos
            //Note that we use strategy index instead of player index so the original camera idx can match user camera
            if (i == activePlayer()->playerStrategyIndex()) {
                data.manualData.saveCamera(i, "UserCamera");
            } else {
                data.manualData.copyCamera(i, "UserCamera", "Camera");
            }
        }
	} else {
        //Clear soundtracks so players don't get wrong belligerent soundtracks
        data.manualData.clearSoundTracks();
    }

	PlayerVect playersToSave(mission.playersData.size(), nullptr);
	for(int i = 0; i < mission.playersData.size(); i++){
		if (mission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_PLAYER
            || mission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_AI
            || mission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_PLAYER_AI){
            terPlayer* player = i < Players.size() - 1 ? Players[i] : nullptr;
			playersToSave[mission.playersShufflingIndices[i]] = player;
		}
	}

    //Store some data in binary prm
    SavePrmBinary savePrmBinary;
    
    //Save RND generator states
    savePrmBinary.X_RND = XRndGet();
    savePrmBinary.logic_RND = logicRND.get();
    savePrmBinary.xm_RND = xm_random_generator.get();

	for (auto& pi : playersToSave) {
		SavePlayerData& savePlayer = data.players.emplace_back();
		if (pi) {
            pi->universalSave(savePlayer, userSave);
            if (!check_command_line("not_triggerchains_binary")) {
                std::swap(savePlayer.currentTriggerChains, savePrmBinary.TriggerChains.emplace_back());
            }
        }
	}

	worldPlayer()->saveWorld(data);

	gameShell->fillControlState(data.manualData.controls);

    XPrmOArchive oaSavePrm;
    oaSavePrm.binary_friendly = true;
    oaSavePrm << WRAP_NAME(data, "SavePrm");
    std::swap(oaSavePrm.buffer(), mission.saveData);
    
    if (!mission.savePathKey().empty()) {
        if (!mission.saveMission(data, userSave)) {
            return false;
        }
    }
    
    //Binary data file content
    XBuffer uncompressedData(10240, true);
    XBuffer binaryData(0, true);

    //---------------------
    // Save Prm Binary

    XPrmOArchive oaSavePrmBinary;
    oaSavePrmBinary.binary_friendly = true;
    oaSavePrmBinary << WRAP_NAME(savePrmBinary, "SavePrmBinary");
    uncompressedData < oaSavePrmBinary.buffer();

	//---------------------
	// Map changes
    if(!vMap.saveGameMap(binaryData)) {
        return false;
    }
    uncompressedData < binaryData;
    binaryData.set(0);
    binaryData.realloc(0);

	if (gameShell->missionEditor() && gameShell->missionEditor()->hardnessChanged()){
		gameShell->missionEditor()->clearHardnessChanged();
		vMap.saveHardness();
	}

	//---------------------
	// Region

	int changedCounter = 0;
    binaryData < REGION_DATA_FILE_VERSION < changedCounter;
	for (auto player : Players) {
        RegionMetaDispatcher* regionPoint = player->RegionPoint;
        if (player == activePlayer()) {
            regionPoint = activeRegionDispatcher();
        }
		MetaRegionLock lock(regionPoint);
		if(regionPoint->changed()){
            binaryData < player->playerStrategyIndex();
            regionPoint->saveEditing(binaryData);
			++changedCounter;
		}
	}
    //Update changedCounter
	if (changedCounter) {
		size_t size = binaryData.tell();
        binaryData.set(sizeof(REGION_DATA_FILE_VERSION));
        binaryData < changedCounter;
        binaryData.set(size);
    }
    uncompressedData < binaryData;
    binaryData.set(0);
    binaryData.realloc(0);
    
    //---------------------
    // Replay data
    if (userSave) {
        serializeGameCommands(binaryData);
    }
    uncompressedData < binaryData;
    binaryData.set(0);
    binaryData.realloc(0);

    //---------------------
    //Compress and save binary data into file
    if (uncompressedData.compress(mission.binaryData) != 0) {
        return false;
    }
    if (mission.savePathKey().empty()) {
        return true;
    } else {
        XStream ff(setExtension(mission.savePathContent(), "bin").c_str(), XS_OUT, 0);
        ff.write(mission.binaryData, mission.binaryData.tell());
        return !ff.ioError();
    }
}

void terUniverse::relaxLoading()
{
	triggerQuant();
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->EconomicQuant();
	triggerQuant();
	FOR_EACH(Players, pi)
		(*pi)->EconomicQuant();
}

//------------------------------------------
void terUniverse::showDebugInfo()
{
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->showDebugInfo();
	
	if(show_field_dispatcher)
		field_dispatcher->showDebugInfo();

//	RegionDispatcher* r = debug_variation ?
//		(*activePlayer()->RegionPoint)[0] : (*activeRegionDispatcher())[0];
//	RegionDispatcher* r = (*activePlayer()->RegionPoint)[0];
//	r->getRasterizeColumn().show(GREEN);
//	r->show(RED, BLUE);

//	if(debug_variation)
//		clusterColumn().show(GREEN);
}

void terUniverse::WriteDebugInfo()
{
	XBuffer DebugWriteBuffer(300,1);
	DebugWriteBuffer.SetDigits(3);
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->WriteDebugInfo(DebugWriteBuffer);
    SDL_ShowCursor(SDL_TRUE);
//	show_debug_window(DebugWriteBuffer, 400, 400);
	RestoreFocus();
}

//----------------------------------------------------------

void terUniverse::SetActivePlayer(int id)
{
	if(activePlayer()){
		*activePlayer()->RegionPoint = *activeRegionDispatcher();
		activePlayer()->SetDeactivePlayer();
	}

	active_player_ = findPlayer(id);
	activePlayer()->SetActivePlayer();
	select.Quant();

    BELLIGERENT_FACTION faction = getBelligerentFaction(activePlayer()->belligerent());
    setBelligerentFactionSound(faction);

	field_dispatcher->Update();

	*activeRegionDispatcher_ = *activePlayer()->RegionPoint;
}

terPlayer* terUniverse::addPlayer(const PlayerData& playerData)
{
    for (auto player : Players) {
        if (player->playerID() == playerData.playerID) {
            player->setPlayerData(playerData);
            return player;
        }
    }
	terPlayer* player = new AIPlayer(playerData);
	Players.push_back(player);
	return player;
}

void terUniverse::UpdateSkinColor()
{
	if(terObjectReflection)
		field_dispatcher->SetAttr(ATTRUNKOBJ_REFLECTION);
	else
		field_dispatcher->ClearAttr(ATTRUNKOBJ_REFLECTION);

//	for(i = 0;i < PERIMETER_MAX_PLAYER;i++)
//		FieldDispatcher->SetColor(NULL,&terPlayerColorData[i],NULL,i);

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->UpdateSkinColor();
}

//-----------------------------------------
void terUniverse::changeOwner(terUnitBase* unit,terPlayer* player)
{
	changeOwnerList.push_back(ChangeOwnerData(unit, player));
}

terUnitBase* terUniverse::createDebugObject(terUnitAttributeID id, const Vect3f& pos, float angle)
{
	terPlayer* player = worldPlayer();
	if(player){
		terUnitBase* p = player->buildUnit(id);
		if(!angle)
			angle = terLogicRNDfrand()*XM_PI*2;
		p->setPose(Se3f(QuatF(angle, Vect3f::K), pos), true);
		p->Start();
		return p;
	}
	return 0;
}


//------------------------------------------------
struct terRealCollisionOperator
{
	float Radius;
	Vect3f Position;
	MatXf Matrix;
 	class RigidBody* BodyPoint;
 	terUnitBase* unit_;
 	terUnitBase* IgnorePoint;

	terRealCollisionOperator(terUnitBase* p)
	{
		unit_ = p;
		BodyPoint = p->GetRigidBodyPoint();
		Matrix = BodyPoint->matrix();
		Position = BodyPoint->position();
		Radius = BodyPoint->radius();
		IgnorePoint = p->GetIgnoreUnit();
	}

	void operator()(terUnitBase* p)
	{
		if(terRealCollisionCount == p->GetRealCollisionCount() && p->alive() && 
			p != IgnorePoint && unit_ != ((terUnitBase*)(p))->GetIgnoreUnit() && 
			!(unit_->excludeCollision() & p->excludeCollision())){

			if(!unit_->isEnemy(p) && ((unit_->collisionGroup() | p->collisionGroup()) & COLLISION_GROUP_ENEMY_ONLY))
				return;
			if(p->collisionGroup() & COLLISION_GROUP_REAL){
				RigidBody* b = p->GetRigidBodyPoint();
				MatXf X12 = b->matrix();
				if(Position.distance2(X12.trans()) < sqr(Radius + b->radius())){
					X12.invert();
					X12.postmult(Matrix);
					if(universe()->multiBodyDispatcher().test(*BodyPoint,*b,X12,true)){
						unit_->Collision(p);
						p->Collision(unit_);
					}
				}
			}
		}
	}
};

void terPlayer::CollisionQuant()
{
	MTL();
	UnitList::iterator i_unit;
	FOR_EACH(Units,i_unit){
		terUnitBase* p = *i_unit;
		if(p->alive()){
			if(p->collisionGroup() & COLLISION_GROUP_REAL){
				int x = p->position2D().xi();
				int y = p->position2D().yi();
				int r = xm::round(p->radius());
				terRealCollisionOperator op(p);
				universe()->UnitGrid.Scan(x, y, r, op);
			}
		}
		p->SetRealCollisionCount(terRealCollisionCount);
	}
}

//-----------------------------------------------------

struct terRealHightOperator
{
	float Radius;
	float PositionX,PositionY;
	float Height;

	terRealHightOperator(float x,float y,float r)
	{
		PositionX = x;
		PositionY = y;
		Radius = r;
		Height = 0;
	}

	void operator()(const terUnitBase* p)
	{
		if(p->alive() && (p->collisionGroup() & COLLISION_GROUP_REAL)){
			RigidBody* b;
			Vect3f v(PositionX, PositionY, 0);
			b = p->GetRigidBodyPoint();
			if(b->underMutation() || !b->prm().kangaroo_allow_on_me || b->flyingMode())
				return;

			b->matrix().invXformPoint(v);
			if(xm::abs(v.x) < b->boxMax().x + Radius && xm::abs(v.y) < b->boxMax().y + Radius){
				if((b->position().z + b->boxMax().z) > Height)
					Height = b->position().z + b->boxMax().z;
			}
		}
	}
};

float GetRealHeight(float x,float y,float r)
{
	terRealHightOperator op(x,y,r);
	universe()->UnitGrid.Scan(xm::round(x), xm::round(y), xm::round(r), op);
	return op.Height;
}

void terUniverse::updateClusterColumn(const sRectS& rect)
{
	int x0 = rect.x;
	int y0 = rect.y;
	int x1 = rect.x + rect.dx;
	int y1 = rect.y + rect.dy;

	for(int y = y0; y < y1; y++){
		CellLine& line = cluster_column_[y];
		int offset = vMap.offsetBuf(0, y);

		int xb = x0;
		bool leveled = vMap.leveled(offset + xb);
		do{
			if(leveled){
				int x;
				for(x = xb + 1; x < x1; ++x)
					if(!vMap.leveled(offset + x))
						break;
				line.add(Interval(xb, x - 1));
				xb = x;
				leveled = false;
			}
			else{
				int x;
				for(x = xb + 1; x < x1; ++x)
					if(vMap.leveled(offset + x))
						break;
				line.sub(Interval(xb, x - 1));
				xb = x;
				leveled = true;
			}
		} while(xb < x1);
	}
}

void terUniverse::switchFieldTransparency()
{
	fieldTransparent_ = !fieldTransparent_;
	field_dispatcher->enableTransparency(fieldTransparent_);
}

terUnitBase* terUniverse::findCorridor() const
{
	terUnitBase* unit = universe()->worldPlayer()->findUnit(UNIT_ATTRIBUTE_CORRIDOR_ALPHA);
	if(unit)
		return unit;
	PlayerVect::const_iterator pi;
	FOR_EACH(Players, pi)
		if((unit = (*pi)->findUnit(UNIT_ATTRIBUTE_CORRIDOR_OMEGA)) != 0)
			return unit;
	return 0;
}

void terUniverse::checkEvent(const Event* event)
{
	if(!enableEventChecking_)
		return;
	
	gameShell->getLogicUpdater().checkEvent(event);
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->checkEvent(event);
}

void terUniverse::triggerQuant()
{
	start_timer_auto(TriggerQuant,STATISTICS_GROUP_TOTAL);

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->triggerQuant();
}

void terUniverse::clearLinkAndDelete()
{
	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->DestroyLink();
	select.Quant();//Потом перенести в graph
	FOR_EACH(Players, pi)
		(*pi)->DeleteQuant();
	monks.deleteQuant();
}

void terUniverse::loadZeroLayer()
{
	typedef std::map<int, terUnitReal*> Map;
	Map map;

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi){
		terPlayer& player = **pi;
		terFrame* frame = player.frame();
		if(frame && frame->zeroLayerPlaced())
			map.insert(Map::value_type(frame->zeroLayerPlaced(), frame));
		terBuildingList::iterator bi;
		FOR_EACH(player.buildingList(UNIT_ATTRIBUTE_CORE), bi){
			terBuilding* core = *bi;
			if(core->zeroLayerPlaced())
				map.insert(Map::value_type(core->zeroLayerPlaced(), core));
		}
	}

	Map::iterator i;
	FOR_EACH(map, i)
		i->second->placeZeroLayer(true);
}

bool terUniverse::soundEvent(SoundEventID event_id)
{
	if(active_player_)
		return active_player_->soundEvent(event_id);

	return false;
}

void terUniverse::resolveLinks()
{
	SaveUnitLinkList::iterator li;
	FOR_EACH(saveUnitLinks_, li)
		(*li)->resolveLink();
	saveUnitLinks_.clear();
}

BEGIN_ENUM_DESCRIPTOR(RealPlayerType, "RealPlayerType");
REGISTER_ENUM(REAL_PLAYER_TYPE_CLOSE, "REAL_PLAYER_TYPE_CLOSE");
REGISTER_ENUM(REAL_PLAYER_TYPE_OPEN, "REAL_PLAYER_TYPE_OPEN");
REGISTER_ENUM(REAL_PLAYER_TYPE_PLAYER, "REAL_PLAYER_TYPE_PLAYER");
REGISTER_ENUM(REAL_PLAYER_TYPE_AI, "REAL_PLAYER_TYPE_AI");
REGISTER_ENUM(REAL_PLAYER_TYPE_PLAYER_AI, "REAL_PLAYER_TYPE_PLAYER_AI");
REGISTER_ENUM(REAL_PLAYER_TYPE_WORLD, "REAL_PLAYER_TYPE_WORLD");
END_ENUM_DESCRIPTOR(RealPlayerType);

