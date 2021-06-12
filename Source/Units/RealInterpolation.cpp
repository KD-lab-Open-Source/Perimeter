#include "StdAfx.h"

#include "RealInterpolation.h"
#include "RealUnit.h"
#include "Universe.h"
#include "Player.h"

#include "Config.h"
#include "PerimeterSound.h"
#include "Runtime.h"

#include "GameShell.h"

terInterpolationReal::terInterpolationReal(terUnitBase* owner)
: terInterpolationPose(owner),
  node(!owner->attr().animationDataTable.empty() ? &owner->attr().animationDataTable[0] : 0)
{
	ObjectPoint = 0;

	HideFlag = 0;

	SpecialEffectID = 0;
	logicObject_ = 0;

	freeze_ = 0;
	heal_ = 0;
	hot_ = 0;
	sight_ = 1.0f;
	build_ = 1.0f;
	life_ = 1.0f;
	hologram_ = 0;

	if(owner->attr().soundSetup.enabled()){
		for(int i = 0; i < owner->attr().soundSetup.sounds.size(); i++)
			soundControllers_.push_back(terSoundController(owner->attr().soundSetup.sounds[i]));
	}
}

terInterpolationReal::~terInterpolationReal()
{
	RELEASE(ObjectPoint);
}

void terInterpolationReal::quant()
{
	NodeControllerList::iterator ni;
	FOR_EACH(nodeControllers_, ni){
		ni->quant();
		if(ni->soundEventID() != SOUND_EVENT_NONE){
			Owner->soundEvent(ni->soundEventID());
			ni->setSoundEventID(SOUND_EVENT_NONE);
		}
	}

	node.quant();

	if(node.soundEventID() != SOUND_EVENT_NONE){
		Owner->soundEvent(node.soundEventID());
		node.setSoundEventID(SOUND_EVENT_NONE);
	}

	if(node.phase_.fresh() && logicObject_ && currentChain())
		logicObject_->SetChannel(currentChain()->chainName, node.phase_.x1());
}

void terInterpolationReal::updateLogicObject()
{
	if(logicObject_){
		if(currentChain())
			logicObject_->SetChannel(currentChain()->chainName, node.phase_.x1());
		logicObject_->SetPosition(Owner->pose());
		logicObject_->Update();
	}
}

void terInterpolationReal::setLogicObject(cLogicObject* logicObject) 
{ 
	logicObject_ = logicObject; 
	
	if(currentChain() && logicObject_)
		logicObject_->SetChannel(currentChain()->chainName, false);
}

void terInterpolationReal::SetModelPoint(cObjectNodeRoot* p)
{
	RELEASE(ObjectPoint);
	ObjectPoint = p;
	ObjectPoint->IncRef();
}

void terInterpolationReal::Start()
{
	terInterpolationBase::Start();

	if(ObjectPoint)
		UpdateSkinColor();

	if(ObjectPoint){

		sColor4f color = Owner->Player->unitColor();
		color -= sColor4f(1.0f,1.0f,1.0f,1.0f);
		color *= hologram_;
		color += sColor4f(1.0f,1.0f,1.0f,1.0f);

		color.r *= life_*(1.0f - hot_);
		color.g *= life_*(1.0f - heal_);
		color.b *= life_*(1.0f - freeze_);
		color.a = sight_*build_;

		sColor4f add_color = sColor4f(hot_, heal_, freeze_, max(heal_,max(max(hot_, freeze_),1.0f - life_)));

		sColorInterpolate c;
		c.color=color;
		c.add_color=add_color;
		color_=c;

		ObjectPoint->ClearAttr(ATTRUNKOBJ_IGNORE);//perfomance warning

		ObjectPoint->SetPosition(matrix());

		SoundControllerList::iterator i_sound;
		FOR_EACH(soundControllers_,i_sound){
			if(i_sound->isPlaying())
				i_sound->setPosition(matrix().trans());
			i_sound->update();
		}

		ObjectPoint->Update();
	}
}

void terInterpolationReal::universalSave(struct SaveInterpolationReal& data) const
{
	node.universalSave(data.node);

	data.nodeControllers.clear();
	NodeControllerList::const_iterator ni;
	FOR_EACH(nodeControllers_, ni){
		data.nodeControllers.push_back(SaveNodeController());
		ni->universalSave(data.nodeControllers.back());
	}
}

