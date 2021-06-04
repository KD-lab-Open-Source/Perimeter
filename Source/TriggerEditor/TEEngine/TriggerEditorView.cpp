// TriggerEditorView.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "TriggerEditorView.h"
#include "TriggerEditorLogic.h"

#include "Custom Controls/ToolTip.h"

#include <Additional/memorydc.h>

#include <cfloat>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TriggerEditorView
int const RECT_INCREASE = 4;
float const SCALE_STEP = 0.85f;

IMPLEMENT_DYNCREATE(TriggerEditorView, PARENT)

TriggerEditorView::TriggerEditorView():
	  scaleManager_(NULL)
	, fScale_(1.f)
	, fMinScale_(1.f)
	, fMaxScale_(1.f)
	, iMinScrollLimX_(0)
	, iMaxScrollLimX_(0)
	, iMaxScrollLimY_(0)
	, iMinScrollLimY_(0)
	, font_(NULL)
	, triggerEditorLogic_(NULL)
	, workAreaRect_(0, 0, 0, 0)
{
}

TriggerEditorView::~TriggerEditorView()
{
}

TriggerEditorLogic* TriggerEditorView::getTriggerEditorLogic() const{
	return triggerEditorLogic_;
}
void TriggerEditorView::setTriggerEditorLogic(TriggerEditorLogic* ptel){
	triggerEditorLogic_ = ptel;
}

CRect const& TriggerEditorView::getWorkArea() const{
	return workAreaRect_;
}

int TriggerEditorView::getWorkAreaHorzAdd() const{
	TEGrid const& grid = getGrid();
	return ((grid.getCellWidth()<<1) + (grid.getAmongCellsW()<<1));
}
int TriggerEditorView::getWorkAreaVertAdd() const{
	TEGrid const& grid = getGrid();
	return ((grid.getCellHeight()<<1) + (grid.getAmongCellsH()<<1));
}

ToolTip const* TriggerEditorView::getToolTip() const{
	return toolTip_.get();
}

TEGrid& TriggerEditorView::getGrid(){
	ASSERT(getTriggerEditorLogic());
	return getTriggerEditorLogic()->getGrid();
}

TEGrid const& TriggerEditorView::getGrid() const{
	ASSERT(getTriggerEditorLogic());
	return getTriggerEditorLogic()->getGrid();
}

HDC TriggerEditorView::getDC() const{
	return deviceContext_;
}

ITEWorkMode* TriggerEditorView::getCurrentWorkMode() const{
	return triggerEditorLogic_->getCurrentWorkMode();
}

CPoint const& TriggerEditorView::getViewportOrg() const{
	return viewportOrg_;
}
void TriggerEditorView::setViewportOrg(int x, int y){
	viewportOrg_.x = x;
	viewportOrg_.y = y;
}
void TriggerEditorView::setViewportOrg(CPoint const& point){
	viewportOrg_ = point;
}


BEGIN_MESSAGE_MAP(TriggerEditorView, PARENT)
	//{{AFX_MSG_MAP(TriggerEditorView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_NOTIFY_EX(TTN_GETDISPINFO, 0, OnTTNGetDispInfo)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TriggerEditorView drawing

void TriggerEditorView::OnDraw(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);
	paintRect(rc);
}

void TriggerEditorView::paintRect(CRect const& r)
{
	SelectClipRgn(deviceContext_, NULL);

	CDC* pdc = &deviceContext_;
	axl::CMemoryDC __mem_dc__(pdc,&CSize(r.Width(), r.Height()));


	CDC* pMemDC = __mem_dc__.GetDC();

	pMemDC->SetViewportOrg(pdc->GetViewportOrg());

	pMemDC->FillSolidRect(&r,RGB(255, 255, 255));

	HGDIOBJ holdf = pMemDC->SelectObject(font_.get());

	CRect rc = r;
	CRect rc2(r);
	scr2Log(rc);

	getCurrentWorkMode()->OnPaint(this, pMemDC, rc);
	pMemDC->SelectObject(holdf);

	BitBlt(deviceContext_, rc2.left, rc2.top, rc2.Width(), rc2.Height(), 
		pMemDC->m_hDC, rc2.left, rc2.top, SRCCOPY);

}


/////////////////////////////////////////////////////////////////////////////
// TriggerEditorView diagnostics

