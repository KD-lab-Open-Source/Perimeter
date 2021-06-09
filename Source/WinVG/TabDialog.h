#pragma once

#include "TreeListCtrl/TreeListCtrl.h"
#include "TreeListCtrl/Null Listeners/NullChangeItemLabelNotifyListener.h"
#include "TreeListCtrl/Null Listeners/NullItemDragdropNotifyListener.h"
#include "TreeListCtrl/Null Listeners/NullItemChangeNotifyListener.h"
#include "TreeListCtrl/Null Listeners/NullItemOperationNotifyListener.h"
#include "TreeListCtrl/Null Listeners/NullTreeListNotifyListener.h"
#include "TreeListCtrl/Null Listeners/NullGeneralNotifyListener.h"
#include "afxwin.h"

class CTabDialog : public CFormView
				   , protected NullChangeItemLabelNotifyListener
				   , protected NullItemChangeNotifyListener
{
	DECLARE_DYNCREATE(CTabDialog)

protected:
	CTabDialog();           // protected constructor used by dynamic creation
	virtual ~CTabDialog();

public:
	enum { IDD = IDD_TAB_DIALOG };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void TreeUpdate();

	void SetRoot(cObject3dx* pUObj,CString filename);
	void AddRoot(cObject3dx* pUObj);
	void SetLogicObj(cObject3dx* pUObj);
	cObject3dx* GetLogicObj(){return logic_obj;};

	cObject3dx* GetRoot();
	void ClearRoot();
	inline vector<cObject3dx*>& GetAllObj(){return UObj;};

	CString GetFileName(){return filename;};
protected:
	vector<cObject3dx*> UObj;
	cObject3dx* logic_obj;
	CString filename;
	CTreeListCtrl treeList;

	void SetHeadAnimationGroup();
	void UpdateAnimationGroup();
	void UpdateVisibilityGroup();
	void UpdateAnimationGroupBox();

	afx_msg void OnSize(UINT nType,int cx,int cy);

	bool onBeginControl(CTreeListCtrl& source, 
						CHANGE_LABEL_NOTIFY_INFO* pclns);
	bool onEndLabelEdit(CTreeListCtrl& source, 
		CHANGE_LABEL_NOTIFY_INFO* pclns);
protected:
	void GetRect(CWnd& wnd,CRect& rc);
	void SetPos(CWnd& wnd,CRect& rc);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	CComboBox visibility_group;
	CComboBox chains;
	afx_msg void OnCbnSelchangeChains();
	afx_msg void OnCbnSelchangeVisibilityGroup();
};