void terInterpolationReal::universalLoad(const SaveInterpolationReal& data)
{
	node.universalLoad(data.node);

	int i = 0;
	NodeControllerList::iterator ni;
	FOR_EACH(nodeControllers_, ni){
		if(i >= data.nodeControllers.size())
			break;
		ni->universalLoad(data.nodeControllers[i++]);
	}
}

void terInterpolationReal::interpolate()
{
	node.interpolate();
	
	NodeControllerList::iterator ni;
	FOR_EACH(nodeControllers_, ni)
		ni->interpolate();


	if(ObjectPoint){
		node.phase_(ObjectPoint,false);
		pose_(ObjectPoint);

		sColor4f color = Owner->Player->unitColor();
		color -= sColor4f(1.0f,1.0f,1.0f,1.0f);
		color *= hologram_;
		color += sColor4f(1.0f,1.0f,1.0f,1.0f);

		color.r *= life_*(1.0f - hot_);
		color.g *= life_*(1.0f - heal_);
		color.b *= life_*(1.0f - freeze_);
		color.a = sight_*build_;

		sColor4f add_color = sColor4f(hot_, heal_, freeze_, max(heal_,max(max(hot_, freeze_),1.0f - life_)));
		sColorInterpolate c;
		c.color=color;
		c.add_color=add_color;
		color_=c;
		color_(ObjectPoint);

		PhaseListType::iterator pi;
		FOR_EACH(PhaseControlList,pi)
			pi->Update();

		SoundControllerList::iterator i_sound;
		FOR_EACH(soundControllers_,i_sound){
			if(i_sound->isPlaying())
				i_sound->setPosition(matrix().trans());
			i_sound->update();
		}
	}
}

void terInterpolationReal::UpdateSkinColor()
{
	if(ObjectPoint==NULL)
		return;

	ObjectPoint->SetSkinColor(Owner->Player->unitColor());
	ObjectPoint->SetAttr(ATTRUNKOBJ_SHADOW);

	if(terObjectReflection)
		ObjectPoint->SetAttr(ATTRUNKOBJ_REFLECTION);
	else
		ObjectPoint->ClearAttr(ATTRUNKOBJ_REFLECTION);
}

void terInterpolationReal::SetModel(const char* name,float scale)
{
	PhaseControlList.clear();

	xassert(fabs(scale) > 1e-6);
	cObjectNodeRoot* NewObjectPoint = createObject(name, Owner->Player->belligerent());
	if(scale > 0)
		NewObjectPoint->SetScale(Vect3f(scale,scale,scale));
	NewObjectPoint->Update();

	const vector<AnimationData>& animationTable = Owner->attr().animationDataTable;
	if(!animationTable.empty()){
		const AnimationData* data = &animationTable[0];
		if(data->groupName)
			node.setVisualObject(NewObjectPoint->FindObject(data->groupName));
		else
			node.setVisualObject(NewObjectPoint);
		if(data->initial())
			node.setChain(data->initial()->chainID);
	}

	nodeControllers_.clear();
	for(int i = 1; i < animationTable.size(); i++){
		cObjectNode* node = NewObjectPoint->FindObject(animationTable[i].groupName);
		if(node){
			nodeControllers_.push_back(NodeController(&animationTable[i]));
			nodeControllers_.back().setVisualObject(node);
		}
		//else
		//	xassert_s(0 && "Animation group not found", animationSetup_->data[i].groupName);
	}

	//Атомарное изменение ObjectPoint
	cObjectNodeRoot* OldObjectPoint=ObjectPoint;
	ObjectPoint=NewObjectPoint;
	if(OldObjectPoint){
		if(gameShell->missionEditor())
		{
			//Кривой фикс для редактора миссий
			//логические объекты должны удаляться в логическом кванте!!!
			stream_interpolator.ClearData();
		}

		OldObjectPoint->Release();
		OldObjectPoint=NULL;
	}
}

void terInterpolationReal::SetSize(float size)
{
	if(ObjectPoint){
		float radius=ObjectPoint->GetBoundRadius();
		ObjectPoint->SetScale(Vect3f(size / radius,size / radius,size / radius));
	}
}

void terInterpolationReal::SetScale(float scale)
{
	if(ObjectPoint)	
		ObjectPoint->SetScale(Vect3f(scale,scale,scale));
}

