#pragma once

void CalcLagrange(vector<float>& data,float& a0,float& a1,float& a2,float& a3);

template<int size_template>
struct VectTemplate
{
	enum 
	{
		size=size_template,
	};

	float data[size];

	float& operator[](int i){return data[i];}
	const float& operator[](int i)const{return data[i];}

	float distance(const VectTemplate& p)
	{
		float d=0;
		for(int i=0;i<size;i++)
		{
			float x=data[i]-p[i];
			d+=x*x;
		}
		return sqrtf(d);
	}
};

template<class T>
class InterpolatePosition
{
	vector<T> position;
public:
	struct One
	{
		ITPL itpl;
		int interval_begin;//потом во float будет переводиться.
		int interval_size;
		T a0,a1,a2,a3;//y=a0+a1*t+a2*t*t+a3*t*t*t;

		inline float IntervalToT(int interval)
		{
			return (interval-interval_begin)/float(interval_size-1)-0.5f;
		}

		inline T Calc(float t)
		{
			float t2=t*t;
			float t3=t2*t;
			T out;
			for(int i=0;i<T::size;i++)
				out[i]=a0[i]+a1[i]*t+a2[i]*t2+a3[i]*t3;
			return out;
		}
	};

	vector<One> out_data;

	InterpolatePosition();
	~InterpolatePosition();

	void Interpolate(vector<T>& position,float delta,bool cycled);

	bool Save(CSaver& saver,int interval_begin,int interval_size);
protected:
	bool cycled;
	void Add(One& one);
	void AddConstant(float delta,int min_interval);
	void AddCubic(float delta);
	void AddUnknownConstant(float delta);
};

template<class T>
InterpolatePosition<T>::InterpolatePosition()
{
	cycled=false;
}

template<class T>
InterpolatePosition<T>::~InterpolatePosition()
{
}

template<class T>
void InterpolatePosition<T>::Interpolate(vector<T>& position_,float delta,bool cycled_)
{
	cycled=cycled_;
	position=position_;
	if(position.empty())
	{
		xassert(0 && !position.empty());
		return;
	}

	One all;
	all.itpl=ITPL_UNKNOWN;
	all.interval_begin=0;
	all.interval_size=position.size();
	out_data.push_back(all);

	AddConstant(delta,8);
	AddCubic(delta);
	AddConstant(delta,2);
	AddUnknownConstant(delta);

	int begin=0;
	for(int i=0;i<out_data.size();i++)
	{
		One& cur=out_data[i];
		xassert(begin==cur.interval_begin);
		xassert(cur.interval_size>0);
		begin+=cur.interval_size;
	}
	xassert(begin==position.size());
}

template<class T>
void InterpolatePosition<T>::Add(One& one)
{
	for(int i=0;i<out_data.size();i++)
	{
		One& cur=out_data[i];
		if(cur.interval_begin<=one.interval_begin && 
			cur.interval_begin+cur.interval_size>one.interval_begin)
		{
			xassert(cur.interval_begin+cur.interval_size>=one.interval_begin+one.interval_size);
			xassert(cur.itpl==ITPL_UNKNOWN);

			if(cur.interval_begin==one.interval_begin && cur.interval_size==one.interval_size)
			{//replace
				cur=one;
				return;
			}

			if(cur.interval_begin==one.interval_begin && cur.interval_size>one.interval_size)
			{//left
				One next=cur;
				next.interval_begin=one.interval_begin+one.interval_size;
				next.interval_size=cur.interval_size-one.interval_size;
				cur=one;
				out_data.insert(out_data.begin()+i+1,next);
				return;
			}

			xassert(cur.interval_begin<one.interval_begin);
			xassert(cur.interval_size>one.interval_size);
			if(cur.interval_begin+cur.interval_size==one.interval_begin+one.interval_size)
			{//right
				cur.interval_size=one.interval_begin-cur.interval_begin;
				out_data.insert(out_data.begin()+i+1,one);
				return;
			}
			
			//middle
			xassert(cur.interval_begin+cur.interval_size>one.interval_begin+one.interval_size);

			One next=cur;
			next.interval_begin=one.interval_begin+one.interval_size;
			next.interval_size=cur.interval_begin+cur.interval_size-(one.interval_begin+one.interval_size);
			cur.interval_size=one.interval_begin-cur.interval_begin;
			out_data.insert(out_data.begin()+i+1,one);
			out_data.insert(out_data.begin()+i+2,next);
			return;
		}
	}

	xassert(0 && "InterpolatePosition::Add");
}

template<class T>
void InterpolatePosition<T>::AddConstant(float delta,int min_interval)
{
	int size=position.size();
	for(int iout=0;iout<out_data.size();iout++)
	{
		One one=out_data[iout];
		int size=one.interval_size;
		int iend=one.interval_begin+one.interval_size;

		if(one.itpl==ITPL_UNKNOWN)
		for(int ibegin=one.interval_begin;ibegin<iend;)
		{
			T begin=position[ibegin];
			int interval_size=1;
			for(int j=ibegin+1;j<iend;j++)
			{
				T cur=position[j];
				float dt=cur.distance(begin);
				if(dt<delta)
				{
					interval_size++;
				}else
				{
					break;
				}
			}

			if(interval_size>=min_interval)
			{
				One one;
				one.itpl=ITPL_CONSTANT;
				one.interval_begin=ibegin;
				one.interval_size=interval_size;
				one.a0=begin;//Потом на среднее по интервалу заменить
				Add(one);
				ibegin+=interval_size;
				break;
			}

			ibegin++;
		}
	}
}

