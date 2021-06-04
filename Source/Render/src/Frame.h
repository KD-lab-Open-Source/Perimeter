#ifndef _FRAME_H_
#define _FRAME_H_
#pragma once

#define FRAME_PHASE_RANGE									1.00001f /* in order to SetPhase(0) != SetPhase(1) */
#define FRAME_PHASE_MAX										0.99999f

class cFrame
{
public:
	cFrame();
	~cFrame();
	void SetCopy(cFrame *Frame) const;

	void Set(float Period=0,float StartPhase=-1,float FinishPhase=-1);
	void AddPhase(float dt);

	inline int IsAnimated()							{ return dcount!=0; }
	inline float GetPhase()							{ return count; }
	inline void SetPhase(float phase)				{ count=fmodf(phase+FRAME_PHASE_RANGE,FRAME_PHASE_RANGE); }
	inline float GetPeriod()						{ return dcount ? 1.f/dcount : 0; }
	inline float GetFinish()						{ return finish; }

private:
	float			count;							// счетчик
	float			finish;							// конечная фаза
	float			dcount;							// приращение в секунду
};

#endif // _FRAME_H_
