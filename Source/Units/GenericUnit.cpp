#include "StdAfx.h"

#include "Runtime.h"

#include "Universe.h"
#include "GenericUnit.h"
#include "IronBullet.h"

#include "Config.h"

#include "PerimeterSound.h"
#include "CorpseDynamic.h"

void drawMark2d( cInterfaceRenderDevice *IRender, float x, float y, float phase, float health, float radiusFactor ) {
	sColor4c color(0, 0, 0, 255);
	color.b = 0;
	if (phase > 0.5f) {
		color.r = 510.0f * (1.0f - phase);
		color.g = 255;
	} else {
		color.r = 255;
		color.g = 510.0f * phase;
	}
	sColor4c colorBG(color);
	colorBG.a = 96;

//	float length = (health * markHealthWidthCoeffA + markHealthWidthCoeffB) * terRenderDevice->GetSizeX();
	float length = (health * markHealthWidthCoeffA + markHealthWidthCoeffB) * terRenderDevice->GetSizeX() * radiusFactor;
	int half = length / 2.0f;

	terRenderDevice->DrawRectangle(x - half - 1, y, length + 1, 2, colorBG, true);
//	terRenderDevice->DrawRectangle(x - half - 1, y - 1, length + 1, 3, colorBG, true);
//	terRenderDevice->DrawRectangle(x - half - 1, y - 1, length + 1, 3, colorBG);
	terRenderDevice->DrawRectangle(x - half, y, length * phase, 2, color);
}
//-----------------------------------------------

terUnitGeneric::terUnitGeneric(const UnitTemplate& data) : terUnitBase(data)
{ 
	MTL();
}

terUnitGeneric::~terUnitGeneric()
{
	MTL();
	if(inserted())
		universe()->UnitGrid.Remove(*this);
}

void terUnitGeneric::Kill()
{
	terUnitBase::Kill();
}

void terUnitGeneric::Start()
{
	terUnitBase::Start();
}

void terUnitGeneric::setPose(const Se3f& pose, bool initPose)
{
	MTL();
	xassert(!dead());
	terUnitBase::setPose(pose, initPose);

	if(inserted())
		universe()->UnitGrid.Move(*this, round(position().x), round(position().y), round(radius()));
	else
		universe()->UnitGrid.Insert(*this, round(position().x), round(position().y), round(radius()));
}

void terUnitGeneric::Quant()
{
	terUnitBase::Quant();	
//	ControlPower = GenericPower;
	UnitDamageCheck();
}

void terUnitGeneric::UnitDamageCheck()
{
	if(alive() && !damageMolecula().isAlive()){
		explode();
		SoundExplosion();
		Kill();
	}
}

bool terUnitGeneric::createCorpse()
{
	if(attr().hasCorpse()){
		if(attr().corpseID()!=UNIT_ATTRIBUTE_CORPSE_DYNAMIC)
		{
			terUnitCorpse* p = safe_cast<terUnitCorpse*>(Player->buildUnit(attr().corpseID()));
			p->setParent(this);
			p->Start();
			return true;
		}else
		{
			terCorpseDynamic* p = safe_cast<terCorpseDynamic*>(Player->buildUnit(UNIT_ATTRIBUTE_CORPSE_DYNAMIC));
			p->setParent(this);
			p->Start();
			return true;
		}
	}

	return false;
}

void terUnitGeneric::explode()
{
	if(attr().corpseID()!=UNIT_ATTRIBUTE_CORPSE_DYNAMIC)
	{
		soundEvent(SOUND_EVENT_EXPLOSION);
		if(const terUnitEffectData* eff_data = attr().getEffectData(explosionID())){
			if(EffectKey* key = attr().getEffect(eff_data->effectName)){
				cEffect* eff = terScene->CreateScaledEffect(*key,avatar()->GetModelPoint(),true);

				MatXf pos = (eff_data->needOrientation) ? avatar()->matrix() : MatXf(Mat3f::ID,avatar()->matrix().trans());
				if(GetRigidBodyPoint() && GetRigidBodyPoint()->diggingModeLagged())
					pos.trans() = To3D(pos.trans());

				eff->SetPosition(pos);
			}
		}
	}

	for(int i = 0; i < attr().debrisNumber(); i++){
		const terDebrisData& db = attr().debrisData(i);
		int num = db.count;

		if(db.countRnd == -1)
			num += round(pow(radius() / 4,1.5f));
		else
			num += terLogicRND(db.countRnd);

		for(int j = 0; j < num; j++){
			Vect3f v;
			v.setSpherical(terLogicRNDfrand() * M_PI * 2.0f,M_PI * 0.45 * terLogicRNDfrand(),terLogicRNDfrand() * db.speed);

			terProjectileBase* p = safe_cast<terProjectileBase*>(Player->buildUnit(db.debrisID)); // RND внутри
			p->setSource(NULL,(Vect3f&)position(),v);
			p->Start();	
		}
	}

	if(needCrater() && attr().corpseID()!=UNIT_ATTRIBUTE_CORPSE_DYNAMIC){
		if(!attr().craterDelay() && attr().craterRadius()){
			terUnitBase* p = Player->buildUnit(attr().craterID());
			p->setPose(Se3f(QuatF::ID, position()), true);
			p->setRadius(attr().craterRadius());
			p->Start();
		}
	}

	createCorpse();
}