void terInterpolationReal::SetChain(const char* name)
{
	node.SetChain(name);

	if(logicObject_)
		logicObject_->SetChannel(name, false);

	if(ObjectPoint){
		int channel = ObjectPoint->GetCurrentChannel();

		ObjectPoint->SetChannel(name, false);

		if(channel != ObjectPoint->GetCurrentChannel())
			node.phase_.initialize();
	}
}

NodeController* terInterpolationReal::animationGroup(AnimationGroupID groupID)
{
	NodeControllerList::iterator ni;
	FOR_EACH(nodeControllers_, ni)
		if(ni->animationGroupID() == groupID)
			return &*ni;

//	xassert(0);
	return 0;
}

void terInterpolationReal::showDebugInfo()
{
	XBuffer buf(256, 1);
	buf.SetDigits(2);
	node.writeDebugInfo(buf);
	buf < "\n";
	NodeControllerList::iterator ni;
	FOR_EACH(nodeControllers_, ni){
		ni->writeDebugInfo(buf);
		buf < "\n";
	}
	show_text(matrix().trans(), buf, CYAN);
}

terRealPhaseControllerType* terInterpolationReal::AddPhaseController(const char* name)
{
	cObjectNode* n = ObjectPoint->FindSubObject(name);
	if(n){
		PhaseControlList.push_back(terRealPhaseControllerType(n));
		return &(PhaseControlList.back());
	}
	return NULL;
}

terSoundController* terInterpolationReal::AddSound(const char* name,int cycled)
{
	soundControllers_.push_back(terSoundController(name,cycled));
	return &(soundControllers_.back());
}

bool terInterpolationReal::playSound(SoundID id)
{
	SoundControllerList::iterator it = find(soundControllers_.begin(),soundControllers_.end(),id);
	if(it != soundControllers_.end()){
		it->play();
		return true;
	}

	return false;
}

bool terInterpolationReal::stopSound(SoundID id)
{
	SoundControllerList::iterator it = find(soundControllers_.begin(),soundControllers_.end(),id);
	if(it != soundControllers_.end()){
		it->stop();
		return true;
	}

	return false;
}

terSoundController* terInterpolationReal::findSound(SoundID id)
{
	SoundControllerList::iterator it = find(soundControllers_.begin(),soundControllers_.end(),id);
	if(it != soundControllers_.end())
		return &*it;

	return NULL;
}

//----------------------------------

terRealPhaseControllerType::terRealPhaseControllerType(cObjectNode* object)
{
	ObjectPoint = object;

	phase_ = 0;
	angle_x_ = 0;
	angle_z_ = 0;
}

void terRealPhaseControllerType::SetChain(const char* name)
{
	ObjectPoint->SetChannel(name,0);
//	ObjectPoint->SetPhase(phase_.x1(),false);
}

void terRealPhaseControllerType::Update()
{
	phase_(ObjectPoint,false);

	if(angle_z_.fresh())
	{
		angle_z_(ObjectPoint,Z_AXIS);
	}
	else
	if(angle_x_.fresh())
	{
		angle_x_(ObjectPoint,X_AXIS);
	//	ObjectPoint->SetRotate(&Mat3f(angle_x_(),X_AXIS));
	}
}

//----------------------------------------------

terSoundController::terSoundController(const char* name,int cycled)
{
	ID_ = SOUND_NONE;

	sound_ = new SND3DSound;
	sound_->Init(name);

	needStart_ = isPlaying_ = false;

	volume_ = 0;
	frequency_ = 0;
	cycled_ = cycled;
}

terSoundController::terSoundController(const SoundControllerSetup& setup)
{
	ID_ = setup.ID;

	sound_ = new SND3DSound;
	sound_->Init(setup.name);

	needStart_ = isPlaying_ = false;

	volume_ = 1;
	frequency_ = 1;

	cycled_ = setup.cycled;
}

terSoundController::~terSoundController()
{
	delete sound_;
}

void terSoundController::setPosition(const Vect3f& position) 
{ 
	sound_->SetPos(position); 
}

void terSoundController::stop()
{
	sound_->Stop(); 
}

void terSoundController::update()
{
	// Balmer temp пока пускай так, а потом должна быть интерполяция
	if(volume_.fresh())
		sound_->SetVolume(volume_.x1());

	if(frequency_.fresh())
		sound_->SetFrequency(frequency_.x1());

	if(terSoundEnable){
		if(cycled_){
			if(needStart_){
				if(!isPlaying_)
					isPlaying_ = sound_->Play(1);

				needStart_ = false;
			}
			else {
				if(isPlaying_)
					isPlaying_ = !(sound_->Stop());
			}
		}
		else {
			if(needStart_){
				if(!isPlaying_)
					isPlaying_ = sound_->Play(0);
				else
					isPlaying_ = sound_->IsPlayed();

				needStart_ = false;
			}
			else if(isPlaying_)
				isPlaying_ = sound_->IsPlayed();
		}
	}
	else if(isPlaying_)
		isPlaying_ = !(sound_->Stop());
}


