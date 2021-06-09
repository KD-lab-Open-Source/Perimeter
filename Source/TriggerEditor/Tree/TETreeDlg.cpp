// TETreeDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "../resource.h"
#include "TETreeDlg.h"
#include "TETreeNotifyListener.h"

// TETreeDlg dialog

IMPLEMENT_DYNAMIC(TETreeDlg, CDialog)
TETreeDlg::TETreeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TETreeDlg::IDD, pParent)
	, teTreeNotifyListener_(NULL)
{
}

TETreeDlg::~TETreeDlg()
{
}

void TETreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM, tree_);
}


BEGIN_MESSAGE_MAP(TETreeDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// TETreeDlg message handlers

BOOL TETreeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	tree_.SetStyle(
		TLC_TREELIST								
		| TLC_DOUBLECOLOR							
		| TLC_SHOWSELFULLROWS						
		| TLC_SHOWSELALWAYS
		
		| TLC_HEADER								
		| TLC_HGRID									
		| TLC_VGRID									
		| TLC_TGRID									
		| TLC_HGRID_EXT								
		| TLC_VGRID_EXT								
		| TLC_HGRID_FULL							
		
		| TLC_TREELINE								
		| TLC_ROOTLINE								
		| TLC_BUTTON								
		| TLC_IMAGE									
		| TLC_HOTTRACK								
		| TLC_DRAG
//		| TLC_HEADDRAGDROP							
		);
	return TRUE;  
}


void TETreeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (tree_.m_hWnd)
	{
		CRect r;
		GetClientRect(r);
		tree_.MoveWindow(r);
	}
}

BOOL TETreeDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(teTreeNotifyListener_);
	if (teTreeNotifyListener_->onNotify(wParam, lParam, pResult))
		return TRUE;
	return CDialog::OnNotify(wParam, lParam, pResult);
}

BOOL TETreeDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	ASSERT(teTreeNotifyListener_);
	if (teTreeNotifyListener_->onCommand(wParam, lParam))
		return TRUE;

	return CDialog::OnCommand(wParam, lParam);
}

void TETreeDlg::OnCancel()
{
}

void TETreeDlg::OnOK()
{
}
