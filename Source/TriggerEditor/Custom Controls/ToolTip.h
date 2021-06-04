// ToolTip.h: interface for the ToolTip class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLTIP_H__A9937CBE_26A0_4108_AC68_773A44398C9E__INCLUDED_)
#define AFX_TOOLTIP_H__A9937CBE_26A0_4108_AC68_773A44398C9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWnd;

class ToolTip  
{
public:
	ToolTip();
	virtual ~ToolTip();
	CString const& getToolTipText() const;
	//! Назначает текст тултипа
	void setToolTipText(CString const& str) const;
	//! показывает тултип
	void trackToolTip(CPoint const& pos) const;
	//! создание тултипа
	HWND create(HWND owner);
	void setOwnerWindow(HWND owner);
	HWND getOnwenerWindow() const;

	//! Вызывать из OnTTNGetDispInfo окна
	BOOL OnTTNGetDispInfo(UINT id, NMHDR * pTTTStruct, LRESULT * pResult ) const;
private:
	//! Текст туллтипа
	mutable CString toolTipText_;
	//! Окно тултипа
	HWND toolTipWindow_;
	//! Окно владельца
	HWND ownerWindow_;
};

#endif // !defined(AFX_TOOLTIP_H__A9937CBE_26A0_4108_AC68_773A44398C9E__INCLUDED_)
