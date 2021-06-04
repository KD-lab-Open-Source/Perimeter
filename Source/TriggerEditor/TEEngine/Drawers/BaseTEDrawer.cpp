#include "stdafx.h"
#include "TEEngine/TriggerEditorView.h"
#include "BaseTEDrawer.h"
#include "TEEngine/te_consts.h"
#include "TEEngine/te_helpers.h"
#include "TEEngine/TEGrid.h"

#include "TriggerExport.h"

#include <xmath.h>

//#include "Game/GameInclude.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BaseTEDrawer::BaseTEDrawer(void):
//  strategyOffset_(0, 0)
//, 
  netPen_(PS_DOT, 1, NET_COLOR)
, autoRestartedLinkPen_(PS_SOLID, 5, AUTO_RESTART_PEN_COLOR)
, defaultTriggerBrush_(TE_NORMAL_ELE_BACK)
, selectedTriggerBrush_(TE_SELECTED_ELE_BACK)
, drawedStrategy_(NULL)
, drawedLinkOwner_(NULL)
, paintRect_(0, 0, 0, 0)
{
}

BaseTEDrawer::~BaseTEDrawer(void)
{
}

void BaseTEDrawer::setPaintRect(CRect const& rc) const{
	paintRect_ = rc;
}

CRect const& BaseTEDrawer::getPaintRect() const{
	return paintRect_;
}

BOOL BaseTEDrawer::setNetPen(int nPenStyle, int nWidth, COLORREF crColor){
	return netPen_.CreatePen(nPenStyle, nWidth, crColor);
}
BOOL BaseTEDrawer::setNetPen(HPEN hPen){
	return netPen_.Attach(hPen);
}
HPEN BaseTEDrawer::getNetPen() const{
	return netPen_;
}

BOOL BaseTEDrawer::setAutoRestartedLinkPen(HPEN hPen){
	return autoRestartedLinkPen_.Attach(hPen);
}
BOOL BaseTEDrawer::setAutoRestartedLinkPen(int nPenStyle, int nWidth, COLORREF crColor){
	return autoRestartedLinkPen_.CreatePen(nPenStyle, nWidth, crColor);
}
HPEN BaseTEDrawer::getAutoRestartedLinkPen() const{
	return autoRestartedLinkPen_;
}

void BaseTEDrawer::setDrawedChain(TriggerChain const* chain) const{
	drawedStrategy_ = chain;
}
TriggerChain const* BaseTEDrawer::getDrawedChain() const{
	return drawedStrategy_;
}

BOOL BaseTEDrawer::setDefaultTriggerBrush(HBRUSH hBrush){
	return defaultTriggerBrush_.Attach(hBrush);
}
BOOL BaseTEDrawer::setDefaultTriggerBrush(COLORREF crColor){
	return defaultTriggerBrush_.CreateSolidBrush(crColor);
}
HBRUSH BaseTEDrawer::getDefaultTriggerBrush() const{
	return defaultTriggerBrush_;
}

BOOL BaseTEDrawer::setSelectedTriggerBrush(HBRUSH hBrush){
	return selectedTriggerBrush_.Attach(hBrush);
}
BOOL BaseTEDrawer::setSelectedTriggerBrush(COLORREF crColor){
	return selectedTriggerBrush_.CreateSolidBrush(crColor);
}
HBRUSH BaseTEDrawer::getSelectedTriggerBrush() const{
	return selectedTriggerBrush_;
}

void BaseTEDrawer::setDrawedLinkOwner(Trigger const* trigger) const{
	drawedLinkOwner_ = trigger;
}
Trigger const* BaseTEDrawer::getDrawedLinkOwner() const{
	return drawedLinkOwner_;
}

void BaseTEDrawer::draw(CDC* pdc, TriggerEditorView* source,
						IDrawData* pdd, CRect const& rcPaint) const
{
	setPaintRect(rcPaint);
	drawNet(pdc, source, rcPaint);
}

void BaseTEDrawer::drawDraggedRect(HDC dc, CRect const&r, HPEN hpen) const
{
	int iMode = SetROP2(dc, R2_NOTXORPEN);
	HGDIOBJ obr = SelectObject(dc, GetStockObject(NULL_BRUSH));
	HGDIOBJ oldp = SelectObject(dc, hpen);

	Rectangle(dc, r.left, r.top, r.right, r.bottom);

	SelectObject(dc, obr);
	SelectObject(dc, oldp);
	SetROP2(dc, iMode);
}

