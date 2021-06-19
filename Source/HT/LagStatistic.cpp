#include "StdAfx.h"
#include "LagStatistic.h"
#include "Runtime.h"

LagStatistic::LagStatistic()
{
	max_average_interval=30;
	small_speed_up=0.9f;
	big_speed_up=0.5f;
	speed_up=1.0f;
	clear();
}

LagStatistic::~LagStatistic()
{
}

void LagStatistic::clear()
{
	cur_data.lag_quant=0;
	cur_data.net_skip=false;
	cur_data.net_wait=0;
}

void LagStatistic::SetLagQuant(int lag_quant)
{
	cur_data.lag_quant=lag_quant;
}

void LagStatistic::SetNetSkip()
{
	cur_data.net_skip=true;
}

void LagStatistic::SetNetWait(float net_wait)
{
	cur_data.net_wait=net_wait;
}

void LagStatistic::NextLogicQuant()
{
	MTAuto lock(&lock_lag);
	list_data.push_front(cur_data);
	if(list_data.size()>max_average_interval)
	{
		list_data.pop_back();
	}

	clear();
}

void LagStatistic::Show()
{
	MTAuto lock(&lock_lag);
	int x=round(terScreenSizeX*0.85f);
	int y=round(terScreenSizeY*0.1f);
	ShowAverage(Vect2i(x,y),max_average_interval);
}

void LagStatistic::CalcAverage(int interval,float& lag_quant,float& net_skip,float& net_wait)
{
	std::list<QuantData>::iterator it;
	int size=0;
	lag_quant=0;
	net_skip=0;
	net_wait=0;
	FOR_EACH(list_data,it)
	{
		QuantData& d=*it;
		lag_quant+=d.lag_quant;
		net_skip+=d.net_skip?1:0;
		net_wait+=d.net_wait;
		size++;
		if(size>=interval)
			break;
	}

	if(size)
	{
		lag_quant/=size;
		net_skip/=size;
		net_wait/=size;
	}
}

void LagStatistic::ShowAverage(Vect2i pos,int interval)
{
	float lag_quant,net_skip,net_wait;
	CalcAverage(interval,lag_quant,net_skip,net_wait);

	Vect2f bmin,bmax;
	terRenderDevice->OutTextRect(0,0,"A",-1,bmin,bmax);
	int height=round(bmax.y-bmin.y);
	char str[128];

	sprintf(str,"lag_quant: %2.2f",lag_quant);
	terRenderDevice->OutText(pos.x,pos.y,str,sColor4f(1,1,1,1));
	pos.y+=height;
	sprintf(str,"net_skip: %2.2f",net_skip);
	terRenderDevice->OutText(pos.x,pos.y,str,sColor4f(1,1,1,1));
	pos.y+=height;
	sprintf(str,"net_wait: %2.2f",net_wait);
	terRenderDevice->OutText(pos.x,pos.y,str,sColor4f(1,1,1,1));
	pos.y+=height;
	sprintf(str,"speed_up: %2.2f",speed_up);
	terRenderDevice->OutText(pos.x,pos.y,str,sColor4f(1,1,1,1));
	pos.y+=height;
}

int LagStatistic::CalcDTime()
{
	float lag_quant,net_skip,net_wait;
	CalcAverage(max_average_interval,lag_quant,net_skip,net_wait);

	int dtime=100;//ms
	float min_lag_quant=1.2f;
	float max_lag_quant=10.0f;
	if(lag_quant<1.2f || net_wait>0)
	{
		if(net_skip>0.2f)
			speed_up=1.1f;
		else
			speed_up=1.0f;
	}else
	{
		float s=clamp((lag_quant-min_lag_quant)/(max_lag_quant-min_lag_quant),0.0f,1.0f);
		speed_up=(big_speed_up-small_speed_up)*s+small_speed_up;
	}

	return round(dtime*speed_up);
}
