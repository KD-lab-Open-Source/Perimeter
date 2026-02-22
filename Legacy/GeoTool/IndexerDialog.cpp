// IndexerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "GeoTool.h"
#include "IndexerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIndexerDialog dialog


CIndexerDialog::CIndexerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIndexerDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIndexerDialog)
	indexerFromX = 0;
	indexerFromY = 0;
	indexerFromZ = 0;
	indexerStepX = 64;
	indexerStepY = 64;
	indexerStepZ = 4;
	indexerToX = 4096;
	indexerToY = 4096;
	indexerToZ = 256;
	indexerTreshold = 0.1f;
	//}}AFX_DATA_INIT
}


void CIndexerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIndexerDialog)
	DDX_Text(pDX, IDC_EDIT_FROM_X, indexerFromX);
	DDX_Text(pDX, IDC_EDIT_FROM_Y, indexerFromY);
	DDX_Text(pDX, IDC_EDIT_FROM_Z, indexerFromZ);
	DDX_Text(pDX, IDC_EDIT_STEP_X, indexerStepX);
	DDX_Text(pDX, IDC_EDIT_STEP_Y, indexerStepY);
	DDX_Text(pDX, IDC_EDIT_STEP_Z, indexerStepZ);
	DDX_Text(pDX, IDC_EDIT_TO_X, indexerToX);
	DDX_Text(pDX, IDC_EDIT_TO_Y, indexerToY);
	DDX_Text(pDX, IDC_EDIT_TO_Z, indexerToZ);
	DDX_Text(pDX, IDC_EDIT_TRESHOLD, indexerTreshold);
	//}}AFX_DATA_MAP
}

tx3d::Vector3D CIndexerDialog::getStep() {
	return tx3d::Vector3D(indexerStepX, indexerStepY, indexerStepZ);
}
tx3d::Vector3D CIndexerDialog::getFrom() {
	return tx3d::Vector3D(indexerFromX, indexerFromY, indexerFromZ);
}
tx3d::Vector3D CIndexerDialog::getTo() {
	return tx3d::Vector3D(indexerToX, indexerToY, indexerToZ);
}

float CIndexerDialog::getTreshold() {
	return indexerTreshold;
}


BEGIN_MESSAGE_MAP(CIndexerDialog, CDialog)
	//{{AFX_MSG_MAP(CIndexerDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIndexerDialog message handlers
