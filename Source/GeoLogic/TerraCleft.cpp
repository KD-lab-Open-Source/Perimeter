// TerraCleft.cpp: implementation of the CTerraCleft class.
//

#include "stdafx.h"
#include "terra.h"
#include "TerraCleft.h"

const int cleftChunkCount    = 10;
const int cleftChunkPosBias  = 5;

const int cleftSlipWidth   = 7;
const int cleftSlipSpacing = 40;
const int cleftRadCount    = 6;
const int cleftNodeBias    = 15;

const int cleftBreakWidth =  4;
const int cleftDepthMultipler = 10;

RECT& LineAA(int Xd, int Yd, int Xf, int Yf, RECT& r);
RECT& geoLine(int xbeg, int ybeg, int sx, int sy, int width, RECT& r);

///////////////////////////////////////////////////////////////////////////////
//
inline int _xnoise()
{
	return terLogicRND(2*cleftNodeBias) - cleftNodeBias;
}
inline int _xnoise1()
{
	return terLogicRND(2*cleftChunkPosBias) - cleftChunkPosBias;
}

inline void union_rect(RECT& rDst, RECT& r1, RECT& r2)
{
	rDst.left  = min(r1.left, r2.left);
	rDst.right = max(r1.right, r2.right);
	rDst.top  = min(r1.top, r2.top);
	rDst.bottom = max(r1.bottom, r2.bottom);
}
inline void update_map(int left, int right, int top, int bottom)
{
	if(right - left > (vMap.H_SIZE>>1))
	{
		update_map(0, left, top, bottom);
		update_map(right, vMap.H_SIZE-1, top, bottom);
	}
	else if(bottom - top > (vMap.V_SIZE>>1))
	{
		update_map(left, right, 0, top);
		update_map(left, right, bottom, vMap.V_SIZE-1);
	}
	else
	{
		if((left != right) && (top != bottom))
		{
			vMap.recalcArea2Grid(left, top, right, bottom);
			vMap.regRender(left, top, right, bottom);
		}
	}
}
inline void update_map(const RECT& r)
{
//	update_map(r.left, r.right, r.top, r.bottom);

	vMap.recalcArea2Grid(r.left, r.top, r.right, r.bottom);
	vMap.regRender(r.left, r.top, r.right, r.bottom);
}

///////////////////////////////////////////////
//
CTerraCleft::CTerraCleft(const Vect2i& pos1, const Vect2i& pos2, int active)
{
	m_to = pos2;
	m_points.push_back(pos1);
	processing = active;

	delta = (pos1-pos2).norm()/cleftChunkCount;

	width = 1<<16;
}
CTerraCleft::~CTerraCleft()
{
}

inline Vect2i CTerraCleft::_next_chunk()
{
	Vect2i  ret;
	Vect2i& last = m_points.back();

	if((m_points.size() == cleftChunkCount - 1) || (m_to == last)) 
		ret = m_to;
	else
	{
		Vect2f v = m_to - last;
		v.normalize(delta);

		ret = last + v + Vect2i(_xnoise1(), _xnoise1());
	}

	return ret;
}

void CTerraCleft::activate_siblings()
{
	SiblingList::iterator it;
	FOR_EACH(m_siblings, it)
		(*it)->processing = prcGo;
}

CTerraCleft& CTerraCleft::attach(CTerraCleft& cleft)
{
	m_siblings.push_back(&cleft);

	return cleft;
}

int CTerraCleft::quant(RECT& r)
{
	if(m_points.size() >= cleftChunkCount)
		return 0;

	Vect2i& a = m_points.back();
	m_points.push_back(_next_chunk());
	Vect2i& b = m_points.back();


	RECT r2;
	if(a != b)
		union_rect(r, r, LineAA(a.x, a.y, b.x, b.y, r2));

	return 1;
}

int CTerraCleft::quant_g(RECT& r)
{
	RECT r2;

	width+=(1<<11)+(1<<10)+(1<<12) + 1;

	PointList::iterator it = m_points.begin();
	while(1)
	{
		Vect2i& a = *it++;
		Vect2i& b = *it;

		if(it==m_points.end())
			break;

		if(a != b)
			union_rect(r, r, geoLine(a.x, a.y, b.x - a.x, b.y - a.y, width>>16, r2));
	}

	return ((width>>16) < cleftBreakWidth) && ((width & 0xFF) < cleftDepthMultipler);
}


