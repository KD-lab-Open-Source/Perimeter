// DrawGraph.cpp : implementation file
//

#include "stdafx.h"
#include "effecttool.h"
#include "DrawGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IS_CTRL() (GetAsyncKeyState(VK_CONTROL) & 0x8000)

const int _spacing = 10;
const int _spacing_left = 30;
const int n_x_axis_bottom_space = 30;
const int _graph_point_sz = 6;

CRect work;
int Y_Ox;
float xScale,yScale;

bool lockScale = false;
GRAPH_BOUND bound(0,0);
float time_max = 0;

float transX(float x, bool sm = true)
{
	if (sm)	return work.left + xScale*x;
	else return xScale*x;
}
float transY(float y, bool sm = true)
{
	if (sm) return work.bottom - yScale*y - Y_Ox;
	else return yScale*y;
}
void Transform(float& x, float& y, bool sm = true)
{
	x = transX(x,sm); 
	y = transY(y,sm);
}
float untransX(float x, bool sm = true)
{
	if (sm) return (x - work.left)/xScale;
	else return x/xScale;
}
float untransY(float y, bool sm =true)
{
	if (sm) return -(y - work.bottom + Y_Ox)/yScale;
	else return y/yScale;
}
void UnTransform(float& x, float& y, bool sm =true)
{
	x = untransX(x, sm);
	y = untransY(y, sm);
}

static CBrush brushGreen(RGB(0, 255, 0));
static CBrush brushRed(RGB(255, 0, 0));
static CBrush brushYellow(RGB(255, 255, 0));
				
static CPen   penBlue(PS_SOLID, 5, RGB(0, 0, 255));
static CPen   penRed(PS_SOLID, 4, RGB(255, 0, 0));
static CPen   penGreen(PS_SOLID, 2, RGB(0, 255, 0));
static CPen   penGray(PS_SOLID, 4, RGB(128, 128, 128));
/*const pen_count = 4;
enum EPens{P_Red,P_Green,P_Blue,P_Gray};
CPen* pens[pen_count] = {&penRed,&penGreen,&penBlue,&penGray};
*/
CDrawGraph* _pWndGraph = 0;

int  _nGraphWidth = 0;
int  _nGraphHeight = 0;


void RepaintGraph()
{
	if(_pWndGraph)
		_pWndGraph->Invalidate();
}
float GetKeyMaxValue(KeyWrapBase& key)
{
	float fMax = -_HUGE;

	int n = key.size();

	if(n>100) return 0;

	for(int i=0; i<n; i++)
		if(fMax < key(i))
			fMax = key(i);

	if(fMax < 0.1f)
		fMax = 0.1f;

	return fMax;
}

