#include "StdAfx.h"

#include "Universe.h"
#include "IronClusterUnit.h"

#include "UniverseInterface.h"
#include "CameraManager.h"

static const float MONK_DELTA_PHASE = 100.f/2000.f;

terUnitMonk::terUnitMonk(const UnitTemplate& data)
{
	position_=Vect3f::ZERO;
	Player=data.player();
	SleepPosition = Vect3f::ZERO;

	monk_mode_ = MONK_MODE_SLEEP;

	path_ = 0;
	pathPosition_ = 0;
	pathIndex_ = 0;
	pathRemainder_ = 0;

	BorderPhase = 0;
	BorderPosition = Vect3f::ZERO;

	alive_=true;
}

terUnitMonk::~terUnitMonk()
{
	if(inserted())
		universe()->monks.grid.Remove(*this);
}

void terUnitMonk::Start()
{
	SleepPosition = position();
	avatar.Start(radius());
}

void terUnitMonk::avatarInterpolation()
{
	switch(monk_mode_){
		case MONK_MODE_SLEEP:
			avatar.Hide();
			avatar.SetProtectionMode(0);
			break;
		case MONK_MODE_TO_BORDER:
		case MONK_MODE_TO_CORE:
			avatar.Show();
			avatar.SetProtectionMode(0);
			break;
		case MONK_MODE_GUARD:
			avatar.Show();
			avatar.SetProtectionMode(1);
			break;
	}

	avatar.setPos(position());
	avatar.interpolate();
}

void terUnitMonk::quant()
{
	switch(monk_mode_){
		case MONK_MODE_SLEEP:
			setPosition(SleepPosition);
			BorderPhase = 0;
			break;

		case MONK_MODE_TO_BORDER:
			BorderPhase += MONK_DELTA_PHASE;
			if(BorderPhase >= 1.0f){
				setPosition(BorderPosition);
				monk_mode_ = MONK_MODE_GUARD;
				BorderPhase = 1.0f;
			}
			else
				CalcBorderPosition();
			break;

		case MONK_MODE_TO_CORE:
			BorderPhase -= MONK_DELTA_PHASE;
			if(BorderPhase <= 0){
				setPosition(SleepPosition);
				monk_mode_ = MONK_MODE_SLEEP;
				BorderPhase = 0;
			}else
				CalcBorderPosition();
			break;

		case MONK_MODE_GUARD: {
			xassert(path_);
			const Vect2s& pos = (*path_)[pathIndex_];
			setPosition(to3D(pos, vMap.hZeroPlast + radius()));
			if(!vMap.leveled(vMap.offsetBuf(pos.x, pos.y))){
				explosion_kill();
				flashCircleLeveling(pos.x, pos.y, fieldPrm.monksWetPlace);
			}
			BorderPhase = 1.0f;
			break; 
		}
	}
}

//--------------------------------------------------------

void terUnitMonk::setPath(const Vect2sVect& path, float pathPosition)
{ 
	path_ = &path; 
	pathPosition_ = 0;
	pathIndex_ = 0;
	pathRemainder_ = 0;
	monk_mode_ = MONK_MODE_TO_BORDER; 
	setPathPosition(pathPosition); 
	BorderPosition = to3D((*path_)[pathIndex_], vMap.hZeroPlast + radius());
}

void terUnitMonk::setPathPosition(float pathPosition)
{
	if(!path_)
		return;
	xassert(path_);
	xassert(monk_mode_!=MONK_MODE_SLEEP && monk_mode_!=MONK_MODE_TO_CORE);
	pathRemainder_ += pathPosition - pathPosition_;
	pathPosition_ = pathPosition;

	for(;;){
		int index = (pathIndex_ + 1) % (*path_).size();
		float d = xm::sqrt(((*path_)[index] - (*path_)[pathIndex_]).norm2());
		if(pathRemainder_ < d)
			break;
		pathRemainder_ -= d;
		pathIndex_ = index;
	}
}

int terUnitMonk::goHome()
{ 
	path_ = 0;
	monk_mode_ = MONK_MODE_TO_CORE;
	BorderPosition = position();
	return 0;
}

void terUnitMonk::CalcBorderPosition()
{
	Vect3f position;
	position.interpolate(SleepPosition, BorderPosition, BorderPhase);
	float t = (0.5f - xm::abs(BorderPhase - 0.5f)) * 2.0f;
	position.z += xm::sqrt(t) * 128.0f;
	setPosition(position);
}

void terUnitMonk::Collision(terUnitMonk* p)
{
	explosion_kill();
	p->explosion_kill();
}

void terUnitMonk::kill()
{
	if(!alive_)
		return;
	alive_=false;
	universe()->monks.kill(this);
}

void terUnitMonk::explosion_kill()
{
	kill();
}

void terUnitMonk::setPosition(const Vect3f& pos)
{
	position_=pos;

	terMonkGridType& grid=universe()->monks.grid;
	if(monk_mode_ != MONK_MODE_SLEEP){
		if(inserted())
			grid.Move(*this, xm::round(position().x), xm::round(position().y), xm::round(radius()));
		else
			grid.Insert(*this, xm::round(position().x), xm::round(position().y), xm::round(radius()));
	}
	else{
		if(inserted())
			grid.Remove(*this);
	}
}
