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
#include "..\ht\ht.h"

#include "qd_textdb.h"

#include "XPrmArchive.h"
#include "BinaryArchive.h"

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

terUniverse::terUniverse(PNetCenter* net_client, MissionDescription& mission, SavePrm& data, PROGRESSCALLBACK loadProgressUpdate) :
	terHyperSpace(net_client, mission), //mission может измениться в случае rePlay-а
	UnitGrid(vMap.H_SIZE, vMap.V_SIZE),
	cluster_column_(vMap.V_SIZE)
{
	loadProgressUpdate(0.5f);
	xassert(vMap.H_SIZE && vMap.V_SIZE);
	xassert(!universe());
	universe_ = this;

	fieldTransparent_ = true;

	setLogicFp();

	XRndSet(1);
	logicRND.set(1);
	xm_random_generator.set(1);

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

	active_player_ = 0;
	activeRegionDispatcher_ = new RegionMetaDispatcher(2,vMap.V_SIZE);

	enableEventChecking_ = false;
	pSpriteCongregation=terScene->CreateSpriteManager(terTextureCongregationUnit);
	pSpriteCongregation->SetAttr(ATTRUNKOBJ_REFLECTION);
	pSpriteCongregationProtection=terScene->CreateSpriteManager(terTextureCongregationProtection);
	pSpriteCongregationProtection->SetAttr(ATTRUNKOBJ_REFLECTION);
	pSpriteCongregationAnnihilation=terScene->CreateSpriteManager(terTextureCongregationAnnihilation);
	pSpriteCongregationAnnihilation->SetAttr(ATTRUNKOBJ_REFLECTION);

	//UniversalLoad

	//---------------------
	global_time.setTime(mission.globalTime); // Нужно установить время до загрузки spg

	//---------------------
	data = SavePrm();
//	XPrmIArchive(mission.saveName()) >> WRAP_NAME(data, "SavePrm");
	mission.loadMission(data);
	SaveManualData& manualData = data.manualData;

	//---------------------
	string mapName = setExtention(mission.saveNameBinary(), "gmp");
	XStream ffMap(0);
	if(ffMap.open(mapName.c_str(), XS_IN)){
		ffMap.close();
		vMap.loadGameMap(mapName.c_str(), IniManager("Perimeter.ini").getInt("TD","FastLoad"));
		UpdateRegionMap(0,0,vMap.H_SIZE - 1,vMap.V_SIZE - 1);
		loadedGmpName_ = mapName;
	}
	else
		loadedGmpName_ = "";
	vMap.initGrid();
	if(manualData.loadHardness)
		vMap.loadHardness();
	loadProgressUpdate(0.9f);
	
	//---------------------
	gameShell->changeControlState(manualData.controls);
	
	// Загрузка игроков
	xassert(manualData.players.size() <= NETWORK_PLAYERS_MAX);
	vector<int> playerLoadIndices;
	for(int i = 0; i < manualData.players.size(); i++){
		if(mission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_PLAYER || mission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_AI){
			int playerIndex = mission.playersShufflingIndices[i];
			if(gameShell->currentSingleProfile.getLastGameType() == UserSingleProfile::SCENARIO)
				mission.playersData[i].colorIndex = belligerentPropertyTable.find(mission.playersData[i].belligerent).colorIndex;

			terPlayer* player = addPlayer(mission.playersData[i]);
			player->setTriggerChains(manualData.players[playerIndex]);
			player->setPlayerStrategyIndex(playerIndex);
			playerLoadIndices.push_back(playerIndex);
			if(data.players.size() > playerIndex){
				player->universalLoad(data.players[playerIndex]);
				player->setDifficulty(mission.playersData[i].difficulty);
			}
		}
	}

	terPlayer* world_player = addPlayer(PlayerData(Players.size(), "World", BELLIGERENT_EXODUS0, playerWorldColorIdx, REAL_PLAYER_TYPE_WORLD));
	world_player->loadWorld(data);
	world_player->setDifficulty(mission.difficulty);


	//---------------------
	XStream ff(0);
	if(ff.open(setExtention(mission.saveNameBinary(), "dat").c_str(), XS_IN)){
		XBuffer binaryData(ff.size());
		ff.read(binaryData.address(), ff.size());

		int changedCounter, version;
		binaryData > version > changedCounter;
		if(version == REGION_DATA_FILE_VERSION){
			for(int i = 0; i < changedCounter; i++){
				unsigned int playerID;
				binaryData > playerID;
				vector<int>::iterator ii = find(playerLoadIndices.begin(), playerLoadIndices.end(), playerID);
				if(ii != playerLoadIndices.end()){
					terPlayer* player = Players[ii - playerLoadIndices.begin()];
					MetaRegionLock lock(player->RegionPoint);
					player->RegionPoint->loadEditing(binaryData);
					player->RasterizeRegion();
				}
				else{
					RegionMetaDispatcher regionMetaDispatcher(2, vMap.V_SIZE);
					{
						MetaRegionLock lock(&regionMetaDispatcher);
						regionMetaDispatcher.loadEditing(binaryData);
					}
				}
			}
		}
	}

	enableEventChecking_ = true;

	//---------------------
	SetActivePlayer(mission.activePlayerID);
	
	if(universe()->activePlayer()){
		const BelligerentProperty& property = belligerentPropertyTable.find(universe()->activePlayer()->belligerent());
		for(int i = 0; i < 3; i++){
			if(manualData.soundTracks[i].empty())
				manualData.soundTracks[i] = property.soundTracks[i];
		}
	}

	//_pShellDispatcher->RegionEndEdit();
	
	ai_tile_map->InitialUpdate();
	cluster_column_.setUnchanged();
	updateClusterColumn(sRect(0, 0, vMap.H_SIZE, vMap.V_SIZE));
	
	//-----------------
	vMap.WorldRender();

	terMapPoint = terScene->CreateMap(GreateTerraInterface(),
		universe()->Players.size());

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

	loadProgressUpdate(1);
}

