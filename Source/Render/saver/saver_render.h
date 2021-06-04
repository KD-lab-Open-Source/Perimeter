#pragma once

inline
CSaver& operator<<(CSaver& s,const sPolygon& p)
{
	s<<p.p1<<p.p2<<p.p3;
	return s;
}

inline
void operator>>(CLoadIterator& it,sPolygon& p)
{
	it>>p.p1;
	it>>p.p2;
	it>>p.p3;
}
