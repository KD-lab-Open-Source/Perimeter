#include "StdAfx.h"

#include "Universe.h"
#include "Player.h"
#include "IronPort.h"

#include "PerimeterSound.h"
#include "Config.h"
#include "Triggers.h"
#include "GameShell.h"
#include "Runtime.h"
#include "CameraManager.h"
#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

REGISTER_CLASS(AttributeBase, AttributeCorridor, "Телепорт");

AttributeCorridor::AttributeCorridor()
{
	disappearingTime = 2000;
	disappearingHeight = 200;
	distanceThreshould = 30;
	moveFrame = 1;

	potentialToAcceleration = 300;
	accelerationDenominator = 400;
	pathVisualizationDelta = 0.05f;
}

terCorridor::terCorridor(const UnitTemplate& data) 
: terBuildingPowered(data)
{
	frame_ = 0;
	mode_ = IDLE;
}

void terCorridor::DestroyLink()
{
	terBuilding::DestroyLink();
	if(frame_ && !frame_->alive())
		frame_ = 0;
}

void terCorridor::input(terFrame* frame)
{
	frame_ = frame;
	realAvatar()->requestChain(CHAIN_OPENED);
	mode_ = OPENING;
}

void terCorridor::Quant()
{
	terBuildingPowered::Quant();
    	
	switch(mode_){
	case OPENING:
		if(frame_ && realAvatar()->chainID() == CHAIN_OPENED){
			if(attr().moveFrame){
				frame_->wayPoints().clear();
				frame_->wayPoints().push_back(position());
				frame_->wayPoints().push_back(position());
			}
			else{
				BodyPoint->way_points.clear();
				BodyPoint->way_points.push_back(frame_->position());
				frame_->BodyPoint->makeStatic();
			}
			mode_ = FRAME_APPROACHING;
		}
		break;
		
	case FRAME_APPROACHING:
		if(position2D().distance2(frame_->position2D()) < sqr(attr().distanceThreshould) || attr().moveFrame && frame_->wayPoints().empty()){
			frame_->BodyPoint->makeStatic();
			mode_ = FRAME_DISAPPEARING;
			disappearingTimer_.start(attr().disappearingTime);
			PositionZ_ = frame_->position().z;
			//frame_->Player->clearFrame();
		}
		break;
		
	case FRAME_DISAPPEARING: {											  
		frame_->BodyPoint->setPose(Se3f(frame_->BodyPoint->orientation(),
			to3D(position2D(), PositionZ_ + attr().disappearingHeight*disappearingTimer_())));
		frame_->realAvatar()->setSight(1 - disappearingTimer_());
		if(disappearingTimer_() == 1){
			universe()->checkEvent(EventUnitPlayer(Event::TELEPORTATION, this, frame_->Player));
			frame_->BodyPoint->setPose(Se3f(frame_->BodyPoint->orientation(), to3D(position2D(), 2000)));
			frame_->realAvatar()->GetModelPoint()->SetAttr(ATTRUNKOBJ_IGNORE);
			//frame_->Player->clearFrame();
			frame_->Kill();
			mode_ = IDLE;
			realAvatar()->requestChain(CHAIN_CLOSED);
		}
		break; }
	}
}

//---------------------------------------------------
terCorridorAlpha::terCorridorAlpha(const UnitTemplate& data) 
: terCorridor(data) 
{ 
	free_ = false; 
	passTime_ = 1200;
	timeOffset_ = 0;
	timer_.start();
	first_quant_=true;
}

void terCorridorAlpha::Start()
{
	terBuilding::Start();

	if(free_)
		makeFree();
}

void terCorridorAlpha::setPose(const Se3f& pose, bool initPose)
{
	if(!free_)
		terBuilding::setPose(pose, initPose);
	else
		terUnitReal::setPose(pose, initPose);
}