#ifdef _DEBUG
void TriggerEditorView::AssertValid() const
{
	PARENT::AssertValid();
}

void TriggerEditorView::Dump(CDumpContext& dc) const
{
	PARENT::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// TriggerEditorView message handlers

void TriggerEditorView::OnInitialUpdate() 
{
	PARENT::OnInitialUpdate();
	
//	CSize sizeTotal;
//	sizeTotal.cx = sizeTotal.cy = 100;
//	SetScrollSizes(MM_TEXT, sizeTotal);
	createFont();
}

void TriggerEditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	
}

void TriggerEditorView::initScale()
{
	fMinScale_ = fMaxScale_ = 1.f;
	fScale_ = 1.f;
}

void TriggerEditorView::initViewportOrg()
{
	viewportOrg_ = CPoint(0, 0);
}

void TriggerEditorView::createFont(){
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));

	lf.lfHeight = scaleY(getGrid().getCellHeight() - (RECT_INCREASE<<1));
	_tcscpy(lf.lfFaceName, _T("Arial Cyr"));

	font_.reset(CreateFontIndirect(&lf));
	SelectObject(deviceContext_, getFont());
}


void TriggerEditorView::SetScaleMgr(IScaleMgr* psmgr)
{
	scaleManager_ = psmgr;
}

void TriggerEditorView::IncreaseScale()
{
	setScale(SA_PLUS);
	Invalidate();
}

void TriggerEditorView::DecreaseScale()
{
	setScale(SA_MINUS);
	Invalidate();
}

float TriggerEditorView::CurrentScale()
{
	return fScale_;
}

void TriggerEditorView::SetScale(float fscale)
{
	setScale(fScale_, fscale);
	Invalidate();
}

void TriggerEditorView::updateScaleInfo()
{
	if (scaleManager_)
		scaleManager_->UpdateScaleInfo();
}

void TriggerEditorView::setScale(float fOldScale, float fNewScale)
{
	if (fNewScale > fMaxScale_)
		fNewScale = fMaxScale_;
	else if (fNewScale < fMinScale_)
		fNewScale = fMinScale_;
	if (fOldScale == fNewScale)
		return;

	//левый верхний угол окна до изменения масштаба
	CPoint ptViewportOrg(viewportOrg_);//в логических координатах
	descalePoint(&ptViewportOrg);//перевели в логические без учета сдвига

	//получили реальную точку,
	//которая соответсвует углу
	ptViewportOrg = -ptViewportOrg;

	CRect rectClient;
	GetClientRect(rectClient);

	//не подвижная точка это центр окна
	CPoint ptConst(rectClient.CenterPoint());
	scr2Log(&ptConst,1);//центр в лог. координатах
	setViewportOrg(0, 0);
	scr2Log(rectClient);

	CRect const& workArea = getWorkArea();
	if(workArea.Width() <= rectClient.Width())
	{
		//если изображение полностью влазит в окно, 
		//то просто ставим его в центер
		ptViewportOrg.x = (rectClient.left + rectClient.right - 
			workArea.left - workArea.right)>>1;
	}
	else
	{
		//иначе стягиваем(расстягиваем) к(от) неподвижной точки
		ptViewportOrg.x = ptConst.x - 
			round((ptConst.x - ptViewportOrg.x)*fOldScale/fNewScale);
	}
	if(workArea.Height() <= rectClient.Height())
	{
		ptViewportOrg.y = (rectClient.top + rectClient.bottom - 
			workArea.top - workArea.bottom)>>1;
	}
	else
	{
		ptViewportOrg.y = ptConst.y - 
			round((ptConst.y - ptViewportOrg.y)*fOldScale/fNewScale);

	}

	fScale_ = fNewScale;

	scalePoint(&ptViewportOrg);
	setViewportOrg(-ptViewportOrg);

	//пересозадем фонт с нужной нам высотой
	createFont();

	setScrollRanges();

	updateScrollers();
	updateScaleInfo();
	getTriggerEditorLogic()->updateChainViewRect(
		*getTriggerEditorLogic()->getTriggerChain());
}

