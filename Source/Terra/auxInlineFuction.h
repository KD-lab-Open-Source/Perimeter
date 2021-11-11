#ifndef __AUXINLINEFUCTION_H__
#define __AUXINLINEFUCTION_H__


inline int getDistX(int v0,int v1)
{
	int d = v0 - v1;
	int ad = xm::abs(d);
	int dd = vMap.H_SIZE - ad;
	if(ad <= dd) return d;
	return d < 0 ? d + vMap.H_SIZE : d - vMap.H_SIZE;
}

inline int calcDX(int v0,int v1)
{
	int d = v1 - v0;
	unsigned int ad = xm::abs(d);
	if(ad < (vMap.H_SIZE/2)) return d;
	return d < 0 ? d + vMap.H_SIZE : d - vMap.H_SIZE;
}

inline int getDistY(int v0,int v1)
{
	int d = v0 - v1;
	int ad = xm::abs(d);
	int dd = vMap.V_SIZE - ad;
	if(ad <= dd) return d;
	return d < 0 ? d + vMap.V_SIZE : d - vMap.V_SIZE;
}

inline int calcDY(int v0,int v1)
{
	int d = v1 - v0;
	unsigned int ad = xm::abs(d);
	if(ad < (vMap.V_SIZE/2)) return d;
	return d < 0 ? d + vMap.V_SIZE : d - vMap.V_SIZE;
}

inline int getDelta(int v0,int v1)
{
	return vMap.YCYCL(v0 - v1 + vMap.V_SIZE);
}

inline int getDeltaX(int v0,int v1)
{
	return vMap.XCYCL(v0 - v1 + vMap.H_SIZE);
}

#endif //__AUXINLINEFUCTION_H__