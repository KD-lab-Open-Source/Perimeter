#include "StdAfx.h"

#include "umath.h"
#include "IVisGeneric.h"
#include "VisGenericDefine.h"
#include "IRenderDevice.h"

#include "terra.h"

#include "Runtime.h"

#include "Universe.h"
#include "Player.h"

#include "Config.h"

//------------------------------------
terAnimationPhaseIteratorType::terAnimationPhaseIteratorType()
{
	cycled_ = 0;
	phase_ = 0;
	deltaPhase_ = 0;
	stopped_ = 0;
}

void terAnimationPhaseIteratorType::checkDelta()
{
	if(deltaPhase_<-1)
		deltaPhase_=-1;
	if(deltaPhase_>1)
		deltaPhase_=1;
}

void terAnimationPhaseIteratorType::startPeriod(float phase, float period, bool cycled)
{
	xassert(phase >= 0 && phase <= 1);
	phase_ = phase;
	setPeriod(period);
	checkDelta();
	cycled_ = cycled;
	stopped_ = 0;
	endPhase_ = deltaPhase_ > 0 ? 1 : 0;
}

void terAnimationPhaseIteratorType::startPhase(float phase, float delta_phase, bool cycled)
{
	xassert(phase>=0 && phase<=1);
	phase_ = phase;
	deltaPhase_ = delta_phase;
	checkDelta();
	cycled_ = cycled;
	stopped_ = 0;
	endPhase_ = deltaPhase_ > 0 ? 1 : 0;
}

void terAnimationPhaseIteratorType::setPeriod(float period)
{
	deltaPhase_ = period ? terLogicTimePeriod/(float)period : 0;
}

float terAnimationPhaseIteratorType::period() const
{
	return deltaPhase_ ? terLogicTimePeriod/deltaPhase_ : 0;
}

void terAnimationPhaseIteratorType::setEndPhase(float endPhase, bool cycled) 
{ 
	xassert(endPhase>=0 && endPhase<=1);
	endPhase_ = endPhase; 
	float deltaPhase_Abs = fabsf(deltaPhase_);
	if(cycled){
		deltaPhase_ = SIGN(deltaPhase_Abs, getDist(phase_, endPhase_, 1 + FLT_EPS));
		if(deltaPhase_ > 0 && endPhase_ < phase_)
			endPhase_ += 1;
		else if(deltaPhase_ < 0 && endPhase_ > phase_)
			endPhase_ -= 1;
	}
	else
		deltaPhase_ = endPhase_ > phase_ ? deltaPhase_Abs : -deltaPhase_Abs;
	cycled_ = false;
}

int terAnimationPhaseIteratorType::Quant()
{
	phase_ += deltaPhase_;
	if(deltaPhase_ > 0 && phase_ > endPhase_ || deltaPhase_ < 0 && phase_ < endPhase_){
		if(cycled_){
			phase_ -= SIGN(deltaPhase_);
		}
		else{
			phase_ = endPhase_;
			stopped_ = 1;
		}
		xassert(phase_>=0 && phase_<=1);
		return 1;
	}
	xassert(phase_>=0 && phase_<=1);
	return 0;
}

//------------------------------------
terInterpolationBase::terInterpolationBase(terUnitBase* owner)
{ 
	Owner = owner;
}
  
void terInterpolationBase::quant()
{
}

void terInterpolationBase::updateLogicObject()
{
	xassert(0);
}

void terInterpolationBase::setLogicObject(cLogicObject* logicObject) 
{ 
	xassert(0);
}

void terInterpolationBase::SetChain(const char* name)
{
	xassert(0);
}

NodeController* terInterpolationBase::animationGroup(AnimationGroupID groupID)
{
	xassert(0);
	return 0;
}

void terInterpolationBase::showDebugInfo()
{
	XBuffer buf(256, 1);
	buf.SetDigits(2);
	buf < "\n";
	show_text(matrix().trans(), buf, CYAN);
}

//--------------------------------------------------
terInterpolationPose::terInterpolationPose(class terUnitBase* owner)
:terInterpolationBase(owner)
{
	setPose(Se3f::ID);
	pose_.initialize();
}


//--------------------------------------------------

terInterpolationMonk::terInterpolationMonk()
{
	HideFlag = true;
	ProtectMode = 0;
	update=true;

	ObjectPoint = NULL;
	ProtectionPoint=NULL;

	
	Size = 1.0f;
}

terInterpolationMonk::~terInterpolationMonk()
{
	RELEASE(ProtectionPoint);
	RELEASE(ObjectPoint);
}

void terInterpolationMonk::Start(float scale)
{
	Size = scale;

	xassert(!ObjectPoint);
	terUniverse* uni=universe();
	ObjectPoint = uni->pSpriteCongregation->Create();
	ProtectionPoint = uni->pSpriteCongregationProtection->Create();

	float s=Size/1.73f;
	ObjectPoint->SetRadius(s);
	ProtectionPoint->SetRadius(2.0f*s);

	ColorUpdate();
}

