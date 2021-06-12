#include "stdafx.h"
#include "tebaseworkmodedrawer.h"

#include "TEEngine/te_consts.h"
#include "TEEngine/TriggerEditorView.h"
#include "TEEngine/te_helpers.h"
#include "TEEngine/WorkModes/TEBaseWorkMode.h"

TEBaseWorkModeDrawer::TEBaseWorkModeDrawer(TEBaseWorkMode* ptrWorkMode)
: workMode_(ptrWorkMode)
, linkOutPen_(PS_SOLID, 2, LINK_OUT_COLOR)
, linkInPen_(PS_SOLID, 2, LINK_IN_COLOR)
, otherLinksPen_(PS_SOLID, 2, OTHER_LINKS_COLOR)
, linkOutBrush_(LINK_OUT_COLOR)
, linkInBrush_(LINK_IN_COLOR)
, otherLinksBrush_(OTHER_LINKS_COLOR)
{
}

TEBaseWorkModeDrawer::~TEBaseWorkModeDrawer(void)
{
}

BOOL TEBaseWorkModeDrawer::setPenCant(HPEN hpen){
	return cantPen_.Attach(hpen);
}

HPEN TEBaseWorkModeDrawer::getPenCant() const{
	return cantPen_;
}


BOOL TEBaseWorkModeDrawer::setLinkOutPen(HPEN hpen){
	return linkOutPen_.Attach(hpen);
}
BOOL TEBaseWorkModeDrawer::setLinkInPen(HPEN hpen){
	return linkInPen_.Attach(hpen);
}
BOOL TEBaseWorkModeDrawer::setOtherLinksPen(HPEN hpen){
	return otherLinksPen_.Attach(hpen);
}

HPEN TEBaseWorkModeDrawer::getLinkOutPen() const{
	return linkOutPen_;
}
HPEN TEBaseWorkModeDrawer::getLinkInPen() const{
	return linkInPen_;
}
HPEN TEBaseWorkModeDrawer::getOtherLinksPen() const{
	return otherLinksPen_;
}

BOOL TEBaseWorkModeDrawer::setLinkOutBrush(HPEN hbrush){
	return linkOutBrush_.Attach(hbrush);
}
BOOL TEBaseWorkModeDrawer::setLinkInBrush(HPEN hbrush){
	return linkInBrush_.Attach(hbrush);
}
BOOL TEBaseWorkModeDrawer::setOtherLinksBrush(HPEN hbrush){
	return otherLinksBrush_.Attach(hbrush);
}

void TEBaseWorkModeDrawer::draw(CDC* pdc, TriggerEditorView* view, 
								IDrawData* pdd, CRect const& rcPaint) const
{
	//для прорисовки сетки
	BaseTEDrawer::draw(pdc, view, pdd, rcPaint);

	DrawingData drawData = pdd->getDrawedChains();
	for(int i = 0; i < drawData.second; ++i)
		DrawChain(view, pdc, rcPaint, *drawData.first[i]);
}

void TEBaseWorkModeDrawer::drawAnchoredLink(TriggerEditorView* view, HDC dc, 
				Trigger const& link_owner, 
				TriggerLink const& link) const
{
	CPoint beg(te_h::getLinkBegin(link_owner, link));
	CPoint end(te_h::getLinkEnd(link));

	drawLinkAnchors(view, dc, link_owner, link);

	int imode = SetROP2(dc, R2_NOTXORPEN);
	DrawLink(view, dc, beg, end,  link);
	SetROP2(dc, imode);
}

void TEBaseWorkModeDrawer::drawDragedLink(TriggerEditorView* view, HDC dc, 
							const CPoint& pt_from, const CPoint& pt_to, int type) const
{
	int iMode = SetROP2(dc, R2_NOTXORPEN);
	HGDIOBJ old_p = NULL;

	if (type == -1)
		old_p = SelectObject(dc, cantPen_);
	else
		old_p = SelectObject(dc, getLinkTypePen(type));

	CPoint p(pt_from);
	view->log2Scr(&p, 1);

	MoveToEx(dc, p.x, p.y, NULL);
	p = pt_to;
	view->log2Scr(&p, 1);
	LineTo(dc, p.x, p.y);
	SelectObject(dc, old_p);

	SetROP2(dc, iMode);
}

void TEBaseWorkModeDrawer::drawDragedTitle(TriggerEditorView* view, 
								std::string const& strTitle, CPoint pos) const
{
	HDC dc = view->getDC();
	BeginPath(dc);

	view->log2Scr(&pos, 1);

	TextOut(dc, pos.x, pos.y, strTitle.c_str(), strTitle.size());

	EndPath(dc);

	int iMode = SetROP2(dc, R2_NOT);
	StrokePath(dc);
	SetROP2(dc, iMode);
}

void TEBaseWorkModeDrawer::drawLinkAnchors(TriggerEditorView* view, HDC dc, 
				Trigger const& link_owner, TriggerLink const& link) const
{
	int iMode = SetROP2(dc, R2_NOTXORPEN);
	HGDIOBJ old = SelectObject(dc, CreateSolidBrush(ANCHOR_COLOR));

	CPoint p = te_h::getLinkBegin(link_owner, link);

	view->log2Scr(&p,1);
	CRect r(p, p);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);

	Rectangle(dc, r.left, r.top, r.right, r.bottom);

	p = te_h::getLinkEnd(link);
	view->log2Scr(&p, 1);
	r.SetRect(p.x, p.y, p.x, p.y);
	r.InflateRect(ANCHOR_SZ, ANCHOR_SZ);

	Rectangle(dc, r.left, r.top, r.right, r.bottom);

	DeleteObject(SelectObject(dc, old));
	SetROP2(dc, iMode);
}

HBRUSH	TEBaseWorkModeDrawer::SelectLinkBrushColor(TriggerLink const& link) const
{
	if (Trigger const* trigger = workMode_->getHilitedLinksTrigger())
	{
		if (getDrawedLinkOwner() == trigger||trigger == link.child)
			return TEBaseWorkModeDrawerBase::SelectLinkBrushColor(link);
		else
			return otherLinksBrush_;
	}
	else
		return TEBaseWorkModeDrawerBase::SelectLinkBrushColor(link);
}

HPEN	TEBaseWorkModeDrawer::SelectLinkPenColor(TriggerLink const& link) const
{
	if (Trigger const* trigger = workMode_->getHilitedLinksTrigger())
	{
		if (getDrawedLinkOwner() == trigger||trigger == link.child)
			return TEBaseWorkModeDrawerBase::SelectLinkPenColor(link);
		else
			return otherLinksPen_;
	}
	else
		return TEBaseWorkModeDrawerBase::SelectLinkPenColor(link);
}
