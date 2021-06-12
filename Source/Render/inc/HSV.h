//Balmer - 2000
#pragma once

//Y = 0.30*R + 0.59*G + 0.11*B перевод цветного в чёрно-белый

//h=0..360,s=0..1,v=0..1

inline void HSVtoRGB(float h,float s,float v,
					 float& r,float& g,float& b)
{
	const float min=1e-5f;
	int i;
	float f,m,n,k;

	if(s<min)
	{
		r=g=b=v;
	}else
	{
		if(h>=360)
		{
			h=0;
		}else
		{
			h=h/60;
		}

		i=(int)h;
		f=h-i;
		m=v*(1-s);
		n=v*(1-s*f);
		k=v*(1-s*(1-f));

		if(i==0)
		{
			r=v;
			g=k;
			b=m;
		}else
		if(i==1)
		{
			r=n;
			g=v;
			b=m;
		}else
		if(i==2)
		{
			r=m;
			g=v;
			b=k;
		}else
		if(i==3)
		{
			r=m;
			g=n;
			b=v;
		}else
		if(i==4)
		{
			r=k;
			g=m;
			b=v;
		}else
		if(i==5)
		{
			r=v;
			g=m;
			b=n;
		}else
			xassert(0);
	}

	xassert(r>=0 && r<=1);
	xassert(g>=0 && g<=1);
	xassert(b>=0 && b<=1);
}


inline sColor4f HSVtoRGB(float h,float s,float v)
{
	sColor4f c;
	HSVtoRGB(h,s,v,
			 c.r,c.g,c.b);
	c.a=1;
	return c;
}

inline void RGBtoHSV(sColor4f c,float& h,float& s,float& v)
{
	float r=c.r,g=c.g,b=c.b;
	v=max(max(r,g),b);
	float temp=min(min(r,g),b);
	if(v==0)
	{
		s=0;
	}else
	{
		s=(v-temp)/v;
	}

	if(s==0)
	{
		h=0;
	}else
	{
		float Cr=(v-r)/(v-temp);
		float Cg=(v-g)/(v-temp);
		float Cb=(v-b)/(v-temp);

		if(r==v)
		{
			h=Cb-Cg;
		}else
		if(g==v)
		{
			h=2+Cr-Cb;
		}else
		if(b==v)
		{
			h=4+Cg-Cr;
		}

		h=60*h;

		if(h<0)h+=360;
	}

}