//----------------------------------------

terInterpolationCorridor::terInterpolationCorridor(terUnitBase* owner)
: terInterpolationReal(owner)
{
	ProtectionMode = 0;

	int k = 0;
	for(int i = 0;i < PERIMETER_PORT_LIGHT_POINT_MAX;i++){
		LightSource[i] = NULL;
		LightDirection[i] = NULL;

		for(int j = 0;j < PERIMETER_PORT_LIGHT_EFFECT_SCALE;j++){
//			LightPoint[k] = terScene->CreateLine3d("RESOURCE\\EFFECT\\corona_port.tga");
			LightPoint[k] = safe_cast<cLine3d*>(terScene->CreateLine3d(terTexturePortFireTrail));
			LightPoint[k]->SetAttr(ATTRUNKOBJ_ADDBLEND);
			LightPoint[k]->SetAnimation(500,0);
			k++;
		}
	}
}

void terInterpolationCorridor::UpdateSkinColor()
{
	terInterpolationReal::UpdateSkinColor();
	if(terObjectReflection){
		for(int i = 0;i < PERIMETER_PORT_LIGHT_EFFECT_MAX;i++){
			LightPoint[i]->SetAttr(ATTRUNKOBJ_REFLECTION);
		}
	}
	else{
		for(int i = 0;i < PERIMETER_PORT_LIGHT_EFFECT_MAX;i++){
			LightPoint[i]->ClearAttr(ATTRUNKOBJ_REFLECTION);
		}
	}
}

void terInterpolationCorridor::SetModel(const char* name,float scale)
{
	terInterpolationReal::SetModel(name,scale);
	GetLightPoint();
}

void terInterpolationCorridor::Start()
{
	terInterpolationReal::Start();
	UpdateLightPoint();
}

void terInterpolationCorridor::interpolate()
{
	terInterpolationReal::interpolate();
	UpdateLightPoint();
}

void terInterpolationCorridor::GetLightPoint()
{
	LightSource[0] = ObjectPoint->FindObject("group lightning_opena1");
	LightDirection[0] = ObjectPoint->FindObject("group lightning_openb1");

	LightSource[1] = ObjectPoint->FindObject("group lightning_opena2");
	LightDirection[1] = ObjectPoint->FindObject("group lightning_openb2");

	LightSource[2] = ObjectPoint->FindObject("group lightning_opena3");
	LightDirection[2] = ObjectPoint->FindObject("group lightning_openb3");

	LightSource[3] = ObjectPoint->FindObject("group lightning_opena4");
	LightDirection[3] = ObjectPoint->FindObject("group lightning_openb4");
}

const float PERIMETER_PORT_LIGHT_SIZE0 = 30.0f;
const float PERIMETER_PORT_LIGHT_SIZE1 = 50.0f;
const float PERIMETER_PORT_LIGHT_SPREAD0 = 4.0f;
const float PERIMETER_PORT_LIGHT_SPREAD1 = 8.0f;