void terCorridorAlpha::MoveQuant()
{
	BodyPoint->way_points.clear();

	Vect2f target;
	if(mode_ == IDLE || !frame_){
		MTAuto lock(&potentials_lock_);
		target = potentialPosition();
		if(potentials_.empty()){
			terUnitReal::MoveQuant();
			return;
		}
	}
	else{
		target = frame_->position2D();
		timeOffset_ -= terLogicTimePeriod;
	}

	Vect2f delta = target - position2D();
	float norm = delta.norm();
	Vect2f acceleration = delta*(attr().potentialToAcceleration*clamp(norm/attr().accelerationDenominator, 0, 1)/(norm + 1));

	//show_vector(position(), to3D(acceleration, 0), WHITE);
	BodyPoint->addAcceleration(Vect3f(acceleration.x, acceleration.y, 0));

//	BodyPoint->setPose(Se3f(orientation(), to3D(potentialPosition(), position().z)));
	terUnitReal::MoveQuant();
}

void terCorridorAlpha::Quant()
{
	if(first_quant_)
	{
		refreshPotentials();
		first_quant_=false;
	}
	terCorridor::Quant();

	if(!free_){
		if(isBuildingEnable())
			makeFree();
	}
	else{
		setBuildingStatus(BUILDING_STATUS_CONSTRUCTED | BUILDING_STATUS_PLUGGED_IN | BUILDING_STATUS_CONNECTED | BUILDING_STATUS_ENABLED | BUILDING_STATUS_POWERED | BUILDING_STATUS_MOUNTED);
	}
}

void terCorridorAlpha::makeFree()
{
	free_ = true;
	BodyPoint->makeDynamic();
	BodyPoint->setFlyingMode(1);
	BodyPoint->way_points.push_back(position() + Vect3f(0, radius(), 0) * Mat3f(angleZ(), Z_AXIS));
	universe()->changeOwner(this, universe()->worldPlayer());
	uninstallBasement();
}

void terCorridorAlpha::checkConnection() 
{
	if(!free_)
		terCorridor::checkConnection();
}

SaveUnitData* terCorridorAlpha::universalSave(SaveUnitData* baseData)
{
	SaveUnitCorridorAlphaData* data = castOrCreate<SaveUnitCorridorAlphaData>(baseData);
	terBuilding::universalSave(data);
	data->free = free_;
	data->passTime = passTime_;
	data->timeOffset = timer_() + timeOffset_;
	return data;
}

void terCorridorAlpha::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitCorridorAlphaData* data = safe_cast<const SaveUnitCorridorAlphaData*>(baseData);
	terBuilding::universalLoad(data);

	if(data->free)
		makeFree();

	passTime_ = data->passTime;
	timeOffset_ = data->timeOffset;
}

void terCorridorAlpha::refreshPotentials() 
{
 	MTAuto lock(&potentials_lock_);
	potentials_.clear();
	terPlayer* world = universe()->worldPlayer();
	const UnitList& unit_list=world->units();
	UnitList::const_iterator ui;
	FOR_EACH(unit_list, ui)
		if((*ui)->attr().ID == UNIT_ATTRIBUTE_ALPHA_POTENTIAL){
			potentials_.push_back((*ui)->position2D());
			safe_cast<terUnitAplhaPotential*>(*ui)->setIndex(potentials_.size());
		}
}

Vect2f terCorridorAlpha::potentialPosition() 
{ 
	if(gameShell->missionEditor())
		refreshPotentials();
	xassert(potentials_lock_.is_lock());
	return potentialPosition(float((timer_() + timeOffset_)*potentials_.size())/(passTime_*1000));
}

Vect2f terCorridorAlpha::potentialPosition(float t) 
{
	xassert(potentials_lock_.is_lock());
	int size = potentials_.size();
	if(!size)
		return Vect2f::ZERO;
	else if(size == 1)
		return potentials_.front();

	int i = round(floor(t));
	float u = t - floor(t);

	Vect2f P[4];
	P[0] = get(i - 1);
	P[1] = get(i);
	P[2] = get(i + 1);
	P[3] = get(i + 2);

	float u2 = u*u;
	float u3 = u*u2;
	return P[3]*((-u2+u3)/2.0f) + P[0]*(-u/2.0+u2-u3/2.0) + P[2]*(2.0*u2-3.0/2.0*u3+u/2.0) + P[1]*(1.0-5.0/2.0*u2+3.0/2.0*u3);
}

