#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "boost/scoped_ptr.hpp"

class TrgProfLegend;
interface ITriggerEditorView;
interface ITriggerChainProfiler;

// TriggerDbgDlg dialog

class TriggerDbgDlg : public CDialog
{
	DECLARE_DYNAMIC(TriggerDbgDlg)

public:
	TriggerDbgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TriggerDbgDlg();

// Dialog Data
	enum { IDD = IDD_DLG_TRIG_DBG };

	bool setTriggerEditorView(ITriggerEditorView* teView);
	void setTriggerChainProfiler(ITriggerChainProfiler* ptrProf);

	bool Load();
	bool Prev() const;
	bool Next() const;

	bool Activate() const;
	bool Deactivate() const;

	void OnShow();
	void OnHide();

	bool canBeActivated() const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLoadDbg();
	afx_msg void OnClearDbgList();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedBtnShowLegend();

	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

	bool MoveSelection(int count) const;
	void SetupScroller();
	bool SelectItem(int iItem) const;
private:
	// ! Список строк
	mutable CListCtrl m_list;
	mutable int lastCurrentRecord_;
	ITriggerEditorView* ptrTriggerEditorView_;

	CScrollBar m_scrlr;
	boost::scoped_ptr<TrgProfLegend> m_ptrTrgProfLegend;

	ITriggerChainProfiler* ptrTriggerChainProfiler_;
};
