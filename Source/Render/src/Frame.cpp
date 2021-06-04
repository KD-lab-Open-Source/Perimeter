#include "StdAfxRD.h"

//////////////////////////////////////////////////////////////////////////////////////////
// реализация cFrame
//////////////////////////////////////////////////////////////////////////////////////////
cFrame::cFrame()
{
	count=0; 
	finish=-1;
	dcount=1.f/2000.0f;
}
cFrame::~cFrame()
{
}
void cFrame::Set(float Period,float Start,float Finish)
{
	if(Period) dcount=SIGN(Period)*fmod(1/fabs(Period),1);
	else dcount=0;
	if(Start!=-1) SetPhase(Start);
	finish=Finish; 
}	
void cFrame::AddPhase(float dt)
{ 
	SetPhase(count+dt*dcount);
	if(finish>=0)
		if(dcount>0)
		{
			if(count>=finish)
				SetPhase(finish),dcount=0;
		}
		else 
		{
			if(count<=finish)
				SetPhase(finish),dcount=0;
		}
}
void cFrame::SetCopy(cFrame *Frame) const
{
	Frame->count=count;
	Frame->finish=finish;
	Frame->dcount=dcount;
}

