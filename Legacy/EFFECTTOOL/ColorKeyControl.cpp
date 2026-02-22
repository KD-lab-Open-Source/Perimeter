// ColorKeyControl.cpp : implementation file
//

#include "StdAfx.h"
#include "EffectTool.h"
#include "ColorKeyControl.h"
#include "AlphaEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorKeyControl

CColorKeyControl::CColorKeyControl()
{
	m_pKey = 0;
	m_pKeyTrack = 0;
	m_bAlpha = false;
	m_Valid = false;
}
CColorKeyControl::~CColorKeyControl()
{
}

void CColorKeyControl::SetKey(CKeyColor* key, bool bAlpha)
{
	m_pKey = key;
	m_bAlpha = bAlpha;
	Invalidate();
}

float CColorKeyControl::x2t(int x)
{
	CRect rc;
	GetClientRect(rc);

	return float(x)/rc.Width();
}
int CColorKeyControl::t2x(float f)
{
	CRect rc;
	GetClientRect(rc);

	return rc.Width()*f;
}

const float _fColorKeyTimeEps = 0.01f;
KeyColor* CColorKeyControl::FindKeyAtPos(int x)
{
	if(!m_pKey)
		return 0;

	float t = x2t(x);

	KeyColor* pKey = 0;

	CKeyColor::iterator it =  m_pKey->begin();
	FOR_EACH(*m_pKey, it)
		if(fabs(it->time - t) < _fColorKeyTimeEps)
		{
			pKey = &(*it);
			break;
		}
	return pKey;
}
KeyColor* CColorKeyControl::AddNewKey(int x)
{
	if(!m_pKey)
		return 0;

	float t = x2t(x);
	if(t < 0.05f)
		t = 0.05f;
	if(t > 0.95f)
		t = 0.95f;

	CKeyColor::iterator it1 =  m_pKey->begin();
	CKeyColor::iterator it2 = it1 + 1;
	while(it1 < m_pKey->end())
	{
		ASSERT(it2 != m_pKey->end());

		if(it1->time <= t && it2->time >= t)
		{
			CKeyColor::iterator it_new = m_pKey->insert(it2);

			it_new->r = it_new->g = it_new->b = it_new->a = 1;
			it_new->time = t;

			return &(*it_new);
		}
		it1++;
		it2++;
	}

	return 0;
}
void CColorKeyControl::DeleteKeyAtPos(int x)
{
	if(!m_pKey)
		return;

	float t = x2t(x);

	CKeyColor::iterator it;
	FOR_EACH(*m_pKey, it)
	{
//		bool b1 = (it==m_pKey->begin());
//		bool b2 = (it==m_pKey->end()-1);
		if((fabs(it->time - t) < _fColorKeyTimeEps)&&
			!(it==m_pKey->begin()||it==(m_pKey->end()-1)))
		{
			m_pKey->erase(it);
			break;
		}
	}
}


BEGIN_MESSAGE_MAP(CColorKeyControl, CWnd)
	//{{AFX_MSG_MAP(CColorKeyControl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_NCHITTEST()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CColorKeyControl message handlers

UINT CColorKeyControl::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

const int _nKeyRectWidth = 2;

static void DrawKeyPosition(CDC* pDC, const CRect& rcWnd, float t, bool bKeyIsBlack)
{
	CRect r = rcWnd;

	int nPos = rcWnd.Width()*t;

	r.left = nPos - _nKeyRectWidth;
	r.right = nPos + _nKeyRectWidth;

	if(r.left < rcWnd.left)
		r.OffsetRect(rcWnd.left - r.left, 0);
	if(r.right > rcWnd.right)
		r.OffsetRect(rcWnd.right - r.right, 0); 

	pDC->SelectStockObject(bKeyIsBlack ? WHITE_PEN : BLACK_PEN);
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Rectangle(r);
}