void TriggerEditorView::setScale(SCALE_ACTION action)
{
	float const fOldScale = fScale_;
	float fNewScale = fOldScale;
	switch(action) {
	case SA_PLUS:
		fNewScale = fNewScale/SCALE_STEP;
		break;
	case SA_MINUS:
		fNewScale = fNewScale*SCALE_STEP;
		break;
	case SA_RESET:
		break;
	}
	setScale(fOldScale, fNewScale);
}

void TriggerEditorView::setScaleRanges()
{
	CRect rcClient;
	GetClientRect(rcClient);


	float fH = static_cast<float>(rcClient.Height())/
		static_cast<float>(getWorkArea().Height());
	float fW = static_cast<float>(rcClient.Width())/
		static_cast<float>(getWorkArea().Width());

	fMinScale_ = 0.3f;
	fMaxScale_ = 5.f;

	if(!_finite(fMinScale_))
		fMinScale_ = 0.f;

	if(fMaxScale_ < fMinScale_)
		fMinScale_ = fMaxScale_;
}

void TriggerEditorView::setScrollRanges()
{
	CRect rectClient;
	GetClientRect(rectClient);

	CPoint bak = getViewportOrg();
	setViewportOrg(0, 0);
	CRect scaledTR(getWorkArea());
	log2Scr(reinterpret_cast<POINT*>(&scaledTR), 2);
	setViewportOrg(bak);

	if (rectClient.Width() < scaledTR.Width()) {
		iMaxScrollLimX_ = -scaledTR.left;
		iMinScrollLimX_ = rectClient.Width() - scaledTR.right;
	}
	else
		iMinScrollLimX_ = iMaxScrollLimX_ = 
			(rectClient.left + rectClient.right - scaledTR.left - scaledTR.right)>>1;

	if (rectClient.Height() < scaledTR.Height()) {
		iMaxScrollLimY_ = -scaledTR.top;
		iMinScrollLimY_ = rectClient.Height() - scaledTR.bottom;
	}
	else
		iMinScrollLimY_ = iMaxScrollLimY_ = 
			(rectClient.top + rectClient.bottom - scaledTR.top - scaledTR.bottom)>>1;

	if (viewportOrg_.x > iMaxScrollLimX_) {
		viewportOrg_.x = iMaxScrollLimX_;
	}
	if (viewportOrg_.y > iMaxScrollLimY_) {
		viewportOrg_.y = iMaxScrollLimY_;
	}
	if (viewportOrg_.x < iMinScrollLimX_) {
		viewportOrg_.x = iMinScrollLimX_;
	}
	if (viewportOrg_.y < iMinScrollLimY_) {
		viewportOrg_.y = iMinScrollLimY_;
	}
}

void TriggerEditorView::updateScrollers()
{
	setHorzScroller();
	setVertScroller();
}

void TriggerEditorView::setHorzScroller()
{
	CPoint p(viewportOrg_);
	CRect rc;
	GetClientRect(rc);

	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if (iMinScrollLimX_ != iMaxScrollLimX_)
	{
		si.nMax = iMaxScrollLimX_ - iMinScrollLimX_ + rc.Width();
		si.nPos = iMaxScrollLimX_ - p.x;
		si.nPage = rc.Width();
	}
	SetScrollInfo(SB_HORZ, &si);
}

void TriggerEditorView::setVertScroller()
{
	CPoint p(viewportOrg_);
	CRect rc;
	GetClientRect(rc);

	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	if (iMinScrollLimY_ != iMaxScrollLimY_)
	{
		si.nMax = iMaxScrollLimY_ - iMinScrollLimY_ + rc.Height();
		si.nPos = iMaxScrollLimY_ - p.y;
		si.nPage = rc.Height();
	}
	SetScrollInfo(SB_VERT, &si);
}

void TriggerEditorView::ensureVisibleRect(const CRect& r){
	CRect rcClient;
	GetClientRect(rcClient);

	float kW = static_cast<float>(rcClient.Width()) /r.Width();
	float kH = static_cast<float>(rcClient.Height())/r.Height();

	setScale(fScale_, (std::min)(kW, kH));
	CRect scaled_r(r);
	log2Scr(scaled_r);
	setViewportOrg(-(scaled_r.left - viewportOrg_.x), -(scaled_r.top - viewportOrg_.y));
	while (extendWorkArea());
	
	//перевыставляем скроллеры, потому что изменили
	//viewportOrg_
	updateScrollers();
}

