#include "StdAfx.h"

#include "Universe.h"
#include "SecondLegion.h"

#include "Config.h"
#include "ForceField.h"

#include "GridTools.h"

#include "PerimeterSound.h"
#include "IronExplosion.h"

int CheckShotLine(Vect3f& from,Vect3f& to);
int CheckFieldLine(Vect3f& from,Vect3f& v,float d,int cluster_id);

//----------------------------------------------------------------

void terMinotaurLegionType::WayPointStart()
{
	terUnitLegionary::WayPointStart();
	ChainEnable = 1;
	StartMainChain();
	HandFlag = 0;
	RelaxCount = 0;
	Transaction = 0;
	LeftLogicControl = LogicObjectPoint->FindObject("fire2");
	RightLogicControl = LogicObjectPoint->FindObject("fire1");
}

void terMinotaurLegionType::Quant()
{
	if(RelaxCount > 0)
		RelaxCount--;
	if(ChainEnable)
		MainChain();
	ChainEnable = 1;
}

void terMinotaurLegionType::AvatarPreMutation()
{
	switch(ChainMode){
		case PERIMETER_MINOTAUR_CHAIN_MODE_MAIN:
			avatar()->SetChain("main");
			break;
		case PERIMETER_MINOTAUR_CHAIN_MODE_LEFT_MINE:
			avatar()->SetChain("fire 01");
			break;
		case PERIMETER_MINOTAUR_CHAIN_MODE_RIGHT_MINE:
			avatar()->SetChain("fire 02");
			break;
	}
	realAvatar()->setPhase(ChainPhase.phase());
}

int terMinotaurLegionType::RequestScriptMove()
{
	MatXf head;
	int visibility;
	terUnitBase* n;

	if(!Transaction){
		if(RelaxCount <= 0){
			RelaxCount = 4;
			HandFlag ^= 1;
			Transaction = 1;
		}
	}

	if(Transaction){
		if(HandFlag){
			if(LeftChain()){
				UpdateLogicPoint();
				LogicObjectPoint->SetChannel("fire 01",ChainPhase.phase());
				LogicObjectPoint->Update();
				LeftLogicControl->GetAnimation(&head,&visibility);

				n = Player->buildUnit(UNIT_ATTRIBUTE_MINE);
				n->setPose(Se3f(QuatF::ID, head.trans()), true);
				n->Start();
				Transaction = 0;
			}
		}else{
			if(RightChain()){
				UpdateLogicPoint();
				LogicObjectPoint->SetChannel("fire 02",ChainPhase.phase());
				LogicObjectPoint->Update();
				RightLogicControl->GetAnimation(&head,&visibility);

				n = Player->buildUnit(UNIT_ATTRIBUTE_MINE);
				n->setPose(Se3f(QuatF::ID, head.trans()), true);
				n->Start();
				Transaction = 0;
			}
		}
		ChainEnable = 0;
	}
	return 1;
}

int terMinotaurLegionType::RequestScriptDisintegrate()
{
	ChainEnable = 0;
	return !BodyPoint->underMutation();
}

int terMinotaurLegionType::RequestScriptIntegrate()
{
	ChainEnable = 0;
	return !BodyPoint->underMutation();
}

void terMinotaurLegionType::StartMainChain()
{
	ChainMode = PERIMETER_MINOTAUR_CHAIN_MODE_MAIN;
	ChainPhase.Start(0,0.1f,1);
}

void terMinotaurLegionType::StartLeftChain()
{
	ChainMode = PERIMETER_MINOTAUR_CHAIN_MODE_LEFT_MINE;
	ChainPhase.Start(0,0.1f,0);
}

void terMinotaurLegionType::StartRightChain()
{
	ChainMode = PERIMETER_MINOTAUR_CHAIN_MODE_RIGHT_MINE;
	ChainPhase.Start(0,0.1f,0);
}

int terMinotaurLegionType::MainChain()
{
	switch(ChainMode){
		case PERIMETER_MINOTAUR_CHAIN_MODE_MAIN:
			ChainPhase.Quant();
			return 1;
		case PERIMETER_MINOTAUR_CHAIN_MODE_RIGHT_MINE:
		case PERIMETER_MINOTAUR_CHAIN_MODE_LEFT_MINE:
			ChainPhase.Quant();
			if(ChainPhase.isEnd())
				StartMainChain();
			break;
	}
	return 0;
}

int terMinotaurLegionType::LeftChain()
{
	switch(ChainMode){
		case PERIMETER_MINOTAUR_CHAIN_MODE_MAIN:
			if(ChainPhase.Quant())
				StartLeftChain();
			break;
		case PERIMETER_MINOTAUR_CHAIN_MODE_RIGHT_MINE:
			ChainPhase.Quant();
			if(ChainPhase.isEnd())
				StartLeftChain();
			break;
		case PERIMETER_MINOTAUR_CHAIN_MODE_LEFT_MINE:
			ChainPhase.Quant();
			if(ChainPhase.isEnd())
				return 1;
			break;
	}
	return 0;
}

int terMinotaurLegionType::RightChain()
{
	switch(ChainMode){
		case PERIMETER_MINOTAUR_CHAIN_MODE_MAIN:
			if(ChainPhase.Quant())
				StartRightChain();
			break;
		case PERIMETER_MINOTAUR_CHAIN_MODE_RIGHT_MINE:
			ChainPhase.Quant();
			if(ChainPhase.isEnd())
				return 1;
			break;
		case PERIMETER_MINOTAUR_CHAIN_MODE_LEFT_MINE:
			ChainPhase.Quant();
			if(ChainPhase.isEnd())
				StartRightChain();
			break;
	}
	return 0;
}