void terInterpolationCorridor::UpdateLightPoint()
{
	if(ProtectionMode){
		int k = 0;
		for(int i = 0;i < PERIMETER_PORT_LIGHT_POINT_MAX;i++){
			Vect3f v0 = LightSource[i]->GetGlobalPosition().trans();
			Vect3f v = LightDirection[i]->GetGlobalPosition().trans() - v0;
			float d = v.norm();
			if(d > FLT_EPS){
				v *= 1.0f / d;

				Vect3f l0 = v;
				l0 *= PERIMETER_PORT_LIGHT_SIZE0;
				l0 += v0;

				Vect3f l1 = v;
				l1 *= PERIMETER_PORT_LIGHT_SIZE1;
				l1 += v0;

				for(int j = 0;j < PERIMETER_PORT_LIGHT_EFFECT_SCALE;j++){
					Vect3f r0 = l0;
					r0 += Vect3f(0.5f - terEffectRND.frand(),0.5f - terEffectRND.frand(),0.5f - terEffectRND.frand()) * Vect3f(PERIMETER_PORT_LIGHT_SPREAD0,PERIMETER_PORT_LIGHT_SPREAD0,PERIMETER_PORT_LIGHT_SPREAD0);

					Vect3f r1 = l1;
					r1 += Vect3f(0.5f - terEffectRND.frand(),0.5f - terEffectRND.frand(),0.5f - terEffectRND.frand()) * Vect3f(PERIMETER_PORT_LIGHT_SPREAD1,PERIMETER_PORT_LIGHT_SPREAD1,PERIMETER_PORT_LIGHT_SPREAD1);

					sVertexLine l[3];
					l[0].pos=v0;
					l[1].pos=r0;
					l[2].pos=r1;
					l[0].color=sColor4c(255,255,255,255);
					l[1].color=sColor4c(255,255,255,112);
					l[2].color=sColor4c(255,255,255,32);
					l[0].width=l[1].width=l[2].width=2.0f;
					l[0].v=0;
					l[1].v=0.5f;
					l[2].v=1.0f;

//					LightPoint[k]->UpdateVertex(0, v0, sColor4c(255,255,255,255), 2.0f, 0);
//					LightPoint[k]->UpdateVertex(1, r0, sColor4c(255,255,255,112), 2.0f, 0.5f);
//					LightPoint[k]->UpdateVertex(2, r1, sColor4c(255,255,255,32), 2.0f, 1.0f);
					LightPoint[k]->UpdateVertex(3, l);
					LightPoint[k]->ClearAttr(ATTRUNKOBJ_IGNORE);
					k++;
				}
			}
			else{
				for(int j = 0;j < PERIMETER_PORT_LIGHT_EFFECT_SCALE;j++){ 
					LightPoint[k]->SetAttr(ATTRUNKOBJ_IGNORE);
					k++;
				}
			}
		}
	}
	else{
		for(int i = 0;i < PERIMETER_PORT_LIGHT_EFFECT_MAX;i++){
			LightPoint[i]->SetAttr(ATTRUNKOBJ_IGNORE);
		}
	}
}

terInterpolationCorridor::~terInterpolationCorridor()
{
	for(int i = 0;i < PERIMETER_PORT_LIGHT_EFFECT_MAX;i++){
		LightPoint[i]->Release();
		LightPoint[i] = NULL;
	}
}

//----------------------------------

terInterpolationLaser::terInterpolationLaser(terUnitBase* owner) : terInterpolationReal(owner),
	laserSource_(NULL)
{
	isEnabled_ = false;
	disableFlag_ = true;
	disableCounter_ = 0;
	sourcePosition_ = Vect3f(0,0,0);
	targetPosition_ = Vect3f(0,0,0);

	CreateLaserPoint();
}

terInterpolationLaser::~terInterpolationLaser()
{
	if(line_)
		line_->Release();

	line_ = NULL;
}

void terInterpolationLaser::interpolate()
{
	terInterpolationReal::interpolate();

	laserLine_(line_);
}

void terInterpolationLaser::UpdateSkinColor()
{
	terInterpolationReal::UpdateSkinColor();

	if(terObjectReflection)
		line_->SetAttr(ATTRUNKOBJ_REFLECTION);
	else
		line_->ClearAttr(ATTRUNKOBJ_REFLECTION);
}

void terInterpolationLaser::CreateLaserPoint()
{
	const terWeaponSetup* p = &Owner->attr().weaponSetup;
	xassert(p);

	line_ = safe_cast<cLine3d*>(terScene->CreateLine3d(terTextureLaser[p->weaponIndex],terTextureLaserZ[p->weaponIndex]));
	line_->SetAttr(ATTRUNKOBJ_ADDBLEND | ATTRUNKOBJ_IGNORE);
	line_->SetAnimation(32,0);

	sVertexLine l[2];
	l[0].pos=Vect3f(0,0,0);
	l[1].pos=Vect3f(0,0,0);
	l[0].color=l[1].color=sColor4c(255,255,255,255);
	l[0].width=l[1].width=p->laserWidth;
	l[0].v=0;
	l[1].v=0.5;

	line_->UpdateVertex(2,l);
}

void terInterpolationLaser::CreateGunSource()
{
	laserSource_ = logicObject_->FindObject("fire");
}

void terInterpolationLaser::SetModel(const char* name,float scale)
{
	terInterpolationReal::SetModel(name,scale);

	CreateGunSource();
}

