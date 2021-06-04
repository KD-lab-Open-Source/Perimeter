#pragma	 once

#include "ScaleInterfaces.h"
class CScaleBar :  public CToolBar
{
public:
	CScaleBar();
	bool Create(CWnd* pparent, DWORD id);
	void SetScalable(IScalable* psclbl);
	void UpdateScaleInfo();

private:
	void AddCombo();

	void OnComboSelChange();
	void OnScaleMinus();
	void OnScalePlus();
	DECLARE_MESSAGE_MAP();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void InitCombo();

	void OnUpdateScaleMinus(CCmdUI* pCmdUI);
	void OnUpdateScalePlus(CCmdUI* pCmdUI);

	void ReadAndSetScale();
private:
	CComboBox m_wndCombo;
	CFont m_font;

	IScalable* m_pScalable;
};
