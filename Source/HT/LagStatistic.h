#pragma once

class LagStatistic
{
public:
	LagStatistic();
	~LagStatistic();

	void SetLagQuant(int lag_quant);//Что возвращает getInternalLagQuant
	void SetNetSkip();// true Слишком быстро идут кванты, этот был проскипован.
	void SetNetWait(float net_wait);// true Слишком медленно идут кванты (сек)

	void NextLogicQuant();
	void Show();

	int CalcDTime();//return ms
protected:
	MTSection lock_lag;
	struct QuantData
	{
		int lag_quant;
		bool net_skip;
		float net_wait;//time 
	};

	int max_average_interval;//quants
	float small_speed_up;
	float big_speed_up;
	float speed_up;
	QuantData cur_data;
	list<QuantData> list_data;

	void ShowAverage(Vect2i pos,int interval);
	void CalcAverage(int interval,float& lag_quant,float& net_skip,float& net_wait);

	void clear();
};