template<class T>
void InterpolatePosition<T>::AddCubic(float delta)
{
	const int nsub=T::size;
	int isub;
	for(int iout=0;iout<out_data.size();iout++)
	{
		One one=out_data[iout];
		int size=one.interval_size;
		int iend=one.interval_begin+one.interval_size;
		if(one.itpl==ITPL_UNKNOWN)
		if(one.interval_size<=1)
		{//linear
/*		Уже не нужно, так как в сплайнах на 1 точку больше стало.
			if(one.interval_size==2)
			{
				One out;
				int ibegin=one.interval_begin;
				out.itpl=ITPL_LINEAR;
				out.interval_begin=ibegin;
				out.interval_size=one.interval_size;
				T p0=position[ibegin];
				T p1=position[ibegin+1];
				
				for(isub=0;isub<nsub;isub++)
				{
					out.a2[isub]=out.a3[isub]=0;
					out.a0[isub]=(p1[isub]+p0[isub])*0.5f;
					out.a1[isub]=p1[isub]-p0[isub];
				}

				Add(out);
				ibegin+=out.interval_size;
			}
*/
		}else
		{//cubic
			for(int ibegin=one.interval_begin;ibegin<iend;)
			{
				int add=min(3,size);
				int interval_size=add-1;
				vector<float> xyz[nsub];

				for(int j=0;j<add;j++)
				{
					T begin=position[ibegin+j];
					for(isub=0;isub<nsub;isub++)
					{
						xyz[isub].push_back(begin[isub]);
					}
				}

				One prev_one;
				for(j=ibegin+add;j<=iend;j++)
				{
					One o;
					T cur;
					if(j<position.size())
						cur=position[j];
					else
					{
						if(cycled)
						{
							cur=position.front();
							//Для кватернионов
							T rot_prev=position.back();
							T cur_inv;
							for(isub=0;isub<nsub;isub++)
								cur_inv[isub]=-cur[isub];
							float d=rot_prev.distance(cur);
							float d_inv=rot_prev.distance(cur_inv);
							if(d_inv<d)
								cur=cur_inv;
						}else
							cur=position.back();
					}

					for(isub=0;isub<nsub;isub++)
						xyz[isub].push_back(cur[isub]);


					for(isub=0;isub<nsub;isub++)
						CalcLagrange(xyz[isub],o.a0[isub],o.a1[isub],o.a2[isub],o.a3[isub]);

					float dt=0;
					for(int k=0;k<xyz[0].size();k++)
					{
						float t=(k/(float)(xyz[0].size()-1)-0.5f);
						T pos=o.Calc(t);
						T posx;
						for(isub=0;isub<nsub;isub++)
							posx[isub]=xyz[isub][k];

						dt+=pos.distance(posx);
					}
					dt/=xyz[0].size();

					if(dt<delta)
					{
						prev_one=o;
						interval_size++;
					}else
					{
						break;
					}
				}

				if(interval_size>1)
				{
					One one=prev_one;
					bool is_linear=true;
					for(isub=0;isub<nsub;isub++)
					{
						if(fabs(one.a2[isub])>delta || fabs(one.a3[isub])>delta)
							is_linear=false;
					}

					one.itpl=is_linear?ITPL_LINEAR:ITPL_SPLINE;
					one.interval_begin=ibegin;
					one.interval_size=interval_size;
					Add(one);
					ibegin+=interval_size;
					break;
				}

				ibegin++;
			}
		}
	}
}

template<class T>
void InterpolatePosition<T>::AddUnknownConstant(float delta)
{
	const int nsub=T::size;
	bool fixed;

	do
	{
		fixed=false;
		for(int iout=0;iout<out_data.size();iout++)
		{
			One one=out_data[iout];
			if(one.itpl==ITPL_UNKNOWN)
			{
				One out;
				out.itpl=ITPL_CONSTANT;
				out.interval_begin=one.interval_begin;
				out.interval_size=1;
				out.a0=position[one.interval_begin];
				Add(out);
				fixed=true;
				break;
			}
		}
	}while(fixed);
}

template<class T>
bool InterpolatePosition<T>::Save(CSaver& saver,int interval_begin,int interval_size)
{
	bool ok=true;
	const int nsub=T::size;
	saver<<(int)out_data.size();
	for(int i=0;i<out_data.size();i++)
	{
		One& o=out_data[i];
		saver<<(int)o.itpl;
		saver<<float((o.interval_begin)/(float)interval_size);
		saver<<float(o.interval_size/(float)interval_size);
		switch(o.itpl)
		{
		case ITPL_SPLINE:
			{
				for(int i=0;i<nsub;i++)
				{
					saver<<o.a0[i]<<o.a1[i]<<o.a2[i]<<o.a3[i];
				}
			}
			break;
		case ITPL_LINEAR:
			{
				for(int i=0;i<nsub;i++)
				{
					saver<<o.a0[i]<<o.a1[i];
				}
			}
			break;
		case ITPL_CONSTANT:
			{
				for(int i=0;i<nsub;i++)
				{
					saver<<o.a0[i];
				}
			}
			break;
		default:
			ok=false;
			break;
		}
	}

	return ok;
}