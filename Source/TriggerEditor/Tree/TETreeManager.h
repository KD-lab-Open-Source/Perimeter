#pragma once

#include <boost/scoped_ptr.hpp>
#include "Custom Controls/CoolDialogBar.h"

class TETreeDlg;
class TETreeNotifyListener;
class xTreeListCtrl;

class TETreeManager
{
public:
	enum eShowHide {SH_HIDE, SH_SHOW};

	TETreeManager(void);
	~TETreeManager(void);

	bool create(CFrameWnd* pParent, 
				DWORD dwStyle = WS_CHILD |CBRS_FLYBY | 
				CBRS_RIGHT | CBRS_SIZE_DYNAMIC);
	void dockBar(UINT dockBarID);
	//! Показать/спрятать
	bool show(eShowHide e);
	//! видимо или нет
	bool isVisible() const;
	//! Реализация дерева
	xTreeListCtrl& getTreeListCtrl() const;
	//! Устанавливает обработчик событий
	void setTETreeNotifyListener(TETreeNotifyListener* ptl);
private:
	boost::scoped_ptr<TETreeDlg> treeDlg_;
	CCoolDialogBar dialogBar_;
	CFrameWnd* parentFrame_;
};
