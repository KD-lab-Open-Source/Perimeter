#pragma once

#include "../ITreeListNotifyListener.h"

class NullTreeListNotifyListener : public ITreeListNotifyListener
{
public:
	NullTreeListNotifyListener(void);
	virtual ~NullTreeListNotifyListener(void);

	//! Not implemented
	virtual void onGetDispInfo(CTreeListCtrl& tree, TLGETDISPINFO* info);
	//! Not implemented
	virtual void onSetDispInfo(CTreeListCtrl& tree, TLGETDISPINFO* info);

	virtual bool onKeyDown(CTreeListCtrl& tree, TLKEYDOWN* info);
	virtual void onGetInfoTip(CTreeListCtrl& tree, TLGETINFOTIP* info);

};