void terInterpolationLaser::SetGunEnable(int enable)
{ 
	if(isEnabled_ && !enable)
		disableCounter_ = 2;

	isEnabled_ = (enable) ? true : false;
}

void terInterpolationLaser::setLaserLine(const Vect3f& target_pos)
{
	if(laserSource_){
		Vect3f v0 = laserSource_->GetGlobalMatrix().trans();
		sourcePosition_ = v0;
		targetPosition_ = target_pos;
		laserLine_.set(sourcePosition_,targetPosition_);
		disableFlag_ = false;
	}
}

void terInterpolationLaser::quant()
{
	terInterpolationReal::quant();

	if(!disableFlag_){
		line_->ClearAttr(ATTRUNKOBJ_IGNORE);

		updateLogicObject();
		sourcePosition_ = laserSource_->GetGlobalMatrix().trans();

		terUnitReal* p = safe_cast<terUnitReal*>(Owner);
		if(terUnitBase* tp = p->weaponTarget())
			targetPosition_ = tp->position();

		laserLine_.changeTarget(targetPosition_);

		if(!isEnabled_){
			Vect3f dr = targetPosition_ - sourcePosition_;
			dr.normalize(dr.norm()/2 * (2 - disableCounter_));

			laserLine_.changeSource(sourcePosition_ + dr);

			if(--disableCounter_ < 0)
				disableFlag_ = true;
		}
		else
			laserLine_.changeSource(sourcePosition_);
	}
	else
		line_->SetAttr(ATTRUNKOBJ_IGNORE);
}

//----------------------------------------------

terInterpolationGun::terInterpolationGun(terUnitBase* owner)
: terInterpolationReal(owner)
{
	GunStatus = GunEnable = 0;
	GunDistance = 0;

	GunSource = NULL;
	GunDirection = NULL;
}

void terInterpolationGun::Start()
{
	terInterpolationReal::Start();
	UpdateGun();
}

void terInterpolationGun::interpolate()
{
	terInterpolationReal::interpolate();
	UpdateGun();
}

terInterpolationGun::~terInterpolationGun()
{
}

void terInterpolationGun::CreateGunSource()
{
	GunSource = ObjectPoint->FindObject("group targeting a");
	GunDirection = ObjectPoint->FindObject("group targeting b");
}

void terInterpolationGun::SetModel(const char* name,float scale)
{
	terInterpolationReal::SetModel(name,scale);
	CreateGunSource();
}

void terInterpolationGun::UpdateGun()
{
	if(GunSource && GunDirection){
		GunStatus = GunEnable;
	}
}

//-------------------------------------------------------

terInterpolationStructure::terInterpolationStructure(terUnitBase* owner)
: terInterpolationReal(owner)
{
	GunDistance = 1.0f;
}

terInterpolationStructure::~terInterpolationStructure()
{
}

//-------------------------------------------------------

void terInterpolationCore::UpdateSkinColor()
{
	terInterpolationConnection::UpdateSkinColor();
	if(terObjectReflection)
		ProtectionPoint->SetAttr(ATTRUNKOBJ_REFLECTION);
	else
		ProtectionPoint->ClearAttr(ATTRUNKOBJ_REFLECTION);
}

terInterpolationCore::terInterpolationCore(terUnitBase* owner)
: terInterpolationConnection(owner)
{
	ProtectionSource = 0;

	protection_parameter_ = 1;

	ProtectionMode = 0;

	ProtectionPoint = terScene->CreateObject(terModelLightRay,NULL);
	float radius=ProtectionPoint->GetBoundRadius();
	ProtectionPoint->SetScale(Vect3f(32.0f / radius,32.0f / radius,500.0f / radius));
}

terInterpolationCore::~terInterpolationCore()
{
	if(ProtectionPoint){
		ProtectionPoint->Release();
		ProtectionPoint = NULL;
	}
}

void terInterpolationCore::Start()
{
	terInterpolationConnection::Start();

	sColor4f color = sColor4f(0,1.0f,0,1.0f) * protection_parameter_;
	color += Owner->Player->unitColor() * (1 - protection_parameter_);
	color.a *= sight_;
	protection_color_=color;

	if(HideFlag)
		ProtectionPoint->SetAttr(ATTRUNKOBJ_IGNORE);
	else{
		if(ProtectionMode)
			ProtectionPoint->ClearAttr(ATTRUNKOBJ_IGNORE);
		else
			ProtectionPoint->SetAttr(ATTRUNKOBJ_IGNORE);
	}

	if(ProtectionSource)
		ProtectionPoint->SetPosition(ProtectionSource->GetGlobalPosition());
	else
		ProtectionPoint->SetPosition(matrix());
}

