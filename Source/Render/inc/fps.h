#pragma once
class FPS
{
	list<double> tiks;
	int max_miliseconds;
public:
	FPS(int _max_miliseconds=2000)
	{
		max_miliseconds=_max_miliseconds;
	}

	void quant()
	{
		double t=clockf();

		tiks.push_back(t);
		int sz=tiks.size();
		if(sz<5)
			return;

		while(!tiks.empty())
		{
			if(sz<6)
				return;
			double num=tiks.back()-tiks.front();
			if(num>max_miliseconds)
			{
				tiks.pop_front();
				sz--;
			}else
				break;
		}
	}

	float GetFPS()
	{
		int sz=tiks.size();
		if(sz<5)
			return 0;
		double num=tiks.back()-tiks.front();
		return ((sz-1)*1000.0f)/num;
	}

	void clear()
	{
		tiks.clear();
	}

	bool is_precisely()
	{
		if(tiks.empty())
			return false;
		return (tiks.back()-tiks.front())>=max_miliseconds*0.8;
	}

	void GetFPSminmax(float& fpsmin,float& fpsmax)
	{
		int sz=tiks.size();
		if(sz<5)
		{
			fpsmin=fpsmax=0;
			return;
		}

		double prevt=tiks.front();
		bool first=true;
		list<double>::iterator it;
		it=tiks.begin();
		++it;
		while(it!=tiks.end())
		{
			double t=*it;
			float fps=1000/(t-prevt);

			if(first)
			{
				first=false;
				fpsmin=fpsmax=fps;
			}else
			{
				fpsmin=min(fpsmin,fps);
				fpsmax=max(fpsmax,fps);
			}

			prevt=t;
			++it;
		}
	}
};
