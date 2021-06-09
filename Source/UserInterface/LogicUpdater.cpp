#include "StdAfx.h"
#include "LogicUpdater.h"
#include "Universe.h"
#include "MusicManager.h"

LogicUpdater::LogicUpdater() {
	MTINIT(lockSect);
	logicData = new LogicData();
	oldLogicData = new LogicData();
}
LogicUpdater::~LogicUpdater() {
	delete logicData;
	delete oldLogicData;
	MTDONE(lockSect);
}

void LogicUpdater::reset() {
	lock();
	delete logicData;
	delete oldLogicData;
	logicData = new LogicData();
	oldLogicData = new LogicData();
	unlock();
}

void LogicUpdater::update() {
	updateIconsData();
}

void LogicUpdater::exchange() {
	lock();
	swap(oldLogicData, logicData);
	unlock();
}

void LogicUpdater::checkEvent(const Event& event) {
	switch (event.type()) {
		case Event::COMPLETE_BUILDING:
			{
				const EventUnitPlayer& eventUnit = safe_cast_ref<const EventUnitPlayer&>(event);
				if (eventUnit.player() == universe()->activePlayer()) {
					MiniMapEvent ev;
					ev.code = EVENT_BUILDING_CREATED;
					ev.pos = eventUnit.unit()->position2D();
					logicData->miniMapEvents.push_back(ev);
				}
				//teleport
				//filth
			}
			break;
		case Event::ATTACK_OBJECT:
			{
				const EventUnitMyUnitEnemy& eventUnit = safe_cast_ref<const EventUnitMyUnitEnemy&>(event);
				if (eventUnit.unitMy()->selectAble() && eventUnit.unitMy()->Player == universe()->activePlayer()) {
					MiniMapEvent ev;
					ev.code = EVENT_UNIT_UNDER_ATTACK;
					ev.pos = eventUnit.unitMy()->position2D();

					terFrame* frame = eventUnit.unitMy()->Player->frame();
					if (frame) {
						if (frame == eventUnit.unitMy()) {
							ev.btnID = SQSH_TAB_BUILD_ID;
							ev.tabNumber = 0;
						} else {
							for (int i = 0; i < 5; i++) {
								if (frame->isSlotUnit(i, eventUnit.unitMy())) {
									ev.btnID = SQSH_FRAME_TERRAIN_BUILD1_ID + i;
									break;
								} else if (
												eventUnit.unitMy()->attr().ID != UNIT_ATTRIBUTE_TERRAIN_MASTER
											&&	eventUnit.unitMy()->attr().ID != UNIT_ATTRIBUTE_BUILD_MASTER
											&&	eventUnit.unitMy()->GetSquadPoint() == universe()->activePlayer()->squad(i)
										) {
									ev.btnID = SQSH_TAB_SQUAD_ID;
									ev.tabNumber = i;
									break;
								}
							}
						}
					}

					logicData->miniMapEvents.push_back(ev);
				}

				if(eventUnit.unitEnemy())
				{
					terPlayer* active_player=universe()->activePlayer();
					if(eventUnit.unitMy()->Player == active_player || eventUnit.unitEnemy()->Player == active_player)
						MusicOnDamage();
				}
			}
			break;
	}
}

void LogicUpdater::lock() {
	MTENTER(lockSect);
}

void LogicUpdater::unlock() {
	MTLEAVE(lockSect);
}