terUniverse::~terUniverse()
{
	DBGCHECK;

	enableEventChecking_ = false;

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi)
		(*pi)->killAllUnits();
	FOR_EACH(Players, pi)
		(*pi)->removeUnits();
	monks.Done();
	HTManager::instance()->ClearDeleteUnit(true);
	FOR_EACH(Players, pi)
		delete *pi;
	Players.clear();

	delete ai_tile_map;
	ai_tile_map = 0;

	RELEASE(field_dispatcher);

	delete activeRegionDispatcher_;

	if(terMapPoint){
		terMapPoint->Release(); 
		terMapPoint = 0;
	}

	RELEASE(pSpriteCongregation);
	RELEASE(pSpriteCongregationProtection);
	RELEASE(pSpriteCongregationAnnihilation);
	terScene->DeleteAutoObject();

	xassert(this == universe());
	universe_ = 0;

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
	
	show_dispatcher.clear();

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
			checkEvent(EventUnitPlayer(Event::CAPTURE_BUILDING, iChange->unit_, iChange->player_));
		}
	changeOwnerList.clear();

	list<sRect>::iterator i_area;
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
	list<sRect>::iterator rc;
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
	terPlayer* player = findPlayer(reg.playerID_);
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
	if(player->frame())
		player->frame()->Kill();
	return true;
}