//-----------------------------------------------
terUnitCorpse::terUnitCorpse(const UnitTemplate& data) : terUnitBase(data)
{
	time_ = lifeTime_ = 0;
	setCrater(0.0f,0);
}

void terUnitCorpse::Start()
{
	terUnitBase::Start();

	time_ = 0;

	AvatarQuant();
	avatar()->Start();
}

void terUnitCorpse::Quant()
{
	terUnitBase::Quant();

	if(++time_ < lifeTime_){
		if(craterRadius_ && time_ > craterDelay_){
			terUnitBase* p = Player->buildUnit(craterID_);
			p->setPose(Se3f(QuatF::ID, position()), true);
			p->setRadius(craterRadius_);
			p->Start();
			craterRadius_ = 0.0f;
		}
	}
	else
		Kill();
}

void terUnitCorpse::setParent(terUnitBase* p)
{
	avatar()->SetModelPoint(p->avatar()->GetModelPoint());
	avatar()->setPose(p->pose());

	setLifeTime(p->attr().corpseLifeTime());

	if(p->attr().craterDelay() && p->needCrater())
		setCrater(p->attr().craterRadius(),p->attr().craterDelay(),p->attr().craterID());
}

//----------------------------------------

terCrater::terCrater(const UnitTemplate& data) : terUnitBase(data),
	effect_(NULL),
	toolzer_(attr().toolzerSetup)
{
	radius_ = 32.0f;
}

terCrater::~terCrater()
{
	if(effect_)
		effect_->Release();
}

void terCrater::Start()
{
	terUnitBase::Start();

	toolzer_.setScale(radius()/toolzer_.radius());
	toolzer_.start();

	if(attr().LifeTime)
		lifeTimer_.start(attr().LifeTime);

	if(EffectKey* key = attr().getEffect(terEffectID(EFFECT_ID_CRATER_EFFECT))){
		effect_ = terScene->CreateEffect(*key,NULL);
		effect_->SetPosition(MatXf(Mat3f::ID,position()));
		effect_->SetParticleRate(1.0f);
	}
}
 
void terCrater::Quant()
{
	terUnitBase::Quant();

	start_timer_auto(CraterQuant,STATISTICS_GROUP_UNITS);

	toolzer_.quant(position().xi(),position().yi(),Player->zerolayer(position2D()));

	if(effect_)
		effect_->SetPosition(MatXf(Mat3f::ID,To3D(position2D())));
	
	if((toolzer_.isFinished() && !attr().LifeTime) || (attr().LifeTime && !lifeTimer_())){
		if(effect_){
/*
			effect_->SetCycled(false);
			effect_->SetParticleRate(0.0f);
			effect_->SetAutoDeleteAfterLife(false);
/*/
			effect_->StopAndReleaseAfterEnd();
/**/
			effect_ = NULL;
		}
		Kill();
	}
}

void terCrater::setPose(const Se3f& poseIn, bool initPose)
{
	Se3f pose = poseIn;
	pose.trans() = To3D(pose.trans());
	terUnitBase::setPose(pose, false);
}

SaveUnitData* terCrater::universalSave(SaveUnitData* base_data)
{
	SaveCraterData* data = castOrCreate<SaveCraterData>(base_data);
	terUnitBase::universalSave(data);

	toolzer_.universalSave(&data->toolzer);

	data->lifeTimer = lifeTimer_();

	return data;
}

void terCrater::universalLoad(const SaveUnitData* base_data)
{
	const SaveCraterData* data = safe_cast<const SaveCraterData*>(base_data);
	terUnitBase::universalLoad(data);

	toolzer_.universalLoad(&data->toolzer);

	if(data->lifeTimer)
		lifeTimer_.start(data->lifeTimer);
}