void TriggerEditorView::scrollView(int dx, int dy)
{
	offsetViewportOrg(dx, dy);
	getTriggerEditorLogic()->updateChainViewRect(
			*getTriggerEditorLogic()->getTriggerChain());
	RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void TriggerEditorView::offsetViewportOrg(int dx, int dy)
{
	CPoint &org = viewportOrg_;
	
	org.Offset(dx, dy);
	
	if (org.x > iMaxScrollLimX_) 
		org.x = iMaxScrollLimX_;

	if (org.y > iMaxScrollLimY_) 
		org.y = iMaxScrollLimY_;
	
	if (org.x < iMinScrollLimX_) 
		org.x = iMinScrollLimX_;

	if (org.y < iMinScrollLimY_) 
		org.y = iMinScrollLimY_;
}


DWORD TriggerEditorView::pointInScrollArea(CPoint const& p)
{
	const int OFFSET = 12;
	CRect rcClt;
	GetClientRect(rcClt);

	scr2Log(&rcClt);
	
	DWORD dwRes = 0;
	
	if (p.x <= rcClt.left + OFFSET)//левый край
		dwRes |= SAP_LEFT;
	else if (p.x >= rcClt.right - OFFSET)//правый край
		dwRes |= SAP_RIGHT;
	
	if (p.y <= rcClt.top + OFFSET)//верхний
		dwRes |= SAP_TOP;
	else if (p.y >= rcClt.bottom - OFFSET)//нижний 
		dwRes |= SAP_BOTTOM;
	return dwRes;
}

int TriggerEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (PARENT::OnCreate(lpCreateStruct) == -1)
		return -1;

	toolTip_.reset(new ToolTip);
	toolTip_->create(m_hWnd);

	dropTarget_.Register(this);	

	deviceContext_.Attach( ::GetDC(m_hWnd));
	initScale();
	initViewportOrg();
	return 0;
}

void TriggerEditorView::OnDestroy() 
{
	::ReleaseDC(m_hWnd, deviceContext_.Detach());	
	PARENT::OnDestroy();
}

BOOL TriggerEditorView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void TriggerEditorView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	PARENT::OnShowWindow(bShow, nStatus);
	
	if (getTriggerEditorLogic())
		getTriggerEditorLogic()->OnShowWindow(this, bShow, nStatus);
}

BOOL TriggerEditorView::OnCmdMsg(UINT nID, int nCode, 
								 void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
		
	BOOL res = PARENT::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
	if (!res)
	{
		switch(nCode) {
		case CN_COMMAND:
			{
				if (getTriggerEditorLogic()
					&&getTriggerEditorLogic()->onCommand(this, nID, 0))
					return TRUE;
				else
					return FALSE;
			}
			break;
		case CN_UPDATE_COMMAND_UI:
			if (getTriggerEditorLogic()
			&&getTriggerEditorLogic()->onUpdateCommandUI(this,
					reinterpret_cast<CCmdUI*>(pExtra)))
				return TRUE;
			else
				return FALSE;
			break;
		}
	}
	return res;
}

BOOL TriggerEditorView::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	
	if (getTriggerEditorLogic()
		&&getTriggerEditorLogic()->onCommand(this, wParam, lParam))
		return TRUE;
	return PARENT::OnCommand(wParam, lParam);
}


void TriggerEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	getTriggerEditorLogic()->onKeyDown(this, nChar, nRepCnt, nFlags);
	PARENT::OnKeyDown(nChar, nRepCnt, nFlags);
}

void TriggerEditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	getTriggerEditorLogic()->onKeyUp(this, nChar, nRepCnt, nFlags);
	PARENT::OnKeyUp(nChar, nRepCnt, nFlags);
}

void TriggerEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	getTriggerEditorLogic()->onLButtonDown(this, nFlags, point);
	PARENT::OnLButtonDown(nFlags, point);
}

void TriggerEditorView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	PARENT::OnLButtonDblClk(nFlags, point);
	getTriggerEditorLogic()->onLButtonDblClk(this, nFlags, point);
}

void TriggerEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	getTriggerEditorLogic()->onLButtonUp(this, nFlags, point);
	PARENT::OnLButtonUp(nFlags, point);
}

void TriggerEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this)
		SetFocus();
	getTriggerEditorLogic()->onMouseMove(this, nFlags, point);
	PARENT::OnMouseMove(nFlags, point);
}