//---------------------------------------------------
void terUniverse::MakeGenericList(const Vect2f& pos, UnitList& unit_list)
{
	Vect3f v0,v1;
	terCamera->calcRayIntersection(pos.x, pos.y, v0, v1);
	Vect3f v01 = v1 - v0;

	float dist, distMin = FLT_INF;
	terUnitBase* unitMin = 0;

	PlayerVect::iterator pi;
	FOR_EACH(Players, pi){
		CUNITS_LOCK(*pi);
		const UnitList& unit_list=(*pi)->units();
		UnitList::const_iterator i_unit;
		FOR_EACH(unit_list, i_unit){
			terUnitBase* unit = *i_unit;
			//if(unit->alive() && unit->selectAble() && (unit->collisionGroup() & (COLLISION_GROUP_ENEMY | COLLISION_GROUP_SELECTED)))
			if(unit->alive() && unit->selectAble() && unit->attr().ID != UNIT_ATTRIBUTE_SQUAD){
				if(unit->avatar() && unit->avatar()->GetModelPoint()){
					safe_cast<cObjectNodeRoot*>(unit->avatar()->GetModelPoint())->Update();
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

	if(unitMin)
		unit_list.push_back(unitMin);
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
MissionDescription::MissionDescription(const char* fname, GameType gameType)
: missionDescriptionID(editMissionDescriptionID)
{
	setChanged();
	xassert(fname);

	quantAmountInPlayReel = 0;
	gameType_ = gameType;

	if(gameType_ == GT_playRellGame){
		init();
		getMissionDescriptionInThePlayReelFile(fname, *this);
		gameType_ = GT_playRellGame;
		setReelName(fname);
	}
	else{
		setSaveName(fname);

		if(getExtention(saveName()) == "spg"){
			string headerName = setExtention(saveName(), "sph");
			XPrmIArchive ia;
			if(ia.open(headerName.c_str()))
				ia >> WRAP_NAME(*this, "MissionDescriptionPrm");
			else if(!ia.open(saveName()))
				return;
			ia >> WRAP_NAME(*this, "MissionDescriptionPrm");
		}
		else{
			BinaryIArchive bia;
			if(!bia.open(saveName()))
				return;
			bia >> WRAP_NAME(*this, "MissionDescriptionPrm");
		}

		worldID_ = vMap.getWorldID(worldName);
		xassert_s(worldID() != -1 && "Не найден мир в worlds.prm: ", worldName);

		missionDescriptionStr_ = qdTextDB::instance().getText(missionDescriptionID);
		if(missionDescriptionStr_ == "")
			missionDescriptionStr_ = missionDescriptionID;

		// Установка АИ
		for(int i = 0; i < min(NETWORK_PLAYERS_MAX, playerAmountScenarioMax); i++){
			playersData[i].clan = i;
			switch(gameType_){
			case GT_SINGLE_PLAYER:
				playersData[i].realPlayerType = i ? REAL_PLAYER_TYPE_AI : REAL_PLAYER_TYPE_PLAYER;
				break;
			case GT_SINGLE_PLAYER_ALL_AI:
				playersData[i].realPlayerType = REAL_PLAYER_TYPE_AI;
				break;
			case GT_SINGLE_PLAYER_NO_AI:
				playersData[i].realPlayerType = REAL_PLAYER_TYPE_PLAYER;
				break;
			case GT_createMPGame:
			case GT_loadMPGame:
				playersData[i].realPlayerType = REAL_PLAYER_TYPE_OPEN;
				break;
			}
		}
	}

	version = currentShortVersion;
}

bool MissionDescription::loadMission(SavePrm& savePrm) const
{
	savePrm = SavePrm();
	MissionDescription missionDescription;
	
	if(getExtention(saveName()) == "spg"){
		XPrmIArchive ia;
		if(!ia.open(saveName()))
			return false;
		ia >> WRAP_NAME(missionDescription, "MissionDescriptionPrm");
		ia >> WRAP_NAME(savePrm, "SavePrm");
	}
	else{
		BinaryIArchive bia;
		if(!bia.open(saveName()))
			return false;
		bia >> WRAP_NAME(missionDescription, "MissionDescriptionPrm");
		bia >> WRAP_NAME(savePrm, "SavePrm");
	}
	return true;
}

bool MissionDescription::saveMission(const SavePrm& savePrm, bool userSave) const 
{
	MissionDescription data = *this;
	
	data.playerAmountScenarioMax = !userSave ? savePrm.manualData.players.size() : universe()->Players.size() - 1;

	if(!strlen(data.worldName))
		data.worldName = vMap.getWorldName(worldID());

	if(!userSave){
		data.activePlayerID = 0;
		for(int i = 0; i < NETWORK_PLAYERS_MAX; i++){
			data.playersData[i].playerID = i;
			data.playersData[i].colorIndex = i;
			data.playersShufflingIndices[i] = i;
		}
		string name = saveName();
		strlwr((char*)name.c_str());
		data.originalSaveName = strstr(name.c_str(), "resource");
	}

//	BinaryOArchive boa(setExtention(saveName(), "spb").c_str());
//	boa << WRAP_NAME(data, "MissionDescriptionPrm");
//	boa << WRAP_NAME(savePrm, "SavePrm");

	XPrmOArchive oa(setExtention(saveName(), "spg").c_str());
	oa << WRAP_NAME(data, "MissionDescriptionPrm");
	oa << WRAP_NAME(savePrm, "SavePrm");
	return oa.close();
}

void MissionDescription::restart() 
{ 
	if(originalSaveName)
		setSaveName(originalSaveName);
}

void MissionDescription::setSaveName(const char* fname) 
{ 
	saveName_ = fname;
	if(getExtention(saveName_.c_str()) != "spb")
		saveName_ = setExtention(saveName_.c_str(), "spg");
	saveNameBinary_ = saveName_;
	saveNameBinary_.erase(saveNameBinary_.size() - 4, saveNameBinary_.size()); 
	for(int i = 0; i < DIFFICULTY_MAX; i++){
		const char* str = missionDifficultyPostfix[i];
		if(!strlen(str))
			continue;
		size_t pos = saveNameBinary_.rfind(str);
		if(pos != string::npos)
			saveNameBinary_.erase(pos, saveNameBinary_.size() - pos);
	}

	missionName_ = saveNameBinary_;
	size_t pos = missionName_.rfind("\\");
	if(pos != string::npos)
		missionName_.erase(0, pos + 1);
	//_strupr((char*)missionName_.c_str());
}

void MissionDescription::setReelName(const char* name) 
{
	fileNamePlayReelGame = name;

	missionNamePlayReelGame = fileNamePlayReelGame;
	size_t pos = missionNamePlayReelGame.rfind("\\");
	if(pos != string::npos)
		missionNamePlayReelGame.erase(0, pos + 1);
}

//---------------------------------------------------------
bool terUniverse::universalSave(const MissionDescription& mission, bool userSave)
{
	SavePrm data;

	data.manualData = gameShell->manualData();

	if(userSave){
		_shellIconManager.save(data.activeTasks);
		data.manualData.interfaceEnabled = true;
		
		for(int i = 0; i < data.manualData.players.size(); i++)
			data.manualData.saveCamera(i, "UserCamera");

		activePlayer()->refreshCameraTrigger("UserCamera");
	}

	PlayerVect playersToSave(NETWORK_PLAYERS_MAX, 0);
	for(int i = 0; i < data.manualData.players.size(); i++){
		if(mission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_PLAYER || mission.playersData[i].realPlayerType == REAL_PLAYER_TYPE_AI){
			playersToSave[mission.playersShufflingIndices[i]] = i < Players.size() - 1 ? Players[i] : 0;
		}
	}

	PlayerVect::iterator pi;
	FOR_EACH(playersToSave, pi){
		data.players.push_back(SavePlayerData());
		if(*pi)
			(*pi)->universalSave(data.players.back(), userSave);
	}

	worldPlayer()->saveWorld(data);

	gameShell->fillControlState(data.manualData.controls);
	
	if(!mission.saveMission(data, userSave))
		return false;

	//---------------------
	// Map changes
	string mapName = setExtention(mission.saveNameBinary(), "gmp");
	if(vMap.IsChanged() || (loadedGmpName_ != "" && (!XStream(0).open(mapName.c_str(), XS_IN) || mapName != loadedGmpName_)) || check_command_line("force_save_gmp")){
		if(!vMap.saveGameMap(mapName.c_str()))
			return false;
		loadedGmpName_ = mapName;
	}
	else if(loadedGmpName_ == "")
		remove(mapName.c_str());

	if(gameShell->missionEditor() && gameShell->missionEditor()->hardnessChanged()){
		gameShell->missionEditor()->clearHardnessChanged();
		vMap.saveHardness();
	}

	//---------------------
	// Region
	if(activePlayer())
		*activePlayer()->RegionPoint = *activeRegionDispatcher();

	XBuffer binaryData(1024, 1);
	int changedCounter = 0;
	binaryData < REGION_DATA_FILE_VERSION < changedCounter;
	FOR_EACH(Players, pi){
		MetaRegionLock lock((*pi)->RegionPoint);
		if((*pi)->RegionPoint->changed()){
			binaryData < (*pi)->playerStrategyIndex();
			(*pi)->RegionPoint->saveEditing(binaryData);
			++changedCounter;
		}
	}
	if(changedCounter){
		int size = binaryData.tell();
		binaryData.set(0);
		binaryData < REGION_DATA_FILE_VERSION < changedCounter;
		XStream ff(setExtention(mission.saveNameBinary(), "dat").c_str(), XS_OUT, 0);
		ff.write(binaryData, size);
		if(ff.ioError())
			return false;
	}
	else
		remove(setExtention(mission.saveName(), "dat").c_str());

	return true;
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
	ShowCursor(1);
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

	switch(activePlayer()->belligerent()){
	case BELLIGERENT_EXODUS0:
	case BELLIGERENT_EXODUS1:
	case BELLIGERENT_EXODUS2:
	case BELLIGERENT_EXODUS3:
	case BELLIGERENT_EXODUS4:
		SNDSetBelligerentIndex(0);
		break;
	case BELLIGERENT_HARKBACKHOOD0:
	case BELLIGERENT_HARKBACKHOOD1:
		SNDSetBelligerentIndex(1);
		break;
	case BELLIGERENT_EMPIRE0:
	case BELLIGERENT_EMPIRE1:
	case BELLIGERENT_EMPIRE2:
	case BELLIGERENT_EMPIRE3:
	case BELLIGERENT_EMPIRE4:
#ifndef _DEMO_
		SNDSetBelligerentIndex(2);
#else
		SNDSetBelligerentIndex(1);
#endif
		break;
	}

	field_dispatcher->Update();

	*activeRegionDispatcher_ = *activePlayer()->RegionPoint;
}

terPlayer* terUniverse::addPlayer(const PlayerData& playerData)
{
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
			angle = terLogicRNDfrand()*M_PI*2;
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
				int r = round(p->radius());
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
			if(fabs(v.x) < b->boxMax().x + Radius && fabs(v.y) < b->boxMax().y + Radius){
				if((b->position().z + b->boxMax().z) > Height)
					Height = b->position().z + b->boxMax().z;
			}
		}
	}
};

float GetRealHeight(float x,float y,float r)
{
	terRealHightOperator op(x,y,r);
	universe()->UnitGrid.Scan(round(x), round(y), round(r), op);
	return op.Height;
}

void terUniverse::updateClusterColumn(const sRect& rect)
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

void terUniverse::checkEvent(const Event& event)
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
	typedef map<int, terUnitReal*> Map;
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
REGISTER_ENUM(REAL_PLAYER_TYPE_WORLD, "REAL_PLAYER_TYPE_WORLD");
END_ENUM_DESCRIPTOR(RealPlayerType);

