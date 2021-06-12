#ifndef __TOOLZER_CONTROLLER_H__
#define __TOOLZER_CONTROLLER_H__

#ifndef __TOOLZER_ENUMS_INL__
#define __TOOLZER_ENUMS_INL__
#include "ToolzerEnums.inl"
#endif // __TOOLZER_ENUMS_INL__

struct ToolzerSetup;
struct SaveToolzerControllerData;

/// Тулзер.
class ToolzerController
{
public:
	ToolzerController(const ToolzerSetup& setup);

	void start(){ setStep(0); isFinished_ = false; }

	void quant(int x,int y,bool is_on_zero_layer = false);

	void setStep(int step);
	void setPhase(ToolzerPhaseID phase);

	/// установка фазы для зацикленных тулзеров
	/**
	запоминает, что запрашивалось, повторные запросы игнорируются
	*/
	bool requestPhase(ToolzerPhaseID phase);

	ToolzerPhaseID requestedPhase() const { return requestedPhase_; }

	bool isFinished() const { return isFinished_; }

	float scale() const { return scale_; }
	void setScale(float scale){ scale_ = scale; }

	float radius() const { return setup_.radius; }

	void showDebugInfo(const Vect3f& position) const;

	static void resetActionOp();

	SaveToolzerControllerData* universalSave(SaveToolzerControllerData* data);
	void universalLoad(const SaveToolzerControllerData* data);

private:

	/// коэфф. масштабтрования тулзера
	float scale_;

	/// индекс текущего шага, -1 если тулзер неактивен
	int currentStep_;
	
	/// фаза, запрошенная в прошлый вызов requestPhase()
	ToolzerPhaseID requestedPhase_;

	/// время работы текущего шага тулзера
	int currentStepTime_;

	/// параметры тулзера, см. Scripts\Toolzers.inl
	const ToolzerSetup& setup_;

	bool isFinished_;

	Vect2i position_;
};

#endif // __TOOLZER_CONTROLLER_H__
