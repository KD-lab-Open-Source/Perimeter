// ToolTip.cpp: implementation of the ToolTip class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "resource.h"
#include "ToolTip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ToolTip::ToolTip():
  ownerWindow_(NULL)
, toolTipWindow_(NULL)
{

}

ToolTip::~ToolTip()
{

}

void ToolTip::setOwnerWindow(HWND owner){
	ownerWindow_ = owner;
}
HWND ToolTip::getOnwenerWindow() const{
	return ownerWindow_;
}

HWND ToolTip::create(HWND owner)
{
	setOwnerWindow(owner);
    // Create the ToolTip control.
    toolTipWindow_ = CreateWindow(TOOLTIPS_CLASS, TEXT(""),
		WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, AfxGetApp()->m_hInstance,
		NULL);
	
    TOOLINFO    ti;
    // Prepare TOOLINFO structure for use as tracking ToolTip.
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
    ti.hwnd   = ownerWindow_;
    ti.uId    = reinterpret_cast<UINT_PTR>(ownerWindow_);
    ti.hinst  = AfxGetApp()->m_hInstance;
    ti.lpszText  = LPSTR_TEXTCALLBACK;
    ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 
    
    // Add the tool to the control, displaying an error if needed.
    if(!::SendMessage(toolTipWindow_,TTM_ADDTOOL,0,(LPARAM)&ti)){
        ::MessageBox(ownerWindow_, _T("Couldn't create the ToolTip control."),
			_T("Error"),MB_OK);
        return NULL;
    }
		
    return(toolTipWindow_);    
}

void ToolTip::trackToolTip(CPoint const& pos) const
{
	if (!toolTipWindow_)
		return;
	::SendMessage(toolTipWindow_, TTM_TRACKPOSITION, 
			0, (LPARAM)MAKELPARAM(pos.x + 15,pos.y + 15));
}

void ToolTip::setToolTipText(CString const& str) const
{
	if (!toolTipWindow_)
		return;
	if (toolTipText_ != str){ 
		TOOLINFO    ti;
		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
		ti.hwnd   = ownerWindow_;
		ti.uId    = reinterpret_cast<UINT_PTR>(ownerWindow_);
		ti.hinst  = AfxGetApp()->m_hInstance;
		ti.lpszText  = LPSTR_TEXTCALLBACK;
		ti.rect.top = ti.rect.bottom = ti.rect.left = ti.rect.right = 0; 

		ti.lParam = NULL;
		// Activate (display) the tracking ToolTip. Then, set a global
		// flag value to indicate that the ToolTip is active, so other
		// functions can check to see if it's visible.
		toolTipText_ = str;
		::SendMessage(toolTipWindow_,TTM_TRACKACTIVATE,
						(WPARAM)(!str.IsEmpty()),(LPARAM)&ti);
	}
}

CString const& ToolTip::getToolTipText() const{
	return toolTipText_;
}

BOOL ToolTip::OnTTNGetDispInfo(UINT id, NMHDR * pTTTStruct, LRESULT * pResult ) const
{
	NMTTDISPINFO* pdi = reinterpret_cast<NMTTDISPINFO *>(pTTTStruct);
	switch(pTTTStruct->code) {
	case TTN_GETDISPINFO:
		::SendMessage(pdi->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 
						0, GetSystemMetrics(SM_CXSCREEN));
 		break;
	}
	pdi->lpszText = const_cast<TCHAR*>((LPCTSTR)getToolTipText());
	pdi->hinst = NULL;
	return TRUE;
}