/////////////////////////////////////////////////////////////////////////////////////
// CTerraCleftSystem

CTerraCleftSystem::CTerraCleftSystem()
{
	m_bGrowing = 1;
}

#define ID(i, j) (((i)<<24)|(j))

void CTerraCleftSystem::init_radial(Vect2i& pos1)
{
	m_nodes[0] = pos1;

	int dR = cleftSlipSpacing;
	int jC = 4;
	for(int i=0, r = dR; i<cleftRadCount; i++, r+=dR)
	{
		double a = 0;
		double dA = 2*M_PI/jC;
		for(int j = 0; j<jC; j++, a+=dA)
		{
			//new point
			Vect2i v(int(pos1.x + r*sin(a))+_xnoise(), int(pos1.y + r*cos(a)) + _xnoise());

			m_nodes[ID(i+1, j)] = v;

			if((i>=4) && (j%2))
				m_nodes.erase(ID(i+1, j));
		}

		jC *= 2;
	}

	///
	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,0)], prcGo));
	grow_radial(m_clefts.back(), 1, 0);
	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,1)], prcGo));
	grow_radial(m_clefts.back(), 1, 1);
	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,2)], prcGo));
	grow_radial(m_clefts.back(), 1, 2);
	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,3)], prcGo));
	grow_radial(m_clefts.back(), 1, 3);
}

void CTerraCleftSystem::init_direct(Vect2i& pos1, Vect2i& pos2)
{
	Vect2f dir  = pos2 - pos1;
	dir.normalize(1);
	Vect2f dirp(-dir.y, dir.x);

	nSlips = int((pos2-pos1).norm())/cleftSlipSpacing + 1;

	for(int i=0; i<nSlips; i++)
	{
		Vect2i vc = pos1 + dir*i*cleftSlipSpacing;

		//расширение в начале
		if(i<cleftSlipWidth/2)
		{
			if(i)
			{
				for(int j=0; j<=2*i; j++)
				{
					Vect2i v = vc + dirp*(j-i)*cleftSlipSpacing + Vect2i(_xnoise(), _xnoise());
					m_nodes[ID(i, j)]= v;
				}
			}
			else
				m_nodes[ID(i, 0)]= vc + Vect2i(_xnoise(), _xnoise());
		}
		else
		{
			//распространение полосы
			for(int j=0; j<cleftSlipWidth; j++)
			{
				Vect2i v = vc + dirp*(j - cleftSlipWidth/2)*cleftSlipSpacing + Vect2i(_xnoise(), _xnoise());
				m_nodes[ID(i, j)] = v;
			}
		}
	}

	//
	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,0)], prcGo));
	grow_direct(m_clefts.back(), 1, 0);
	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,1)], prcGo));
	grow_direct(m_clefts.back(), 1, 1);
	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,2)], prcGo));
	grow_direct(m_clefts.back(), 1, 2);
}
void CTerraCleftSystem::init_direct_thin(Vect2i& pos1, Vect2i& pos2)
{
	Vect2f dir  = pos2 - pos1;
	dir.normalize(1);
	Vect2f dirp(-dir.y, dir.x);

	nSlips = int((pos2-pos1).norm())/cleftSlipSpacing + 1;

	for(int i=0; i<nSlips; i++)
	{
		//распространение полосы
		Vect2i vc = pos1 + dir*i*cleftSlipSpacing;
		m_nodes[ID(i, 0)] = vc + Vect2i(_xnoise(), _xnoise());

		Vect2i v = vc - dirp*2*cleftSlipSpacing + Vect2i(_xnoise(), _xnoise());
		m_nodes[ID(i, 1)] = v;

		v = vc + dirp*2*cleftSlipSpacing + Vect2i(_xnoise(), _xnoise());
		m_nodes[ID(i, 2)] = v;
	}

	m_clefts.push_back(CTerraCleft(m_nodes[0], m_nodes[ID(1,0)], prcGo));
	grow_direct_thin(m_clefts.back(), 1, 0);
}
void CTerraCleftSystem::init(int type, Vect2i& pos1, Vect2i& pos2)
{
	if(type == csysRadial)
		init_radial(pos1);

	if(type == csysDirectional)
		init_direct(pos1, pos2);

	if(type == csysDirectionalThin)
		init_direct_thin(pos1, pos2);

	m_nodes.clear();
	_xmp.clear();
}