void terInterpolationMonk::ColorUpdate()
{
	if(update)
	{
		update=false;
		if(HideFlag){
			ObjectPoint->SetIgnore(true);
			ProtectionPoint->SetIgnore(true);
		}
		else{
			ObjectPoint->SetIgnore(false);
			ProtectionPoint->SetIgnore(!(ProtectMode == 1));
		}
	}
}

void terInterpolationMonk::setPos(const Vect3f& pos)
{
	pose_ = pos;
	Vect3f protect=pos;
	protect.z+=Size;
	pose_protection=protect;
}

void terInterpolationMonk::interpolate()
{
	if(ObjectPoint){
		ColorUpdate();
		pose_((cIUnkObj*)ObjectPoint);
		if(ProtectMode==1)
		{
			pose_protection((cIUnkObj*)ProtectionPoint);
		}
	}
}

//------------------------------------------

terInterpolationCopy::terInterpolationCopy(terUnitBase* owner)
: terInterpolationPose(owner)
{
	ObjectPoint = 0;
}

terInterpolationCopy::~terInterpolationCopy()
{
	RELEASE(ObjectPoint);
}

void terInterpolationCopy::SetModelPoint(cObjectNodeRoot* p)
{
	RELEASE(ObjectPoint);
	ObjectPoint = p;
	ObjectPoint->IncRef();
}

void terInterpolationCopy::Start()
{
	terInterpolationBase::Start();
	if(ObjectPoint){
		ObjectPoint->ClearAttr(ATTRUNKOBJ_REFLECTION | ATTRUNKOBJ_SHADOW);
		UpdateSkinColor();
		ObjectPoint->SetPosition(matrix());
	}
}

void terInterpolationCopy::interpolate()
{
	if(ObjectPoint)
	{
		pose_(ObjectPoint);
	}
}


void terInterpolationCopy::UpdateSkinColor()
{
	if(ObjectPoint)
		ObjectPoint->SetSkinColor(Owner->Player->unitColor());
}

//------------------------------------------------------

terInterpolationUninstall::terInterpolationUninstall(terUnitBase* owner)
: terInterpolationCopy(owner)
{
	sight_ = 1.0f;
	hologram_ = 0.0f;
}

terInterpolationUninstall::~terInterpolationUninstall()
{
}

void terInterpolationUninstall::Start()
{
	terInterpolationCopy::Start();

	updateColor();
}

void terInterpolationUninstall::interpolate()
{
	terInterpolationCopy::interpolate();

	updateColor();
	color_(ObjectPoint);
}

void terInterpolationUninstall::updateColor()
{
	sColor4f color = Owner->Player->unitColor();
	color -= sColor4f(1.0f,1.0f,1.0f,1.0f);
	color *= hologram_;
	color += sColor4f(1.0f,1.0f,1.0f,1.0f);

	color.a = sight_;

	sColorInterpolate c;
	c.color=color;
//	c.add_color=add_color;
	color_=c;
}

//------------------------------------------------------
NodeController::NodeController(const AnimationData* animationData)
: animationData_(animationData)
{ 
	visualObject_ = 0;

	currentChain_ = 0;
	if(animationData_ && animationData_->initial())
		setChain(animationData_->initial()->chainID);

	soundEventID_ = SOUND_EVENT_NONE;

	phase_ = 0;
	requestDone_ = true;
	requestedPhase_ = -1;
}

void NodeController::setChain(ChainID chainID) 
{
	requestDone_ = true;
	requestedPhase_ = -1;
	chainPath_.clear();
	setChainInternal(chainID);
}

void NodeController::setChainInternal(ChainID chainID) 
{
	currentChain_ = animationData_->find(chainID);
	if(currentChain_){
		SetChain(currentChain()->chainName);
		setSoundEventID(currentChain()->soundID);
		if(!currentChain()->periodDeviation)
			phaseIterator_.startPeriod(currentChain()->phase, currentChain()->period, currentChain()->cycled());
		else{
			int period = currentChain()->period;
			if(period > 0)
				period += terLogicRND(currentChain()->periodDeviation);
			else
				period -= terLogicRND(currentChain()->periodDeviation);
			phaseIterator_.startPeriod(terLogicRND(1), period, currentChain()->cycled());
		}
	//	phaseIterator_.Quant();
		phase_.initialize();
		phase_ = phaseIterator_.phase();
	}
}

void NodeController::setAnimationSpeed(float speed)
{
	xassert(currentChain());
	phaseIterator_.setPeriod(speed > FLT_EPS ? currentChain()->period/speed : 0);
}

void NodeController::requestPhase(float phase)
{
	xassert(currentChain());

	phaseIterator_.setEndPhase(phase, currentChain()->cycled());
}