void BaseTEDrawer::drawNet(CDC* pdc, TriggerEditorView* source, CRect const& rcPaint) const
{
	CRect r(rcPaint);

	TEGrid const& grid = source->getGrid();

	//для случая, когда LEFT становиться отрицательным
	//страхуемся и вычитаем -1, чтобы сетка рисовалась 
	//из-за левого края окна. То же для TOP
	int const union_cell_sx = grid.getFullCellWidth();
	int const union_cell_sy = grid.getFullCellHeight();
	r.left = (r.left/union_cell_sx - 1)* union_cell_sx;
	r.right = r.right/union_cell_sx * union_cell_sx + union_cell_sx;

	r.top = (r.top/union_cell_sy - 1)* union_cell_sy;
	r.bottom = r.bottom/union_cell_sy * union_cell_sy + union_cell_sy;


	HGDIOBJ hOldPen = pdc->SelectObject(netPen_);

	int i = r.left;
	CPoint p;
	while (i < r.right) {
		p.x = i;
		p.y = r.top;
		source->log2Scr(&p, 1);

		pdc->MoveTo(p);

		p.x = i;
		p.y = r.bottom;
		source->log2Scr(&p, 1);

		pdc->LineTo(p);
		i += grid.getCellWidth();

		p.x = i;
		p.y = r.top;
		source->log2Scr(&p, 1);

		pdc->MoveTo(p);

		p.x = i;
		p.y = r.bottom;
		source->log2Scr(&p, 1);

		pdc->LineTo(p);

		i += grid.getAmongCellsW();
	}

	i = r.top;
	while (i < r.bottom) {
		p.x = r.left;
		p.y = i;
		source->log2Scr(&p, 1);
		pdc->MoveTo(p);

		p.x = r.right;
		p.y = i;
		source->log2Scr(&p, 1);
		pdc->LineTo(p);

		i += grid.getCellHeight();

		p.x = r.left;
		p.y = i;
		source->log2Scr(&p, 1);
		pdc->MoveTo(p);

		p.x = r.right;
		p.y = i;
		source->log2Scr(&p, 1);
		pdc->LineTo(p);

		i += grid.getAmongCellsH();
	}

	pdc->SelectObject(hOldPen);
}

void BaseTEDrawer::DrawTrigger(TriggerEditorView* source, CDC* pdc, 
							   Trigger const& trigger) const
{
	CRect r = trigger.boundingRect();
	source->log2Scr(r);

	HGDIOBJ oldbr = NULL;
	if (trigger.selected())
		oldbr = pdc->SelectObject(selectedTriggerBrush_);
	else
		oldbr = pdc->SelectObject(defaultTriggerBrush_);

	POINT const ellipce = {6,6};
	pdc->RoundRect(&r,ellipce);

	int idef = source->scaleX(RECT_INCREASE);
	r.DeflateRect(idef, idef);

	pdc->SelectObject(SelectTriggerBackBrush(trigger));
	pdc->RoundRect(&r,ellipce);

	pdc->SetBkMode(TRANSPARENT);

	idef = source->scaleX(2);
	r.left += idef? idef : 1;
	pdc->DrawText(trigger.displayText(), -1, r,
					DT_WORD_ELLIPSIS|DT_TOP|DT_CENTER);

	pdc->SelectObject(oldbr);
}

HBRUSH BaseTEDrawer::SelectTriggerBackBrush(Trigger const& trigger) const
{
	return reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
}
HBRUSH	BaseTEDrawer::SelectLinkBrushColor(TriggerLink const& link) const
{
	return reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
}
HPEN	BaseTEDrawer::SelectLinkPenColor(TriggerLink const& link) const
{
	return reinterpret_cast<HPEN>(GetStockObject(BLACK_PEN));
}

