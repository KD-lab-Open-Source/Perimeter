#pragma once

#include "Custom Controls/xTreeListCtrl.h"
// TETreeDlg dialog

class TETreeNotifyListener;

class TETreeDlg : public CDialog
{
	DECLARE_DYNAMIC(TETreeDlg)

public:
	TETreeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TETreeDlg();

// Dialog Data
	enum { IDD = IDD_TREE_DLG };
	xTreeListCtrl& getTreeCtrl(){
		return tree_;
	}
	TETreeNotifyListener* getTETreeNotifyListener() const{
		return teTreeNotifyListener_;
	}
	void setTETreeNotifyListener(TETreeNotifyListener* pl) {
		teTreeNotifyListener_ = pl;
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
private:
	xTreeListCtrl tree_;
	TETreeNotifyListener *teTreeNotifyListener_;
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	virtual void OnOK();
};
