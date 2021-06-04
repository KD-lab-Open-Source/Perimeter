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
#include "..\ht\ht.h"

#include "CorpseDynamic.h"

terCorpseDynamic::terCorpseDynamic(const UnitTemplate& data)
:terUnitReal(data)
{
	OriginalAttribute=NULL;
	setCrater(0.0f,0);
	is_kill=false;
}

terCorpseDynamic::~terCorpseDynamic()
{
}

void terCorpseDynamic::setParent(terUnitBase* p)
{
	OriginalAttribute = &p->attr();
	avatar()->SetModelPoint(p->avatar()->GetModelPoint());
	
	BodyPoint->build(*attr().rigidBodyPrm, avatar()->GetModelPoint(), p->GetRigidBodyPoint()->boxMin(), p->GetRigidBodyPoint()->boxMax());

	setPose(p->pose(),false);
//	avatar()->setPose(p->pose());
	
	if(p->needCrater() && p->attr().craterRadius())
		setCrater(p->attr().craterRadius(),p->attr().craterDelay(),p->attr().craterID());

	if(p->GetRigidBodyPoint())
		BodyPoint->setVelocity(p->GetRigidBodyPoint()->velocity());

	for(int i = 0; i < OriginalAttribute->effectsData.effects.size(); i++){
		terEffectID id=OriginalAttribute->effectsData.effects[i].effectID;
		switch(id){
		case EFFECT_ID_UNIT_SMOKE:
			if(EffectKey* key = OriginalAttribute->getEffect(OriginalAttribute->effectsData.effects[i].effectID)){
				cEffect* effect = terScene->CreateScaledEffect(*key,avatar()->GetModelPoint());
				effect->SetPosition(avatar()->matrix());
				effect_data=OriginalAttribute->effectsData.effects[i];

				//Да, не радует меня эта строчка, хоть класс terEffectController
				//копируй
				effect_data.effectID=EFFECT_ID_LASER_HIT;
				effectControllers_.push_back(terEffectController(&effect_data,effect));
			}
			break;
		}
	}

	dark_timer.start(dataCorpseDynamic.dark_duration);
}

void terCorpseDynamic::Collision(terUnitBase* p)
{
//	Kill();
}

void terCorpseDynamic::AvatarQuant()
{
	avatar()->setPose(pose());

	float d=dataCorpseDynamic.dark_duration;
	float m=dataCorpseDynamic.min_dark;
	realAvatar()->setLife(dark_timer()/d*(1-m)+m);
	terUnitReal::AvatarQuant();
}

void terCorpseDynamic::Quant()
{
	terUnitReal::Quant();

	Vect3f pos=To3D(position());

	if(pos.z>=position().z)
	{
		destruct();
	}

	if(is_kill)
	{
		if(!kill_timer)
			Kill();
	}
}

void terCorpseDynamic::setRealModel(int modelIndex, float scale)
{
//	terUnitReal::setRealModel(modelName,logicName,scale);
}

void terCorpseDynamic::explode()
{
	if(const terUnitEffectData* eff_data = OriginalAttribute->getEffectData(explosionID())){
		if(EffectKey* key = OriginalAttribute->getEffect(eff_data->effectName)){
			cEffect* eff = terScene->CreateScaledEffect(*key,avatar()->GetModelPoint(),true);

			MatXf pos = (eff_data->needOrientation) ? avatar()->matrix() : MatXf(Mat3f::ID,avatar()->matrix().trans());
			pos.trans()=To3D(pos.trans());

			eff->SetPosition(pos);
		}
	}
}

void terCorpseDynamic::destruct()
{
	if(is_kill)
		return;
	is_kill=true;
	if(craterRadius_){
		terUnitBase* p = Player->buildUnit(craterID_);
		p->setPose(Se3f(QuatF::ID, position()), true);
		p->setRadius(craterRadius_);
		p->Start();
		craterRadius_ = 0.0f;
	}

	if(OriginalAttribute->soundSetup.enabled()){
		const SoundEventSetup* ev = OriginalAttribute->soundSetup.findEvent(SOUND_EVENT_EXPLOSION);
		if(ev) soundEvent(ev);
	}

	explode();
	kill_timer.start(1000);
	realAvatar()->setSight(0.8f);
}

void terCorpseDynamic::UnitDamageCheck()
{
}