void TriggerEditorView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (getTriggerEditorLogic())
		getTriggerEditorLogic()->onRButtonDown(this, nFlags, point);
	PARENT::OnRButtonDown(nFlags, point);
}

void TriggerEditorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (getTriggerEditorLogic())
		getTriggerEditorLogic()->onRButtonUp(this, nFlags, point);
	
	PARENT::OnRButtonUp(nFlags, point);
}

BOOL TriggerEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (!getTriggerEditorLogic()->onSetCursor(this, pWnd, nHitTest, message))
		return PARENT::OnSetCursor(pWnd, nHitTest, message);
	return TRUE;
}

void TriggerEditorView::OnSize(UINT nType, int cx, int cy) 
{
	updateViewWorkArea();
	PARENT::OnSize(nType, cx, cy);
}

DROPEFFECT TriggerEditorView::OnDragOver(COleDataObject* pData, 
										 DWORD dwKeyState, CPoint point) 
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	
	if (pData->IsDataAvailable(uiId))
	{
		HGLOBAL hData = pData->GetGlobalData(uiId);
		void* pdata = GlobalLock(hData);
		GlobalUnlock(hData);

		return getTriggerEditorLogic()->onDragOver(this, pdata, dwKeyState, point);
	}
	return DROPEFFECT_NONE;
}

BOOL TriggerEditorView::OnDrop(COleDataObject* pData, 
							   DROPEFFECT dropEffect, CPoint point) 
{
	UINT uiDataFormat = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	ASSERT (pData->IsDataAvailable(uiDataFormat)); 
	
	HGLOBAL hData = pData->GetGlobalData(uiDataFormat);
	void* pdata = GlobalLock(hData);
	GlobalUnlock(hData);
	
	return getTriggerEditorLogic()->onDropData(this, pdata, dropEffect, point);
}

DROPEFFECT TriggerEditorView::OnDragEnter(COleDataObject* pData, 
										  DWORD dwKeyState, CPoint point) 
{
	UINT uiId = RegisterClipboardFormat(CString((LPTSTR)IDS_CB_FORMAT_NAME));
	
	if (pData->IsDataAvailable(uiId))
	{
		HGLOBAL hData = pData->GetGlobalData(uiId);
		void* pdata = GlobalLock(hData);
		GlobalUnlock(hData);

		return getTriggerEditorLogic()->onDragOver(this, pdata, dwKeyState, point);
	}
	return DROPEFFECT_NONE;
}

bool TriggerEditorView::extendWorkArea()
{
	bool res = false;

	CPoint const& viewportOrg = getViewportOrg();
	if (viewportOrg.x >= iMaxScrollLimX_) {
		extendWAToLeft(descaleX(viewportOrg.x - iMaxScrollLimX_));
		res = true;
	}
	if (viewportOrg.y >= iMaxScrollLimY_) {
		extendWAToTop(descaleY(viewportOrg.y - iMaxScrollLimY_));
		res = true;
	}
	if (viewportOrg.x <=  iMinScrollLimX_) {
		extendWAToRight(descaleX(iMinScrollLimX_ - viewportOrg.x));
		res = true;
	}
	if (viewportOrg.y <=  iMinScrollLimY_) {
		extendWAToBottom(descaleY(iMinScrollLimY_ - viewportOrg.y));
		res = true;
	}
	return res;
}

void TriggerEditorView::extendWAToLeft(int ex)
{

	const int offset = ex ? ex : getWorkAreaHorzAdd();
	workAreaRect_.left -= offset;

	TriggerEditorLogic& logic = *getTriggerEditorLogic();
	CSize sz(offset, 0);
	logic.updateChainViewRect(*logic.getTriggerChain());

	setScrollRanges();
	updateScrollers();
}

void TriggerEditorView::extendWAToRight(int ex)
{
	workAreaRect_.right += ex ? ex : getWorkAreaHorzAdd();
	TriggerEditorLogic& logic = *getTriggerEditorLogic();

	logic.updateChainViewRect(*logic.getTriggerChain());
	setScrollRanges();
	updateScrollers();
}

