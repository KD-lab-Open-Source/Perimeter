// TerraCleft.h: interface for the CTerraCleft class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRACLEFT_H__20A4C072_87B4_4201_A764_C92EC5A51678__INCLUDED_)
#define AFX_TERRACLEFT_H__20A4C072_87B4_4201_A764_C92EC5A51678__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xmath.h"
#include <map>

enum
{
	prcWait=0,
	prcGo,
	prcGrow,
	prcStop
};

class CTerraCleft  
{
	typedef list<Vect2i> PointList;
	PointList            m_points;

	double  delta;
	Vect2i  m_to;
	Vect2i _next_chunk();

	int    width;
public:

	typedef list<CTerraCleft*> SiblingList;
	SiblingList m_siblings;
	int processing;

	void activate_siblings();

	CTerraCleft(const Vect2i& pos1, const Vect2i& pos2, int active = prcWait);
	~CTerraCleft();

	int  quant(RECT& r);
	int  quant_g(RECT& r);

	CTerraCleft& attach(CTerraCleft& cleft);
	Vect2i& to(){
		return m_to;
	}
};

///////////////////////////////////////
//

enum
{
	csysRadial,
	csysDirectional,
	csysDirectionalThin,
};

class CTerraCleftSystem
{
	typedef list<CTerraCleft> CleftList;
	typedef map<int, Vect2i>  NodeMap;

	int           nSlips;
	map<int, int> _xmp;

	NodeMap       m_nodes;
	CleftList     m_clefts;


	Vect2i* find_node(int id);
	void init_radial(const Vect2i& pos1);
	void init_direct(const Vect2i& pos1, const Vect2i& pos2);
	void init_direct_thin(const Vect2i& pos1, const Vect2i& pos2);
	void grow_radial(CTerraCleft& par, int i, int j);
	void grow_direct(CTerraCleft& par, int i, int j);
	void grow_direct_thin(CTerraCleft& par, int i, int j);
public:

	int   m_bGrowing;

	CTerraCleftSystem();

	void init(int type, const Vect2i& pos1, const Vect2i& pos2 = Vect2i(0,0));
	int quant();
};

#endif // !defined(AFX_TERRACLEFT_H__20A4C072_87B4_4201_A764_C92EC5A51678__INCLUDED_)
