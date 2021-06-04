#pragma once

class CTreeListItem;
class CTreeListCtrl;

typedef struct _TLKEYDOWN {
	WORD wVKey;
	UINT flags;
} TLKEYDOWN;

typedef struct _TLGETINFOTIP {
	LPTSTR pszText;
	int cchTextMax;
	CTreeListItem* item;
	int iSubItem;
} TLGETINFOTIP;


typedef struct _TLGETDISPINFO{
}TLGETDISPINFO;
class ITreeListNotifyListener
{
public:
	//! Not implemented
	virtual void onGetDispInfo(CTreeListCtrl& tree, TLGETDISPINFO* info) = 0;
	//! Not implemented
	virtual void onSetDispInfo(CTreeListCtrl& tree, TLGETDISPINFO* info) = 0;

	virtual bool onKeyDown(CTreeListCtrl& tree, TLKEYDOWN* info) = 0;
	virtual void onGetInfoTip(CTreeListCtrl& tree, TLGETINFOTIP* info) = 0;
};
