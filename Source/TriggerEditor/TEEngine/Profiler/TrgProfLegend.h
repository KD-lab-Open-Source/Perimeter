#pragma once


// TrgProfLegend dialog

class TrgProfLegend : public CDialog
{
	DECLARE_DYNAMIC(TrgProfLegend)

public:
	TrgProfLegend(CWnd* pParent = NULL);   // standard constructor
	virtual ~TrgProfLegend();

// Dialog Data
	enum { IDD = IDD_TRG_PROFILER_LEGEND };
	void initColors(COLORREF doned_link, COLORREF actve_link, 
					COLORREF inactive_link, COLORREF doned_ele, 
					COLORREF active_ele, COLORREF inactive_ele, COLORREF waiting_ele);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
private:
	CBrush m_brDonedLink;
	CBrush m_brActiveLink;
	CBrush m_brInactiveLink;

	CBrush m_brDonedEle;
	CBrush m_brActiveEle;
	CBrush m_brInactiveEle;
	CBrush m_brWaitingEle;
};
