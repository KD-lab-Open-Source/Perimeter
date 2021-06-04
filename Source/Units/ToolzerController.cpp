
#include "stdafx.h"

#include "UnitAttribute.h"
#include "ToolzerController.h"

#include "terra.h"
#include "Save.h"

/// управление воздействием тулзера
class ToolzerActionOp : private baseToolzer
{
public:
	ToolzerActionOp(){ }

	void operator ()(const struct ToolzerActionData& action,float scale = 1.0f);
	void setPosition(int x,int y){ start(x,y); }

	static ToolzerActionOp* instance(){ xassert(instance_); return instance_; }
	static void resetInstance();

private:

	/// никогда не вызывается, определена чтобы класс не был абстракным
	int simpleQuant(){ return 0; }

	static ToolzerActionOp* instance_;
};

ToolzerActionOp* ToolzerActionOp::instance_ = NULL;

ToolzerController::ToolzerController(const ToolzerSetup& setup) : setup_(setup),
	position_(-1,-1),
	isFinished_(false)
{
	scale_ = setup_.scale;
	requestedPhase_ = TOOLZER_PHASE_NONE;
	setStep(-1);
}

void ToolzerController::quant(int x,int y,bool is_on_zero_layer)
{
	if(!setup_.enabled()) 
		return;

	if(currentStep_ == -1)
		return;

	const ToolzerStepData& step = setup_.step(currentStep_);

	if(currentStepTime_ < step.duration){
		Vect2i dr = Vect2i(x,y) - position_;

		ToolzerActionOp::instance()->setPosition(x,y);

		if(float(dr.norm2()) >= step.dr * step.dr){
			position_ = Vect2i(x,y);

			bool chaos_flag = (setup_.workOnChaos) ? true : (To3D(Vect2i(x,y)).z > 0);

			if(!vMap.IsHardWCC(position_) && (!is_on_zero_layer || setup_.destroyOwnZeroLayer) && chaos_flag){
				for(int i = 0; i < step.actions.size(); i++)
					(*ToolzerActionOp::instance())(step[i],scale_);

				if(step.buildingDamageRadius && currentStepTime_ >= step.duration - 1){
					switch(step.buildingDamageMode){
					case BUILDING_DAMAGE_CIRCLE:
						damagingBuildingsTolzer(position_.x,position_.y,step.buildingDamageRadius);
						break;
					case BUILDING_DAMAGE_SQUARE:
						damagingBuildingsTolzerS(position_.x,position_.y,step.buildingDamageRadius);
						break;
					}
				}
			}
		}
	}
	else
		position_ = Vect2i(x,y);

	if(++currentStepTime_ >= step.duration){
		if(currentStep_ == setup_.steps.size() - 1)
			isFinished_ = true;

		if(step.nextPhaseID != TOOLZER_PHASE_NONE){
			currentStepTime_ = 0;

			if(step.nextPhaseID == TOOLZER_PHASE_DEFAULT){
				if(++currentStep_ >= setup_.steps.size())
					currentStep_ = 0;
			}
			else
				setPhase(step.nextPhaseID);
		}
	}
}

void ToolzerController::setStep(int step)
{
	currentStep_ = step;
	currentStepTime_ = 0;
}

void ToolzerController::setPhase(ToolzerPhaseID phase)
{
	if(!setup_.enabled()) 
		return;

	int index = setup_.stepIndex(phase);

	if(index != currentStep_)
		setStep(index);
}

bool ToolzerController::requestPhase(ToolzerPhaseID phase)
{
	if(requestedPhase_ != phase){
		requestedPhase_ = phase;
		setPhase(phase);
		return true;
	}

	return false;
}

void ToolzerController::showDebugInfo(const Vect3f& position) const
{
	if(currentStep_ != -1){
		XBuffer msg;
		msg < "toolzer time " <= currentStepTime_ < "/" <= currentStep_;
		msg < " pos " <= position_.x < " " <= position_.y;
		if(isFinished_) msg < " finished";

		show_text(position,msg,GREEN);
	}
}

