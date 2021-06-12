#include "StdAfx.h"
#include ".\nullcustomdrawnotifylistener.h"

NullCustomDrawNotifyListener::NullCustomDrawNotifyListener(void)
{
}

NullCustomDrawNotifyListener::~NullCustomDrawNotifyListener(void)
{
}

DWORD NullCustomDrawNotifyListener::onPrepaint(CTreeListCtrl& source, 
											   CONTROL_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}
DWORD NullCustomDrawNotifyListener::onPostpaint(CTreeListCtrl& source, 
												CONTROL_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}
DWORD NullCustomDrawNotifyListener::onPreerase(CTreeListCtrl& source, 
											   CONTROL_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}
DWORD NullCustomDrawNotifyListener::onPosterase(CTreeListCtrl& source, 
												CONTROL_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}

DWORD NullCustomDrawNotifyListener::onItemPrepaint(CTreeListCtrl& source, 
												   ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}
DWORD NullCustomDrawNotifyListener::onItemPostpaint(CTreeListCtrl& source, 
													ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}
DWORD NullCustomDrawNotifyListener::onItemPreerase(CTreeListCtrl& source, 
												   ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}
DWORD NullCustomDrawNotifyListener::onItemPosterase(CTreeListCtrl& source, 
													ITEM_CUSTOM_DRAW_INFO* pcdi)
{
	return CDRF_DODEFAULT;
}

DWORD NullCustomDrawNotifyListener::onSubitemPrepaint(CTreeListCtrl& source, 
													  SUBITEM_CUSTOM_DRAW_INFO* pscdi)
{
	return CDRF_DODEFAULT;
}
DWORD NullCustomDrawNotifyListener::onSubitemPostpaint(CTreeListCtrl& source, 
													   SUBITEM_CUSTOM_DRAW_INFO* pscdi)
{
	return CDRF_DODEFAULT;
}