bool PointOnLine(const CPoint& p, const CPoint& p1, const CPoint& p2)
{
	CRect rc(p1, p2);
	rc.NormalizeRect();
	rc.InflateRect(4, 4);

	if(rc.PtInRect(p))
	{
		Vect2f a(p.x - p1.x, p.y - p1.y);
		Vect2f b(p2.x - p1.x, p2.y - p1.y);

		a.normalize(1);
		b.normalize(1);

		return fabs(1.0f - a.dot(b)) < 0.001f;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
//

CGraphObject::CGraphObject(CGraphObject* p)
{
	m_pParent = p;
	m_bActive = false;
}

/////////////////////////////////////////////////////////////////////////////
//

CGraphPoint::CGraphPoint(CGraphObject* p, int n, KeyWrapBase& key, 
						 int x, int y, CBrush* pBr, CBrush* pBrSel) : CGraphObject(p), m_pt(x, y) , m_key(key)
{
	m_nNumber = n;
	m_pBrush = pBr;
	m_pBrushSelected = pBrSel;
}

void CGraphPoint::Draw(CDC* pDC)
{
	pDC->SelectStockObject(BLACK_PEN);

	if(m_bActive)
		pDC->SelectObject(m_pBrushSelected);
	else
		pDC->SelectObject(m_pBrush);

	CRect rc(m_pt, m_pt);
	rc.InflateRect(_graph_point_sz, _graph_point_sz);
	pDC->Ellipse(rc);
}
CGraphObject* CGraphPoint::HitTest(const CPoint& pt)
{
	CRect rc(m_pt, m_pt);
	rc.InflateRect(_graph_point_sz, _graph_point_sz);

	return rc.PtInRect(pt) ? this : 0;
}
void CGraphPoint::OffsetPos(int x, int y)
{
	CGraphLine* pParent = dynamic_cast<CGraphLine*>(m_pParent);
	int n = GetState();
	if(n & TRACK_VERT)
		m_pt.y += y;
	if(n & TRACK_HORZ)
	{
		m_pt.x += x;

		if(pParent)
			pParent->CheckHorzPos(this);
	}
//	CGraphLine* pParent = dynamic_cast<CGraphLine*>(m_pParent);
/*	if (m_bActive&&pParent&&IS_CTRL())
	{
		CGraphLine::PointVectorType::iterator it;
		FOR_EACH(pParent->m_points,it)
			if (!(*it)->m_bActive) 
			{
				(*it)->OffsetPos(0, y);
//				(*it)->OnChange();
			}
	}*/
}

/////////////////////////////////////////////////////////////////////////////
//
CBlueLinePoint::CBlueLinePoint(CGraphObject* p, int n, KeyWrapBase& key, int x, int y) : 
CGraphPoint(p, n, key, x, y, (dynamic_cast<CGraphBlueLine*>(p)&&(dynamic_cast<CGraphBlueLine*>(p))->type==0) ? &brushGreen : &brushYellow, &brushRed) 
{
}
void CBlueLinePoint::OffsetPos(int x, int y)
{
	CGraphBlueLine* pParent = dynamic_cast<CGraphBlueLine*>(m_pParent);
	ASSERT(pParent);
	if (pParent->type==1&&m_key.size()==1)
	{
		ASSERT(pParent->m_points.size()==2);
		pParent->m_points[0]->CGraphPoint::OffsetPos(0, y);
		pParent->m_points[1]->CGraphPoint::OffsetPos(0, y);
		pParent->m_points[0]->CGraphPoint::OnChange();
		pParent->m_points[1]->CGraphPoint::OnChange();
	}
	else CGraphPoint::OffsetPos(x, y);
}

void CBlueLinePoint::Draw(CDC* pDC)
{
	m_bActive = _pDoc->m_nCurrentGenerationPoint == m_nNumber;
	CGraphPoint::Draw(pDC);

}
#include "OptTree.h"
extern COptTree* tr;
void CBlueLinePoint::OnChange()
{
	if (!_pWndGraph) return;
	const int type = ((CGraphBlueLine*)m_pParent)->type;
	_pWndGraph->CalcTransform();
	float t = untransX(m_pt.x);
	if (type&&(UINT)m_nNumber < m_key.size())
	{
		float y = untransY(m_pt.y)*((CGraphBlueLine*)m_pParent)->koef;
		CGraphKeyLine* pParent = (CGraphKeyLine*)m_pParent;
		if (pParent&&IS_CTRL())
		{
			float dv = y - m_key(m_nNumber);
			for(int i=0;i<pParent->m_key.size();++i)
				pParent->m_key(i) += dv;
		}
		else m_key(m_nNumber) = y;
	}
	ASSERT(_pDoc && _pDoc->m_pActiveEmitter);
	if (m_nNumber == _pDoc->m_pActiveEmitter->emitter_pos().size()-1 && _pDoc->m_pActiveEmitter->emitter_pos().size()!=1)
		_pDoc->m_pActiveEmitter->ChangeLifetime(t);
	else if(type==0 || m_key.size()>1)
		_pDoc->set_gen_point_time(m_nNumber, t);
}
int CBlueLinePoint::GetState()
{ 
	DWORD st=0;
	CKeyPos& keypos = (_pDoc->m_pActiveEmitter->data) ? _pDoc->m_pActiveEmitter->data->emitter_position : 
							_pDoc->m_pActiveEmitter->GetLight()->emitter_position;
	if (m_nNumber > 0 && m_nNumber < keypos.size() - 1)
		st|= TRACK_HORZ|REMOVEABLE;
	if (((CGraphBlueLine*)m_pParent)->type)
		st|=TRACK_VERT;
	if (m_nNumber == keypos.size()-1)
		st|= TRACK_HORZ;
	return st;
}


/////////////////////////////////////////////////////////////////////////////
//
CGraphLine::CGraphLine(KeyWrapBase& key, CPen* pPen) : CGraphObject(0), m_key(key)
{
	m_bVisible = false;
	m_bCanCreatePoints = true;
	m_pPen = pPen;
}

CGraphObject* CGraphLine::HitTest(const CPoint& pt)
{
	if(!m_bVisible)
		return 0;
	
	if(m_points.size() > 1)
	{
		int sz = m_points.size();
		for(int i=0; i<sz; i++)
			if(m_points[i]->HitTest(pt))
				return m_points[i];

		bool bLineHit = false;
		for(i=1; i<sz; i++)
		{
			const CPoint& p1 = m_points[i-1]->GetPos();
			const CPoint& p2 = m_points[i]->GetPos();

			if(bLineHit = PointOnLine(pt, p1, p2))
				break;
		}

		if(bLineHit && m_bCanCreatePoints)
			return CreateNewPoint(pt);
	}

	return 0;
}

void CGraphLine::Draw(CDC* pDC)
{
	if(!m_bVisible)
		return;

	if(!m_points.empty())
	{
		pDC->SelectObject(m_pPen);
		pDC->MoveTo(m_points[0]->GetPos());

		PointVectorType::iterator i;
		FOR_EACH(m_points, i)
			pDC->LineTo((*i)->GetPos());

		FOR_EACH(m_points, i)
			(*i)->Draw(pDC);
	}
}
int CGraphLine::GetNumObj(CGraphObject* Obj)
{
	int n;
	for(n = m_points.size()-1; n>=0; --n)
		if (Obj == m_points[n])
			break;
//	int n = std::distance((CGraphObject*)*m_points.begin(),Obj);
//	if ((UINT)n>=m_points.size())
//		n = -1;
	return n;
}

CGraphObject* CGraphLine::GetObj(int n)
{
	if ((UINT)n>=m_points.size())
		return NULL;
	return m_points[n];
}

void CGraphLine::CheckHorzPos(CGraphPoint* pPoint)
{
	CPoint& pp = pPoint->GetPos();

	int k = 0;
	PointVectorType::iterator i;
	FOR_EACH(m_points, i)
	{
		if(*i == pPoint)
		{				
			if(k > 0)
			{
				const CPoint& p = (*(i-1))->GetPos();
				if(p.x > pp.x)
					pp.x = p.x;
			}
			if(k < m_points.size() - 1)
			{
				const CPoint& p = (*(i+1))->GetPos();
				if(p.x < pp.x)
					pp.x = p.x;
			}
			break;
		}

		k++;
	}
}

CGraphBlueLine::CGraphBlueLine(int tp,KeyWrapBase& key,float koef_) : CGraphLine(key,tp==0? &penBlue : &penGreen),
								type(tp),koef(koef_)
{
}
float CGraphBlueLine::GetBoundValue(GRAPH_BOUND& f)
{
	f.max = 0;
	f.min = 100000;
	if (type)
	for(int i=0;i<m_key.size();++i)
		if (m_key(i)>f.max) 
			f.max = m_key(i);
		else if (m_key(i)<f.min) 
			f.min = m_key(i);
	float tmax = 0;
	if (m_key.size())
		tmax = m_key.time(m_key.size()-1);
	f.max/=koef;
	f.min/=koef;
	if (f.max<0.1f) f.max = 0.1f;
	if (f.min>0) f.min = 0;
	return tmax;
}
void CGraphBlueLine::Update(bool bFullUpdate)
{
	if(!_pDoc->m_pActiveEmitter||!_pWndGraph){
		m_points.clear();
		return;
	}

/*	if(_pDoc->m_pActiveEmitter->Class()==EMC_LIGHT){
		m_bVisible = false;
		_pDoc->m_nCurrentGenerationPoint = 0;
		return;
	}*/
	CKeyPos& keypos = _pDoc->m_pActiveEmitter->emitter_pos();
	int nPoints = keypos.size();
	
	m_bVisible = type==1 || nPoints>1;

	if(!m_bVisible)
		return;
	if(_pDoc->m_pActiveEmitter)
	{
		_pWndGraph->CalcTransform();
//		float fScaleX = _nGraphWidth/_pWndGraph->GetTimescaleMax();
//		float fScaleY = (_nGraphHeight - _spacing)/_pWndGraph->GetMaxValue();

//		int ty = _nGraphHeight + n_x_axis_bottom_space*0.8;

		m_points.clear();

		if (type == 1 && nPoints==1)
		{
			float x = _pDoc->m_pActiveEmitter->EmitterLifeTime();
			float y = m_key(0)/koef;
			Transform(x,y);
			m_points.push_back(new CBlueLinePoint(this, 0, m_key, transX(0), y));
			m_points.push_back(new CBlueLinePoint(this, 0, m_key, x, y));
		}
		else for(int i=0; i<nPoints; i++)
		{
			float y =  m_key(i)/koef;
			float x = keypos[i].time*_pDoc->m_pActiveEmitter->EmitterLifeTime();//_pDoc->get_gen_point_time(i);
			Transform(x,y);
			if (type==0)
				y = work.bottom + 20;
			m_points.push_back(new CBlueLinePoint(this, i, m_key, x, y));
		}
	}
}
CGraphPoint* CGraphBlueLine::CreateNewPoint(const CPoint& pt)
{
	CKeyPos& keypos = (_pDoc->m_pActiveEmitter->data) ? 
				_pDoc->m_pActiveEmitter->data->emitter_position : 
				_pDoc->m_pActiveEmitter->GetLight()->emitter_position;
	if (type==1&&keypos.size()==1)
		return NULL;
	CGraphPoint* p = 0;
	int k = 0;
	PointVectorType::iterator it;
	FOR_EACH(m_points, it)
	{
		if((*it)->GetPos().x > pt.x)
		{
			float x1= m_points[k-1]->GetPos().x;
			float x2 = (*it)->GetPos().x;
			float x = pt.x;
			float tg =(x-x1)/(x2-x1);
			_pDoc->m_pActiveEmitter->InsertGenerationPoint(k,tg);
			_pDoc->m_pActiveEmitter->SetGenerationPointTime(k,untransX(pt.x)/_pDoc->m_pActiveEmitter->EmitterLifeTime());
			it = m_points.insert(it, p = new CBlueLinePoint(this, k, m_key, pt.x, type==1? pt.y : (*it)->GetPos().y));

			while(++it != m_points.end())
				(*it)->m_nNumber++;

			break;
		}

		k++;
	}

	_pWndGraph->Invalidate();
	return p;
}
void CGraphBlueLine::OnRemoveObject(CGraphObject* pObject)
{
	int k = 0;
	PointVectorType::iterator i;
	FOR_EACH(m_points, i)
	{
		if(pObject == *i)
		{
			_pDoc->m_pActiveEmitter->DeleteGenerationPoint(k);
			_pDoc->m_nCurrentGenerationPoint = k-1;
			PointVectorType::iterator it = i;
			while(++it != m_points.end())
				(*it)->m_nNumber--;

			delete *i;
			m_points.erase(i);

			break;
		}
		k++;
	}
}

/////////////////////////////////////////////////////////////////////////////
//
CKeyLinePoint::CKeyLinePoint(CGraphObject* p, int n, KeyWrapBase& key, int x, int y,  bool b1, bool b2) : 
	CGraphPoint(p, n, key, x, y, &brushYellow, &brushRed) 
{
	m_bPrimaryKey = b1;
	m_bNoVertTrack = b2;
}

void CKeyLinePoint::Draw(CDC* pDC)
{
	m_bActive = _pDoc->m_nCurrentParticlePoint == m_nNumber;

	CGraphPoint::Draw(pDC);
}
void CKeyLinePoint::OnChange()
{
	_pWndGraph->CalcTransform();

	float x = m_pt.x;
	float y = m_pt.y;
	UnTransform(x,y);
	CGraphKeyLine* pParent = (CGraphKeyLine*)m_pParent;
	if (pParent&&IS_CTRL())
	{
		float dv = y - m_key(m_nNumber);
		for(int i=0;i<pParent->m_key.size();++i)
			pParent->m_key(i) += dv;
	}
	else m_key(m_nNumber) = y;

	bool bLastPoint = m_nNumber == m_key.size() - 1;
	if(bLastPoint) //последняя точка - изменяем время жизни частиц
	{
		m_key.time(m_nNumber) = 1.0f;

		_pDoc->change_particle_life_time(_pDoc->m_nCurrentGenerationPoint, x);
	}
	_pDoc->set_particle_key_time(_pDoc->m_nCurrentGenerationPoint, m_nNumber, x);
}

int CKeyLinePoint::GetState()
{
	int st = TRACK_VERT|TRACK_HORZ|REMOVEABLE;
//	if (m_bPrimaryKey)
//		st&=~TRACK_VERT;
	if (m_nNumber == 0)
		st&=~(REMOVEABLE|TRACK_HORZ);
	if (m_nNumber == m_key.size() - 1)
		st&=~REMOVEABLE;
	if (m_bNoVertTrack)
		st&=~TRACK_VERT;
	return st;
/*	if(!m_bNoVertTrack)
	{
		if(m_nNumber == 0 || !m_bPrimaryKey)
			return TRACK_VERT;
		else if(m_nNumber == m_key.size() - 1)
			return TRACK_VERT|TRACK_HORZ;

		return TRACK_VERT|TRACK_HORZ|REMOVEABLE;
	}
	else
	{
		if(m_nNumber == 0)
			return 0;

		if(m_nNumber == m_key.size() - 1)
			return TRACK_HORZ;

		return TRACK_HORZ|REMOVEABLE;
	}

	return 0;
*/
}

/////////////////////////////////////////////////////////////////////////////
//
CGraphKeyLine::CGraphKeyLine(KeyWrapBase& key, bool bPrimary, bool bNoVertTrack) :
		CGraphLine(key, bPrimary ? &penRed : &penGreen)
{
	if(/**/m_bPrimaryKey = bPrimary)
		m_bNoVertTrack = bNoVertTrack;
	else
		m_bNoVertTrack = false;

//	m_bNoVertTrack = bNoVertTrack;
	m_bCanCreatePoints = true;//bPrimary;
}
CGraphKeyLine::~CGraphKeyLine()
{
	delete &m_key;
}
CGraphPoint* CGraphKeyLine::CreateNewPoint(const CPoint& pt)
{
	CGraphPoint* p = 0;

	int k = 0;
	PointVectorType::iterator it;
	FOR_EACH(m_points, it)
	{
		if((*it)->GetPos().x > pt.x)
		{
			float x1= m_points[k-1]->GetPos().x;
			float x2 = (*it)->GetPos().x;
			float x = pt.x;
			float tg =(x-x1)/(x2-x1);
			it = m_points.insert(it, p = new CKeyLinePoint(this, k, m_key, pt.x, pt.y, m_bPrimaryKey, m_bNoVertTrack));
			
			_pDoc->m_pActiveEmitter->InsertParticleKey(k,tg);
			if(m_bNoVertTrack)
			{
				p->GetPos().y = _nGraphHeight;
			}

			while(++it != m_points.end())
			{
				(*it)->m_nNumber++;
			}
			break;
		}

		k++;
	}

	_pWndGraph->Invalidate();
	return p;
}
void CGraphKeyLine::Update(bool bFullUpdate)
{
	if(/*!m_pKey || */!_pDoc->m_pActiveEmitter)
	{
		m_bVisible = false;
		return;
	}
	else
		m_bVisible = true;

	m_points.clear();

	_pWndGraph->CalcTransform();
//	float fScaleX = _nGraphWidth/_pWndGraph->GetTimescaleMax();
//	float fScaleY = (_nGraphHeight - _spacing)/_pWndGraph->GetMaxValue();

	int sz = m_key.size();

	if(sz>100) return;

	for(int k=0; k<sz; k++)
	{
//		int x = _spacing_left + _pDoc->get_particle_key_time(_pDoc->m_nCurrentGenerationPoint, k)*fScaleX;
//		int y = _nGraphHeight - m_key(k)*fScaleY;
		float x = _pDoc->get_particle_key_time(_pDoc->m_nCurrentGenerationPoint, k);
		float y = m_key(k);
		Transform(x,y);
		m_points.push_back(new CKeyLinePoint(this, k, m_key, x, y, m_bPrimaryKey, m_bNoVertTrack));
	}
}
void CGraphKeyLine::OnRemoveObject(CGraphObject* pObject)
{
	int k = 0;
	PointVectorType::iterator it;
	FOR_EACH(m_points, it)
	{
		if(*it == pObject)
		{
			if(k==0||k==m_points.size()-1)
				return;
			_pDoc->m_pActiveEmitter->DeleteParticleKey(k);
			_pDoc->m_nCurrentParticlePoint = 0;

			PointVectorType::iterator i = it;
			while(++it != m_points.end())
				(*it)->m_nNumber--;
			
			delete *i;
			m_points.erase(i);

			break;
		}

		k++;
	}
}
float CGraphKeyLine::GetBoundValue(GRAPH_BOUND& f)
{
	f.max = 0;
	f.min = 100000;
	int n = m_key.size();
	for(int i=0; i<n; i++)
		if(f.max < m_key(i))
			f.max = m_key(i);
		else if(f.min > m_key(i))
			f.min = m_key(i);
	if(f.max < 0.1f)
		f.max = 0.1f;
	if (f.min >0)
		f.min = 0;
	float tmax = 0;
	if (m_key.size())
		tmax = m_key.time(m_key.size()-1);
	return tmax;
}


/////////////////////////////////////////////////////////////////////////////
//
CTimelineContainer::CTimelineContainer() : CGraphObject(0)
{
}

void CTimelineContainer::Draw(CDC* pDC)
{
	ItemVectorType::iterator it;
	FOR_EACH(m_items, it)
		(*it)->Draw(pDC);
}
CGraphObject* CTimelineContainer::HitTest(const CPoint& pt)
{
	CGraphObject* p = 0;
	ItemVectorType::iterator it;
	FOR_EACH(m_items, it)
		if(p = (*it)->HitTest(pt))
			break;

	return p;
}
void CTimelineContainer::Update(bool bFullUpdate)
{
	if(bFullUpdate)
	{
		m_items.clear();

		if(_pDoc->m_pActiveEmitter == 0)
		{
			if(_pDoc->m_pActiveEffect)
			{
				int sz = _pDoc->m_pActiveEffect->emitters.size();
				for(int i=0; i<sz; i++)
				{
					if(_pDoc->m_pActiveEffect->emitters[i]->data)
						m_items.push_back(new CEmitterTimeline(this, i, _pDoc->m_pActiveEffect->emitters[i]->data->name.c_str()));
					else
						m_items.push_back(new CEmitterTimeline(this, i, _pDoc->m_pActiveEffect->emitters[i]->data_light->name.c_str()));
				}
			}
		}
	}
	else
	{
		ItemVectorType::iterator it;
		FOR_EACH(m_items, it)
			(*it)->Update(false);
	}
}
int CTimelineContainer::GetNumObj(CGraphObject* obj)
{
	int i= m_items.size()-1;
	for(;i>=-0;--i)
		if (obj == m_items[i])
			break;
	return i;
}
CGraphObject* CTimelineContainer::GetObj(int ix)
{
	ASSERT((UINT)ix<m_items.size());
	return m_items[ix];
}


CEmitterTimeline::CEmitterTimeline(CGraphObject* p, int n, LPCTSTR name) : CGraphObject(p),m_CurPoint(-1)
{
	m_nEmitter = n;

	m_name = name;

	Update(false);
}

void CEmitterTimeline::Draw(CDC* pDC)
{
	if(m_points.empty()) return;
	CEmitterData* pKey = _pDoc->m_pActiveEffect->emitters[m_nEmitter];

	if(pKey->bActive)
		pDC->SelectObject(&penRed);
	else
		pDC->SelectObject(&penGray);

	pDC->MoveTo(m_points.front());
	pDC->LineTo(m_points.back());

	pDC->SelectStockObject(BLACK_PEN);
	pDC->SelectObject(&brushYellow);

	PointList::iterator i;
	FOR_EACH(m_points, i)
	{
		CRect rc(*i, *i);
		rc.InflateRect(4, 4);
		pDC->Ellipse(rc);
	}

	// Draw the name
	COLORREF oldcol = pDC->SetTextColor(RGB(0,0,0));

	int x = m_points.back().x + 10;
	int y = m_points.front().y + 3;
	UINT oldalign = pDC->SetTextAlign(TA_LEFT|TA_BASELINE);

	pDC->TextOut(x, y, m_name);

	pDC->SetTextColor(oldcol);
	pDC->SetTextAlign(oldalign);
}
CGraphObject* CEmitterTimeline::HitTest(const CPoint& pt)
{
	CRect rc(m_points.front(), m_points.back());
	rc.InflateRect(4, 4);
	if (rc.PtInRect(pt))
	{
		m_CurPoint = -1;
		for(int i=0;i<m_points.size();++i)
		{
			if (abs(pt.x - m_points[i].x)<=4 && abs(pt.y - m_points[i].y)<=4)
			{
				m_CurPoint = i;
				if (m_CurPoint)
					break;
			}
		}
		return this;
	}
	return NULL;
}
void CEmitterTimeline::OffsetPos(int x, int y)
{
	if (m_CurPoint<0)
	{
		PointList::iterator i;
		FOR_EACH(m_points, i)
			i->x += x;
	}else
	{
		m_points[m_CurPoint].x += x;
//		if (m_points[m_CurPoint].x>work.right)
//			m_points[m_CurPoint].x = work.right;
		if(m_CurPoint>0)
			if (m_points[m_CurPoint].x<m_points[m_CurPoint-1].x) 
				m_points[m_CurPoint].x = m_points[m_CurPoint-1].x;
		if	(m_CurPoint<m_points.size()-1)
			 if (m_points[m_CurPoint].x>m_points[m_CurPoint+1].x) 
				m_points[m_CurPoint].x = m_points[m_CurPoint+1].x;
	}

}
void CEmitterTimeline::Update(bool bFullUpdate)
{
	if(!_pWndGraph) return;
	const int emitter_step = 10;

	_pWndGraph->CalcTransform();
	float y = transY(m_nEmitter + 1);

	CEmitterData* pKey = _pDoc->m_pActiveEffect->emitters[m_nEmitter];
	if (m_CurPoint>=0)
	{
		if (m_CurPoint==0)
		{
			float t = untransX(m_points[0].x);
			if (t<0) t=0;
			float dt = pKey->EmitterCreateTime() - t;
			pKey->EmitterCreateTime() = t;
			pKey->EmitterLifeTime() += dt;
		}
		else if (m_CurPoint==m_points.size()-1)
		{
			float t = untransX(m_points[m_points.size()-1].x);
			t-=pKey->EmitterCreateTime();
			if (t<.05f) t = .05f;
			pKey->ChangeLifetime(t);
		}
		else
		{
			float time_from = pKey->EmitterCreateTime();
			float t = untransX(m_points[m_CurPoint].x) - time_from;
			pKey->SetGenerationPointTime(m_CurPoint, t/pKey->EmitterLifeTime());
		}
	}
	m_points.clear();

	float time_from = 0; 
	int c = 0;
	time_from = pKey->EmitterCreateTime();
	c = pKey->emitter_pos().size();
	for(int i=0; i<c; i++)
	{
		float x = transX(time_from + pKey->emitter_pos()[i].time*pKey->EmitterLifeTime());//_pDoc->get_gen_point_time(i, pKey->data));
		m_points.push_back(CPoint(x, y));
	}
	if(c == 1)
	{
		float time_to = pKey->EmitterEndTime();
		float x = transX(time_to);
		m_points.push_back(CPoint(x, y));
	}
}
CPoint& CEmitterTimeline::GetPos()
{
	static CPoint r(-1,-1);
	if (m_CurPoint<m_points.size())
		return m_points[m_CurPoint];
	else 
		return r;
}
void CEmitterTimeline::OnChange()
{
	_pWndGraph->CalcTransform();
//	float fScale = _nGraphWidth/_pWndGraph->GetTimescaleMax();

	EmitterKeyInterface* pKey = _pDoc->m_pActiveEffect->key[m_nEmitter];
	if (m_CurPoint<0)
		pKey->emitter_create_time = untransX(m_points.front().x);
//	pKey->emitter_create_time = float(m_points.front().x - _spacing_left)/fScale;
	if(pKey->emitter_create_time < 0)
		pKey->emitter_create_time = 0;

	m_pParent->Update(false);
	_pWndGraph->GetParent()->SendMessage(WM_SLIDER_UPDATE);
}

/////////////////////////////////////////////////////////////////////////////
// CDrawGraph

CDrawGraph::CDrawGraph()
{
	_pWndGraph = this;

	m_ptPrevPoint = CPoint(0, 0);
	m_pTrack = 0;

	m_fLastKeylinesMax = 0;
	m_fLastTimeMax = 0;
	m_fEffectTimelineMax = 0;
	m_bLockKeylinesMaxUpdate = false;
	m_bLockTimeMaxUpdate = false;

	m_FontTick.CreatePointFont(80, "Arial");

	m_objects.push_back(new CGraphBlueLine);
	m_objects.push_back(new CTimelineContainer);
}

CDrawGraph::~CDrawGraph()
{
}
void CDrawGraph::Init()
{
	SetTimer(1, 100, 0);
}
void CDrawGraph::Update(bool bFullUpdate)
{
	if(_pDoc)
	{
		int nObj = -1;
		CGraphObject* parent = NULL;
		if (m_pTrack)
		{
			parent = m_pTrack->GetParent();
			if (parent)	
			{
				nObj = parent->GetNumObj(m_pTrack);
				if(nObj!=-1) m_pTrack  = NULL;
			}
		}

		ObjectVectorType::iterator i;
		FOR_EACH(m_objects, i)
			(*i)->Update(bFullUpdate);

		if(_pDoc->m_pActiveEmitter ==  0 && _pDoc->m_pActiveEffect && bFullUpdate)
			m_fEffectTimelineMax = 1.1*_pDoc->m_pActiveEffect->GetTotalLifeTime();

		Invalidate();
		if (parent)
			if(nObj!=-1)m_pTrack = parent->GetObj(nObj);
	}
}

void CDrawGraph::RemoveObject(CGraphObject* pObject)
{
	if(pObject->GetState() & CGraphObject::REMOVEABLE)
	{
		ObjectVectorType::iterator i;
		FOR_EACH(m_objects, i)
			(*i)->OnRemoveObject(pObject);

		GetParent()->SendMessage(WM_ACTIVE_POINT);
	}
}
void CDrawGraph::ShowKeys(list<CKey*>& keys, BSKey* pKeyBS, bool b, bool last,list<CKeyWithParam>& emit_param )
{
	//удалить все CGraphKeyLine
	ObjectVectorType::iterator i = m_objects.begin();
	while(i != m_objects.end())
	{
		if((typeid(**i) == typeid(CGraphKeyLine))||((typeid(**i) == typeid(CGraphBlueLine))&&((CGraphBlueLine*)*i)->type))
		{
			delete *i;
			i = m_objects.erase(i);
		}
		else 
			i++;
	}
	int k = 0;
	list<CKey*>::iterator i_key;
	FOR_EACH(keys, i_key)
		m_objects.push_back(new CGraphKeyLine(*(new KeyFltWrap(**i_key)), (k++ == keys.size()-1)&&last, b));

	list<CKeyWithParam>::iterator it;
	FOR_EACH(emit_param,it)
		m_objects.push_back(new CGraphBlueLine(1,*(new KeyFltWrap(*it->p)),it->dat));

	if(pKeyBS)
		m_objects.push_back(new CGraphKeyLine(*(new KeyBsWrap(*pKeyBS)), false, false));

	Update(true);
	Invalidate();
}

void CDrawGraph::ShowSplineKey(CKeyPosHermit& key)
{
	m_objects.push_back(new CGraphKeyLine(*(new KeyFltWrap(key)), true, true));

	Update(true);
	Invalidate();
}

void CDrawGraph::ShowLightKey(CKeyPos& key)
{
	m_objects.push_back(new CGraphKeyLine(*(new KeyFltWrap(key)), true, true));

	Update(true);
	Invalidate();
}
/*
float CDrawGraph::GetMaxValue(GRAPH_BOUND* bn)
{
//	if(m_bLockKeylinesMaxUpdate && (m_fLastKeylinesMax != 0))
//		return m_fLastKeylinesMax;

	GRAPH_BOUND f;
	f.max = 0;
	f.min = 10000;

	ObjectVectorType::iterator i = m_objects.begin();
	FOR_EACH(m_objects, i)
	{
		CGraphLine* pp;
		if(pp = dynamic_cast<CGraphLine*>(*i))
		{
			GRAPH_BOUND _f;
			pp->GetBoundValue(_f);
			if(f.max < _f.max)
				f.max = _f.max;
			if (f.min > _f.min)
				f.min = _f.min;
		}
	}
	if (bn)
		*bn = f;
	return m_fLastKeylinesMax = f.max;
}*/
float CDrawGraph::GetTimescaleMax(bool all /*= false*/)
{
	float tmax;
	if(_pDoc->m_pActiveEmitter)
	{
//		if(m_bLockTimeMaxUpdate && (m_fLastTimeMax != 0))
//			tmax = m_fLastTimeMax;
//		else
		{
			m_fLastTimeMax = _pDoc->m_pActiveEmitter->ParticleLifeTime();
			if (all)
			{
				float t  = _pDoc->m_pActiveEmitter->EmitterLifeTime();	
				if (m_fLastTimeMax<t)
					m_fLastTimeMax = t;
			}
			if(m_fLastTimeMax < 1.0f)
				m_fLastTimeMax = 1.0f;
			tmax=m_fLastTimeMax;
		}
	}
	else if(_pDoc->m_pActiveEffect)
	{
		float f = _pDoc->m_pActiveEffect->GetTotalLifeTime();
		if(f > m_fEffectTimelineMax)
			m_fEffectTimelineMax = f;
		tmax= m_fEffectTimelineMax;
	}
	return tmax;
}

BEGIN_MESSAGE_MAP(CDrawGraph, CWnd)
	//{{AFX_MSG_MAP(CDrawGraph)
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawGraph message handlers

UINT CDrawGraph::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}
BOOL CDrawGraph::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
void CDrawGraph::OnSize(UINT nType, int cx, int cy) 
{
	work.top = 10;
	work.left = _spacing_left;
	work.right = cx - _spacing - _spacing_left;
	work.bottom = cy - n_x_axis_bottom_space;
	_nGraphWidth = cx - _spacing - _spacing_left;
	_nGraphHeight = cy - n_x_axis_bottom_space;
	Update(false);
}
void MoveTo(CDC* pDC, float x, float y) //x,y - real values, for all: y,x>0
{
	Transform(x,y);
	pDC->MoveTo(x, y);
}
void LineTo(CDC* pDC, float x, float y)
{
	Transform(x,y);
	pDC->LineTo(x, y);
}

void TextOut(CDC* pDC, float x, float y,CString& s)
{
	Transform(x,y);
	pDC->TextOut(x,y,s);
}
void CDrawGraph::CalcTransform()
{
	float tmax;
	GRAPH_BOUND f;
	if (!_pDoc->m_pActiveEmitter)
	{
		int n = _pDoc->m_pActiveEffect->emitters.size();
		if (n<10)n = 10;
		f.max = n+1;
		f.min = 0;
		tmax = GetTimescaleMax();
	}
	else 
	{
		f.max = 0;
		f.min = 10000;
//		tmax = 0;
		ObjectVectorType::iterator i = m_objects.begin();
		bool all = false;
		FOR_EACH(m_objects, i)
		{
			CGraphLine* pp;
			if(pp = dynamic_cast<CGraphLine*>(*i))
			{
				GRAPH_BOUND _f;
				pp->GetBoundValue(_f);
				if(f.max < _f.max)
					f.max = _f.max;
				if (f.min > _f.min)
					f.min = _f.min;
			}
			if (dynamic_cast<CGraphBlueLine*>(*i))
				all = dynamic_cast<CGraphBlueLine*>(*i)->type;
		}
		tmax = GetTimescaleMax(all);
		if (f.max>200)
			f.max = 200;
		if (f.min<-200)
			f.min = -200;
	}
	if (lockScale)
	{
		if (f.max>bound.max)
			bound.max = f.max;
		if (f.min<bound.min)
			bound.min = f.min;
		if (tmax>time_max)
			time_max = tmax;
	}else
	{
		bound.max = f.max;
		bound.min = f.min;
		time_max = tmax;
	}
	yScale = work.Height()/(bound.max-bound.min);
	Y_Ox = -yScale*bound.min;
	xScale = work.Width()/time_max;
}
void CDrawGraph::DrawAxes(CDC* pDC, const CRect& rcClient)
{
	static CPen penAxis(PS_SOLID, 2, RGB(0, 0, 0));
	CalcTransform();
	GRAPH_BOUND bn = bound; 
	float tmax = time_max;
	bool showYlabel = true;
	if (!_pDoc->m_pActiveEmitter)
	{
		int n = _pDoc->m_pActiveEffect->emitters.size();
		bn.max = n+1;
		bn.min = 0;
		yScale = work.Height()/(bn.max-bn.min);
		Y_Ox = 0;
		showYlabel = false;
	}

	pDC->SelectObject(&penAxis);
	MoveTo(pDC,0, bn.min);
	LineTo(pDC,0, bn.max);
	MoveTo(pDC,0, 0);
	LineTo(pDC,tmax, 0);


	pDC->SelectStockObject(BLACK_PEN);
	pDC->SelectObject(&m_FontTick);
	pDC->SetTextAlign(TA_TOP|TA_CENTER);
	pDC->SetBkMode(TRANSPARENT);

	CString s;
	int tick_size = 6;

	float dt = 0.1f;
	if (tmax>3)	dt = 0.5f;
	if (tmax>10) dt = 1;
	if(tmax>30) dt=10;
	for(float t =0; t<=tmax; t+=dt)
	{
		MoveTo(pDC, t, 0);
		LineTo(pDC, t, tick_size/yScale);
		s.Format("%.1f", t);
		if (t)TextOut(pDC, t, 0, s);
	}
	if (showYlabel)
	{
		float dy = 0.1f;
		if (bn.max-bn.min>1) dy = 0.5f;
		if (bn.max-bn.min>5) dy = 1;
		if (bn.max-bn.min>10) dy = 2;
		if (bn.max-bn.min>20) dy = 4;
		if (bn.max-bn.min>40) dy = 8;
		if (bn.max-bn.min>80) dy = 10;
		if (bn.max-bn.min>100) dy = 20;
		if (bn.max-bn.min>200) dy = 40;
		for(float y = 0; y<=bn.max; y+=dy)
		{
			MoveTo(pDC, 0, y);
			LineTo(pDC, tick_size/xScale, y);
			s.Format("%.1f", y);
			TextOut(pDC, -2.3f*tick_size/xScale, y+5/yScale, s);
		}
		for(float y = -dy; y>=bn.min; y-=dy)
		{
			MoveTo(pDC, 0, y);
			LineTo(pDC, tick_size/xScale, y);
			s.Format("%.1f", y);
			TextOut(pDC, -2.3f*tick_size/xScale, y+5/yScale, s);
		}
	}
}

const int _nAxisTickSize = 7;
const float _fTickDelta  = 0.1f;
/*
void CDrawGraph::DrawAxisLabels(CDC* pDC, const CRect& rcClient)
{
return ;
	pDC->SelectStockObject(BLACK_PEN);
	pDC->SelectObject(&m_FontTick);
	pDC->SetTextAlign(TA_TOP|TA_CENTER);
	pDC->SetBkMode(TRANSPARENT);

	CString s;

	//x
	float fTimeTotal = GetTimescaleMax();
	float fTime = 0;

	float _fTickDeltaX = _fTickDelta;
	if(fTimeTotal >= 3.0f)
		_fTickDeltaX = 1.0f;
	if(fTimeTotal >= 35.0f)
		_fTickDeltaX = 10.0f;

	float f = fTimeTotal/_fTickDeltaX;
	int   nTickCount = round(f);
	float fDeltaX = float(_nGraphWidth)/f;
	int   nTickY1 = rcClient.Height() - n_x_axis_bottom_space;
	for(int i=0; i<nTickCount+1; i++, fTime += _fTickDeltaX)
	{
		pDC->MoveTo(_spacing_left + fDeltaX*i, nTickY1);
		pDC->LineTo(_spacing_left + fDeltaX*i, nTickY1 + _nAxisTickSize);

		s.Format("%.1f", fTime);
		pDC->TextOut(_spacing_left + fDeltaX*i, nTickY1 + _nAxisTickSize, s);
	}


	if(!_pDoc->m_pActiveEmitter)
		return;

	//y
	pDC->SetTextAlign(TA_BASELINE|TA_LEFT);
	GRAPH_BOUND bn;
	float fValueMax = GetMaxValue(&bn);

	float fTickDeltaY = _fTickDelta;
	if(fValueMax >= 2.0f)	fTickDeltaY *= 2;
	if(fValueMax >= 3.0f)	fTickDeltaY *= 2;
	if(fValueMax >= 4.0f)	fTickDeltaY *= 2;
	if(fValueMax >= 10.0f)	fTickDeltaY *= 2;
	if(fValueMax >= 20.0f)	fTickDeltaY *= 2;
	if(fValueMax >= 40.0f)	fTickDeltaY *= 2;

	nTickCount = round(fValueMax/fTickDeltaY);
	float fDeltaY = float(_nGraphHeight - _spacing)/nTickCount;
	int   nTickX1 = _spacing_left;
	nTickY1 = rcClient.Height() - n_x_axis_bottom_space;
	float fValue = 0;
	for(i=0; i<nTickCount+1; i++, fValue += fTickDeltaY)
	{
		pDC->MoveTo(nTickX1, nTickY1 - i*fDeltaY);
		pDC->LineTo(nTickX1-_nAxisTickSize,  nTickY1 - i*fDeltaY);

		s.Format("%.1f", fValue);
		pDC->TextOut(0, nTickY1 - i*fDeltaY, s);
	}
}
*/
void CDrawGraph::DrawTime(CDC* pDC, const CRect& rcClient)
{
	if(!theApp.scene.GetEffect())
		return;

	float fTime = theApp.scene.GetEffect()->GetTime()/GetTimescaleMax();

	pDC->SelectStockObject(BLACK_PEN);

	int x = _spacing_left + fTime*_nGraphWidth;
	pDC->MoveTo(x, 0);
	pDC->LineTo(x, rcClient.bottom);
}

void CDrawGraph::OnPaint() 
{
	CPaintDC dc(this);

	CRect rcWindow;
	GetClientRect(rcWindow);

	CDC mdc;
	CBitmap bmp;
	mdc.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, rcWindow.Width(), rcWindow.Height());
	mdc.SelectObject(&bmp);

	::FillRect(mdc, rcWindow, HBRUSH(::GetStockObject(WHITE_BRUSH)));

	DrawAxes(&mdc, rcWindow);
	//DrawAxisLabels(&mdc, rcWindow);

	ObjectVectorType::iterator i;
	FOR_EACH(m_objects, i)
		(*i)->Draw(&mdc);

	if(theApp.scene.bPause)	
		DrawTime(&mdc, rcWindow);

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &mdc, 0, 0, SRCCOPY);
}