void ToolzerActionOp::operator ()(const struct ToolzerActionData& action,float scale)
{
	int radius = round(scale * float(action.rad));

	switch(action.actionID){
	case TOOLZER_4ZP:
		if(action.hAppr < 0)
			toolzerAligmentTerrain4ZP.setHAppoximation(vMap.hZeroPlast);
		else
			toolzerAligmentTerrain4ZP.setHAppoximation(action.hAppr);

		toolzerAligmentTerrain4ZP.influenceDZ(x,y,radius,action.dz * (1 << VX_FRACTION),action.smMode);
		break;
	case TOOLZER_VH:
		if(action.hAppr < 0)
			toolzerAligmentTerrainVariableH.setHAppoximation(vMap.hZeroPlast);
		else
			toolzerAligmentTerrainVariableH.setHAppoximation(action.hAppr);

		toolzerAligmentTerrainVariableH.influenceDZ(x,y,radius,action.dz * (1 << VX_FRACTION),action.smMode);
		break;
	case TOOLZER_H:
		if(action.hAppr < 0)
			toolzerChangeTerHeight.setHAppoximation(vMap.hZeroPlast);
		else
			toolzerChangeTerHeight.setHAppoximation(action.hAppr);

		toolzerChangeTerHeight.influenceDZ(x,y,radius,action.dz * (1 << VX_FRACTION),action.smMode);
		break;
	case TOOLZER_NZP:
		if(action.hAppr < 0)
			toolzerChangeTerHeightIfNotZP.setHAppoximation(vMap.hZeroPlast);
		else
			toolzerChangeTerHeightIfNotZP.setHAppoximation(action.hAppr);

		toolzerChangeTerHeightIfNotZP.influenceDZ(x,y,radius,action.dz * (1 << VX_FRACTION),action.smMode);
		break;
	case TOOLZER_VLD:
		if(action.hAppr < 0)
			toolzerPlottingVeryLightDam.setHAppoximation(vMap.hZeroPlast);
		else
			toolzerPlottingVeryLightDam.setHAppoximation(action.hAppr);

		toolzerPlottingVeryLightDam.influenceDZ(x,y,radius,action.dz * (1 << VX_FRACTION),action.smMode);
		break;
	case TOOLZER_S:
		if(action.hAppr < 0)
			toolzerPlotingSoot.setHAppoximation(vMap.hZeroPlast);
		else
			toolzerPlotingSoot.setHAppoximation(action.hAppr);

		toolzerPlotingSoot.influenceDZ(x,y,radius,action.dz * (1 << VX_FRACTION),action.smMode);
		break;
	case TOOLZER_EZ:
		if(action.hAppr < 0)
			toolzerEraseZP.setHAppoximation(vMap.hZeroPlast);
		else
			toolzerEraseZP.setHAppoximation(action.hAppr);

		toolzerEraseZP.influenceDZ(x,y,radius,action.dz * (1 << VX_FRACTION),action.smMode);
		break;
	case TOOLZER_NONE:
		break;
	default:
		xassert(0 && "Bad toolzer action ID");
		break;
	}
}

void ToolzerActionOp::resetInstance()
{
	delete instance_;
	instance_ = new ToolzerActionOp;
}

void ToolzerController::resetActionOp()
{
	ToolzerActionOp::resetInstance();
}

SaveToolzerControllerData* ToolzerController::universalSave(SaveToolzerControllerData* data)
{
	data->scale = scale_;
	data->currentStep = currentStep_;
	data->requestedPhase = requestedPhase_;
	data->currentStepTime = currentStepTime_;
	data->isFinished = isFinished_;

	data->position = position_;

	return data;
}

void ToolzerController::universalLoad(const SaveToolzerControllerData* data)
{
	scale_ = data->scale;
	currentStep_ = data->currentStep;
	requestedPhase_ = ToolzerPhaseID(data->requestedPhase);
	currentStepTime_ = data->currentStepTime;
	isFinished_ = data->isFinished;

	position_ = data->position;
}