void BaseTEDrawer::DrawLink(HDC dc, const CPoint& pt_from, const CPoint& pt_to, 
			  CPoint* ptrg, HPEN pen, HGDIOBJ brush) 
{
	HGDIOBJ old = SelectObject(dc, pen);
	MoveToEx(dc, pt_from.x, pt_from.y, NULL);
	LineTo(dc, pt_to.x, pt_to.y);
	SelectObject(dc, old);
	if (ptrg)
	{
		old = SelectObject(dc, brush);
		Polygon(dc, ptrg, 3);
		SelectObject(dc, old);
	}
}

void BaseTEDrawer::DrawLink(TriggerEditorView* source, HDC dc, CPoint pf, 
					 CPoint pt,TriggerLink const& link) const
{

	CRect rc(pt, pt);
	rc.InflateRect(15, 15);
	if (rc.IntersectRect(rc, getPaintRect()))
	{
		te_h::TRIANGLE linkTrg;
		te_h::getLinkTriangle(pf, pt, linkTrg);

		source->log2Scr(&pf, 1);
		source->log2Scr(&pt, 1);
		linkTrg[0] = pt;//присваиваем здесь, чтобы избежать повторного пересчета 
		source->log2Scr(linkTrg + 1, 2);
		if (link.autoRestarted())
			DrawLink(dc, pf, pt, linkTrg, autoRestartedLinkPen_, GetStockObject(NULL_BRUSH));

		DrawLink(dc, pf, pt, linkTrg, SelectLinkPenColor(link), SelectLinkBrushColor(link));
	}
	else
	{
		source->log2Scr(&pf, 1);
		source->log2Scr(&pt, 1);
		if (link.autoRestarted())
			DrawLink(dc, pf, pt, NULL, 
						autoRestartedLinkPen_, GetStockObject(NULL_BRUSH));

		DrawLink(dc, pf, pt, NULL, 
						SelectLinkPenColor(link), SelectLinkBrushColor(link));
	}
}

void BaseTEDrawer::DrawTriggerLinks(TriggerEditorView* source, 
									CDC* pdc, 
									RECT const& rcEle, 
									Trigger const& trigger) const
{
	CPoint center = te_h::get_center(rcEle);
	OutcomingLinksList const& children = trigger.outcomingLinks();
	setDrawedLinkOwner(&trigger);
	OutcomingLinksList::const_iterator itr = children.begin(),
												end = children.end();
	for(; itr != end; ++itr)
	{
		TriggerLink const& link = *itr;
		CPoint beg(center);
		beg.Offset(link.parentOffset());

		CPoint end(te_h::get_center(link.child->boundingRect()));
		end.Offset(link.childOffset());

		DrawLink(source, *pdc, beg, end, link);
	}
	setDrawedLinkOwner(NULL);
}

void BaseTEDrawer::DrawTriggers(TriggerEditorView* source, 
								CDC* pdc, 
								const CRect& rcPaint, 
								TriggerChain const& chain) const
{
	TriggerList const& lst = chain.triggers;
	typedef TriggerList::const_iterator Iterator;
	TEGrid const& grid = source->getGrid();
	Iterator itr = lst.begin(), end = lst.end();
	for(;itr != end; ++itr){
		Trigger const& trigger = *itr;
		CRect itrs = trigger.boundingRect();

		if (itrs.IntersectRect(&rcPaint,itrs)) 
			DrawTrigger(source, pdc, trigger);
	}
}

void BaseTEDrawer::DrawChain(TriggerEditorView* source, CDC* pdc, const CRect& rcPaint, 
					  TriggerChain const& chain) const
{
	CRect intersect;
	if (!intersect.IntersectRect(&rcPaint, &chain.boundingRect())) {
		return;
	}
	setDrawedChain(&chain);

	DrawLinks(source, pdc, chain);
	DrawTriggers(source, pdc, intersect, chain);
	setDrawedChain(NULL);
}

void BaseTEDrawer::DrawLinks(TriggerEditorView* source, 
							 CDC* pdc, 
							 TriggerChain const& chain) const
{
	TriggerList const& lst = chain.triggers;

	typedef TriggerList::const_iterator BaseIterator;
	typedef BaseIterator Iterator;
	Iterator itr = lst.begin(), end = lst.end();
	for(;itr != end; ++itr)
	{
		Trigger const& trigger = *itr;
		CRect rc(trigger.boundingRect());
		DrawTriggerLinks(source, pdc, rc, trigger);
	}
}