#include "OptTree.h"
extern COptTree* tr;
void CDrawGraph::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ptPrevPoint = point;
	ObjectVectorType::iterator i;
	FOR_EACH(m_objects, i)
		if(m_pTrack = (*i)->HitTest(point))
		{
			m_bLockKeylinesMaxUpdate = true;
			m_bLockTimeMaxUpdate = true;
			lockScale = true;

			const type_info& _type = typeid(*m_pTrack);

			if(_type == typeid(CBlueLinePoint))
			{
				_pDoc->m_nCurrentGenerationPoint = ((CBlueLinePoint*)m_pTrack)->m_nNumber;
			}
			else if((_type == typeid(CKeyLinePoint)))
			{
				_pDoc->m_nCurrentParticlePoint = ((CKeyLinePoint*)m_pTrack)->m_nNumber;
			}
			tr->SetControlsData(false);
			break;
		}

	Invalidate();
	SetCapture();
}

void CDrawGraph::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(nFlags & MK_LBUTTON)
	{
		CRect r;
		GetClientRect(r);

		m_bLockKeylinesMaxUpdate = point.y > _spacing;
		m_bLockTimeMaxUpdate = point.x < r.right - _spacing;

		CPoint ptDelta(0,0);

		if(m_pTrack && (m_pTrack->GetState() & CGraphObject::TRACK_ALL))
		{
			if (m_pTrack->GetPos().x>=0)
				ptDelta = point - m_pTrack->GetPos();
			else
				ptDelta = point - m_ptPrevPoint;
			m_pTrack->OffsetPos(ptDelta.x, ptDelta.y);
			m_pTrack->OnChange();
			tr->CheckRangeEmitter();
			Update(false);
		}
		m_ptPrevPoint = point;
	}
}
void CDrawGraph::OnLButtonUp(UINT nFlags, CPoint pointa) 
{
	if(m_pTrack)
	{
		if(m_pTrack->GetState() & CGraphObject::TRACK_ALL)
			m_pTrack->OnChange();

		if(_pDoc->m_pActiveEmitter)
			GetParent()->SendMessage(WM_ACTIVE_POINT);
	}

	lockScale = false;
	m_bLockKeylinesMaxUpdate = false;
	m_bLockTimeMaxUpdate = false;
	m_pTrack = 0;

	Update(false);
	ReleaseCapture();
}

void CDrawGraph::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CGraphObject* pObject;

	ObjectVectorType::iterator i;
	FOR_EACH(m_objects, i)
		if(pObject = (*i)->HitTest(point))
		{
			RemoveObject(pObject);
			Update(false);

			break;
		}
}

void CDrawGraph::OnTimer(UINT nIDEvent) 
{
	if(theApp.scene.bPause)
		Invalidate();
	
	CWnd::OnTimer(nIDEvent);
}
void CDrawGraph::OnDestroy() 
{
	KillTimer(1);

	CWnd::OnDestroy();
}