void TriggerEditorView::extendWAToTop(int ex)
{
	const int offset = ex ? ex : getWorkAreaVertAdd();
	workAreaRect_.top -= offset;

	CSize sz(0, offset);

	TriggerEditorLogic& logic = *getTriggerEditorLogic();
	TriggerChain& chain = *logic.getTriggerChain();
	logic.updateChainViewRect(chain);

	setScrollRanges();
	updateScrollers();
}

void TriggerEditorView::extendWAToBottom(int ex)
{
	workAreaRect_.bottom += ex ? ex : getWorkAreaVertAdd();

	TriggerEditorLogic& logic = *getTriggerEditorLogic();

	logic.updateChainViewRect(*logic.getTriggerChain());
	setScrollRanges();
	updateScrollers();
}

void TriggerEditorView::updateViewWorkArea()
{
	TriggerEditorLogic const* const logic = getTriggerEditorLogic();
	if (logic&&logic->getTriggerChain()) 
	{
		GetClientRect(workAreaRect_);
		scr2Log(workAreaRect_);

		TriggerChain& chain = *logic->getTriggerChain();
		workAreaRect_.UnionRect(workAreaRect_, &chain.boundingRect());
		workAreaRect_.InflateRect(getGrid().getFullCellWidth()<<1, 
						getGrid().getFullCellHeight()<<1);

	}
	else
	{
		GetClientRect(workAreaRect_);
		scr2Log(workAreaRect_);
	}
	setScaleRanges();
	setScrollRanges();
	updateScrollers();
}

void TriggerEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	if (GetScrollInfo(SB_HORZ, &si, SIF_ALL)) {
		int dx = 0;
		
		switch(nSBCode) {
		case SB_LINELEFT:
			dx = 1;
			si.nPos -= 1;
			break;
		case SB_LINERIGHT:
			dx = -1;
			si.nPos += 1;
			break;
		case SB_PAGERIGHT:
			dx = si.nPage;
			dx = -dx;
			si.nPos += si.nPage;
			break;
		case SB_PAGELEFT:
			dx = si.nPage;
			si.nPos -= si.nPage;
			break;
		case SB_THUMBTRACK:
			dx = si.nPos - nPos;
			si.nPos = nPos;
			break;
		default:
			return;
		}
		SetScrollInfo(SB_HORZ, &si);
		scrollView(dx, 0);
//		ExtendWorkArea();
	}
	
	PARENT::OnHScroll(nSBCode, nPos, pScrollBar);
}

void TriggerEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SCROLLINFO si;
	if (GetScrollInfo(SB_VERT, &si, SIF_ALL)) {
		int dx = 0;
		
		switch(nSBCode) {
		case SB_LINEUP:
			dx = 1;
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			dx = -1;
			si.nPos += 1;
			break;
		case SB_PAGEDOWN:
			dx = si.nPage;
			dx = -dx;
			si.nPos += si.nPage;
			break;
		case SB_PAGEUP:
			dx = si.nPage;
			si.nPos -= si.nPage;
			break;
		case SB_THUMBTRACK:
			dx = si.nPos - nPos;
			si.nPos = nPos;
			break;
		default:
			return;
		}
		SetScrollInfo(SB_VERT, &si);
		scrollView(0, dx);
//		ExtendWorkArea();
	}
	
	PARENT::OnVScroll(nSBCode, nPos, pScrollBar);
}

void TriggerEditorView::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnDrawItem(nIDCtl, lpdis);
	PARENT::OnDrawItem(nIDCtl, lpdis);
}

void TriggerEditorView::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis)
{
	if (getCurrentWorkMode())
		getCurrentWorkMode()->OnMeasureItem(nIDCtl, lpmis);
	PARENT::OnMeasureItem(nIDCtl, lpmis);
}

void TriggerEditorView::redraw() {
	RedrawWindow();
}

BOOL TriggerEditorView::OnTTNGetDispInfo(UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	if (reinterpret_cast<HWND>(pTTTStruct->idFrom) != *this)
		return FALSE;

	if (getToolTip())
		getToolTip()->OnTTNGetDispInfo(id, pTTTStruct, pResult);
	return TRUE;
}

int TriggerEditorView::OnMouseWheel(UINT fFlags, short zDelta, CPoint point)
{
	if (zDelta > 0)
		setScale(SA_PLUS);
	else
		setScale(SA_MINUS);
	Invalidate();
	return PARENT::OnMouseWheel(fFlags, zDelta, point);
}