void NodeController::requestChain(ChainID requestedChainID, float phase) 
{
	requestDone_ = false;
	requestedPhase_ = -1;

	xassert(currentChain());

	// Проверка на реверс между двумя смежными цепочками
	if(currentChain()->enableReverse){
		const AnimationChain* requestedChain = animationData_->find(requestedChainID);
		xassert(requestedChain);
		if(!strcmp(requestedChain->chainName, currentChain()->chainName)){
			chainPath_.clear();
			currentChain_ = requestedChain;
			phaseIterator_.startPhase(phaseIterator_.phase(), currentChain()->period ? terLogicTimePeriod/(float)currentChain()->period : 0, currentChain()->cycled());
			if(phase >= 0)
				requestPhase(phase);
			return;
		}
	}

	requestedPhase_ = phase;
	bool log = findChainPath(requestedChainID, chainPath_);
	xassert(log);
}

void NodeController::requestChainNode(ChainNodeID requestedNode)
{
	xassert(currentChain());

	requestDone_ = false;
	requestedPhase_ = -1;

	if(currentChain()->end == requestedNode)
		return;
	
	chainPath_.clear();
	int pathLength = 1000;
	ChainVect chainPath;
	for(int i = 0; i < animationData_->chains.size(); i++){
		const AnimationChain& aci = animationData_->chains[i];
		if(aci.end == requestedNode && !aci.cycled() && findChainPath(aci.chainID, chainPath) && pathLength > chainPath.size()){
			pathLength = chainPath.size();
			chainPath_ = chainPath;
		}
	}
	xassert(!chainPath_.empty());
}

bool NodeController::findChainPath(ChainID requestedChain, ChainVect& chainPath) 
{
	for(int i = 0; i < animationData_->chains.size(); i++)
		const_cast<int&>(animationData_->chains[i].counter) = 0;

	ChainVect currentFront, nextFront;

	int counter = 1;
	const_cast<int&>(currentChain()->counter) = counter;
	currentFront.push_back(currentChain());

	bool found = false;
	while(!found){
		++counter;
		ChainVect::iterator ci;
		FOR_EACH(currentFront, ci){
			ChainNodeID node = (*ci)->end;
			for(int j = 0; j < animationData_->chains.size(); j++){
				const AnimationChain& acj = animationData_->chains[j];
				if(acj.begin == node){
					if(acj.chainID == requestedChain){
						found = true;
						break;
					}
					if(!acj.counter && !acj.cycled()){
						const_cast<int&>(acj.counter) = counter;
						nextFront.push_back(&acj);
					}
				}
			}
		}
		if(nextFront.empty() && !found)
			return false;
		swap(currentFront, nextFront);
		nextFront.clear();
	}

	chainPath.clear();
	const AnimationChain* chain = animationData_->find(requestedChain);
	while(--counter){
		chainPath.push_back(chain);
		ChainNodeID node = chain->begin;
		for(int j = 0; j < animationData_->chains.size(); j++){
			const AnimationChain& acj = animationData_->chains[j];
			if(acj.counter == counter && acj.end == node){
				chain = &acj;
			}
		}
	}
	return true;
}

void NodeController::quant() 
{
	if(phaseIterator_.deltaPhase()){
		if(phaseIterator_.Quant()){
			if(!chainPath_.empty()){
				setChainInternal(chainPath_.back()->chainID);
				chainPath_.pop_back();
				if(chainPath_.empty() && requestedPhase_ >= 0)
					requestPhase(requestedPhase_);
			}
			else {
				if(currentChain() && currentChain()->cycled())
					setSoundEventID(currentChain()->soundID);

				requestDone_ = true;
			}
		}

		phase_ = phaseIterator_.phase();
	}
}

void NodeController::interpolate() 
{ 
	if(phase_.fresh() && visualObject_)
		phase_(visualObject_, animationData_ ? animationData_->setPhaseRecursive : 0);
}

void NodeController::SetChain(const char* name)
{
	if(visualObject_)
		visualObject_->SetChannel(name, false);
}

void NodeController::setVisualObject(cObjectNode* visualObject) 
{ 
	xassert(visualObject); 
	visualObject_ = visualObject; 

	if(currentChain() && visualObject_)
		visualObject_->SetChannel(currentChain()->chainName, false);
}

void NodeController::universalSave(SaveNodeController& data) const 
{
	data.currentChain = chainID();
	data.phase = phaseIterator_.phase();

	data.requestedChain = requestedChainID();
	data.requestedPhase = requestedPhase_;
}

void NodeController::universalLoad(const SaveNodeController& data)
{
	if(data.currentChain != CHAIN_NONE){
		setChain(data.currentChain);
		setPhase(data.phase);
	}

	if(data.requestedChain != CHAIN_NONE)
		requestChain(data.requestedChain, data.requestedPhase);
}

void NodeController::writeDebugInfo(XBuffer& buf)
{
	if(!animationData_)
		return;
	buf < (animationData_->groupName ? animationData_->groupName : "Root") < ": ";
	if(currentChain()){
		buf < getEnumName(chainID()) < "(" < currentChain()->chainName < ", " <= phase_.x1() < ")";
		if(!requestDone())
			buf < " -> " < getEnumName(requestedChainID());
		if(visualObject())
			buf < " // " < visualObject()->GetCurrentChannelName();
	}
}