void terInterpolationCore::setProtectionParameter(float protection_parameter) 
{ 
	NodeController* controller = animationGroup(ANIMATION_GROUP_CHARGING);
	if(controller){
		if(protection_parameter == 0 && protection_parameter_ == 0)
			controller->requestChain(CHAIN_DISCHARGED);
		else if(protection_parameter == 1 && protection_parameter_ == 1)
			controller->requestChain(CHAIN_CHARGED);
		else
			controller->requestChain(protection_parameter > protection_parameter_ ? CHAIN_CHARGE : CHAIN_DISCHARGE, protection_parameter);
	}
	protection_parameter_ = protection_parameter; 
}

void terInterpolationCore::setProtectionParameterImmediately(float protection_parameter) 
{ 
	NodeController* controller = animationGroup(ANIMATION_GROUP_CHARGING);
	if(controller){
		if(protection_parameter == 0 && protection_parameter_ == 0)
			controller->setChain(CHAIN_DISCHARGED);
		else if(protection_parameter == 1 && protection_parameter_ == 1)
			controller->setChain(CHAIN_CHARGED);
		else{
			controller->setChain(protection_parameter > protection_parameter_ ? CHAIN_CHARGE : CHAIN_DISCHARGE);
			controller->setPhase(protection_parameter);
		}
	}
	protection_parameter_ = protection_parameter; 
}


void terInterpolationCore::interpolate()
{
	terInterpolationConnection::interpolate();

	if(HideFlag)
		ProtectionPoint->SetAttr(ATTRUNKOBJ_IGNORE);
	else{
		if(ProtectionMode){
			sColor4f color = sColor4f(0,1.0f,0,1.0f)*protection_parameter_;
			color += Owner->Player->unitColor() * (1 - protection_parameter_);
			color.a *= sight_;

			protection_color_=color;
			protection_color_(ProtectionPoint);
			ProtectionPoint->ClearAttr(ATTRUNKOBJ_IGNORE);
		}
		else
			ProtectionPoint->SetAttr(ATTRUNKOBJ_IGNORE);
	}

	if(ProtectionSource)
		ProtectionPoint->SetPosition(ProtectionSource->GetGlobalPosition());
	else
		ProtectionPoint->SetPosition(matrix());
}

void terInterpolationCore::SetModel(const char* name,float scale)
{
	terInterpolationConnection::SetModel(name,scale);
	ProtectionSource = ObjectPoint->FindObject("group lighting");
}

//-------------------------------------------------------

void terInterpolationLeech::SetModel(const char* name,float scale)
{
	terInterpolationReal::SetModel(name,scale);

	if(!Owner->attr().ConnectionPointNames.empty())
		connectionPoint_ = ObjectPoint->FindObject(Owner->attr().ConnectionPointNames[0]);
}

Vect3f terInterpolationLeech::getConnectionPosition() const
{
	if(connectionPoint_)
		return connectionPoint_->GetGlobalPosition().trans();
	else
		return matrix().trans();
}

//-------------------------------------------------------

Vect3f terInterpolationConnection::GetConnectionPosition(const Vect3f& target)
{
	if(!ConnectionPoints.empty()){
		Vect3f point = ConnectionPoints.front()->GetGlobalPosition().trans();
		if(ConnectionPoints.size() == 1)
			return point;
		float dist = target.distance2(point);
		connections_container_t::iterator i = ConnectionPoints.begin();
		for(++i; i != ConnectionPoints.end(); ++i){
			const Vect3f& v = (*i)->GetGlobalPosition().trans();
			float d = v.distance2(target);
			if(dist > d){
				dist = d;
				point = v;
			}
		}
		return point;
	}
	else
		return matrix().trans();
}

void terInterpolationConnection::SetModel(const char* name,float scale)
{
	terInterpolationStructure::SetModel(name,scale);

	ConnectionPoints.clear();

	for(int i = 0; i < Owner->attr().ConnectionPointNames.size(); i++)
		if(cObjectNode* cp = ObjectPoint->FindObject(Owner->attr().ConnectionPointNames[i]))
			ConnectionPoints.push_back(cp);
}