void terCorridorAlpha::ShowInfo()
{
	terCorridor::ShowInfo();

	if(gameShell->missionEditor())
		refreshPotentials();

	if(marked()){
		MTAuto lock(&potentials_lock_);
		Vect3f posPrev = To3D(potentialPosition(-attr().pathVisualizationDelta));
		for(float t = 0; t < potentials_.size(); t += attr().pathVisualizationDelta){
			Vect3f pos = To3D(potentialPosition(t));
			terRenderDevice->DrawLine(posPrev, pos, pathColor);
			posPrev = pos;
		}
	}
}

void terCorridorAlpha::putPath(list<Vect2f>& miniMapPath) {
//	refreshPotentials();

//	miniMapPath.push_back(potentialPosition(-attr().pathVisualizationDelta));
	MTAuto lock(&potentials_lock_);
	for(float t = 0; t < potentials_.size(); t += attr().pathVisualizationDelta){
		miniMapPath.push_back(potentialPosition(t));
	}
}

void terCorridorAlpha::showDebugInfo()
{
	terCorridor::showDebugInfo();
	if(show_corridor_alpha_way)
		show_vector(To3D(potentialPosition()), 7, YELLOW);
}

//-------------------------------------------------------
terCorridorOmega::terCorridorOmega(const UnitTemplate& data) 
:	terCorridor(data),
minedIcon_(terOmegaMinedIcon) 
{ 
	upgraded_ = true;
	mode_ = IDLE;
}

SaveUnitData* terCorridorOmega::universalSave(SaveUnitData* baseData)
{
	SaveUnitCorridorOmegaData* data = castOrCreate<SaveUnitCorridorOmegaData>(baseData);
	terBuilding::universalSave(data);
	
	data->upgraded = upgraded_;
	return data;
}

void terCorridorOmega::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitCorridorOmegaData* data = safe_cast<const SaveUnitCorridorOmegaData*>(baseData);
	terBuilding::universalLoad(data);
	
	upgraded_ = data->upgraded;
}

bool terCorridorOmega::readyToTeleportate() const 
{ 
	return upgraded_ && isBuildingEnable() && !mined() && isConstructed(); 
}

bool terCorridorOmega::mined() const 
{ 
	return upgraded() && gameShell->manualData().omegaMissionType == SaveManualData::OMEGA_MINE 
		|| !upgraded() && gameShell->manualData().omegaMissionType == SaveManualData::OMEGA_DEMINE; 
}

void terCorridorOmega::checkConnection() 
{
	if(!frame_)
		terCorridor::checkConnection();
}

void terCorridorOmega::ShowInfo()
{
	terCorridor::ShowInfo();
	
	minedIcon_.quant();
	if(mined()){
		MatXf m = avatar()->matrix();
		m.trans().z += radius()*attr().iconDistanceFactor;
		minedIcon_.show(m.trans());
	}
}

//------------------------------------------------------------
int terUnitAplhaPotential::indexLast_ = 0;

terUnitAplhaPotential::terUnitAplhaPotential(const UnitTemplate& data) 
: terUnitBase(data) 
{
	index_ = indexLast_++;
}

void terUnitAplhaPotential::ShowInfo()
{
	if(gameShell->missionEditor()){
		Vect3f e,w;
		terCamera->GetCamera()->ConvertorWorldToViewPort(&position(),&w,&e);
		terRenderDevice->SetFont(gameShell->debugFont());
		XBuffer buf;
		buf <= index_;
		buf.SetDigits(2);
		terRenderDevice->OutText(round(e.x),round(e.y),buf,selected() ? sColor4f(1,1,1,1) : sColor4f(0.8f,0.8f,0.8f,1));
		terRenderDevice->SetFont(0);
	}
}