static void Fill(CDC* pDC, const CRect& rc, const sColor4f& c1, const sColor4f& c2, bool bAlpha)
{
	BYTE r1 = c1.r*255;
	BYTE r2 = c2.r*255;

	BYTE g1 = c1.g*255;
	BYTE g2 = c2.g*255;

	BYTE b1 = c1.b*255;
	BYTE b2 = c2.b*255;

	BYTE a1 = c1.a*255;
	BYTE a2 = c2.a*255;

	CRect r = rc;
	for(int i = rc.left; i < rc.right; i++)
	{
		float f = float(i - rc.left)/rc.Width();

		COLORREF rgb;

		if(bAlpha)
			rgb = RGB(
			BYTE(a1*(1-f) + a2*f), 
			BYTE(a1*(1-f) + a2*f), 
			BYTE(a1*(1-f) + a2*f)); 
		else
			rgb = RGB(
			BYTE(r1*(1-f) + r2*f), 
			BYTE(g1*(1-f) + g2*f), 
			BYTE(b1*(1-f) + b2*f)); 

		r.left = i; r.right = i+1;
		pDC->FillSolidRect(r, rgb);
	}

	//::GradientFill(*pDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
}
void CColorKeyControl::OnPaint() 
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

	if(!m_Valid)
		return;

	CRect rc = rcWindow;

	CKeyColor::iterator it1 =  m_pKey->begin();

	if(!m_Valid||!m_pKey||!m_pKey->size())
		return;

	if(!it1)
		return;

	CKeyColor::iterator it2 = it1 + 1;
	while(it2 != m_pKey->end())
	{
		rc.left = t2x(it1->time);
		rc.right = t2x(it2->time);

		Fill(&mdc, rc, *it1, *it2, m_bAlpha);

		it1++; it2++;
	}

	FOR_EACH(*m_pKey, it1)
	{
		bool bIsBlack;

		float mn=0.5f;
		if(m_bAlpha)
			bIsBlack = it1->a < mn;
		else
		{
			float f=(it1->r+it1->g+it1->b)/3;
			bIsBlack = f < mn;
		}
		DrawKeyPosition(&mdc, rcWindow, it1->time, bIsBlack);
	}

	dc.BitBlt(0, 0, rcWindow.Width(), rcWindow.Height(), &mdc, 0, 0, SRCCOPY);
}

void CColorKeyControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(!m_pKey||!m_Valid)
		return;

	SetCapture();

	KeyColor* pKey = FindKeyAtPos(point.x);

	if(!pKey)
	{
		pKey = AddNewKey(point.x);

		Invalidate();
	}

	if(pKey->time != 0 && pKey->time != 1.0f)
		m_pKeyTrack = pKey;
}
void CColorKeyControl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	DeleteKeyAtPos(point.x);
	Invalidate();
}
void CColorKeyControl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_pKeyTrack = 0;

	ReleaseCapture();

	theApp.scene.InitEmitters();
}
void CColorKeyControl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_pKeyTrack)
	{
		m_pKeyTrack->time = x2t(point.x);

		if(m_pKeyTrack->time < 0.05f)
			m_pKeyTrack->time = 0.05f;
		if(m_pKeyTrack->time > 0.95f)
			m_pKeyTrack->time = 0.95f;

		Invalidate();
	}
}
void CColorKeyControl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(!m_Valid)
		return;

	KeyColor* pKey = FindKeyAtPos(point.x);
	if(pKey)
	{
		if(m_bAlpha)
		{
			CAlphaEditDlg dlg;
			dlg.m_alpha = 255*pKey->a;

			if(dlg.DoModal() == IDOK)
				pKey->a = float(dlg.m_alpha)/255.0f;
		}
		else
		{
			CColorDialog dlg(RGB(255*pKey->r, 255*pKey->g, 255*pKey->b), CC_FULLOPEN);

			if(dlg.DoModal() == IDOK)
			{
				COLORREF rgb = dlg.GetColor();

				pKey->r = float(GetRValue(rgb))/255.0f;
				pKey->g = float(GetGValue(rgb))/255.0f;
				pKey->b = float(GetBValue(rgb))/255.0f;
			}
		}

		theApp.scene.InitEmitters();
		Invalidate();
	}
}

BOOL CColorKeyControl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
