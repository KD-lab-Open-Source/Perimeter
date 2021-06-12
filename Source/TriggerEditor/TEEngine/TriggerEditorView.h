#if !defined(AFX_TRIGGEREDITORVIEW_H__1A772501_7E52_4E41_90E2_5E6611EC9854__INCLUDED_)
#define AFX_TRIGGEREDITORVIEW_H__1A772501_7E52_4E41_90E2_5E6611EC9854__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TriggerEditorView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TriggerEditorView view
#include "Scale/ScaleInterfaces.h"
#include "gdi_resource.h"
#include <boost/scoped_ptr.hpp>
#include "ITriggerEditorView.h"

class TriggerEditorLogic;
class ToolTip;
class TEGrid;
interface ITEWorkMode;

#define PARENT CView
class TriggerEditorView : public PARENT
						, public IScalable
						, public ITriggerEditorView
{
	typedef gdi_resource_guard<HFONT> Font;


protected:
	TriggerEditorView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(TriggerEditorView)

// Attributes
public:
	using PARENT::GetCurrentMessage;
	enum SCALE_ACTION{SA_MINUS, SA_PLUS, SA_RESET};
	//! Scroll Area Place
	enum {
		SAP_LEFT	= 1,
		SAP_RIGHT	= 2,
		SAP_TOP		= 4,
		SAP_BOTTOM	= 16,
	};

	virtual void SetScaleMgr(IScaleMgr* psmgr);
	virtual void IncreaseScale();
	virtual void DecreaseScale();
	virtual	float CurrentScale();
	virtual void SetScale(float fscale);

	virtual void redraw();

	TriggerEditorLogic* getTriggerEditorLogic() const;
	void setTriggerEditorLogic(TriggerEditorLogic* ptel);

	void updateScrollers();
	void scrollView(int dx, int dy);
	bool extendWorkArea();
	//! Обновляет информацию о рабочей области
	void updateViewWorkArea();
	//! Левый верхний угол r совмещает с левым верхним углом окна
	void ensureVisibleRect(CRect const& r);

	ToolTip const* getToolTip() const;
	
	TEGrid const& getGrid() const;
	TEGrid& getGrid();

	HDC getDC() const;

	//! определяем находится ли точка у краев окна
	DWORD pointInScrollArea(CPoint const& p);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TriggerEditorView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL


// Implementation
protected:
	virtual ~TriggerEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(TriggerEditorView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);
	BOOL OnTTNGetDispInfo(UINT id, NMHDR * pTTTStruct, LRESULT * pResult );
	afx_msg int OnMouseWheel(UINT fFlags, short zDelta, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();
protected:
	void offsetViewportOrg(int dx, int dy);

	bool testForBetweenPlots(CPoint p);

	void setScale(float fOldScale, float fNewScale);
	void setScale(SCALE_ACTION action);

	void setHorzScroller();
	void setVertScroller();

	//! Вычисляет пределы масштабирования
	void setScaleRanges();
	//! Вычисляет пределы скроллирования
	void setScrollRanges();
	//! Обновляет информацию о масштабе в scale-Мэнеджере
	void updateScaleInfo();
	//! Создаёт фонт нужного размера
	void createFont();

	void initViewportOrg();
	void initScale();

	CRect const& getWorkArea() const;
	void paintRect(CRect const& r);

	void extendWAToLeft(int ex);
	void extendWAToRight(int x);
	void extendWAToTop(int ex);
	void extendWAToBottom(int ex);

	//! Насколько будем наращивать рабочую область по горизонтали
	int getWorkAreaHorzAdd() const;
	//! Насколько будем наращивать рабочую область по вертикали
	int getWorkAreaVertAdd() const;


	ITEWorkMode* getCurrentWorkMode() const;
	CPoint const& getViewportOrg() const;
	void setViewportOrg(int x, int y);
	void setViewportOrg(CPoint const& point);
public:

	void log2Scr(CSize* psz){
		POINT p0 = {0, 0};
		POINT p1 = {psz->cx, psz->cy};
		log2Scr(&p0, 1);
		log2Scr(&p1, 1);
		psz->cx = p1.x - p0.x;
		psz->cy = p1.y - p0.y;
	}
	//перевод координат
	void log2Scr(POINT* p, int cnt){
		for(int i = cnt; --i>=0;)
		{
			scalePoint(p);
			p->x += viewportOrg_.x;
			p->y += viewportOrg_.y;
			++p;
		}
	}
	
	void scr2Log(POINT* p, int cnt){
		for(int i = cnt; --i>=0;)
		{
			p->x -= viewportOrg_.x;
			p->y -= viewportOrg_.y;
			descalePoint(p);
			++p;
		}
	}
	
	void log2Scr(RECT* pr){
		log2Scr(reinterpret_cast<POINT*>(pr), 2);
	}
	
	void scr2Log(RECT* pr){
		scr2Log(reinterpret_cast<POINT*>(pr), 2);
	}
	int scaleX(const int x){
		return round(x*fScale_);
	}
	int scaleY(const int y){
		return round(y*fScale_);
	}

	int descaleX(const int x){
		return round(x/fScale_);
	}
	int descaleY(const int y){
		return round(y/fScale_);
	}

protected:	
	void scalePoint(POINT*p){
		p->x = scaleX(p->x);
		p->y = scaleY(p->y);
	}
	void scalePoint(POINT*p, int cnt){
		for(int i = cnt; --i>=0;)
			scalePoint(p++);
	}
	
	void descalePoint(POINT*p){
		p->x = descaleX(p->x);
		p->y = descaleY(p->y);
	}
	void descalePoint(POINT*p, int cnt){
		for(int i = cnt; --i>=0;)
			descalePoint(p++);
	}

	HFONT getFont() const{
		return font_.get();
	}

private:
	//! Объект, управляющий масштабом
	IScaleMgr* scaleManager_;
	//! 
	CPoint viewportOrg_;
	//! Текщий масштаб
	float fScale_;
	//! Минимальный масштаб
	float fMinScale_;
	//! Максимальный масштаб
	float fMaxScale_;
	//! Минимальная позиция скроллера по горизонтали
	int iMinScrollLimX_;
	//! Максимальная позиция скроллера по горизонтали
	int iMaxScrollLimX_;
	//! Максимальная позиция скроллера по вертикали
	int iMaxScrollLimY_;
	//! Минимальная позиция скроллера по вертикали
	int iMinScrollLimY_;
	//! Фонт, которым выводится текст
	Font font_;
	//! DC окна
	CDC deviceContext_;
	//! Рабочая область окна
	CRect workAreaRect_;

	//! Логика работы
	TriggerEditorLogic *triggerEditorLogic_;
	//! Объект реализующий drag&drop
	COleDropTarget dropTarget_;
	//! Тултип
	boost::scoped_ptr<ToolTip> toolTip_;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIGGEREDITORVIEW_H__1A772501_7E52_4E41_90E2_5E6611EC9854__INCLUDED_)
