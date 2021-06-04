#pragma once
#include "icustomdrawnotifylistener.h"

class NullCustomDrawNotifyListener :
	public ICustomDrawNotifyListener
{
public:
	NullCustomDrawNotifyListener(void);
	virtual ~NullCustomDrawNotifyListener(void);

	virtual DWORD onPrepaint(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi);
	virtual DWORD onPostpaint(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi);
	virtual DWORD onPreerase(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi);
	virtual DWORD onPosterase(CTreeListCtrl& source, CONTROL_CUSTOM_DRAW_INFO* pcdi);

	virtual DWORD onItemPrepaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
	virtual DWORD onItemPostpaint(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
	virtual DWORD onItemPreerase(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);
	virtual DWORD onItemPosterase(CTreeListCtrl& source, ITEM_CUSTOM_DRAW_INFO* pcdi);

	virtual DWORD onSubitemPrepaint(CTreeListCtrl& source, 
									SUBITEM_CUSTOM_DRAW_INFO* pscdi);
	virtual DWORD onSubitemPostpaint(CTreeListCtrl& source, 
									SUBITEM_CUSTOM_DRAW_INFO* pscdi);
};