int CTerraCleftSystem::quant()
{
	RECT rUpdate = {INT_MAX, INT_MAX, -INT_MAX, -INT_MAX};
	int nstops;

	CleftList::iterator it;
	FOR_EACH(m_clefts, it)
	{
		if((it->processing == prcGo) && !it->quant(rUpdate))
		{
			it->activate_siblings();

			if(m_bGrowing)
				it->processing = prcGrow;
			else
			{
				it->processing = prcStop;
				nstops++;
			}
		}
		else if((it->processing == prcGrow) && !it->quant_g(rUpdate))
		{
			it->processing = prcStop;
			nstops++;
		}
	}
	//update_map(rUpdate);

	return nstops != m_clefts.size();
}

#define PUSH(id) {if(b = find_node(id)){m_clefts.push_back(CTerraCleft(*a, *b));}}

void CTerraCleftSystem::grow_radial(CTerraCleft& par, int i, int j)
{
	if(i == cleftRadCount)
		return;

	if(_xmp.find(ID(i, j)) != _xmp.end()) //уже включено
		return;

	Vect2i* b;
	Vect2i* a = find_node(ID(i, j));
	if(!a)
		return;

	_xmp[ID(i,j)] = 1;

	int jC = 2<<i;
	if(j != jC-1)
		PUSH(ID(i, j+1))
	else
		PUSH(ID(i, 0))

	PUSH(ID(i+1, j*2))
	if(b) grow_radial(par.attach(m_clefts.back()), i+1, j*2);

	if(j != 0)
	{
		PUSH(ID(i+1, j*2 - 1))
		if(b) grow_radial(par.attach(m_clefts.back()), i+1, j*2-1);
	}
	else
	{
		PUSH(ID(i+1, jC*2 - 1))
		if(b) grow_radial(par.attach(m_clefts.back()), i+1, jC*2-1);
	}


	PUSH(ID(i+1, j*2+1))
	if(b) grow_radial(par.attach(m_clefts.back()), i+1, j*2+1);
}

inline Vect2i* CTerraCleftSystem::find_node(int id)
{
	NodeMap::iterator it = m_nodes.find(id);

	return it == m_nodes.end() ? 0 : &it->second;
}

void CTerraCleftSystem::grow_direct(CTerraCleft& par, int i, int j)
{
	if(_xmp.find(ID(i, j)) != _xmp.end()) //уже включено
		return;

	Vect2i* a = find_node(ID(i, j)); 
	if(!a)
		return;

	_xmp[ID(i,j)] = 1;

	if(i<cleftSlipWidth/2)
	{
		//расширение в начале
		if(j < 2*i)
		{
			m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i, j+1)]));
			grow_direct(par.attach(m_clefts.back()), i, j+1);
		}
		if(j == 0)
		{
			m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i+1,0)]));
			grow_direct(par.attach(m_clefts.back()), i+1, 0);
		}
		if(j == 2*i)
		{
			m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i+1,2*(i+1))]));
			grow_direct(par.attach(m_clefts.back()), i+1, 2*(i+1));
		}
		m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i+1, j+1)]));
		grow_direct(par.attach(m_clefts.back()), i+1, j+1);
	}
	else
	{
		//основная полоса
		if(i != nSlips-1)
		{
			m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i+1, j)]));
			grow_direct(par.attach(m_clefts.back()), i+1, j);
		}
		if(j < cleftSlipWidth-1)
		{
			m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i, j+1)]));
			grow_direct(par.attach(m_clefts.back()), i, j+1);
		}
	}
}
void CTerraCleftSystem::grow_direct_thin(CTerraCleft& par, int i, int j)
{
	Vect2i* a = find_node(ID(i, j)); 
	if(!a)
		return;

	if(i != nSlips-1)
	{
		m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i+1, j)]));
		grow_direct_thin(par.attach(m_clefts.back()), i+1, j);

		if(terLogicRND(10)>6)
		{
			m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i, 1)]));
			par.attach(m_clefts.back());
		}
		if(terLogicRND(10)>6)
		{
			m_clefts.push_back(CTerraCleft(*a, m_nodes[ID(i, 2)]));
			par.attach(m_clefts.back());
		}
	}
}

